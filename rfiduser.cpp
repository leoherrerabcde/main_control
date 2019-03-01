#include "rfiduser.h"
#include "SCCLog.h"

extern SCCLog globalLog;

RFIDUser::RFIDUser(const std::string& deviceName) : Device(deviceName)
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

DeviceResult RFIDUser::processDataReceived()
{
    return DeviceResult::DeviceIdle;
}

std::string RFIDUser::getUserId()
{
    std::string strUserId;

    return strUserId;
}

