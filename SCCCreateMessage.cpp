#include "SCCCreateMessage.h"

#include <sstream>


SCCCreateMessage::SCCCreateMessage()
{
    //ctor
}

SCCCreateMessage::~SCCCreateMessage()
{
    //dtor
}

std::string SCCCreateMessage::makeMessage(const std::list<ArgumentStruct>& paramList)
{
    std::stringstream ss;

    ss << "<";
    bool bSeparator = false;
    for (auto itArg : paramList)
    {
        if (bSeparator)
            ss << "," << itArg.strArgName << ":" << itArg.value;
        else
            ss << itArg.strArgName << ":" << itArg.value;
        bSeparator = true;
    }
    ss << ">";
    return std::string(ss.str());
}

void SCCCreateMessage::addParam(const std::string& param, const std::string& value)
{
    m_paramList.push_back(std::make_pair(param, value));
}

void SCCCreateMessage::addBody(const std::string& param, const std::string& value)
{
    std::string strBody("|");
    strBody += value;
    strBody += "|";
    m_paramList.push_back(std::make_pair(param, strBody));
}

std::string SCCCreateMessage::makeMessage()
{
    std::stringstream ss;

    ss << "<";
    bool bSeparator = false;
    for (auto itArg : m_paramList)
    {
        if (bSeparator)
            ss << "," << itArg.first << ":" << itArg.second;
        else
            ss << itArg.first << ":" << itArg.second;
        bSeparator = true;
    }
    ss << ">";
    return std::string(ss.str());
}

