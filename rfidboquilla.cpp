#include "rfidboquilla.h"
#include "SCCLog.h"

RFIDBoquilla::RFIDBoquilla()
{

}

RFIDBoquilla::~RFIDBoquilla()
{

}

int RFIDBoquilla::init()
{
    SCCLog::print("Initializating RFID Nozzle Receiver...");
    idVehicleList.init();

    SCCLog::print("RFID Nozlle ready to use.");
    return 0;
}

bool RFIDBoquilla::isRFIDReceived()
{
    return false;
}

DeviceResult RFIDBoquilla::processDataReceived()
{
    return DeviceResult::DeviceIdle;
}

