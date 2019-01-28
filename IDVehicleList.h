#ifndef IDVEHICLELIST_H
#define IDVEHICLELIST_H

#include "IDList.h"

class IDVehicleList : public IDList
{
    public:
        IDVehicleList();
        virtual ~IDVehicleList();

        virtual bool isValidID(std::string& strIDVehicle);
        virtual void init();

    protected:

    private:
};

#endif // IDVEHICLELIST_H
