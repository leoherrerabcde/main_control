#include "MainCtrlSettings.h"


std::unordered_map<std::string, ArgumentStruct> st_RFIDParam =
{
    {PARAM_COM_PORT,{PARAM_COM_PORT, Integer, 0}},
    {PARAM_BAUD_RATE, {PARAM_BAUD_RATE, Integer, 115200}},
    {PARAM_SERVICE_NAME, {PARAM_SERVICE_NAME, AlphaNumeric, "commPort"}},
    {PARAM_PATH_NAME, {PARAM_PATH_NAME, AlphaNumeric, "/home/pi/Documents/source/commPort/bin/Release"}}
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

bool MainCtrlSettings::getValue(const std::string& deviceName, const std::string& argName, int& val, const int& valDefault = 0)
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

bool MainCtrlSettings::getValue(const std::string& deviceName, const std::string& argName, std::string& val, const std::string& valDefault = 0)
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

