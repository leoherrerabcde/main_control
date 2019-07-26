#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include <list>


#include "MainCtrlSettings.h"
#include "SCCLog.h"
#include "SCCDeviceNames.h"

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
    Device(const std::string& deviceName = "", bool bShowdata = false);
    //Device(const Device&) = default;
    virtual ~Device() {};

    virtual int init(MainCtrlSettings& settings);
    virtual bool is_data_received() {return false;};
    virtual void send_data(std::string& data, int len = 0) {};
    virtual void send_data(unsigned char* data, int len = 0) {};
    virtual int get_data(std::string& data) {return 0;};
    virtual int get_data(unsigned char** data) {return 0;};
    virtual int num_bytes_received() {return 0;};

    virtual bool processDataReceived(const std::string& msg = "");

    virtual void setDeviceName(const std::string& strName)
    {
    if (strName == "")
        globalLog << "HERE" << std::endl;
    m_DeviceName = strName;
    }
    virtual std::string name() {return m_DeviceName;}

    virtual bool getBodyMessage (const std::string& msg, const std::string& valueName, std::string& value);
    virtual bool getValueMessage(const std::string& msg, const std::string& valueName, std::string& value);
    virtual bool getValueMessage(const std::string& msg, const std::string& valueName, double& value);
    virtual bool getValueMessage(const std::string& msg, const std::string& valueName, float& value);
    virtual bool getValueMessage(const std::string& msg, const std::string& valueName, long& value);
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
    virtual int launchService(std::list<int>& portList);

    int getAliveCounter() const {return m_iAliveCounter;}
    int incAliveCounter() {return ++m_iAliveCounter;}
    bool isServiceAlive() {bool res(m_bServiceAlive); m_bServiceAlive=false;return res;}
    void setServiceAlive() {m_bServiceAlive = true;}

    int getTimerHandler() {return m_TimerHnd;}
    void setTimerHandler(const int tmr) {m_TimerHnd=tmr;}

    void disconnect();

    bool isAliveMessage(const std::string& data);
    bool isFrameType(const std::string& header, const std::string& data);

    void setShowData() {m_bShowData = true;}
    virtual void displayDevice() {};
    static void getComPortList(std::list<int>& portList);
    static void removeComPort(std::list<int>& portList, int port);
    int getComPort() {return m_iComPort;}
    static int launchService(const std::string& servicePathName, const std::string& args);
    bool killService();
    std::string popenQuickService(const std::string& quickService, const std::list<std::string>argList = std::list<std::string>());
    bool searchPIDService(std::list<int>& pidList);
    bool isServiceRunning();

    void setState(int nState) {m_iState = nState;}
    int getState() {return m_iState;}
    void nextState() {++m_iState;}
    void resetState(int nState = 0) {m_iState = nState;}

protected:

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
    std::string m_strServiceName;
    std::string m_strServiceCmd;

    int         m_iAliveCounter;
    bool        m_bServiceAlive;
    bool        m_bLaunchingService;
    bool        m_bServiceLaunched;

    bool        m_bShowData;
    int         m_iComPort;
    int         m_iBaudRate;
    int         m_iRemotePort;
    int         m_TimerHnd;
    int         m_iState;
};

#endif // DEVICE_H
