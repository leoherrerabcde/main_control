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
#include "SCCDeviceNames.h"

void proccesNewConnection(CSocket& sckServer, MainCtrlSettings& settings, std::list<CSocket*>& socketList);
void processDataNewClients(std::list<CSocket*>& socketNewList,
                       std::list<CSocket*>& socketList,
                       std::unordered_map<std::string,Device*>& dvcList,
                       std::vector<Device*>& onTheFlyDvcList);

void processDataClients(std::list<CSocket*>& socketList,
                        std::unordered_map<std::string,Device*>& dvcList);

int main(int argc, char* argv[])
{
    std::unordered_map<std::string,Device*> deviceList;
    std::vector<Device*> onTheFlyDeviceList;

    RFIDBoquilla rfidBoquilla(DEVICE_RFID_BOQUILLA);
    RFIDUser rfidUser(DEVICE_RFID_BOMBERO);
    ElectroValvCtrl electroValv(DEVICE_ELECTRO_VALVE);
    Device guiApp(DEVICE_GUI);

    commGSM modCommGSM(DEVICE_SERVER);
    LucesEstado lucesDeEstado(DEVICE_STATUS_LIGHTS);

    MainState mainState;
    MainCtrlSettings mainSettings;

    CSocket socketServer;
    std::list<CSocket*> socketClientList;
    std::list<CSocket*> socketNewClientList;
    socketServer.setLocalPort(mainSettings.serverPort);
    socketServer.listen();

    rfidBoquilla.init(mainSettings);
    rfidUser.init(mainSettings);
    electroValv.init(mainSettings);
    modCommGSM.init(mainSettings);
    lucesDeEstado.init(mainSettings);

    Device* pDvc;

    pDvc = &rfidBoquilla;
    deviceList.insert(std::make_pair(pDvc->name(), pDvc));

    pDvc = &rfidUser;
    deviceList.insert(std::make_pair(pDvc->name(), pDvc));

    pDvc = &electroValv;
    deviceList.insert(std::make_pair(pDvc->name(), pDvc));

    pDvc = &guiApp;
    deviceList.insert(std::make_pair(pDvc->name(), pDvc));


    MainState::State stateRbpi;

    stateRbpi = mainState.getLastState();

    SCCAlive keepAlive;
    keepAlive.start(mainSettings.noResponseTimeMilli);
    int mainTmr = keepAlive.addTimer(mainSettings.mainTimerInterval);

    for(;;)
    {
        bool bSleep = true;

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
        socketList.push_back(newClient);
        sckServer.listen();
    }

}

void processDataNewClients(std::list<CSocket*>& socketNewList,
                       std::list<CSocket*>& socketList,
                       std::unordered_map<std::string,Device*>& dvcList,
                       std::vector<Device*>& onTheFlyDvcList)
{
    for (auto itSck = socketNewList.begin(); itSck != socketNewList.end(); ++itSck)
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
                socketList.push_back(*itSck);
                socketNewList.erase(itSck);
            }
        }
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
