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
    std::string getCmdUnLockReader();

    bool isUserDetected() {return m_bCardDetected;}

    virtual void displayDevice();

private:

    std::string m_strCardSerialNum;
    bool        m_bCardDetected;

    int         m_iBeepDuration;
    int         m_iBeepTimes;
    int         m_iLockTimeOut;
};

#endif // RFIDUSER_H
