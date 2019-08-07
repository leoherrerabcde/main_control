/*#include "lucesestado.h"
#include "commgsm.h"
#include "electrovalvctrl.h"
#include "rfidboquilla.h"
#include "rfiduser.h"
#include "mainstate.h"
//#include "IDVehicleList.h"
//#include "IDUserList.h"
#include "IDList.h"
#include "MainCtrlSettings.h"
#include "CSocket.h"
#include "SCCLog.h"
#include "SCCAlive.h"
#include "SCCRemoteServer.h"
#include "SCCFlowmeter.h"
#include "SCCDeviceNames.h"
#include "SCCCreateMessage.h"
//#include "SCCRegisters.h"
#include "SCCFuelTransaction.h"*/

#include "main_functions.h"
//#include "SCCDisplay.h"
#include "SCCDeviceParams.h"
#include "../commPort/SCCRealTime.h"
#include "main_fuel_manage.h"

/*#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <list>*/

using namespace std;


//SCCDisplay glDisplay;
SCCLog  globalLog(std::cout);
bool    gl_bVerbose(true);

int main(int argc, char* argv[])
{
    bool bShowData(false);
    bool bLaunchDisable(false);
    std::string strNumRegToSent;

    if (argc > 1)
    {
        for (int i = 1; i<argc;)
        {
            std::string strArg(argv[i++]);
            if (strArg == "--launch_disable")
                bLaunchDisable = true;
            if (strArg == "--NumRegToSent")
            {
                strNumRegToSent = argv[i++];
            }
        }
    }

    std::unordered_map<std::string,Device*> deviceList;
    std::unordered_map<int,Device*> timerDeviceMap;
    std::unordered_map<std::string,CSocket*> socketMap;
    std::vector<Device*> onTheFlyDeviceList;
    std::list<int> portList;

    Device::getComPortList(portList);
    SCCFuelTransaction fuelRegister(TABLE_REGISTERS);
    SCCFlowmeter flowmeter(DEVICE_OVALGEARFLOWM, bShowData);
    RFIDBoquilla rfidBoquilla(DEVICE_RFID_BOQUILLA, bShowData);
    RFIDUser rfidUser(DEVICE_RFID_BOMBERO, bShowData);
    ElectroValvCtrl electroValve(DEVICE_ELECTRO_VALVE, bShowData);
    //Device guiApp(DEVICE_GUI, bShowData);
    SCCRemoteServer restApi(DEVICE_REST_SERVICE, bShowData);

    commGSM modCommGSM(DEVICE_SERVER, bShowData);
    LucesEstado lucesDeEstado(DEVICE_STATUS_LIGHTS, bShowData);

    /*IDUserList UserList(TABLE_USERS);
    IDVehicleList VehicleList(TABLE_VEHICLES);*/
    IDList UserList(TABLE_USERS);
    IDList VehicleList(TABLE_VEHICLES);

    MainState mainState;
    MainCtrlSettings mainSettings;

    CSocket socketServer;
    std::list<CSocket*> socketClientList;
    std::list<CSocket*> socketNewClientList;
    socketServer.setLocalPort(mainSettings.serverPort);

    Device* pDvc;

    pDvc = &rfidBoquilla;
    deviceList.insert(std::make_pair(pDvc->name(), pDvc));

    pDvc = &rfidUser;
    deviceList.insert(std::make_pair(pDvc->name(), pDvc));

    /*pDvc = &electroValve;
    deviceList.insert(std::make_pair(pDvc->name(), pDvc));*/

    /*pDvc = &guiApp;
    deviceList.insert(std::make_pair(pDvc->name(), pDvc));*/

    pDvc = &restApi;
    deviceList.insert(std::make_pair(pDvc->name(), pDvc));

    pDvc = &flowmeter;
    deviceList.insert(std::make_pair(pDvc->name(), pDvc));

    int stateRbpi;

    stateRbpi = mainState.getLastState();

    SCCAlive keepAlive;
    keepAlive.throwDisable();
    keepAlive.start(mainSettings.noResponseTimeMilli);

    int mainTmr             = keepAlive.addTimer(mainSettings.mainTimerInterval);
    int rqtTblTmr           = keepAlive.addTimer(mainSettings.requestTableTmrInterval);
    int tmrConnectServer    = keepAlive.addTimer(mainSettings.tmrServerConnect);
    int nRetryCounter       = 0;
    //int tmrWaitServerResponse   = 0;
    int tmrConnectRetry     = keepAlive.addTimer(mainSettings.tmrServerRetry);
    keepAlive.stopTimer(tmrConnectRetry);

    bool bConnectToServer(false);

    flowmeter.init(mainSettings);
    rfidBoquilla.init(mainSettings);
    rfidUser.init(mainSettings);
    electroValve.init(mainSettings);
    restApi.init(mainSettings);
    modCommGSM.init(mainSettings);
    lucesDeEstado.init(mainSettings);

    flowmeter.setTimerHandler(keepAlive.addTimer(mainSettings.tmrDeviceTimeOut));
    rfidBoquilla.setTimerHandler(keepAlive.addTimer(mainSettings.tmrDeviceTimeOut));
    rfidUser.setTimerHandler(keepAlive.addTimer(mainSettings.tmrDeviceTimeOut));
    //restApi.setTimerHandler(keepAlive.addTimer(mainSettings.tmrDeviceTimeOut));
    timerDeviceMap.insert(std::make_pair(flowmeter.getTimerHandler(), &flowmeter));
    timerDeviceMap.insert(std::make_pair(rfidBoquilla.getTimerHandler(), &rfidBoquilla));
    timerDeviceMap.insert(std::make_pair(rfidUser.getTimerHandler(), &rfidUser));


    UserList.init(mainSettings);
    VehicleList.init(mainSettings);

    fuelRegister.init(mainSettings);
    int tmrFuelTransaction = keepAlive.addTimer(30000);
    int tmrAuthorizedVehicle = keepAlive.addTimer(10000);
    keepAlive.stopTimer(tmrAuthorizedVehicle);

    restApi.setDestinationPath(fuelRegister.getRegHistoPath());
    if (strNumRegToSent != "")
        restApi.setNumRegisterSent(stoi(strNumRegToSent));

    double dAcumFlowOld = 0.0;

    std::string strTagVehicle;
    std::string strIdUser;
    std::string strDriver;

    bool bRfidReaderDisable = false;
    //bool bRfidReaderChange  = false;
    bool bStateChange       = false;
    bool bCancelTransaction = false;

    globalLog << "Main Loop Started." << std::endl;

    socketServer.listen();
    bool bConnectImmediately = false;

    std::list<std::string> DeviceToLaunchList =
    {
        DEVICE_REST_SERVICE,
        DEVICE_RFID_BOMBERO,
        DEVICE_RFID_BOQUILLA,
        DEVICE_OVALGEARFLOWM,
    };

    std::list<std::string> DevicePendingToLaunchList;

    std::string strDeviceLaunched = verifyDeviceService(DeviceToLaunchList, deviceList, portList, bLaunchDisable);
    int tmrLaunchService = keepAlive.addTimer(mainSettings.tmrLaunchService);


    ProcessFuelEvents    process_fuel_events(
            mainState,
            keepAlive,
            fuelRegister,
            UserList,
            VehicleList,
            electroValve,
            flowmeter,
            rfidBoquilla,
            rfidUser,
            tmrFuelTransaction,
            tmrAuthorizedVehicle,
            stateRbpi,
            bRfidReaderDisable,
            bStateChange,
            socketMap,
            deviceList,
            strTagVehicle,
            strIdUser,
            strDriver
            );

    for(;;)
    {
        bool bSleep = true;

        process_fuel_events.check_events();

        if (proccesNewConnection(socketServer, mainSettings, socketNewClientList))
        {
        }
        if (processDataNewClients(socketNewClientList, socketClientList, deviceList, onTheFlyDeviceList, socketMap, portList))
        {
            keepAlive.resetTimer(tmrLaunchService);
            strDeviceLaunched = verifyDeviceService(DeviceToLaunchList, deviceList, portList, bLaunchDisable);
            if (strDeviceLaunched == "")
            {
                if (DevicePendingToLaunchList.empty())
                    keepAlive.stopTimer(tmrLaunchService);
                else
                {
                    DeviceToLaunchList.insert(DeviceToLaunchList.end(), DevicePendingToLaunchList.begin(), DevicePendingToLaunchList.end());
                    DevicePendingToLaunchList.clear();
                }
            }
        }
        if (keepAlive.isTimerEvent(tmrLaunchService))
        {
            bool bLaunchService(true);
            if (strDeviceLaunched != "")
            {
                auto itDvc = deviceList.find(strDeviceLaunched);
                if (itDvc != deviceList.end())
                {
                    Device* pDvc = itDvc->second;
                    if (pDvc->isServiceRunning())
                    {
                        bLaunchService = false;
                    }
                    else
                    {
                        DevicePendingToLaunchList.push_back(strDeviceLaunched);
                    }
                }
            }
            if (bLaunchService)
            {
                strDeviceLaunched = verifyDeviceService(DeviceToLaunchList, deviceList, portList, bLaunchDisable);
                if (strDeviceLaunched == "")
                {
                    if (DevicePendingToLaunchList.empty())
                        keepAlive.stopTimer(tmrLaunchService);
                    else
                    {
                        DeviceToLaunchList.insert(DeviceToLaunchList.end(), DevicePendingToLaunchList.begin(), DevicePendingToLaunchList.end());
                        DevicePendingToLaunchList.clear();
                    }
                }
                else
                {
                    globalLog << SCCRealTime::getTimeStamp() << ":\t" << "Device Launched: " << strDeviceLaunched << std::endl;
                }
            }
        }
        processDataClients(socketClientList, deviceList, keepAlive);

        if (keepAlive.isTimerEvent(mainTmr))
        {
            verifyDeviceTimer(socketClientList, deviceList, DevicePendingToLaunchList, keepAlive, socketMap);
            //sendAliveMessage(socketClientList, deviceList);
            std::string msg;
            msg = aliveMsg(deviceList, socketMap);
            globalLog << SCCRealTime::getTimeStamp() << std::endl;
            globalLog << msg << "\n\n";
            if (restApi.getServicePID())
            {
                if (restApi.isRetryError())
                    globalLog << "Retry Error" << std::endl;
                if (restApi.isWaitingResponse())
                    globalLog << "Waiting for Response" << std::endl;
                else
                    globalLog << "Not waiting for Response" << std::endl;
                globalLog << "Connection to Remote DB in " << keepAlive.millisecondsToMMSS(keepAlive.getUpdate(tmrConnectServer)) << " seconds" << std::endl;
                globalLog << "Connection Retry Timer in " << keepAlive.millisecondsToMMSS(keepAlive.getUpdate(tmrConnectRetry)) << std::endl;
                globalLog << "Times to Retry: " << nRetryCounter << std::endl;
                globalLog << std::endl;
            }
            //globalLog << std::endl();
        }
        if (keepAlive.isTimerEvent(rqtTblTmr))
        {
            sendRequestTable(socketClientList, deviceList);
        }

        if (restApi.getServicePID())
        {
            if (keepAlive.isTimerEvent(tmrConnectServer) || bConnectImmediately)
            {
                bConnectImmediately = false;
                bConnectToServer    = true;
                restApi.clearWaitingResponse();
                //keepAlive.resetTimer(tmrConnectRetry);
                fuelRegister.getRegisterList(restApi.getRegisterList());
                restApi.startConnection(fuelRegister.getMemberList());
            }
            if (bConnectToServer)
            {
                if (!restApi.isWaitingResponse())
                {
                    keepAlive.resetTimer(tmrConnectRetry);
                    keepAlive.stopTimer(tmrConnectServer);
                    std::string strBody;
                    if (!restApi.isRegisterListEmpty())
                    {
                        restApi.resetState();
                        restApi.getNextRegisterRequest(strBody);
                        SCCFileManager::writeFile(fuelRegister.getRegisterPath(), "registers.json", strBody);
                        auto itSck = socketMap.find(restApi.name());
                        if (itSck == socketMap.end())
                            break;
                        SCCCreateMessage sccPostMsg;
                        sccPostMsg.addParam(MSG_HEADER_TYPE, DEVICE_REST_SERVICE);
                        sccPostMsg.addParam(MSG_SERV_URL_HEADER, restApi.getUrlPostMethod());
                        sccPostMsg.addParam(MSG_SERV_METHOD_HEADER, MSG_SERV_METHOD_POST);
                        sccPostMsg.addBody(MSG_SERV_BODY_HEADER, strBody);
                        std::string msg = sccPostMsg.makeMessage();
                        CSocket* pSck = itSck->second;
                        pSck->sendData(msg);
                        globalLog << "Sending Registers to " << restApi.getUrlPostMethod() << std::endl;
                        restApi.setWaitingResponse();
                        restApi.nextState();
                    }
                    else if (!restApi.isTableListEmpty())
                    {
                        restApi.getNextTableRequest(strBody);
                        auto itSck = socketMap.find(restApi.name());
                        if (itSck == socketMap.end())
                            break;
                        SCCCreateMessage sccPostMsg;
                        sccPostMsg.addParam(MSG_HEADER_TYPE, DEVICE_REST_SERVICE);
                        int index = restApi.getCurrentTableIndex();
                        std::string strUrlGet = restApi.getUrlGetMethod(index);
                        sccPostMsg.addParam(MSG_SERV_URL_HEADER, strUrlGet);
                        sccPostMsg.addParam(MSG_SERV_METHOD_HEADER, MSG_SERV_METHOD_GET);
                        sccPostMsg.addParam(MSG_SERV_BODY_HEADER, strBody);
                        std::string msg = sccPostMsg.makeMessage();
                        CSocket* pSck = itSck->second;
                        pSck->sendData(msg);
                        globalLog << "Asking Table" << index << " from " << strUrlGet << std::endl;
                        restApi.setWaitingResponse();
                        restApi.nextState();
                    }
                    else
                    {
                        if (restApi.isTableBody())
                        {
                            VehicleList.writeTable(restApi.getBodyFromTable(0));
                            UserList.writeTable(restApi.getBodyFromTable(1));
                            globalLog << "Writing Tables" << std::endl;
                            /*std::string strUrl = restApi.getUrlPostMethod();
                            strUrl = "";
                            restApi.setUrl(strUrl);*/
                            restApi.nextState();
                        }
                        bConnectToServer = false;
                        keepAlive.stopTimer(tmrConnectRetry);
                        keepAlive.resetTimer(tmrConnectServer);
                        nRetryCounter = 0;
                        restApi.clearRetryError();
                    }
                }
                /*else
                {
                }*/
                if (keepAlive.isTimerEvent(tmrConnectRetry))
                {
                    restApi.clearWaitingResponse();
                    //keepAlive.resetTimer(tmrConnectRetry);
                    ++nRetryCounter;
                    if (nRetryCounter >= mainSettings.nMaxServerRetry)
                    {
                        keepAlive.stopTimer(tmrConnectRetry);
                        keepAlive.resetTimer(tmrConnectServer);
                        nRetryCounter = 0;
                        bConnectToServer = 0;
                        restApi.setRetryError();
                    }
                }
            }
        }
        else
        {
            keepAlive.stopTimer(tmrConnectRetry);
            keepAlive.stopTimer(tmrConnectServer);
            bConnectImmediately = true;
        }

        if (bSleep == true)
            usleep(10000);

        keepAlive.update();
    }

    keepAlive.stopTimer(mainTmr);

    return 0;
}

