#ifndef DEVICE_H
#define DEVICE_H

#include <string>

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
    Device();
    virtual ~Device() {};

    virtual int init() { return 0;};
    virtual bool is_data_received() {return false;};
    virtual void send_data(std::string& data, int len = 0) {};
    virtual void send_data(unsigned char* data, int len = 0) {};
    virtual int get_data(std::string& data) {return 0;};
    virtual int get_data(unsigned char** data) {return 0;};
    virtual int num_bytes_received() {return 0;};

    virtual DeviceResult processDataReceived() {return DeviceResult::AuthorizeID;};
};

#endif // DEVICE_H
