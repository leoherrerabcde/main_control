#ifndef IDVEHICLELIST_H
#define IDVEHICLELIST_H

#include "IDList.h"

class IDVehicleList : public IDList
{
    public:
        IDVehicleList(const std::string& typeTable = "");
        virtual ~IDVehicleList();

        virtual bool isValidID(const std::string& strId);
        virtual void init(MainCtrlSettings& settings, const std::string& typeTable = "");

        //virtual bool readTable();

    protected:

    private:

};

#endif // IDVEHICLELIST_H
