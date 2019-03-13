#include "SCCRemoteServer.h"
#include "SCCLog.h"
#include "SCCDeviceParams.h"

#include <sstream>

extern SCCLog globalLog;

static std::list<std::string> st_TableList =
{
    PARAM_TABLE_BOMBEROS,
    PARAM_TABLE_CONDUCTORES,
    PARAM_TABLE_PROPIETARIOS,
    PARAM_TABLE_VEHICULOS,
};

SCCRemoteServer::SCCRemoteServer(const std::string& deviceName, bool bShowData) : Device(deviceName, bShowData), m_bWaitResponse(false)
{
    //ctor
}

SCCRemoteServer::~SCCRemoteServer()
{
    //dtor
}

int SCCRemoteServer::init(MainCtrlSettings& settings)
{
    globalLog << "Initializating Api Rest Service..." << std::endl;

    /* Launch Service*/

    settings.getValue(m_DeviceName,PARAM_GET_MODIFIED, m_getModifiedUrl);
    settings.getValue(m_DeviceName,PARAM_GET_TABLE   , m_getTableUrl);
    settings.getValue(m_DeviceName,PARAM_POST_CONFIRM, m_postConfirmUrl);
    settings.getValue(m_DeviceName,PARAM_POST_REGISTS, m_postRegister);
    settings.getValue(m_DeviceName,PARAM_URL_API_REST, m_urlApiRest);

    std::stringstream sService;

    std::string pathName;
    std::string serviceName;
    const std::string paramPathName(PARAM_PATH_NAME);
    const std::string paramServiceName(PARAM_SERVICE_NAME);

    settings.getValue(m_DeviceName,paramPathName,pathName);
    settings.getValue(m_DeviceName,paramServiceName,serviceName);

    sService << "python " << pathName << "/" << serviceName;

    std::stringstream sArgs;

    int remotePort;

    remotePort = settings.serverPort;

    sArgs << " --remote_port " << remotePort;


    std::string strService(sService.str());
    std::string strArgs(sArgs.str());

    setServicePath(strService);
    setServiceArgs(strArgs);
    //int ret = launchService(strService, strArgs);
    //int ret = 0;

    //SCCLog::print("RFID Nozzle Initiated.");
    m_bLaunchingService = true;
    return 0;
}

void SCCRemoteServer::startConnection()
{
    SCCFileManager::getFileList(m_strRegisterPath, m_RegisterList);
    m_TableList.clear();
    for (auto table: st_TableList)
    {
        m_TableList.push_back(table);
    }
}

std::string SCCRemoteServer::getNextTableRequest()
{
}

std::string SCCRemoteServer::getNextRegisterRequest()
{
    if (m_RegisterList.size())
    {

    }
}
