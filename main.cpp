#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <list>

using namespace std;

#include "lucesestado.h"
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
#include "SCCFuelTransaction.h"

SCCLog  globalLog(std::cout);
bool    gl_bVerbose(true);

void verifyDeviceService(std::unordered_map<std::string,Device*> & dvcList);
void proccesNewConnection(CSocket& sckServer, MainCtrlSettings& settings, std::list<CSocket*>& socketList);
void processDataNewClients(std::list<CSocket*>& socketNewList,
                       std::list<CSocket*>& socketList,
                       std::unordered_map<std::string,Device*>& dvcList,
                       std::vector<Device*>& onTheFlyDvcList);

void processDataClients(std::list<CSocket*>& socketList,
                        std::unordered_map<std::string,Device*>& dvcList);
void sendAliveMessage(std::list<CSocket*>& socketList,
                        std::unordered_map<std::string,Device*>& dvcList);
void sendRequestTable(std::list<CSocket*>& socketList,
                        std::unordered_map<std::string,Device*>& dvcList);

int main(int argc, char* argv[])
{
    bool bShowData(false);

    std::unordered_map<std::string,Device*> deviceList;
    std::vector<Device*> onTheFlyDeviceList;

    SCCFuelTransaction fuelRegister(TABLE_REGISTERS);
    SCCFlowmeter flowmeter(DEVICE_FLOWMETER, bShowData);
    RFIDBoquilla rfidBoquilla(DEVICE_RFID_BOQUILLA, bShowData);
    RFIDUser rfidUser(DEVICE_RFID_BOMBERO, bShowData);
    ElectroValvCtrl electroValv(DEVICE_ELECTRO_VALVE, bShowData);
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

    pDvc = &electroValv;
    deviceList.insert(std::make_pair(pDvc->name(), pDvc));

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
    electroValv.init(mainSettings);
    restApi.init(mainSettings);
    modCommGSM.init(mainSettings);
    lucesDeEstado.init(mainSettings);

    UserList.init(mainSettings);
    VehicleList.init(mainSettings);

    fuelRegister.init(mainSettings);
    int tmrFuelTransaction = 0;

    std::cout << "Main Loop Started." << std::endl;

    socketServer.listen();

    for(;;)
    {
        bool bSleep = true;
        stateRbpi = mainState.getCurrentState();
        if (stateRbpi == MainState::State::waitForInitTransaction)
        {
            bool bAuthorizedUser = false;
            bool bAuthorizedVehicle = false;
            if (rfidUser.isUserDetected())
            {
                bAuthorizedUser = UserList.isValidID(rfidUser.getUserId());
            }
            if (rfidBoquilla.isTagDetected())
            {
                bAuthorizedVehicle = VehicleList.isValidID(rfidBoquilla.getTagId());
            }
            mainState.processUserAuthorization(bAuthorizedUser);
            mainState.processVehicleAuthorization(bAuthorizedVehicle);
        }
        else if (stateRbpi == MainState::State::RFIDBoquilla)
        {
            fuelRegister.initTransaction(flowmeter.getAcumFlow());
            fuelRegister.addUser(rfidUser.getUserId());
            tmrFuelTransaction = keepAlive.addTimer(150000);
            bool bAuthorizedVehicle = false;
            if (rfidBoquilla.isTagDetected())
            {
                bAuthorizedVehicle = VehicleList.isValidID(rfidBoquilla.getTagId());
            }
            mainState.processVehicleAuthorization(bAuthorizedVehicle);
        }
        else if (stateRbpi == MainState::RFIDUser)
        {
        }
        else if (stateRbpi == MainState::startingTransaction)
        {
        }
        else if (stateRbpi == MainState::chargingFuel)
        {
        }
        else if (stateRbpi == MainState::chargingPaused)
        {
        }
        else if (stateRbpi == MainState::finishingTransaction)
        {
        }

        proccesNewConnection(socketServer, mainSettings, socketNewClientList);
        processDataNewClients(socketNewClientList, socketClientList, deviceList, onTheFlyDeviceList);
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

void proccesNewConnection(CSocket& sckServer, MainCtrlSettings& settings, std::list<CSocket*>& socketList)
{
    if (sckServer.getState() == sckHostResolved)
    {
        CSocket* newClient = sckServer.getSocket();
        newClient->setBufferSize(settings.sckBufferSize);
        newClient->runRcvLoop();
        socketList.push_back(newClient);
        sckServer.listen();
    }

}

void processDataNewClients(std::list<CSocket*>& socketNewList,
                       std::list<CSocket*>& socketList,
                       std::unordered_map<std::string,Device*>& dvcList,
                       std::vector<Device*>& onTheFlyDvcList)
{
    for (auto itSck = socketNewList.begin(); itSck != socketNewList.end();)
    {
        Device pDvc;
        if (pDvc.processDataReceived((*itSck)->getData()))
        {
            if (pDvc.name() != "")
            {
                (*itSck)->setIDClient(pDvc.name());
                auto it = dvcList.find((*itSck)->getIDClient());
                if (it != dvcList.end())
                {
                    Device* pDevice = it->second;
                    pDevice->setServicePID(pDvc.getServicePID());
                    pDevice->addData(pDvc.getData());
                }
                else
                {
                    Device* pDevice = new Device(pDvc);
                    dvcList.insert(std::make_pair(pDevice->name(), pDevice));
                    onTheFlyDvcList.push_back(pDevice);
                }
                //std::stringstream ss;
                globalLog << "New Device connected: " << pDvc.name() << std::endl;
                //SCCLog::print(ss.str());
                socketList.push_back(*itSck);
                itSck = socketNewList.erase(itSck);
                continue;
            }
        }
        ++itSck;
    }
}

void processDataClients(std::list<CSocket*>& sockeList,
                        std::unordered_map<std::string,Device*>& dvcList)
{
    for (auto itSck : sockeList)
    {
        auto it = dvcList.find(itSck->getIDClient());
        if (it != dvcList.end())
        {
            Device* pDevice = it->second;
            std::string msg = itSck->getData();
            if (msg != "")
            {
                if (gl_bVerbose)
                    std::cout << pDevice->name() << ": " << msg << std::endl;
                pDevice->processDataReceived(msg);
            }
            /*else
                pDevice->processDataReceived(itSck->getData());*/
        }
    }
}

void sendAliveMessage(std::list<CSocket*>& socketList,
                        std::unordered_map<std::string,Device*>& dvcList)
{
    for (auto it = socketList.begin(); it != socketList.end();)
    {
        CSocket* itSck = *it;
        auto itDvc = dvcList.find(itSck->getIDClient());
        if (itDvc != dvcList.end())
        {
            Device* pDevice = itDvc->second;
            if (!pDevice->isServiceAlive())
            {
                SCCCreateMessage sccAliveMsg;
                sccAliveMsg.addParam(MSG_HEADER_TYPE, MSG_SERV_ALIVE_HEADER);
                sccAliveMsg.addParam(MSG_SERV_ALIVE_COUNT, std::to_string(pDevice->incAliveCounter()));
                std::string msg = sccAliveMsg.makeMessage();
                if (!itSck->isConnected())
                {
                    it = socketList.erase(it);
                    pDevice->disconnect();
                    continue;
                }
                if (!itSck->sendData(msg))
                {
                    it = socketList.erase(it);
                    pDevice->disconnect();
                    continue;
                }
            }
            else
            {
                /* Socket connected but no device detected */
                itSck->disconnect();
                it = socketList.erase(it);
                continue;
            }
        }
        ++it;
    }
}


void verifyDeviceService(std::unordered_map<std::string,Device*> & dvcList)
{
    for (auto itDvc : dvcList)
    {
        Device* pDvc = itDvc.second;
        if (pDvc->getServicePID() == 0)
            pDvc->launchService();
    }
}

void sendRequestTable(std::list<CSocket*>& socketList,
                        std::unordered_map<std::string,Device*>& dvcList)
{
}

