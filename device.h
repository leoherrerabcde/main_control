#ifndef DEVICE_H
#define DEVICE_H

#include <string>

#include "MainCtrlSettings.h"
//#include "SCCDeviceNames.h"

enum DeviceResult
{
    DeviceIdle = 0,
    IncompletedReceive,
    NonAuthorizeID,
    AuthorizeID
};

class Device
{
public:
    Device(const std::string& deviceName = "");
    virtual ~Device() {};

    virtual int init(const MainCtrlSettings& settings) { return 0;};
    virtual bool is_data_received() {return false;};
    virtual void send_data(std::string& data, int len = 0) {};
    virtual void send_data(unsigned char* data, int len = 0) {};
    virtual int get_data(std::string& data) {return 0;};
    virtual int get_data(unsigned char** data) {return 0;};
    virtual int num_bytes_received() {return 0;};

    virtual bool processDataReceived(const std::string& msg = "") {m_strBuffer += msg; return false;}

    virtual void setDeviceName() {m_DeviceName = "";}
    virtual std::string name() {return m_DeviceName;}

protected:

    virtual int launchService(const std::string& servicePathName, const std::string& args;)
    virtual void pushData(const std::string& msg) {m_strBuffer += msg;}
    virtual std::string popFrontMessage();
    virtual bool isBufferEmpty();

    std::string m_DeviceName;
    std::string m_strBuffer;

    int         m_pidService;
};

#endif // DEVICE_H
