#include "IDUserList.h"
#include "SCCLog.h"

extern SCCLog globalLog ;

IDUserList::IDUserList(const std::string& typeTable = "") : IDList(typeTable)
{
    //ctor
}

IDUserList::~IDUserList()
{
    //dtor
}

bool IDUserList::isValidID(std::string& strID)
{
    globalLog <<"Reading User ID List..." << std::endl;

    globalLog  << "User ID List ready to use." << std::endl;
    return false;
}

void IDUserList::init(MainCtrlSettings& settings, const std::string& typeTable = "")
{
    if (typeTable != "")
        m_strTableType = typeTable;

    IDList::init();
}

