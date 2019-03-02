#include "electrovalvctrl.h"
#include "SCCLog.h"

extern SCCLog globalLog;

ElectroValvCtrl::ElectroValvCtrl(const std::string& deviceName, bool bShowdata) : Device(deviceName, bShowdata)
{

}

ElectroValvCtrl::~ElectroValvCtrl()
{

}

int ElectroValvCtrl::init(MainCtrlSettings& settings)
{
    globalLog << "Initializating Electro Valve..." << std::endl;

    globalLog << "Electro Valve ready to use." << std::endl;

    return 0;
}

