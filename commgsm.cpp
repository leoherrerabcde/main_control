#include "commgsm.h"

commGSM::commGSM(const std::string& deviceName) : Device(deviceName)
{

}

commGSM::~commGSM()
{

}

int commGSM::init(MainCtrlSettings& settings)
{
    return 0;
}

