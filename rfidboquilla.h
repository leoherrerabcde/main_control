#ifndef RFIDBOQUILLA_H
#define RFIDBOQUILLA_H

#include "device.h"
#include "IDVehicleList.h"

class RFIDBoquilla : public Device
{
public:
    RFIDBoquilla();
    ~RFIDBoquilla();

    virtual int init();
    bool isRFIDReceived();

    IDVehicleList idVehicleList;

};

#endif // RFIDBOQUILLA_H
