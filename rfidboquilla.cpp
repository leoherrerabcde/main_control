#include "rfidboquilla.h"

RFIDBoquilla::RFIDBoquilla()
{

}

RFIDBoquilla::~RFIDBoquilla()
{

}

int RFIDBoquilla::init()
{
    idVehicleList.init();

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

