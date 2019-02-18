#include "electrovalvctrl.h"
#include "SCCLog.h"

ElectroValvCtrl::ElectroValvCtrl(const std::string& deviceName)
{

}

ElectroValvCtrl::~ElectroValvCtrl()
{

}

int ElectroValvCtrl::init(MainCtrlSettings& settings)
{
    SCCLog::print("Initializating Electro Valve...");

    SCCLog::print("Electro Valve ready to use.");

    return 0;
}

