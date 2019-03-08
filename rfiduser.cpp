#include "rfiduser.h"
#include "SCCLog.h"
#include "SCCDeviceVars.h"
#include "SCCDisplay.h"

#include <iostream>

extern SCCDisplay glDisplay;
extern SCCLog globalLog;

RFIDUser::RFIDUser(const std::string& deviceName, bool bShowdata) : Device(deviceName, bShowdata), m_bCardDetected(false)
{

}

RFIDUser::~RFIDUser()
{

}

int RFIDUser::init(MainCtrlSettings& settings)
{
    globalLog << "Initializating RFID User..." << std::endl;

    globalLog << "RFID User ready to use." << std::endl;

    std::stringstream sService;
    std::string pathName;
    std::string serviceName;
    const std::string paramPathName(PARAM_PATH_NAME);
    const std::string paramServiceName(PARAM_SERVICE_NAME);

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

    setServicePath(strService);
    setServiceArgs(strArgs);
    //int ret = launchService(strService, strArgs);
    //int ret = 0;

    //SCCLog::print("RFID Nozzle Initiated.");
    settings.getValue(m_DeviceName, PARAM_BEEP_DURATION, m_iBeepDuration, 100);
    settings.getValue(m_DeviceName, PARAM_BEEP_TIMES, comPort, m_iBeepTimes);
    settings.getValue(m_DeviceName, PARAM_TIME_OUT, comPort, m_iLockTimeOut);

    m_bLaunchingService = true;
    return 0;
}

bool RFIDUser::processDataReceived(const std::string& msg)
{
    pushData(msg);

    while (!isBufferEmpty())
    {
        std::string data;
        data = popFrontMessage();

        if (isAliveMessage(data))
            continue;
        if (!isFrameType(DEVICE_RFID_BOMBERO, data))
            continue;
        std::string strValue;
        std::string strTag;

        std::string strCardSerialNum;
        std::string strCardDetected;

        bool res = getValueMessage(data, VAR_CARD_DETECTED, strCardDetected);
        res = res && getValueMessage(data, VAR_CARD_SERIALNUM, strCardSerialNum);

        if (res)
        {
            if (strCardDetected == "false")
            {
                m_bCardDetected = false;
            }
            else
            {
                m_bCardDetected = true;
            }
            m_strCardSerialNum = strCardSerialNum;
            setServiceAlive();
            /*glDisplay.printLine(VAR_CARD_DETECTED, strCardDetected);
            glDisplay.printLine(VAR_CARD_SERIALNUM, strCardSerialNum);*/
        }
        if (m_bShowData)
            std::cout << data << std::endl;
    }
    return true;
}

void RFIDUser::displayDevice()
{
    /*glDisplay.printLine("Device", m_DeviceName);
    glDisplay.printLine(VAR_CARD_DETECTED, "");
    glDisplay.printLine(VAR_CARD_SERIALNUM, "");*/
}

/*std::string RFIDUser::getUserId()
{
    std::string strUserId;

    return strUserId;
}*/

std::string RFIDUser::getCmdBeepSound()
{
    std::stringstream ss;

    ss << FRAME_START_MARK ;

    ss << MSG_HEADER_TYPE << ASSIGN_CHAR << DEVICE_RFID_BOMBERO;

    ss << SEPARATOR_CHAR << MSG_COMMAND_TYPE 		<< ASSIGN_CHAR << CMD_RFIDUSER_BEEPSOUND;
    ss << SEPARATOR_CHAR << PARAM_BEEP_DURATION 	<< ASSIGN_CHAR << m_iBeepDuration;
    ss << SEPARATOR_CHAR << PARAM_BEEP_TIMES 	    << ASSIGN_CHAR << m_iBeepTimes;

    ss << FRAME_STOP_MARK;

    return std::string(ss.str());
}

std::string RFIDUser::getCmdLockReader()
{
    std::stringstream ss;

    ss << FRAME_START_MARK ;

    ss << MSG_HEADER_TYPE << ASSIGN_CHAR << DEVICE_RFID_BOMBERO;

    ss << SEPARATOR_CHAR << MSG_COMMAND_TYPE 		<< ASSIGN_CHAR << CMD_RFIDUSER_LOCKREADER;
    ss << SEPARATOR_CHAR << PARAM_TIME_OUT 		    << ASSIGN_CHAR << m_iLockTimeOut;

    ss << FRAME_STOP_MARK;

    return std::string(ss.str());
}
