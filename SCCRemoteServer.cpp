#include "SCCRemoteServer.h"
#include "SCCLog.h"
#include "SCCDeviceParams.h"
#include "SCCFileManager.h"
#include "../jsonParser/JsonParser.h"


#include <sstream>


extern SCCLog globalLog;

static std::list<std::string> st_TableList =
{
    PARAM_TABLE_VEHICULOS,
    PARAM_TABLE_BOMBEROS,
    //PARAM_TABLE_CONDUCTORES,
    //PARAM_TABLE_PROPIETARIOS,
};

SCCRemoteServer::SCCRemoteServer(const std::string& deviceName, bool bShowData) : Device(deviceName, bShowData), m_bWaitResponse(false), m_TableIndex(0), m_bRetryError(false)
{
    //ctor
}

SCCRemoteServer::~SCCRemoteServer()
{
    //dtor
}

std::string SCCRemoteServer::getUrlPostMethod()
{
    std::string url(m_urlApiRest);

    url += "/";
    url += m_postRegister;
    return url;
}

std::string SCCRemoteServer::getUrlGetMethod(int index)
{
    std::string url(m_urlApiRest);

    url += "/";
    url += m_getTableUrl[index];
    return url;
}

int SCCRemoteServer::init(MainCtrlSettings& settings)
{
    globalLog << "Initializating Api Rest Service..." << std::endl;

    /* Launch Service*/

    std::string strUrl;
    settings.getValue(m_DeviceName,PARAM_GET_MODIFIED, m_getModifiedUrl);
    settings.getValue(m_DeviceName,PARAM_GET_TABLE_VEHI, strUrl);
    m_getTableUrl.push_back(strUrl);
    settings.getValue(m_DeviceName,PARAM_GET_TABLE_USER, strUrl);
    m_getTableUrl.push_back(strUrl);
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

    m_strServiceName = serviceName;

    sService << "python " << pathName << "/" << serviceName;

    std::stringstream sArgs;

    int remotePort;

    remotePort      = settings.serverPort;
    m_strIDDevice   = settings.m_strMyName;

    sArgs << " --remote_port " << remotePort;


    std::string strService(sService.str());
    std::string strArgs(sArgs.str());

    setServicePath(strService);
    setServiceArgs(strArgs);
    //int ret = launchService(strService, strArgs);
    //int ret = 0;

    //SCCLog::print("RFID Nozzle Initiated.");
    m_bLaunchingService = true;
    killService();
    return 0;
}

void SCCRemoteServer::startConnection(const std::list<std::string>& strMemberList)
{
    //SCCFileManager::getFileList(m_strRegisterPath, m_RegisterList);
    m_TableList.clear();
    m_MemberList.clear ();
    m_TableBody.clear();
    m_TableIndex = 0;
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
    if (m_TableList.size())
    {
        strBody = m_TableList.front();
        //++m_TableIndex;
        return true;
    }
    return false;
}

bool SCCRemoteServer::getNextRegisterRequest(std::string& strBody)
{
    if (m_RegisterList.size())
    {
        std::list<std::string> registerToSent;
        size_t pos = m_iNumRegisterSent;
        if (m_RegisterList.size() < pos)
            pos = m_RegisterList.size();
        auto itLast = m_RegisterList.begin();
        std::advance(itLast, pos);
        registerToSent.insert(registerToSent.begin(), m_RegisterList.begin(), itLast);
        return JsonParser::getPlaneText(PARAM_ID_SURTIDOR, m_strIDDevice, registerToSent, m_MemberList, strBody);
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
        res = res && getBodyMessage(data, MSG_SERV_BODY_HEADER         , strBody);

        if (res)
        {
            clearWaitingResponse();
            if (strMethod == MSG_SERV_METHOD_POST)
            {
                JsonParser jsonDoc;
                jsonDoc.loadPlaneText(strBody);
                if (jsonDoc.isDocJsonReady() && jsonDoc.getDocJsonValue("message") == "ok")
                {
                    removeRegisters();
                }
                globalLog << "Method Post Returned" << std::endl;
            }
            else if (strMethod == MSG_SERV_METHOD_GET)
            {
                m_TableBody.push_back(strBody);
                m_TableList.erase(m_TableList.begin());
                globalLog << "Method Get Returned" << std::endl;
                ++m_TableIndex;
            }
        }
        if (m_bShowData)
            std::cout << data << std::endl;
    }
    return true;
}

void SCCRemoteServer::removeRegisters()
{
    moveRegisterListToHistoryFolder();
    if ((unsigned int)m_iNumRegisterSent > m_RegisterList.size())
        m_RegisterList.clear();
    else
    {
        auto itLast = m_RegisterList.begin();
        std::advance(itLast, m_iNumRegisterSent);
        m_RegisterList.erase(m_RegisterList.begin(), itLast);
    }
}

void SCCRemoteServer::moveRegisterListToHistoryFolder()
{
    for (auto strRegFile : m_RegisterList)
    {
        moveFileToHistoFolder(strRegFile);
    }
}

void SCCRemoteServer::moveFileToHistoFolder(const std::string& strFile)
{
    if (m_registerDestination == "")
        return;
    SCCFileManager fileRegister(strFile);
    fileRegister.moveTo(m_registerDestination);

}
