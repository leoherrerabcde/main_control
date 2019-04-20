#include "SCCFuelTransaction.h"
#include "SCCLog.h"
#include "../commPort/SCCRealTime.h"
#include "SCCDeviceVars.h"
#include "SCCGeneralDefines.h"
#include "../jsonParser/JsonParser.h"

#include <set>
#include <list>


extern SCCLog globalLog;

#define PRINT_DBG(v)    globalLog << __LINE__ << "\t" << v << std::endl


static std::list<std::string> st_MemberList =
{
    VAR_REGISTER_NUMBER        ,
    VAR_REGISTER_TIME_INIT     ,
    VAR_REGISTER_DISPENSA_ID   ,
    VAR_REGISTER_USER_ID       ,
    VAR_REGISTER_CONDUCTOR_ID  ,
    VAR_REGISTER_VEHICLE_ID    ,
    VAR_REGISTER_ODOMETER      ,
    VAR_REGISTER_HOROMETER     ,
    VAR_REGISTER_TYPE          ,
    VAR_REGISTER_INIT_FLOW     ,
    VAR_REGISTER_END_FLOW      ,
    VAR_REGISTER_TIME_END      ,
    VAR_REGISTER_LITERS        ,
};

SCCFuelTransaction::SCCFuelTransaction(const std::string& deviceName, bool bShowdata) : Device(deviceName, bShowdata)
{
    //ctor
}

SCCFuelTransaction::~SCCFuelTransaction()
{
    //dtor
}

std::list<std::string>& SCCFuelTransaction::getMemberList()
{
    return st_MemberList;
}

int SCCFuelTransaction::init(MainCtrlSettings& settings)
{
    globalLog << "Initializating Register Class ..." << std::endl;
    //idVehicleList.init();

    std::stringstream sService;

    settings.getValue(m_DeviceName,PARAM_PATH_NAME,m_strRegisterPath);
    settings.getValue(m_DeviceName,PARAM_SERVICE_NAME,m_strRegisterName);
    settings.getValue(m_DeviceName,PARAM_FILE_EXTENSION,m_strFileExtension);
    settings.getValue(m_DeviceName,PARAM_NEW_REGS_PATH,m_strNewRegsPath);
    settings.getValue(m_DeviceName,PARAM_HISTO_REGS_PATH,m_strHistoRegsPath);
    settings.getValue(m_DeviceName,PARAM_CONSE_NUM_LENGTH,m_iConseNumLength);
    settings.getValue(m_DeviceName,PARAM_UPPER_REG_NUM,m_iUpperRegNum);

    sService << m_strRegisterPath << "/" << m_strRegisterName << m_strFileExtension;

    std::string strService(sService.str());

    setServicePath(strService);

    m_filemanRegister << strService;

    m_bLaunchingService = true;

    //finishTransaction();

    return 0;
}

bool SCCFuelTransaction::initTransaction(const double dCurrentFlowAcum)
{
    finishTransaction(dCurrentFlowAcum);

    int regNum = getLastRegisterNumber();
    ++regNum;
    //clearRegData();
    std::stringstream ss;
    ss << MSG_HEADER_TYPE << ASSIGN_CHAR << HEADER_REGISTER;
    addRegisterNumber(ss, regNum);
    addTimeIni(ss);

    m_filemanRegister.appendToFile(std::string(ss.str()));
    return true;
}

bool SCCFuelTransaction::addUser(const std::string& strID)
{
    std::stringstream ss;
    ss << SEPARATOR_CHAR << VAR_REGISTER_USER_ID << ASSIGN_CHAR << strID;

    m_filemanRegister.appendToFile(std::string(ss.str()));

    return true;
}

bool SCCFuelTransaction::addVehicle(const std::string& strID)
{
    std::stringstream ss;
    ss << SEPARATOR_CHAR << VAR_REGISTER_VEHICLE_ID << ASSIGN_CHAR << strID;

    m_filemanRegister.appendToFile(std::string(ss.str()));

    return true;
}

void SCCFuelTransaction::addFlowMeterBegin(const double dCurrentFlowAcum)
{
    std::stringstream ss;
    ss << SEPARATOR_CHAR << VAR_REGISTER_INIT_FLOW << ASSIGN_CHAR << dCurrentFlowAcum;

    m_filemanRegister.appendToFile(std::string(ss.str()));
}

void SCCFuelTransaction::addFlowMeterEnd(std::stringstream& ss, const double dCurrentFlowAcum)
{
    ss << SEPARATOR_CHAR << VAR_REGISTER_END_FLOW << ASSIGN_CHAR << dCurrentFlowAcum;
}

void SCCFuelTransaction::addTimeIni(std::stringstream& ss)
{
    ss << SEPARATOR_CHAR << VAR_REGISTER_TIME_INIT << ASSIGN_CHAR << SCCRealTime::getTimeStamp(true);
}

void SCCFuelTransaction::addTimeEnd(std::stringstream& ss)
{
    ss << SEPARATOR_CHAR << VAR_REGISTER_TIME_END << ASSIGN_CHAR << SCCRealTime::getTimeStamp(true);
}

void SCCFuelTransaction::addRegisterNumber(std::stringstream& ss, const int regNum)
{
    ss << SEPARATOR_CHAR << VAR_REGISTER_NUMBER << ASSIGN_CHAR << regNum;
}

