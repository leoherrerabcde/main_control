#ifndef MAIN_FUEL_MANAGE_H
#define MAIN_FUEL_MANAGE_H

#include "electrovalvctrl.h"
#include "rfidboquilla.h"
#include "rfiduser.h"
#include "mainstate.h"
#include "IDList.h"
#include "MainCtrlSettings.h"
#include "CSocket.h"
#include "SCCLog.h"
#include "SCCAlive.h"
#include "SCCRemoteServer.h"
#include "SCCFlowmeter.h"
#include "SCCDeviceNames.h"
#include "SCCCreateMessage.h"
#include "SCCFuelTransaction.h"

class ProcessFuelEvents
{
public:
    ProcessFuelEvents(
        MainState& _mainState,
        SCCAlive& _keepAlive,
        SCCFuelTransaction& _fuelRegister,
        IDList& _UserList,
        IDList& _VehicleList,
        ElectroValvCtrl& _electroValve,
        SCCFlowmeter& _flowmeter,
        RFIDBoquilla& _rfidBoquilla,
        RFIDUser& _rfidUser,
        int _tmrFuelTransaction,
        int _tmrAuthorizedVehicle,
        int& _stateRbpi,
        bool& _bRfidReaderDisable,
        bool& _bStateChange,
        std::unordered_map<std::string,CSocket*>& _socketMap,
        std::unordered_map<std::string,Device*>& _deviceList,
        std::string& _strTagVehicle,
        std::string& _strIdUser,
        std::string& _strDriver
    );

    void check_events();




private:

    MainState& mainState;
    SCCAlive& keepAlive;
    SCCFuelTransaction& fuelRegister;
    IDList& UserList;
    IDList& VehicleList;
    ElectroValvCtrl& electroValve;
    SCCFlowmeter& flowmeter;
    RFIDBoquilla& rfidBoquilla;
    RFIDUser& rfidUser;
    int tmrFuelTransaction;
    int tmrAuthorizedVehicle;
    int& stateRbpi;
    bool& bRfidReaderDisable;
    bool& bStateChange;
    std::unordered_map<std::string,CSocket*>& socketMap;
    std::unordered_map<std::string,Device*>& deviceList;
    std::string& strTagVehicle;
    std::string& strIdUser;
    std::string& strDriver;

    double dAcumFlowOld;
    bool bCancelTransaction;

};

#endif

