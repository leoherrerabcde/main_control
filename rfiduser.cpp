#include "rfiduser.h"
#include "SCCLog.h"
#include "SCCDeviceVars.h"


#include <iostream>


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
        }
        if (m_bShowData)
            std::cout << data << std::endl;
    }
    return true;
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