bool SCCFuelTransaction::finishTransaction(const double dCurrentFlowAcum)
{
    if (m_filemanRegister.isFileExist())
    {
        int number = getRegisterNumber(getServicePath());

        std::string dataFile;
        m_filemanRegister.readFile(dataFile);

        bool bFlowEnd, bTimeEnd;
        double /*dFlowIni,*/ dFlowEnd;
        std::string strValue;
        std::stringstream ss;
        bFlowEnd = getValueMessage(dataFile, VAR_REGISTER_END_FLOW, dFlowEnd);
        //PRINT_DBG(ss.str());
        if (!bFlowEnd)
            addFlowMeterEnd(ss, dCurrentFlowAcum);
        //PRINT_DBG(ss.str());
        bTimeEnd = getValueMessage(dataFile, VAR_REGISTER_TIME_END, strValue);
        //PRINT_DBG(ss.str());
        if (!bTimeEnd)
            addTimeEnd(ss);
        if (!bFlowEnd || !bTimeEnd)
            m_filemanRegister.appendToFile(std::string(ss.str()));

        SCCFileManager dst(m_strRegisterPath);
        std::string str(m_strRegisterName);
        str += "_";
        str += regNumber2String(number);
        str += m_strFileExtension;
        dst << m_strNewRegsPath << str;
        m_filemanRegister.copyFile(dst.getAbsFileName());
        m_filemanRegister.deleteFile();

        return true;
    }
    return true;
}
std::string SCCFuelTransaction::regNumber2String(const int regNumber)
{
    std::string strNum(m_iConseNumLength, '0');

    strNum += std::to_string(regNumber);
    strNum = strNum.substr(strNum.length()-m_iConseNumLength);
    return strNum;
}

int SCCFuelTransaction::getLastRegisterNumber(const std::string& strPath)
{
    SCCFileManager newRegsPath(strPath);

    std::list<std::string> fileList;
    std::set<unsigned long> numberList;

    newRegsPath.getFileList(fileList);

    for (auto filename : fileList)
    {
        int num = getRegisterNumber(filename);
        if (num >= LOWER_REGISTER_NUM && num <= m_iUpperRegNum)
            numberList.insert(getRegisterNumber(filename));
    }

    if (numberList.size())
    {
        auto rit = numberList.rbegin();
        return *rit;
    }
    return LOWER_REGISTER_NUM-1;
}

int SCCFuelTransaction::getLastRegisterNumber()
{
    std::string strLastFile;

    if (m_filemanRegister.isFileExist())
        return getRegisterNumber(m_filemanRegister.getAbsFileName());

    SCCFileManager newRegsPath(m_strRegisterPath);
    newRegsPath << m_strNewRegsPath;
    int num = getLastRegisterNumber(newRegsPath.getAbsFileName());
    if (num >= LOWER_REGISTER_NUM)
        return num;

    SCCFileManager histoRegsPath(m_strRegisterPath);
    histoRegsPath << m_strHistoRegsPath;
    num = getLastRegisterNumber(histoRegsPath.getAbsFileName());

    return num;
}

std::string SCCFuelTransaction::getRegHistoPath()
{
    SCCFileManager histoRegsPath(m_strRegisterPath);
    histoRegsPath << m_strHistoRegsPath;
    return histoRegsPath.getAbsFileName();
}

int SCCFuelTransaction::getRegisterNumber(const std::string& strFileName)
{
    std::string strNum;

    //PRINT_DBG(strFileName);
    std::string::size_type pos = strFileName.find(m_strRegisterName);
    if ( pos == std::string::npos)
        return LOWER_REGISTER_NUM-1;

    strNum = strFileName.substr(pos + m_strRegisterName.length()+1);
    //PRINT_DBG(strNum);
    if (strNum.length() == m_iConseNumLength + m_strFileExtension.length())
        strNum = strNum.substr(0, m_iConseNumLength);
    else
        strNum = "";
    //PRINT_DBG(strNum);
    if (strNum != "")
    {
        strNum.erase(0, strNum.find_first_not_of('0'));
        int num = std::stoi(strNum.c_str());
        if (num > m_iUpperRegNum || num < LOWER_REGISTER_NUM)
            num = LOWER_REGISTER_NUM-1;
        return num;
    }

    SCCFileManager reg(strFileName);
    std::string dataFile;
    reg.readFile(dataFile);
    int num;
    if (getValueMessage(dataFile, VAR_REGISTER_NUMBER, num))
        return num;

    return LOWER_REGISTER_NUM-1;
}

bool SCCFuelTransaction::getRegisterList(std::list<std::string>& regList)
//bool SCCFuelTransaction::getNewRegisterJson(std::string& strJson)
{
    std::list<std::string>  tmpFileList;
    std::list<std::string>  registerFileList;

    regList.clear();
    SCCFileManager newRegPath(m_strRegisterPath);
    newRegPath << m_strNewRegsPath;
    newRegPath.getFileList(tmpFileList);

    if (!tmpFileList.size())
        return false;

    tmpFileList.sort();

    for (auto regFile : tmpFileList)
    {
        int num = getRegisterNumber(regFile);
        if (num < LOWER_REGISTER_NUM || num > m_iUpperRegNum)
            continue;
        SCCFileManager tmpFile(m_strRegisterPath);
        tmpFile << m_strNewRegsPath;
        tmpFile << regFile;
        regList.push_back(tmpFile.getAbsFileName());
    }

    if (!regList.size())
        return false;

    /*std::string strPlaneJson;

    JsonParser::getPlaneText(registerFileList, st_MemberList, strPlaneJson);*/
    return true;
}
