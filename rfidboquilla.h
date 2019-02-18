#ifndef RFIDBOQUILLA_H
#define RFIDBOQUILLA_H

#include "device.h"
#include "IDVehicleList.h"

class RFIDBoquilla : public Device
{
public:
    RFIDBoquilla();
    ~RFIDBoquilla();

    virtual int init(const MainCtrlSettings& settings);
    bool isRFIDReceived();

    IDVehicleList idVehicleList;

    //virtual DeviceResult processDataReceived();
    virtual bool processDataReceived(const std::string& msg = "");

};

#endif // RFIDBOQUILLA_H
