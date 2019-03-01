#include "device.h"
#include "SCCLog.h"

#include <stdlib.h>
#include <sstream>
#include <cstring>

extern bool     gl_bVerbose;
extern SCCLog   globalLog;


Device::Device(const std::string& deviceName)
    : m_DeviceName(deviceName),
    m_pidService(0),
    m_bDeviceDetected(false),
    m_iAliveCounter(0),
    m_bLaunchingService(false)
{

}
int Device::launchService()
{
    if (!m_bLaunchingService)
    {
        m_bLaunchingService = true;
        return launchService(m_strServicePathName, m_strServiceArgs);
    }
    return 0;
}

int Device::launchService(const std::string& servicePathName, const std::string& args)
{
    std::stringstream ss;

    if (servicePathName == "" || args == "")
        return 0;

    ss << servicePathName << " " << args << " &";
    char cmdProgram[ss.str().length()+1];

    memcpy(cmdProgram, ss.str().c_str(), ss.str().length());
    cmdProgram[ss.str().length()] = 0;

    if (!system(NULL))
        return 0;

    if (gl_bVerbose)
        globalLog << "Launching " << cmdProgram << LF_CHAR;

    int status = system(cmdProgram);

    return status;
}

std::string Device::popFrontMessage()
{
    std::string msg;

    if (isBufferEmpty())
        return msg;

    std::string::size_type pos = m_strBuffer.find(FRAME_STOP_MARK);

    msg = m_strBuffer.substr(1, pos-1);
    m_strBuffer = m_strBuffer.substr(pos+1);
    return msg;
}

bool Device::getValueMessage(const std::string& msg, const std::string& valueName, bool& value)
{
    std::string strValue;
    bool res = getValueMessage(msg, valueName, strValue);
    if (!res)
        return false;
    if (strValue == "false")
        value = false;
    else if (strValue == "true")
        value = true;
    else
        return false;
    return true;
}

bool Device::getValueMessage(const std::string& msg, const std::string& valueName, int& value)
{
    std::string strValue;
    bool res = getValueMessage(msg, valueName, strValue);
    if (!res)
        return false;
    value = std::stoi(strValue);
    return true;
}

bool Device::getValueMessage(const std::string& msg, const std::string& valueName, std::string& value)
{
    std::string::size_type pos_ini = msg.find(valueName);

    if (pos_ini == std::string::npos)
        return false;

    pos_ini += valueName.length()+1;

    if (pos_ini >= msg.length())
        return false;

    std::string::size_type pos_end = msg.find(',', pos_ini + 1);

    if (pos_end == std::string::npos)
        pos_end = msg.length();

    if (pos_end <= pos_ini)
        return false;

    value = msg.substr(pos_ini, pos_end - pos_ini);

    return true;
}

bool Device::isBufferEmpty()
{
    std::string::size_type pos = m_strBuffer.find(FRAME_START_MARK);

    if (pos == std::string::npos)
    {
        m_strBuffer = "";
        return true;
    }

    if (pos > 0)
    {
        m_strBuffer = m_strBuffer.substr(pos);
    }

    pos = m_strBuffer.find(FRAME_STOP_MARK);

    if (pos == std::string::npos)
    {
        return true;
    }

    return false;
}

bool Device::processDataReceived(const std::string& msg)
{
    pushData(msg);

    bool bMsgDetected = false;

    while (!isBufferEmpty())
    {
        std::string data;
        data = popFrontMessage();
        if (!m_bDeviceDetected)
        {
            std::string strValue;
            bool res = getValueMessage(data, DEVICE_NAME, strValue);
            if (res)
            {
                setDeviceName(strValue);

                m_bDeviceDetected = true;
                bMsgDetected = true;
                res =  getValueMessage(data, SERVICE_PID, strValue);
                if (res)
                    m_pidService = std::atoi(strValue.c_str());
                break;
                //std::stringstream ss;
                globalLog << "Device Name: " << name() << std::endl;
                //SCCLog::print(ss.str());

            }
        }
        else
        {
            break;
        }
    }
    return bMsgDetected;
}

bool Device::isAliveMessage(const std::string& data)
{
    std::string strValue;
    bool res;

    res = getValueMessage(data, MSG_HEADER_TYPE, strValue);
    if (!res || strValue != MSG_SERV_ALIVE_HEADER)
        return false;

    res = getValueMessage(data, MSG_SERV_ALIVE_COUNT, strValue);
    if (!res)
        return false;

    if (m_iAliveCounter == std::stoi(strValue))
        setServiceAlive();

    return true;
}

bool Device::isFrameType(const std::string& header, const std::string& data)
{
    std::string strValue;
    bool res;

    res = getValueMessage(data, MSG_HEADER_TYPE, strValue);

    if (!res || strValue != header)
        return false;

    return true;
}

