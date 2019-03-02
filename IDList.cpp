#include "IDList.h"
#include "SCCFileManager.h"

IDList::IDList(const std::string& typeTable) : m_strTableType(typeTable) , m_bTableReady(false)
{
    //ctor
}

IDList::~IDList()
{
    //dtor
}

void IDList::init(MainCtrlSettings& settings, const std::string& typeTable)
{
    if (typeTable != "")
        m_strTableType = typeTable;

    //std::stringstream sService;
    //std::string pathName;
    //std::string serviceName;
    //const std::string paramPathName(PARAM_PATH_NAME);
    //const std::string paramServiceName(PARAM_SERVICE_NAME);

    settings.getValue(m_strTableType,PARAM_PATH_NAME    ,m_strTablePath);
    settings.getValue(m_strTableType,PARAM_SERVICE_NAME ,m_strTableName);

    readTable();
}

bool IDList::readTable()
{
    std::string fileName(m_strTablePath);
    fileName += "/";
    fileName += m_strTableName;

    bool res = SCCFileManager::readFile(fileName, m_strData);

    m_bTableReady = res;
    return res;
}

bool IDList::writeTable(const std::string& strData)
{
    SCCFileManager fileman(m_strTablePath);

    fileman << fileman.getTempFileName();

    return fileman.writeFile(strData);
}


bool IDList::isValidID(const std::string& strId)
{
    return false;
}

std::string IDList::getAtributeValue(const std::string& strID, const std::string strAtribute)
{
    return "";
}

void IDList::setAtributeValue(const std::string& strID, const std::string strAtribute, const std::string& strValue)
{
}



