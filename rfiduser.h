#ifndef RFIDUSER_H
#define RFIDUSER_H

#include "device.h"

class RFIDUser : public Device
{
public:
    RFIDUser();
    ~RFIDUser();

    virtual int init();
};

#endif // RFIDUSER_H
