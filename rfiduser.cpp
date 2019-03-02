#include "rfiduser.h"
#include "SCCLog.h"

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
            if (strCardSerialNum == "false")
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

