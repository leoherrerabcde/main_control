#include "electrovalvctrl.h"
#include "SCCLog.h"

ElectroValvCtrl::ElectroValvCtrl()
{

}

ElectroValvCtrl::~ElectroValvCtrl()
{

}

int ElectroValvCtrl::init(const MainCtrlSettings& settings)
{
    SCCLog::print("Initializating Electro Valve...");

    SCCLog::print("Electro Valve ready to use.");

    return 0;
}

