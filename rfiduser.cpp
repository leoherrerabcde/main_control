#include "rfiduser.h"

RFIDUser::RFIDUser()
{

}

RFIDUser::~RFIDUser()
{

}

int RFIDUser::init()
{

    return 0;
}

DeviceResult RFIDUser::processDataReceived()
{
    return DeviceResult::DeviceIdle;
}


