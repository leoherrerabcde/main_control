#include <iostream>
#include <unistd.h>

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
    RFIDBoquilla rfidBoquilla;
    RFIDUser rfidUser;
    ElectroValvCtrl electroValv;
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
            /*if (rfidBoquilla.isRFIDReceived() > 0)
            {
                std::string rfidReceived;
                rfidBoquilla.get_data(rfidReceived);
                if (idVehicleList.isValidID(rfidBoquilla))
                {
                    stateRbpi |= MainState::RFIDBoquilla;
                    lucesDeEstado.updateState();
                }
            }
            else
            {
                if (rfidUser.num_bytes_received() > 0)
                {

                }
            }*/
        }
        if (socketServer.getState() == sckHostResolved)
        {
            socketClientList.push_back(socketServer.getSocket());
            socketServer.listen();
        }
        if (bSleep == true)
            usleep(50);
        keepAlive.update();
    }

    keepAlive.stopTimer(mainTmr);

    return 0;
}
