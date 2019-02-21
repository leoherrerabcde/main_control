#include "IDVehicleList.h"
#include "SCCLog.h"

extern SCCLog globalLog;

IDVehicleList::IDVehicleList()
{
    //ctor
}

IDVehicleList::~IDVehicleList()
{
    //dtor
}

bool IDVehicleList::isValidID(std::string& strIDVehicle)
{
    globalLog << "Loading Vehicle ID List..." << std::endl;

    globalLog << "Vehicle ID List ready to use." << std::endl;

    return false;
}

void IDVehicleList::init()
{

}
