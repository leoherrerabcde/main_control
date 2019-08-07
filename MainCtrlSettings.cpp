#include "MainCtrlSettings.h"

/*ArgumentStruct stComPort  = {PARAM_COM_PORT, Required, Integer, true, "0"};
ArgumentStruct stBaudRate = {PARAM_BAUD_RATE, Required, Integer, true, "115200"};
ArgumentStruct stServName = {PARAM_SERVICE_NAME, Required, AlphaNumeric, true, "commPort"};
ArgumentStruct stPathName = {PARAM_PATH_NAME, Required, AlphaNumeric, true, "/home/pi/Documents/source/commPort/bin/Release"};*/

//#define URL_HOST        "http://157.230.225.248"
#define URL_HOST        "192.168.43.161"

std::unordered_map<std::string, ArgumentStruct> st_RFIDParam =
{
    {PARAM_COM_PORT,{PARAM_COM_PORT, Required, Integer, true, "2"}},
    {PARAM_BAUD_RATE, {PARAM_BAUD_RATE, Required, Integer, true, "115200"}},
    {PARAM_SERVICE_NAME, {PARAM_SERVICE_NAME, Required, AlphaNumeric, true, "commPort"}},
    //{PARAM_PATH_NAME, {PARAM_PATH_NAME, Required, AlphaNumeric, true, "/home/pi/Documents/source/commPort/bin/Release"}}
    {PARAM_PATH_NAME, {PARAM_PATH_NAME, Required, AlphaNumeric, true, "/home/pi/paltech/petrosure/petrobox/bin/Release"}}
};

std::unordered_map<std::string, ArgumentStruct> st_RFIDUserParam =
{
    {PARAM_COM_PORT,{PARAM_COM_PORT, Required, Integer, true, "0"}},
    {PARAM_BAUD_RATE, {PARAM_BAUD_RATE, Required, Integer, true, "9600"}},
    {PARAM_SERVICE_NAME, {PARAM_SERVICE_NAME, Required, AlphaNumeric, true, "rfidUser"}},
    //{PARAM_PATH_NAME, {PARAM_PATH_NAME, Required, AlphaNumeric, true, "/home/pi/Documents/source/rfidUser/bin/Release"}},
    {PARAM_PATH_NAME, {PARAM_PATH_NAME, Required, AlphaNumeric, true, "/home/pi/paltech/petrosure/petrobox/bin/Release"}},
    {PARAM_TIME_OUT, {PARAM_TIME_OUT, Required, Integer, true, "120000"}}

};

std::unordered_map<std::string, ArgumentStruct> st_FlowmeterParam =
{
    {PARAM_COM_PORT,{PARAM_COM_PORT, Required, Integer, true, "1"}},
    {PARAM_BAUD_RATE, {PARAM_BAUD_RATE, Required, Integer, true, "9600"}},
    {PARAM_SERVICE_NAME, {PARAM_SERVICE_NAME, Required, AlphaNumeric, true, "ovalgearFlowm"}},
    //{PARAM_PATH_NAME, {PARAM_PATH_NAME, Required, AlphaNumeric, true, "/home/pi/Documents/source/flowmCom/bin/Release"}}
    //{PARAM_PATH_NAME, {PARAM_PATH_NAME, Required, AlphaNumeric, true, "/home/pi/Documents/source/ovalgearFlowm/bin/Release"}}
    {PARAM_PATH_NAME, {PARAM_PATH_NAME, Required, AlphaNumeric, true, "/home/pi/paltech/petrosure/petrobox/bin/Release"}}
};

std::unordered_map<std::string, ArgumentStruct> st_RestApiParam =
{
    {PARAM_GET_MODIFIED,{PARAM_GET_MODIFIED, Required, AlphaNumeric, true, "api/modificadas"}},
    {PARAM_GET_TABLE_VEHI,{PARAM_GET_TABLE_VEHI, Required, AlphaNumeric, true, "api/listavehiculos/0"}},
    {PARAM_GET_TABLE_USER,{PARAM_GET_TABLE_USER, Required, AlphaNumeric, true, "api/personal/0"}},
    {PARAM_POST_CONFIRM,{PARAM_POST_CONFIRM, Required, AlphaNumeric, true, "api/modificadas/cierre"}},
    {PARAM_POST_REGISTS,{PARAM_POST_REGISTS, Required, AlphaNumeric, true, "api/control"}},
    {PARAM_URL_API_REST,{PARAM_URL_API_REST, Required, AlphaNumeric, true, "http://157.230.225.248:3000"}},
    {PARAM_SERVICE_NAME,{PARAM_SERVICE_NAME, Required, AlphaNumeric, true, "remote_server.py"}},
    //{PARAM_PATH_NAME   ,{PARAM_PATH_NAME   , Required, AlphaNumeric, true, "/home/pi/Documents/source/python/remoteServer"}},
    {PARAM_PATH_NAME   ,{PARAM_PATH_NAME   , Required, AlphaNumeric, true, "/home/pi/paltech/petrosure/petrobox/bin/Release/python/remoteServer"}},
    {PARAM_NUM_REGIST_SENT,{PARAM_NUM_REGIST_SENT, Required, Integer, true, "100"}},
};

