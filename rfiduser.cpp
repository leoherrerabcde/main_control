#include "rfiduser.h"
#include "SCCLog.h"

RFIDUser::RFIDUser(const std::string& deviceName)
{

}

RFIDUser::~RFIDUser()
{

}

int RFIDUser::init(MainCtrlSettings& settings)
{
    SCCLog::print("Initializating RFID User...");

    SCCLog::print("RFID User ready to use.");
    return 0;
}

DeviceResult RFIDUser::processDataReceived()
{
    return DeviceResult::DeviceIdle;
}


