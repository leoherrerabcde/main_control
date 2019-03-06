#ifndef RFIDUSER_H
#define RFIDUSER_H

#include "device.h"
#include "../rfidUser/SCCRfidUserProtocol.h"

class RFIDUser : public Device
{
public:
    RFIDUser(const std::string& deviceName = "", bool bShowdata = false);
    ~RFIDUser();

    virtual int init(MainCtrlSettings& settings);
    virtual bool processDataReceived(const std::string& msg);

    std::string getUserId() {return m_strCardSerialNum;};

    std::string getCmdBeepSound();
    std::string getCmdLockReader();

    bool isUserDetected() {return m_bCardDetected;}

private:

    std::string m_strCardSerialNum;
    bool        m_bCardDetected;

    int         m_iBeepDuration;
    int         m_iBeepTimes;
    int         m_iLockTimeout;
};

#endif // RFIDUSER_H
