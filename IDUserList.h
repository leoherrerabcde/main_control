#ifndef IDUSERLIST_H
#define IDUSERLIST_H

#include "IDList.h"

class IDUserList : public IDList
{
    public:
        IDUserList(const std::string& typeTable = "");
        virtual ~IDUserList();

        virtual void init(MainCtrlSettings& settings, const std::string& typeTable = "");
        virtual bool isValidID(const std::string& strId);

    protected:

    private:
};

#endif // IDUSERLIST_H
