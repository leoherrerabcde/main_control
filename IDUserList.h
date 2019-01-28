#ifndef IDUSERLIST_H
#define IDUSERLIST_H

#include "IDList.h"

class IDUserList : public IDList
{
    public:
        IDUserList();
        virtual ~IDUserList();

        virtual void init();
        virtual bool isValidID(std::string& strIDVehicle);

    protected:

    private:
};

#endif // IDUSERLIST_H