std::unordered_map<std::string, ArgumentStruct> st_TableUserParam =
{
    {PARAM_SERVICE_NAME,{PARAM_SERVICE_NAME, Required, AlphaNumeric, true, "users.json"}},
    //{PARAM_PATH_NAME   ,{PARAM_PATH_NAME   , Required, AlphaNumeric, true, "/home/pi/Documents/source/main_control/tables"}},
    {PARAM_PATH_NAME   ,{PARAM_PATH_NAME   , Required, AlphaNumeric, true, "/home/pi/paltech/petrosure/petrobox/bin/Release/tables"}},
    {PARAM_TBL_ID_KEY  ,{PARAM_TBL_ID_KEY  , Required, AlphaNumeric, true, "idpersonal"}},
    {PARAM_TBL_VALUE_KEY,{PARAM_TBL_VALUE_KEY  , Optional, AlphaNumeric, true, "tipousuario"}},
    //{PARAM_PATH_ERROR  ,{PARAM_PATH_ERROR  , Required, AlphaNumeric, true, "/home/pi/Documents/source/main_control/errors"}},
    {PARAM_PATH_ERROR  ,{PARAM_PATH_ERROR  , Required, AlphaNumeric, true, "/home/pi/paltech/petrosure/petrobox/bin/Release/errors"}},
    //{PARAM_TBL_ID_KEY  ,{PARAM_TBL_ID_KEY  , Required, AlphaNumeric, true, "id_user"}},
};

std::unordered_map<std::string, ArgumentStruct> st_TableVehicleParam =
{
    {PARAM_SERVICE_NAME,{PARAM_SERVICE_NAME, Required, AlphaNumeric, true, "vehicle.json"}},
    //{PARAM_PATH_NAME   ,{PARAM_PATH_NAME   , Required, AlphaNumeric, true, "/home/pi/Documents/source/main_control/tables"}},
    {PARAM_PATH_NAME   ,{PARAM_PATH_NAME   , Required, AlphaNumeric, true, "/home/pi/paltech/petrosure/petrobox/bin/Release/tables"}},
    {PARAM_TBL_ID_KEY  ,{PARAM_TBL_ID_KEY  , Required, AlphaNumeric, true, "NRFID"}},
    //{PARAM_PATH_ERROR  ,{PARAM_PATH_ERROR  , Required, AlphaNumeric, true, "/home/pi/Documents/source/main_control/errors"}},
    {PARAM_PATH_ERROR  ,{PARAM_PATH_ERROR  , Required, AlphaNumeric, true, "/home/pi/paltech/petrosure/petrobox/bin/Release/errors"}},
    //{PARAM_TBL_ID_KEY  ,{PARAM_TBL_ID_KEY  , Required, AlphaNumeric, true, "id_vehiculo"}},
};

std::unordered_map<std::string, ArgumentStruct> st_FuelRegisterParam =
{
    {PARAM_FILE_EXTENSION,{PARAM_FILE_EXTENSION, Required, AlphaNumeric, true, ".txt"}},
    {PARAM_SERVICE_NAME,{PARAM_SERVICE_NAME, Required, AlphaNumeric, true, "fuelRegister"}},
    //{PARAM_PATH_NAME   ,{PARAM_PATH_NAME   , Required, AlphaNumeric, true, "/home/pi/Documents/source/main_control/registers"}},
    {PARAM_PATH_NAME   ,{PARAM_PATH_NAME   , Required, AlphaNumeric, true, "/home/pi/paltech/petrosure/petrobox/bin/Release/registers"}},
    {PARAM_NEW_REGS_PATH,{PARAM_NEW_REGS_PATH, Required, AlphaNumeric, true, "new_registers"}},
    {PARAM_HISTO_REGS_PATH,{PARAM_HISTO_REGS_PATH, Required, AlphaNumeric, true, "histo_registers"}},
    {PARAM_UPPER_REG_NUM,{PARAM_UPPER_REG_NUM, Required, Integer, true, "999999"}},
    {PARAM_CONSE_NUM_LENGTH,{PARAM_CONSE_NUM_LENGTH, Required, Integer, true, "6"}},

};


std::unordered_map<std::string, std::unordered_map<std::string, ArgumentStruct>*> st_DeviceParam =
{
    {DEVICE_RFID_BOQUILLA, &st_RFIDParam},
    {DEVICE_RFID_BOMBERO , &st_RFIDUserParam},
    {DEVICE_OVALGEARFLOWM, &st_FlowmeterParam},
    {DEVICE_REST_SERVICE , &st_RestApiParam},
    {TABLE_USERS         , &st_TableUserParam},
    {TABLE_VEHICLES      , &st_TableVehicleParam},
    {TABLE_REGISTERS     , &st_FuelRegisterParam},
};

MainCtrlSettings::MainCtrlSettings()
{
    //ctor
    serverPort              = 4000;
    noResponseTimeMilli     = 7000;
    mainTimerInterval       = 15000;
    sckBufferSize           = 4096;
    requestTableTmrInterval = 60000;
    tmrServerConnect        = 45000; //300000;//*/(8*3600*1000);
    tmrServerRetry          = 15000; //30000;//(15*60*1000);
    tmrDeviceTimeOut        = 30000;
    tmrLaunchService        = 5000;
    nMaxServerRetry         = 3;
    m_strMyName             = "CamionSurtidor001";
    m_strIdDispensador      = "01";
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

