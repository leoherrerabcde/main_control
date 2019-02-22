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
#include "IDVehicleList.h"
#include "MainCtrlSettings.h"
#include "CSocket.h"
#include "SCCLog.h"
#include "SCCAlive.h"
#include "SCCRemoteServer.h"
#include "SCCDeviceNames.h"
#include "SCCCreateMessage.h"

SCCLog globalLog(std::cout);

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

int main(int argc, char* argv[])
{

    std::unordered_map<std::string,Device*> deviceList;
    std::vector<Device*> onTheFlyDeviceList;

    RFIDBoquilla rfidBoquilla(DEVICE_RFID_BOQUILLA);
    RFIDUser rfidUser(DEVICE_RFID_BOMBERO);
    ElectroValvCtrl electroValv(DEVICE_ELECTRO_VALVE);
    Device guiApp(DEVICE_GUI);
    SCCRemoteServer restApi(DEVICE_REST_SERVICE);

    commGSM modCommGSM(DEVICE_SERVER);
    LucesEstado lucesDeEstado(DEVICE_STATUS_LIGHTS);

    MainState mainState;
    MainCtrlSettings mainSettings;

    CSocket socketServer;
    std::list<CSocket*> socketClientList;
    std::list<CSocket*> socketNewClientList;
    socketServer.setLocalPort(mainSettings.serverPort);
    socketServer.listen();

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

    MainState::State stateRbpi;

    stateRbpi = mainState.getLastState();

    SCCAlive keepAlive;
    keepAlive.throwDisable();
    keepAlive.start(mainSettings.noResponseTimeMilli);
    int mainTmr = keepAlive.addTimer(mainSettings.mainTimerInterval);

    rfidBoquilla.init(mainSettings);
    rfidUser.init(mainSettings);
    electroValv.init(mainSettings);
    restApi.init(mainSettings);
    modCommGSM.init(mainSettings);
    lucesDeEstado.init(mainSettings);

    std::cout << "Main Loop Started." << std::endl;

    for(;;)
    {
        bool bSleep = true;
        //verifyDeviceService(deviceList);
        if (stateRbpi == MainState::waitForInitTransaction)
        {
            DeviceResult processResult = DeviceResult::IncompletedReceive;
            if (processResult == DeviceResult::DeviceIdle)
            {
            }
            else if (processResult == DeviceResult::IncompletedReceive)
            {
                bSleep = false;
            }
            else if (processResult == DeviceResult::NonAuthorizeID)
            {
                bSleep = false;
            }
            else if (processResult == DeviceResult::AuthorizeID)
            {
                bSleep = false;
            }
        }

        proccesNewConnection(socketServer, mainSettings, socketNewClientList);
        processDataNewClients(socketNewClientList, socketClientList, deviceList, onTheFlyDeviceList);
        processDataClients(socketClientList, deviceList);

        if (keepAlive.isTimerEvent(mainTmr))
            sendAliveMessage(socketClientList, deviceList);

        if (bSleep == true)
            usleep(50);

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
            pDevice->processDataReceived(itSck->getData());
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
                sccAliveMsg.addParam(MSG_SERV_ALIVE_HEADER, MSG_SERV_ALIVE_HEADER);
                sccAliveMsg.addParam(MSG_SERV_ALIVE_COUNT, std::to_string(pDevice->incAliveCounter()));
                std::string msg = sccAliveMsg.makeMessage();
                if (!itSck->sendData(msg))
                    it = socketList.erase(it);
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
