#include "rfiduser.h"
#include "SCCLog.h"

RFIDUser::RFIDUser()
{

}

RFIDUser::~RFIDUser()
{

}

int RFIDUser::init()
{
    SCCLog::print("Initializating RFID User...");

    SCCLog::print("RFID User ready to use.");
    return 0;
}

DeviceResult RFIDUser::processDataReceived()
{
    return DeviceResult::DeviceIdle;
}


