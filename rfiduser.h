#ifndef RFIDUSER_H
#define RFIDUSER_H

#include "device.h"

class RFIDUser : public Device
{
public:
    RFIDUser(const std::string& deviceName = "");
    ~RFIDUser();

    virtual int init(MainCtrlSettings& settings);
    virtual DeviceResult processDataReceived();

    std::string getUserId();
};

#endif // RFIDUSER_H
