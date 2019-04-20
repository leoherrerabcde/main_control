#include "IDList.h"
#include "SCCFileManager.h"
#include "SCCLog.h"

extern SCCLog globalLog;

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
    {
        globalLog << "JSON File cannot be read from disk.\nThe application can not continue working and it will be closed.\n";
        return false;
    }
    std::cout << m_strTableType << " -> Parsing Table." << std::endl;
    m_jsonParser.loadPlaneText(m_strData);
    if (!m_jsonParser.isDocJsonReady())
    {
        globalLog << "JSON File is not parsed.\nThe application can not continue working and it will be closed.\n";
        return false;
    }

    std::cout << m_strTableType << " -> Creating Id Table." << std::endl;
    m_jsonParser.createIdTable();

    if (!m_jsonParser.isMapIDReady())
    {
        globalLog << "Table was not created.\nThe application can not continue working and it will be closed.\n";
        return false;
    }

    return true;
}

bool IDList::parseTable(const std::string& strData)
{
    JsonParser jsonParser;

    jsonParser.setIdKeyLabel(m_strIdKeyLabel);

    /*if (!m_bTableReady)
        globalLog << "JSON File cannot be read from disk.\nThe application can not continue working and it will be closed.\n";*/

    globalLog << m_strTableType << " -> Parsing Table." << std::endl;

    jsonParser.loadPlaneText(strData);
    if (!jsonParser.isDocJsonReady())
    {
        globalLog << "JSON File is not parsed.\nThe application can not continue working and it will be closed.\n";
        return false;
    }

    std::cout << m_strTableType << " -> Creating Id Table." << std::endl;
    jsonParser.createIdTable();

    if (!jsonParser.isMapIDReady())
    {
        globalLog << "Table was not created.\nThe application can not continue working and it will be closed.\n";
        return false;
    }

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
    // Creating Temporary File
    SCCFileManager fileman(m_strTablePath);
    fileman << fileman.getTempFileName();

    // Writing Json data to Temp File
    fileman.writeFile(strData);

    // Validating Json Data
    bool bValid = parseTable(strData);
    if (!bValid)
    {
        globalLog << "Parse Error in Json Data" << std::endl;
        return false;
    }

    // Data Valid. Saving json file
    // Creating Destination File
    SCCFileManager fileDest(m_strTablePath);
    fileDest << m_strTableName;

    // Moving Source to Dest
    bool ret = fileman.moveFile(fileDest.getAbsFileName());

    // If file dest could not be created
    if (!ret)
    {
        globalLog << "File '" << fileDest.getAbsFileName() << "' could not be created" << std::endl;
        return false;
    }

    // The Destination File was created
    m_strData = strData;
    bValid = parseTable();

    if (!bValid)
        globalLog << "An Unexpected error happens during Json parser" << std::endl;

    return bValid;
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



