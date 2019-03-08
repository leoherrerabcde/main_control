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

    std::unordered_map<std::string,Device*> deviceList;
    std::unordered_map<std::string,CSocket*> socketMap;
    std::vector<Device*> onTheFlyDeviceList;

    SCCFuelTransaction fuelRegister(TABLE_REGISTERS);
    SCCFlowmeter flowmeter(DEVICE_FLOWMETER, bShowData);
    RFIDBoquilla rfidBoquilla(DEVICE_RFID_BOQUILLA, bShowData);
    RFIDUser rfidUser(DEVICE_RFID_BOMBERO, bShowData);
    ElectroValvCtrl electroValve(DEVICE_ELECTRO_VALVE, bShowData);
    Device guiApp(DEVICE_GUI, bShowData);
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

    pDvc = &guiApp;
    deviceList.insert(std::make_pair(pDvc->name(), pDvc));

    pDvc = &restApi;
    deviceList.insert(std::make_pair(pDvc->name(), pDvc));

    pDvc = &flowmeter;
    deviceList.insert(std::make_pair(pDvc->name(), pDvc));

    MainState::State stateRbpi;

    stateRbpi = mainState.getLastState();

    SCCAlive keepAlive;
    keepAlive.throwDisable();
    keepAlive.start(mainSettings.noResponseTimeMilli);

    int mainTmr = keepAlive.addTimer(mainSettings.mainTimerInterval);
    int rqtTblTmr = keepAlive.addTimer(mainSettings.requestTableTmrInterval);

    flowmeter.init(mainSettings);
    rfidBoquilla.init(mainSettings);
    rfidUser.init(mainSettings);
    electroValve.init(mainSettings);
    restApi.init(mainSettings);
    modCommGSM.init(mainSettings);
    lucesDeEstado.init(mainSettings);

    UserList.init(mainSettings);
    VehicleList.init(mainSettings);

    fuelRegister.init(mainSettings);
    int tmrFuelTransaction = 0;
    //int tmrServiceLaunched = 0;

    double dAcumFlowOld = 0.0;
    std::string strTagVehicle;

    globalLog << "Main Loop Started." << std::endl;

    socketServer.listen();

    for(;;)
    {
        bool bSleep = true;
        if (tmrFuelTransaction)
        {
            if (keepAlive.isTimerEvent(tmrFuelTransaction))
            {
                electroValve.closeValve();
                fuelRegister.finishTransaction(flowmeter.getAcumFlow());
                //fuelTransactionTimeOut();
                keepAlive.stopTimer(tmrFuelTransaction);
                tmrFuelTransaction = 0;
                mainState.processFuelingTimeOut();
            }
        }
        stateRbpi = mainState.getCurrentState();
        if (stateRbpi == MainState::State::waitForInitTransaction)
        {
            bool bAuthorizedUser        = false;
            bool bAuthorizedVehicle     = false;
            //bool bInitFuelTransaction   = false;
            if (rfidUser.isUserDetected())
            {
                bAuthorizedUser = UserList.isValidID(rfidUser.getUserId());
                if (bAuthorizedUser)
                {
                    sendData(UserList.getTableType(), socketMap, rfidUser.getCmdBeepSound());
                    sendData(UserList.getTableType(), socketMap, rfidUser.getCmdLockReader());
                }
            }
            if (rfidBoquilla.isTagDetected())
            {
                bAuthorizedVehicle = VehicleList.isValidID(rfidBoquilla.getTagId());
            }
            if (bAuthorizedUser || bAuthorizedVehicle)
            {
                dAcumFlowOld = flowmeter.getAcumFlow();
                fuelRegister.initTransaction(flowmeter.getAcumFlow());
                if (bAuthorizedUser)
                    fuelRegister.addUser(rfidUser.getUserId());
                if (bAuthorizedVehicle)
                {
                    fuelRegister.addVehicle(rfidBoquilla.getTagId());
                    strTagVehicle = rfidBoquilla.getTagId();
                }
                fuelRegister.addFlowMeterBegin(flowmeter.getAcumFlow());
                tmrFuelTransaction = keepAlive.addTimer(30000);
                mainState.processUserAuthorization(bAuthorizedUser);
                mainState.processVehicleAuthorization(bAuthorizedVehicle);
            }
        }
        else if (stateRbpi == MainState::State::RFIDBoquilla)
        {
            bool bAuthorizedUser = false;
            if (rfidUser.isUserDetected())
            {
                bAuthorizedUser = UserList.isValidID(rfidUser.getUserId());
            }
            if (bAuthorizedUser)
            {
                sendData(UserList.getTableType(), socketMap, rfidUser.getCmdBeepSound());
                sendData(UserList.getTableType(), socketMap, rfidUser.getCmdLockReader());
                fuelRegister.addUser(rfidUser.getUserId());
                mainState.processUserAuthorization(bAuthorizedUser);
                keepAlive.resetTimer(tmrFuelTransaction);
                dAcumFlowOld = flowmeter.getAcumFlow();
                electroValve.openValve();
            }
        }
        else if (stateRbpi == MainState::RFIDUser)
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
        }
        /*else if (stateRbpi == MainState::startingTransaction)
        {
        }*/
        else if (stateRbpi == MainState::chargingFuel)
        {
            if (dAcumFlowOld != flowmeter.getAcumFlow())
            {
                keepAlive.resetTimer(tmrFuelTransaction);
                dAcumFlowOld = flowmeter.getAcumFlow();
            }
            if (!rfidBoquilla.isTagDetected() || strTagVehicle != rfidBoquilla.getTagId())
            {
                mainState.processLostVehicleTag();
                electroValve.closeValve();
            }
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
        /*else if (stateRbpi == MainState::finishingTransaction)
        {
        }*/

        if (proccesNewConnection(socketServer, mainSettings, socketNewClientList))
        {
            /*if (tmrServiceLaunched)
            {
                if (verifyDeviceService(deviceList))
                    keepAlive.resetTimer(tmrServiceLaunched);
            }
        }
        else
        {
            if (tmrServiceLaunched)
            {
                if (keepAlive.isTimerEvent(tmrServiceLaunched))
                {
                    if (verifyDeviceService(deviceList))
                        keepAlive.resetTimer(tmrServiceLaunched);
                    else
                        keepAlive.stopTimer(tmrServiceLaunched);
                }
            }
            else
            {
                if (deviceList.size())
                {
                    if (verifyDeviceService(deviceList))
                        tmrServiceLaunched = keepAlive.addTimer(10000);
                }
            }*/
        }
        processDataNewClients(socketNewClientList, socketClientList, deviceList, onTheFlyDeviceList, socketMap);
        processDataClients(socketClientList, deviceList);

        if (keepAlive.isTimerEvent(mainTmr))
        {
            //sendAliveMessage(socketClientList, deviceList);
            //verifyDeviceService(deviceList);
        }
        if (keepAlive.isTimerEvent(rqtTblTmr))
        {
            sendRequestTable(socketClientList, deviceList);
        }

        if (bSleep == true)
            usleep(1000);

        keepAlive.update();
    }

    keepAlive.stopTimer(mainTmr);

    return 0;
}

