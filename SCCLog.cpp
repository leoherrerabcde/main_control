#include "SCCLog.h"
#include <iostream>

/*SCCLog::SCCLog()
{
    //ctor
}*/

SCCLog::~SCCLog()
{
    /*if (m_bLogEn && m_bLogOpened)
        m_fmLogFile.close();*/
}

void SCCLog::print(const std::string& msg)
{
    std::cout << msg << std::endl;
}

