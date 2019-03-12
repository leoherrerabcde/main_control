#ifndef RFIDBOQUILLA_H
#define RFIDBOQUILLA_H

#include "device.h"
//#include "IDVehicleList.h"

class RFIDBoquilla : public Device
{
public:
    RFIDBoquilla(const std::string& deviceName = "", bool bShowdata = false);
    ~RFIDBoquilla();

    virtual int init(MainCtrlSettings& settings);
    bool isRFIDReceived();

    //IDVehicleList idVehicleList;

    //virtual DeviceResult processDataReceived();
    virtual bool processDataReceived(const std::string& msg = "");

    std::string getTagId() {return m_strTag;}

    bool isTagDetected() {return m_bTagDetected;}


private:

    bool            m_bBatteryAlarm;
    bool            m_bFail;
    bool            m_bNozzleActived;
    bool            m_bTagDetected;

    std::string     m_strTag;

};

#endif // RFIDBOQUILLA_H
