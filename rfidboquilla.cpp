#include "rfidboquilla.h"
#include "SCCLog.h"

#include <sstream>

RFIDBoquilla::RFIDBoquilla(const std::string& deviceName)
{

}

RFIDBoquilla::~RFIDBoquilla()
{

}

int RFIDBoquilla::init(MainCtrlSettings& settings)
{
    SCCLog::print("Initializating RFID Nozzle Receiver...");
    idVehicleList.init();

    /* Launch Service*/
    std::stringstream sService;
    std::string pathName;
    std::string serviceName;

    settings.getValue(m_DeviceName,PARAM_PATH_NAME,pathName);
    settings.getValue(m_DeviceName,PARAM_SERVICE_NAME,serviceName);

    sService << pathName << "/" << serviceName;

    std::stringstream sArgs;

    int comPort, baudRate, remotePort;

    settings.getValue(m_DeviceName, PARAM_COM_PORT, comPort, 0);
    settings.getValue(m_DeviceName, PARAM_BAUD_RATE, baudRate, 0);
    remotePort = settings.serverPort;

    sArgs << comPort << " " << baudRate << " " << remotePort;

    std::string strService(sService.str());
    std::string strArgs(sArgs.str());

    int ret = launchService(strService, strArgs);

    SCCLog::print("RFID Nozlle ready to use.");

    return ret;
}

bool RFIDBoquilla::isRFIDReceived()
{
    return false;
}

/*DeviceResult RFIDBoquilla::processDataReceived()
{
    return DeviceResult::DeviceIdle;
}*/

bool RFIDBoquilla::processDataReceived(const std::string& msg)
{
    pushData(msg);

    while (!isBufferEmpty())
    {
        std::string data;
        data = popFrontMessage();
        if (!m_bDeviceMatched)
        {
            std::string strValue;
            bool res = getValueMessage(data, DEVICE_NAME, strValue);
            if (res && name() == strValue)
            {
                m_bDeviceMatched = true;
                res =  getValueMessage(data, SERVICE_PID, strValue);
                if (res)
                    m_pidService = std::atoi(strValue.c_str());
            }
        }
        else
        {
        }
    }
    return true;
}

