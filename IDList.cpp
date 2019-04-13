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
    settings.getValue(m_strTableType,PARAM_TBL_ID_KEY   ,m_strIdKeyLabel);


    std::cout << m_strTableType << " -> Reading Table from disk." << std::endl;
    readTable();
    /*std::string errMsg;
    errMsg += m_strTableType;
    errMsg += */
    parseTable();
}

bool IDList::parseTable()
{
    m_jsonParser.setIdKeyLabel(m_strIdKeyLabel);

    if (!m_bTableReady)
        throw("JSON File cannot be read from disk.\nThe application can not continue working and it will be closed.\n");

    std::cout << m_strTableType << " -> Parsing Table." << std::endl;
    m_jsonParser.loadPlaneText(m_strData);
    if (!m_jsonParser.isDocJsonReady())
        throw("JSON File is not parsed.\nThe application can not continue working and it will be closed.\n");

    std::cout << m_strTableType << " -> Creating Id Table." << std::endl;
    m_jsonParser.createIdTable();

    if (!m_jsonParser.isMapIDReady())
        throw("Table was not created.\nThe application can not continue working and it will be closed.\n");

    return true;
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

    fileman.writeFile(strData);

    SCCFileManager fileDest(m_strTablePath);
    fileDest << m_strTableName;

    bool ret = fileman.moveFile(fileDest.getFileName());

    m_strData = strData;

    return ret & parseTable();
}


bool IDList::isValidID(const std::string& strId)
{
    if (!m_jsonParser.isMapIDReady() || strId == "")
        return false;

    return m_jsonParser.hasID(strId);
    //return true;
}

std::string IDList::getAtributeValue(const std::string& strID, const std::string strAtribute)
{
    return "";
}

void IDList::setAtributeValue(const std::string& strID, const std::string strAtribute, const std::string& strValue)
{
}



