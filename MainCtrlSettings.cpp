#include "MainCtrlSettings.h"

/*ArgumentStruct stComPort  = {PARAM_COM_PORT, Required, Integer, true, "0"};
ArgumentStruct stBaudRate = {PARAM_BAUD_RATE, Required, Integer, true, "115200"};
ArgumentStruct stServName = {PARAM_SERVICE_NAME, Required, AlphaNumeric, true, "commPort"};
ArgumentStruct stPathName = {PARAM_PATH_NAME, Required, AlphaNumeric, true, "/home/pi/Documents/source/commPort/bin/Release"};*/

std::unordered_map<std::string, ArgumentStruct> st_RFIDParam =
{
    {PARAM_COM_PORT,{PARAM_COM_PORT, Required, Integer, true, "0"}},
    {PARAM_BAUD_RATE, {PARAM_BAUD_RATE, Required, Integer, true, "115200"}},
    {PARAM_SERVICE_NAME, {PARAM_SERVICE_NAME, Required, AlphaNumeric, true, "commPort"}},
    {PARAM_PATH_NAME, {PARAM_PATH_NAME, Required, AlphaNumeric, true, "/home/pi/Documents/source/commPort/bin/Release"}}
};

std::unordered_map<std::string, std::unordered_map<std::string, ArgumentStruct>*> st_DeviceParam =
{
    {DEVICE_RFID_BOQUILLA, &st_RFIDParam},
};

MainCtrlSettings::MainCtrlSettings()
{
    //ctor
    serverPort = 4000;
    noResponseTimeMilli = 5000;
    mainTimerInterval = 15000;
    sckBufferSize = 256;
}

MainCtrlSettings::~MainCtrlSettings()
{
    //dtor
}

bool MainCtrlSettings::getValue(const std::string& deviceName, const std::string& argName, int& val, const int& valDefault)
{
    auto itDevice = st_DeviceParam.find(deviceName);

    if (itDevice == st_DeviceParam.end())
        return false;

    std::unordered_map<std::string, ArgumentStruct>& deviceParam = *itDevice->second;

    auto itArg = deviceParam.find(argName);

    if (itArg == deviceParam.end())
        return false;

    ArgumentStruct& argSt = itArg->second;

    return argSt.getValue(val, valDefault);
}

bool MainCtrlSettings::getValue(const std::string& deviceName, const std::string& argName, std::string& val, const std::string& valDefault)
{
    auto itDevice = st_DeviceParam.find(deviceName);

    if (itDevice == st_DeviceParam.end())
        return false;

    std::unordered_map<std::string, ArgumentStruct>& deviceParam = *itDevice->second;

    auto itArg = deviceParam.find(argName);

    if (itArg == deviceParam.end())
        return false;

    ArgumentStruct& argSt = itArg->second;

    return argSt.getValue(val, valDefault);
}

