#include <iostream>
#include <unistd.h>
#include <unordered_map>

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


int main(int argc, char* argv[])
{
    std::unordered_map<std::string,Device*> deviceList;

    RFIDBoquilla rfidBoquilla;
    RFIDUser rfidUser;
    ElectroValvCtrl electroValv;
    Device guiApp;

    commGSM modCommGSM;
    LucesEstado lucesDeEstado;
    MainState mainState;
    MainCtrlSettings mainSettings;

    CSocket socketServer;
    std::vector<CSocket*> socketClientList;
    socketServer.setLocalPort(mainSettings.serverPort);
    socketServer.listen();

    rfidBoquilla.init();
    rfidUser.init();
    electroValv.init();
    modCommGSM.init();
    lucesDeEstado.init();

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
            DeviceResult processResult = rfidBoquilla.processDataReceived();
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
        if (socketServer.getState() == sckHostResolved)
        {
            CSocket* newClient = socketServer.getSocket();
            newClient->setBufferSize(mainSettings.sckBufferSize);
            socketClientList.push_back(newClient);
            socketServer.listen();
        }

        for (auto itSck :socketClientList)
        {
            if (itSck->getIDClient() == "")
            {
                Device pDvc;
                if (pDvc.processDataReceived(itSck->getData()))
                    itSck->setIDClient(pDvc.name());
            }
            else
            {
                auto it = deviceList.find(itSck->getIDClient());
                if (it != deviceList.end())
                {
                    Device* pDevice = it->second;
                    pDevice->processDataReceived(itSck->getData());
                }
            }
        }

        if (bSleep == true)
            usleep(50);
        keepAlive.update();
    }

    keepAlive.stopTimer(mainTmr);

    return 0;
}
