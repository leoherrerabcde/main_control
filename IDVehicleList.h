#ifndef IDVEHICLELIST_H
#define IDVEHICLELIST_H

#include "IDList.h"

class IDVehicleList : public IDList
{
    public:
        IDVehicleList();
        virtual ~IDVehicleList();

        bool isValidID(std::string& strIDVehicle);
        void init();

    protected:

    private:
};

#endif // IDVEHICLELIST_H
