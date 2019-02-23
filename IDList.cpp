#include "IDList.h"
#include "SCCFileManager.h"

IDList::IDList(const std::string& typeTable = "") : m_strTableType(typeTable) , m_bTableReady(false)
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

}

int IDList::readTable()
{
    std::string fileName(m_strTablePath);
    fileName += "/";
    fileName += m_strTableName;

    bool res = SCCFileManager::readFile(fileName, m_strData);

    m_bTableReady = res;
    return res;
}

int IDList::writeTable(const std::string& strData)
{
}


bool isValidID(std::string& strId)
{
}

