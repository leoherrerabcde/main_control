#ifndef SCCCREATEMESSAGE_H
#define SCCCREATEMESSAGE_H

#include "../commPort/SCCArgumentParser.h"

#include <list>
#include <utility>

class SCCCreateMessage
{
    public:
        SCCCreateMessage();
        virtual ~SCCCreateMessage();

        static std::string makeMessage(const std::list<ArgumentStruct>& paramList);
        std::string makeMessage();
        void addParam(const std::string& param, const std::string& value);
        void addBody(const std::string& param, const std::string& value);

    protected:

    private:

        std::list<std::pair<std::string,std::string>> m_paramList;
};

#endif // SCCCREATEMESSAGE_H
