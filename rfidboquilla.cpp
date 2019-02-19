#include "rfidboquilla.h"
#include "SCCLog.h"
#include "../commPort/SCCWirelessRcvrProtocol.h"

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
    const std::string paramPathName(PARAM_PATH_NAME);
    const std::string paramServiceName(PARAM_PATH_NAME);

    settings.getValue(m_DeviceName,paramPathName,pathName);
    settings.getValue(m_DeviceName,paramServiceName,serviceName);

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

        std::string strValue;
        std::string strTag;

        bool bBatAlarm(m_bBatteryAlarm);
        bool bFail(m_bFail);
        bool bNozzleDetected(m_bNozzleActived);
        bool bTagDetected(m_bTagDetected);

        bool res = getValueMessage(data, VAR_BATTERY_ALARM, bBatAlarm);
        res = res && getValueMessage(data, VAR_FAIL_STATUS, bFail);
        res = res && getValueMessage(data, VAR_NOZZLE_ACTIVED, bNozzleDetected);
        res = res && getValueMessage(data, VAR_TAG_DETECTED, strTag);

        if (res)
        {
            if (strTag == "false")
            {
                strTag = "";
                bTagDetected = false;
            }
        }
    }
    return true;
}

