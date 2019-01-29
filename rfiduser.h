#ifndef RFIDUSER_H
#define RFIDUSER_H

#include "device.h"

class RFIDUser : public Device
{
public:
    RFIDUser();
    ~RFIDUser();

    virtual int init();
    virtual DeviceResult processDataReceived();
};

#endif // RFIDUSER_H
