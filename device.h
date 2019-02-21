#ifndef DEVICE_H
#define DEVICE_H

#include <string>

#include "MainCtrlSettings.h"
#include "SCCLog.h"
//#include "SCCDeviceNames.h"

extern SCCLog globalLog;

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
    //Device(const Device&) = default;
    virtual ~Device() {};

    virtual int init(MainCtrlSettings& settings) { return 0;};
    virtual bool is_data_received() {return false;};
    virtual void send_data(std::string& data, int len = 0) {};
    virtual void send_data(unsigned char* data, int len = 0) {};
    virtual int get_data(std::string& data) {return 0;};
    virtual int get_data(unsigned char** data) {return 0;};
    virtual int num_bytes_received() {return 0;};

    virtual bool processDataReceived(const std::string& msg = "");

    virtual void setDeviceName(const std::string& strName) {m_DeviceName = strName;}
    virtual std::string name() {return m_DeviceName;}

    virtual bool getValueMessage(const std::string& msg, const std::string& valueName, std::string& value);
    virtual bool getValueMessage(const std::string& msg, const std::string& valueName, int& value);
    virtual bool getValueMessage(const std::string& msg, const std::string& valueName, bool& value);
    virtual std::string getData() {std::string msg(m_strBuffer); m_strBuffer = ""; return msg;}

    virtual int getServicePID() {return m_pidService;}
    virtual void setServicePID(int pid) {m_pidService = pid;}

    virtual void addData(const std::string msg) {m_strBuffer += msg;}

    void setServicePath(const std::string& path) {m_strServicePathName = path;}
    std::string getServicePath() const {return m_strServicePathName;}
    void setServiceArgs(const std::string& args) {m_strServiceArgs = args;}
    std::string getServiceArgs() const {return m_strServiceArgs;}

    int launchService();

    int getAliveCounter() const {return m_iAliveCounter;}
    int incAliveCounter() {return ++m_iAliveCounter;}
    bool isServiceAlive() {bool res(m_bServiceAlive); m_bServiceAlive=false;return res;}
    void setServiceAlive() {m_bServiceAlive = false;}

protected:

    virtual int launchService(const std::string& servicePathName, const std::string& args);
    virtual void pushData(const std::string& msg) {m_strBuffer += msg;}
    virtual std::string popFrontMessage();
    virtual bool isBufferEmpty();

    std::string m_DeviceName;
    std::string m_strBuffer;

    int         m_pidService;
    bool        m_bDeviceDetected;
    //bool        m_bDeviceMatched;
    std::string m_strServicePathName;
    std::string m_strServiceArgs;

    int         m_iAliveCounter;
    bool        m_bServiceAlive;
};

#endif // DEVICE_H
