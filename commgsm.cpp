#include "commgsm.h"

commGSM::commGSM(const std::string& deviceName, bool bShowdata) : Device(deviceName, bShowdata)
{

}

commGSM::~commGSM()
{

}

int commGSM::init(MainCtrlSettings& settings)
{
    return 0;
}

