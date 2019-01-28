#include <iostream>

using namespace std;

#include "lucesestado.h"
#include "commgsm.h"
#include "electrovalvctrl.h"
#include "rfidboquilla.h"
#include "rfiduser.h"
#include "mainstate.h"
#include "IDVehicleList.h"

int main(int argc, char* argv[])
{
    //cout << "Hello world!" << endl;
    RFIDBoquilla rfidBoquilla;
    RFIDUser rfidUser;
    ElectroValvCtrl electroValv;
    commGSM modCommGSM;
    LucesEstado lucesDeEstado;
    MainState mainState;

    rfidBoquilla.init();
    rfidUser.init();
    electroValv.init();
    modCommGSM.init();
    lucesDeEstado.init();

    MainState::State stateRbpi;

    stateRbpi = mainState.getLastState();

    for(;;)
    {
        if (stateRbpi == MainState::waitForInitTransaction)
        {
            DeviceResult processResult = rfidBoquilla.processDataReceived();
            if (processResult == DeviceResult::DeviceIdle)
            {
            }
            else if (processResult == DeviceResult::IncompletedReceive)
            {
            }
            else if (processResult == DeviceResult::NonAuthorizeID)
            {
            }
            else if (processResult == DeviceResult::AuthorizeID)
            {
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
    }

    return 0;
}
