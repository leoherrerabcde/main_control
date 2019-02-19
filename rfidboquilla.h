#ifndef RFIDBOQUILLA_H
#define RFIDBOQUILLA_H

#include "device.h"
#include "IDVehicleList.h"

class RFIDBoquilla : public Device
{
public:
    RFIDBoquilla(const std::string& deviceName = "");
    ~RFIDBoquilla();

    virtual int init(MainCtrlSettings& settings);
    bool isRFIDReceived();

    IDVehicleList idVehicleList;

    //virtual DeviceResult processDataReceived();
    virtual bool processDataReceived(const std::string& msg = "");

private:

    bool            m_bBatteryAlarm;
    bool            m_bFail;
    bool            m_bNozzleActived;
    bool            m_bTagDetected;

    std::string     m_strTag;

};

#endif // RFIDBOQUILLA_H
