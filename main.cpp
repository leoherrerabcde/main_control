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
#include "SCCDisplay.h"
#include "SCCDeviceParams.h"
#include "../commPort/SCCRealTime.h"

/*#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <list>*/

using namespace std;


SCCDisplay glDisplay;
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
    std::unordered_map<std::string,CSocket*> socketMap;
    std::vector<Device*> onTheFlyDeviceList;
    std::list<int> portList;

    Device::getComPortList(portList);
    SCCFuelTransaction fuelRegister(TABLE_REGISTERS);
    SCCFlowmeter flowmeter(DEVICE_FLOWMETER, bShowData);
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
    //int tmrWaitServerResponse   = 0;
    int tmrConnectRetry     = keepAlive.addTimer(mainSettings.tmrServerRetry);

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


    UserList.init(mainSettings);
    VehicleList.init(mainSettings);

    fuelRegister.init(mainSettings);
    int tmrFuelTransaction = keepAlive.addTimer(30000);
    int tmrAuthorizedVehicle = keepAlive.addTimer(10000);
    keepAlive.stopTimer(tmrAuthorizedVehicle);
    //int tmrServiceLaunched = 0;

    /*SCCFileManager newRegPath(fuelRegister.getRegisterPath());
    newRegPath << fuelRegister.getRegisterNewPath();
    restApi.setRegisterPath(std::string(newRegPath.str()));*/
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
    bool bFirstTime = false;
    //bool bWaitingConnection = true;

    std::string strDeviceLaunched = verifyDeviceService(deviceList, portList, bLaunchDisable);
    int tmrLaunchService = keepAlive.addTimer(mainSettings.tmrLaunchService);

    for(;;)
    {
        bool bSleep = true;
        if (keepAlive.isTimerEvent(tmrFuelTransaction))
        {
            electroValve.closeValve();
            fuelRegister.finishTransaction(flowmeter.getAcumFlow());
            //fuelTransactionTimeOut();
            keepAlive.stopTimer(tmrFuelTransaction);
            mainState.processFuelingTimeOut();
        }
        stateRbpi = mainState.getCurrentState();
        if (stateRbpi == MainState::State::waitForInitTransaction)
        {
            if (bRfidReaderDisable)
            {
                sendData(UserList.getTableType(), socketMap, deviceList, rfidUser.getCmdUnLockReader());
                bRfidReaderDisable = false;
            }
            bool bAuthorizedUser        = false;
            bool bAuthorizedVehicle     = false;
            bool bAuthorizedDriver      = false;

            bStateChange = false;
            //bool bInitFuelTransaction   = false;
            if (rfidUser.isUserDetected())
            {
                std::string strId = rfidUser.getUserId();
                std::string strType = UserList.getAtributeValue(strId);
                //bAuthorizedUser = UserList.isValidID(rfidUser.getUserId());
                if (strType == PARAM_FIREFIGHTER_COD)
                {
                    bAuthorizedUser = true;
                    strIdUser = strId;
                    mainState.processUserAuthorization(bAuthorizedUser);
                    sendData(UserList.getTableType(), socketMap, deviceList, rfidUser.getCmdBeepSound());
                }
                if (strType == PARAM_DRIVER_COD)
                {
                    bAuthorizedDriver = true;
                    strDriver = strId;
                    mainState.processDriverAuthorization(bAuthorizedDriver);
                    sendData(UserList.getTableType(), socketMap, deviceList, rfidUser.getCmdBeepSound());
                    //sendData(UserList.getTableType(), socketMap, rfidUser.getCmdLockReader());
                }
            }
            if (rfidBoquilla.isTagDetected())
            {
                std::string strTag = rfidBoquilla.getTagId();
                bAuthorizedVehicle = VehicleList.isValidID(strTag);
                if (bAuthorizedVehicle)
                {
                    strTagVehicle = strTag;
                    //tmrAuthorizedVehicle = keepAlive.addTimer(10000);
                    keepAlive.resetTimer(tmrAuthorizedVehicle);
                    mainState.processVehicleAuthorization(bAuthorizedVehicle);
                }
            }
            if (bAuthorizedUser || bAuthorizedVehicle || bAuthorizedDriver)
            {
                dAcumFlowOld = flowmeter.getAcumFlow();
                fuelRegister.initTransaction(flowmeter.getAcumFlow());
                if (bAuthorizedUser)
                    fuelRegister.addUser(strIdUser);
                if (bAuthorizedDriver)
                    fuelRegister.addDriver(strDriver);
                if (bAuthorizedVehicle)
                    fuelRegister.addVehicle(strTagVehicle);

                fuelRegister.addFlowMeterBegin(flowmeter.getAcumFlow());
                keepAlive.resetTimer(tmrFuelTransaction);
                bStateChange = true;
            }
        }
        else if ((stateRbpi & MainState::State::RFIDBoquilla) || (stateRbpi & MainState::State::RFIDUser) || (stateRbpi & MainState::State::RFIDDriver))
        {
            bool bAuthorizedUser        = (stateRbpi & MainState::State::RFIDUser);
            bool bAuthorizedVehicle     = (stateRbpi & MainState::State::RFIDBoquilla);
            bool bAuthorizedDriver      = (stateRbpi & MainState::State::RFIDDriver);
            bool bChangeDetected        = false;

            bStateChange = false;

            if (rfidUser.isUserDetected())
            {
                std::string strType = UserList.getAtributeValue(rfidUser.getUserId());
                std::string strId = rfidUser.getUserId();
                if (strType == PARAM_FIREFIGHTER_COD)
                {
                    if (!bAuthorizedUser)
                    {
                        bAuthorizedUser = true;
                        sendData(UserList.getTableType(), socketMap, deviceList, rfidUser.getCmdBeepSound());
                        strIdUser = strId;
                        fuelRegister.addUser(strIdUser);
                        mainState.processUserAuthorization(bAuthorizedUser);
                        bChangeDetected = true;
                    }
                    else
                    {
                        if (strId != strIdUser)
                            bCancelTransaction = true;
                    }
                }
                if (strType == PARAM_DRIVER_COD)
                {
                    std::string strType = UserList.getAtributeValue(rfidUser.getUserId());
                    std::string strId = rfidUser.getUserId();
                    if (!bAuthorizedDriver)
                    {
                        bAuthorizedDriver = true;
                        sendData(UserList.getTableType(), socketMap, deviceList, rfidUser.getCmdBeepSound());
                        strDriver = strId;
                        fuelRegister.addDriver(strDriver);
                        mainState.processDriverAuthorization(bAuthorizedUser);
                        bChangeDetected = true;
                    }
                    else
                    {
                        if (strId != strDriver)
                            bCancelTransaction = true;
                    }
                }
                //bAuthorizedUser = UserList.isValidID(rfidUser.getUserId());
                if (bChangeDetected)
                {
                    bChangeDetected = false;
                    keepAlive.resetTimer(tmrFuelTransaction);
                    if (bAuthorizedUser && bAuthorizedDriver)
                    {
                        sendData(UserList.getTableType(), socketMap, deviceList, rfidUser.getCmdLockReader());
                        bRfidReaderDisable = true;
                    }
                }
            }
            if (!bAuthorizedVehicle)
            {
                if (rfidBoquilla.isTagDetected())
                {
                    std::string strTag = rfidBoquilla.getTagId();
                    bAuthorizedVehicle = VehicleList.isValidID(strTag);
                    if (bAuthorizedVehicle)
                    {
                        strTagVehicle = strTag;
                        fuelRegister.addVehicle(strTagVehicle);
                        mainState.processVehicleAuthorization(bAuthorizedVehicle);
                        keepAlive.resetTimer(tmrFuelTransaction);
                        //tmrAuthorizedVehicle = keepAlive.addTimer(10000);
                        keepAlive.resetTimer(tmrAuthorizedVehicle);
                        //dAcumFlowOld = flowmeter.getAcumFlow();
                        //electroValve.openValve();
                    }
                }
            }
            else
            {
                if (rfidBoquilla.isTagDetected())
                {
                    if (strTagVehicle == rfidBoquilla.getTagId())
                    /*{
                        mainState.processLostVehicleTag();
                        bAuthorizedVehicle = false;
                        bCancelTransaction = true;
                    }
                    else*/
                        keepAlive.resetTimer(tmrAuthorizedVehicle);
                }
                /*else
                {*/
                if (keepAlive.isTimerEvent(tmrAuthorizedVehicle))
                {
                    keepAlive.stopTimer(tmrAuthorizedVehicle);
                    mainState.processLostVehicleTag();
                    bCancelTransaction = true;
                }
                //}
            }

            if (bAuthorizedVehicle && bAuthorizedDriver && bAuthorizedUser)
            {
                electroValve.openValve();
                bStateChange = true;
            }
        }
        /*else if (stateRbpi == MainState::RFIDUser)
        {
            bool bAuthorizedVehicle = false;
            if (rfidBoquilla.isTagDetected())
            {
                bAuthorizedVehicle = VehicleList.isValidID(rfidBoquilla.getTagId());
            }
            if (bAuthorizedVehicle)
            {
                fuelRegister.addVehicle(rfidBoquilla.getTagId());
                mainState.processVehicleAuthorization(bAuthorizedVehicle);
                keepAlive.resetTimer(tmrFuelTransaction);
                dAcumFlowOld = flowmeter.getAcumFlow();
                electroValve.openValve();
            }
        }*/
        /*else if (stateRbpi == MainState::startingTransaction)
        {
        }*/
        else if (stateRbpi == MainState::chargingFuel)
        {
            bStateChange = false;
            if (dAcumFlowOld != flowmeter.getAcumFlow())
            {
                keepAlive.resetTimer(tmrFuelTransaction);
                dAcumFlowOld = flowmeter.getAcumFlow();
            }
            if (!rfidBoquilla.isTagDetected() || strTagVehicle != rfidBoquilla.getTagId())
            {
                if (keepAlive.isTimerEvent(tmrAuthorizedVehicle))
                {
                    mainState.processLostVehicleTag();
                    electroValve.closeValve();
                    keepAlive.stopTimer(tmrAuthorizedVehicle);
                }
            }
            else
                keepAlive.resetTimer(tmrAuthorizedVehicle);
        }
        else if (stateRbpi == MainState::chargingPaused)
        {
            if (rfidBoquilla.isTagDetected())
            {
                if (strTagVehicle == rfidBoquilla.getTagId())
                {
                    mainState.processResumeFueling();
                    electroValve.openValve();
                }
                else
                {
                    electroValve.closeValve();
                    fuelRegister.finishTransaction(flowmeter.getAcumFlow());
                    mainState.processFinishFueling();
                }
            }
        }
        if (bCancelTransaction)
        {
            electroValve.closeValve();
            fuelRegister.finishTransaction(flowmeter.getAcumFlow());
            //fuelTransactionTimeOut();
            keepAlive.stopTimer(tmrFuelTransaction);
            mainState.processFuelingTimeOut();
            bCancelTransaction = false;
        }
        /*else if (stateRbpi == MainState::finishingTransaction)
        {
        }*/

        if (proccesNewConnection(socketServer, mainSettings, socketNewClientList))
        {
        }
        if (processDataNewClients(socketNewClientList, socketClientList, deviceList, onTheFlyDeviceList, socketMap, portList))
        {
            //bFirstTime = false;
            //tmrLaunchService
            keepAlive.resetTimer(tmrLaunchService);
            strDeviceLaunched = verifyDeviceService(deviceList, portList, bLaunchDisable);
            if (strDeviceLaunched == "")
            {
                keepAlive.stopTimer(tmrLaunchService);
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
                    bLaunchService = false;
                }
            }
            if (bLaunchService)
            {
                strDeviceLaunched = verifyDeviceService(deviceList, portList, bLaunchDisable);
                if (strDeviceLaunched == "")
                {
                    keepAlive.stopTimer(tmrLaunchService);
                }
            }
        }
        processDataClients(socketClientList, deviceList, keepAlive);
        verifyDeviceTimer(socketClientList, deviceList, keepAlive);

        if (keepAlive.isTimerEvent(mainTmr))
        {
            //sendAliveMessage(socketClientList, deviceList);
            std::string msg;
            msg = aliveMsg(deviceList, socketMap);
            globalLog << SCCRealTime::getTimeStamp() << std::endl;
            globalLog << msg << "\n\n";
            //globalLog << std::endl();
        }
        if (keepAlive.isTimerEvent(rqtTblTmr))
        {
            sendRequestTable(socketClientList, deviceList);
        }
        if (!restApi.getServicePID())
            bFirstTime = false;
        if (restApi.getServicePID() && (keepAlive.isTimerEvent(tmrConnectServer) || !bFirstTime))
        {
            bFirstTime          = true;
            bConnectToServer    = true;
            restApi.clearWaitingResponse();
            keepAlive.resetTimer(mainSettings.tmrServerRetry);
            fuelRegister.getRegisterList(restApi.getRegisterList());
            restApi.startConnection(fuelRegister.getMemberList());
        }
        if (bConnectToServer)
        {
            if (!restApi.isWaitingResponse())
            {
                keepAlive.resetTimer(tmrConnectRetry);
                std::string strBody;
                if (!restApi.isRegisterListEmpty())
                {
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
                    restApi.setWaitingResponse();
                }
                else if (!restApi.isTableListEmpty())
                {
                    restApi.getNextTableRequest(strBody);
                    auto itSck = socketMap.find(restApi.name());
                    if (itSck == socketMap.end())
                        break;
                    SCCCreateMessage sccPostMsg;
                    sccPostMsg.addParam(MSG_HEADER_TYPE, DEVICE_REST_SERVICE);
                    sccPostMsg.addParam(MSG_SERV_URL_HEADER, restApi.getUrlGetMethod(restApi.getCurrentTableIndex()));
                    sccPostMsg.addParam(MSG_SERV_METHOD_HEADER, MSG_SERV_METHOD_GET);
                    sccPostMsg.addParam(MSG_SERV_BODY_HEADER, strBody);
                    std::string msg = sccPostMsg.makeMessage();
                    CSocket* pSck = itSck->second;
                    pSck->sendData(msg);
                    restApi.setWaitingResponse();
                }
                else
                {
                    if (restApi.isTableBody())
                    {
                        VehicleList.writeTable(restApi.getBodyFromTable(0));
                        UserList.writeTable(restApi.getBodyFromTable(1));
                    }
                    bConnectToServer = false;
                    keepAlive.stopTimer(tmrConnectRetry);
                }
            }
            /*else
            {
            }*/
            if (keepAlive.isTimerEvent(tmrConnectRetry))
            {
                restApi.clearWaitingResponse();
            }
        }

        if (bSleep == true)
            usleep(10000);

        keepAlive.update();
    }

    keepAlive.stopTimer(mainTmr);

    return 0;
}

