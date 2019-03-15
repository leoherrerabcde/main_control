#include "SCCRemoteServer.h"
#include "SCCLog.h"
#include "SCCDeviceParams.h"
#include "SCCFileManager.h"
#include "../jsonParser/JsonParser.h"


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
    settings.getValue(m_DeviceName,PARAM_NUM_REGIST_SENT, m_iNumRegisterSent);

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

void SCCRemoteServer::startConnection(const std::list<std::string>& strMemberList)
{
    //SCCFileManager::getFileList(m_strRegisterPath, m_RegisterList);
    m_TableList.clear();
    m_MemberList.clear ();
    //m_MemberList.insert(strMemberList.begin(), strMemberList.end());
    for (auto member: strMemberList)
    {
        m_MemberList.push_back(member);
    }
    for (auto table: st_TableList)
    {
        m_TableList.push_back(table);
    }
}

bool SCCRemoteServer::getNextTableRequest(std::string& strBody)
{
    if (!m_TableList.size())
    {
        strBody = m_TableList.front();
        return true;
    }
    return false;
}

bool SCCRemoteServer::getNextRegisterRequest(std::string& strBody)
{
    if (!m_RegisterList.size())
    {
        std::list<std::string> registerToSent;
        size_t pos = m_iNumRegisterSent;
        if (m_RegisterList.size() < pos)
            pos = m_RegisterList.size();
        auto itLast = m_RegisterList.begin();
        std::advance(itLast, pos);
        registerToSent.insert(registerToSent.begin(), m_RegisterList.begin(), itLast);
        return JsonParser::getPlaneText(registerToSent, m_MemberList, strBody);
    }
    return false;
}

bool SCCRemoteServer::processDataReceived(const std::string& msg)
{
    pushData(msg);

    while (!isBufferEmpty())
    {
        std::string data;
        data = popFrontMessage();

        if (isAliveMessage(data))
            continue;
        if (!isFrameType(DEVICE_REST_SERVICE, data))
            continue;

        std::string strMethod;
        std::string strBody;

        bool res = true;
        res = res && getValueMessage(data, MSG_SERV_METHOD_HEADER       , strMethod);
        res = res && getValueMessage(data, MSG_SERV_BODY_HEADER         , strBody);

        if (res)
        {
            clearWaitingResponse();
            if (strMethod == MSG_SERV_METHOD_POST)
            {
                removeRegisters();
            }
            else if (strMethod == MSG_SERV_METHOD_GET)
            {
                m_TableList.erase(m_TableList.begin());
            }
        }
        if (m_bShowData)
            std::cout << data << std::endl;
    }
    return true;
}

void SCCRemoteServer::removeRegisters()
{
    if (m_iNumRegisterSent > m_RegisterList.size())
        m_RegisterList.clear();
    else
    {
        auto itLast = m_RegisterList.begin();
        std::advance(itLast, m_iNumRegisterSent);
        m_RegisterList.erase(m_RegisterList.begin(), itLast);
    }
}


