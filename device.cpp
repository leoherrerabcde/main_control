#include "device.h"

#include <stdlib.h>

Device::Device(const std::string& deviceName)
    : m_DeviceName(deviceName)
{

}

int Device::launchService(const std::string& servicePathName, const std::string& args)
{
    std::stringstream ss;

    ss << servicePathName << " " <<args;

    int status = system(ss.str().c_str());

    return status;
}

std::string Device::popFrontMessage()
{
}

bool Device::isBufferEmpty()
{
    std::string::pos pos = m_strBuffer.find('{');
}

