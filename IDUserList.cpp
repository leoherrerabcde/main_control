#include "IDUserList.h"
#include "SCCLog.h"

extern SCCLog globalLog ;

IDUserList::IDUserList()
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

void IDUserList::init()
{

}

