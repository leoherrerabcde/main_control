#include "device.h"

#include <stdlib.h>
#include <sstream>

Device::Device(const std::string& deviceName)
    : m_DeviceName(deviceName), m_bServiceConnected(false), m_bDeviceMatched(false)
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
    std::string msg;

    if (isBufferEmpty())
        return msg;

    std::string::size_type pos = m_strBuffer.find('}');

    msg = m_strBuffer.substr(1, pos-1);
    m_strBuffer = m_strBuffer.substr(pos+1);
    return msg;
}

bool Device::getValueMessage(const std::string& msg, const std::string& valueName, std::string& value)
{
    std::string::size_type pos_ini = m_strBuffer.find(valueName);

    if (pos_ini == std::string::npos)
        return false;

    pos_ini += valueName.length()+2;

    if (pos_ini >= msg.length())
        return false;

    std::string::size_type pos_end = m_strBuffer.find(',', pos_ini + 1);

    if (pos_end == std::string::npos)
        pos_end = m_strBuffer.length();

    if (pos_end <= pos_ini)
        return false;

    value = msg.substr(pos_ini, pos_end - pos_ini -1);

    return true;
}

bool Device::isBufferEmpty()
{
    std::string::size_type pos = m_strBuffer.find('{');

    if (pos == std::string::npos)
    {
        m_strBuffer = "";
        return true;
    }

    if (pos > 0)
    {
        m_strBuffer = m_strBuffer.substr(pos);
    }

    pos = m_strBuffer.find('}');

    if (pos == std::string::npos)
    {
        return true;
    }

    return false;
}

