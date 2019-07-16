#include "main_fuel_manage.h"
#include "main_functions.h"







ProcessFuelEvents::ProcessFuelEvents(
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
):
    mainState(_mainState),
    keepAlive(_keepAlive),
    fuelRegister(_fuelRegister),
    UserList(_UserList),
    VehicleList(_VehicleList),
    electroValve(_electroValve),
    flowmeter(_flowmeter),
    rfidBoquilla(_rfidBoquilla),
    rfidUser(_rfidUser),
    tmrFuelTransaction(_tmrFuelTransaction),
    tmrAuthorizedVehicle(_tmrAuthorizedVehicle),
    stateRbpi(_stateRbpi),
    bRfidReaderDisable(_bRfidReaderDisable),
    bStateChange(_bStateChange),
    socketMap(_socketMap),
    deviceList(_deviceList),
    strTagVehicle(_strTagVehicle),
    strIdUser(_strIdUser),
    strDriver(_strDriver)

{
    dAcumFlowOld = 0.0;
    bCancelTransaction = false;
}

void ProcessFuelEvents::check_events()
{
        if (keepAlive.isTimerEvent(tmrFuelTransaction))
        {
            electroValve.closeValve();
            fuelRegister.finishTransaction(flowmeter.getAcumFlow());
            //fuelTransactionTimeOut();
            keepAlive.stopTimer(tmrFuelTransaction);
            mainState.processFuelingTimeOut();
        }
        stateRbpi = mainState.getCurrentState();
        if (stateRbpi == MainState::State::waitForInitTransaction)
        {
            if (bRfidReaderDisable)
            {
                sendData(UserList.getTableType(), socketMap, deviceList, rfidUser.getCmdUnLockReader());
                bRfidReaderDisable = false;
            }
            bool bAuthorizedUser        = false;
            bool bAuthorizedVehicle     = false;
            bool bAuthorizedDriver      = false;

            bStateChange = false;
            //bool bInitFuelTransaction   = false;
            if (rfidUser.isUserDetected())
            {
                std::string strId = rfidUser.getUserId();
                std::string strType = UserList.getAtributeValue(strId);
                //bAuthorizedUser = UserList.isValidID(rfidUser.getUserId());
                if (strType == PARAM_FIREFIGHTER_COD)
                {
                    bAuthorizedUser = true;
                    strIdUser = strId;
                    mainState.processUserAuthorization(bAuthorizedUser);
                    sendData(UserList.getTableType(), socketMap, deviceList, rfidUser.getCmdBeepSound());
                }
                if (strType == PARAM_DRIVER_COD)
                {
                    bAuthorizedDriver = true;
                    strDriver = strId;
                    mainState.processDriverAuthorization(bAuthorizedDriver);
                    sendData(UserList.getTableType(), socketMap, deviceList, rfidUser.getCmdBeepSound());
                    //sendData(UserList.getTableType(), socketMap, rfidUser.getCmdLockReader());
                }
            }
            if (rfidBoquilla.isTagDetected())
            {
                std::string strTag = rfidBoquilla.getTagId();
                bAuthorizedVehicle = VehicleList.isValidID(strTag);
                if (bAuthorizedVehicle)
                {
                    strTagVehicle = strTag;
                    //tmrAuthorizedVehicle = keepAlive.addTimer(10000);
                    keepAlive.resetTimer(tmrAuthorizedVehicle);
                    mainState.processVehicleAuthorization(bAuthorizedVehicle);
                }
            }
            if (bAuthorizedUser || bAuthorizedVehicle || bAuthorizedDriver)
            {
                dAcumFlowOld = flowmeter.getAcumFlow();
                fuelRegister.initTransaction(flowmeter.getAcumFlow());
                if (bAuthorizedUser)
                    fuelRegister.addUser(strIdUser);
                if (bAuthorizedDriver)
                    fuelRegister.addDriver(strDriver);
                if (bAuthorizedVehicle)
                    fuelRegister.addVehicle(strTagVehicle);

                fuelRegister.addFlowMeterBegin(flowmeter.getAcumFlow());
                keepAlive.resetTimer(tmrFuelTransaction);
                bStateChange = true;
            }
        }
        else if ((stateRbpi & MainState::State::RFIDBoquilla) || (stateRbpi & MainState::State::RFIDUser) || (stateRbpi & MainState::State::RFIDDriver))
        {
            bool bAuthorizedUser        = (stateRbpi & MainState::State::RFIDUser);
            bool bAuthorizedVehicle     = (stateRbpi & MainState::State::RFIDBoquilla);
            bool bAuthorizedDriver      = (stateRbpi & MainState::State::RFIDDriver);
            bool bChangeDetected        = false;

            bStateChange = false;

            if (rfidUser.isUserDetected())
            {
                std::string strType = UserList.getAtributeValue(rfidUser.getUserId());
                std::string strId = rfidUser.getUserId();
                if (strType == PARAM_FIREFIGHTER_COD)
                {
                    if (!bAuthorizedUser)
                    {
                        bAuthorizedUser = true;
                        sendData(UserList.getTableType(), socketMap, deviceList, rfidUser.getCmdBeepSound());
                        strIdUser = strId;
                        fuelRegister.addUser(strIdUser);
                        mainState.processUserAuthorization(bAuthorizedUser);
                        bChangeDetected = true;
                    }
                    else
                    {
                        if (strId != strIdUser)
                            bCancelTransaction = true;
                    }
                }
                if (strType == PARAM_DRIVER_COD)
                {
                    std::string strType = UserList.getAtributeValue(rfidUser.getUserId());
                    std::string strId = rfidUser.getUserId();
                    if (!bAuthorizedDriver)
                    {
                        bAuthorizedDriver = true;
                        sendData(UserList.getTableType(), socketMap, deviceList, rfidUser.getCmdBeepSound());
                        strDriver = strId;
                        fuelRegister.addDriver(strDriver);
                        mainState.processDriverAuthorization(bAuthorizedUser);
                        bChangeDetected = true;
                    }
                    else
                    {
                        if (strId != strDriver)
                            bCancelTransaction = true;
                    }
                }
                //bAuthorizedUser = UserList.isValidID(rfidUser.getUserId());
                if (bChangeDetected)
                {
                    bChangeDetected = false;
                    keepAlive.resetTimer(tmrFuelTransaction);
                    if (bAuthorizedUser && bAuthorizedDriver)
                    {
                        sendData(UserList.getTableType(), socketMap, deviceList, rfidUser.getCmdLockReader());
                        bRfidReaderDisable = true;
                    }
                }
            }
            if (!bAuthorizedVehicle)
            {
                if (rfidBoquilla.isTagDetected())
                {
                    std::string strTag = rfidBoquilla.getTagId();
                    bAuthorizedVehicle = VehicleList.isValidID(strTag);
                    if (bAuthorizedVehicle)
                    {
                        strTagVehicle = strTag;
                        fuelRegister.addVehicle(strTagVehicle);
                        mainState.processVehicleAuthorization(bAuthorizedVehicle);
                        keepAlive.resetTimer(tmrFuelTransaction);
                        //tmrAuthorizedVehicle = keepAlive.addTimer(10000);
                        keepAlive.resetTimer(tmrAuthorizedVehicle);
                        //dAcumFlowOld = flowmeter.getAcumFlow();
                        //electroValve.openValve();
                    }
                }
            }
            else
            {
                if (rfidBoquilla.isTagDetected())
                {
                    if (strTagVehicle == rfidBoquilla.getTagId())
                    /*{
                        mainState.processLostVehicleTag();
                        bAuthorizedVehicle = false;
                        bCancelTransaction = true;
                    }
                    else*/
                        keepAlive.resetTimer(tmrAuthorizedVehicle);
                }
                /*else
                {*/
                if (keepAlive.isTimerEvent(tmrAuthorizedVehicle))
                {
                    keepAlive.stopTimer(tmrAuthorizedVehicle);
                    mainState.processLostVehicleTag();
                    bCancelTransaction = true;
                }
                //}
            }

            if (bAuthorizedVehicle && bAuthorizedDriver && bAuthorizedUser)
            {
                electroValve.openValve();
                bStateChange = true;
            }
        }
        /*else if (stateRbpi == MainState::RFIDUser)
        {
            bool bAuthorizedVehicle = false;
            if (rfidBoquilla.isTagDetected())
            {
                bAuthorizedVehicle = VehicleList.isValidID(rfidBoquilla.getTagId());
            }
            if (bAuthorizedVehicle)
            {
                fuelRegister.addVehicle(rfidBoquilla.getTagId());
                mainState.processVehicleAuthorization(bAuthorizedVehicle);
                keepAlive.resetTimer(tmrFuelTransaction);
                dAcumFlowOld = flowmeter.getAcumFlow();
                electroValve.openValve();
            }
        }*/
        /*else if (stateRbpi == MainState::startingTransaction)
        {
        }*/
        else if (stateRbpi == MainState::chargingFuel)
        {
            bStateChange = false;
            if (dAcumFlowOld != flowmeter.getAcumFlow())
            {
                keepAlive.resetTimer(tmrFuelTransaction);
                dAcumFlowOld = flowmeter.getAcumFlow();
            }
            if (!rfidBoquilla.isTagDetected() || strTagVehicle != rfidBoquilla.getTagId())
            {
                if (keepAlive.isTimerEvent(tmrAuthorizedVehicle))
                {
                    mainState.processLostVehicleTag();
                    electroValve.closeValve();
                    keepAlive.stopTimer(tmrAuthorizedVehicle);
                }
            }
            else
                keepAlive.resetTimer(tmrAuthorizedVehicle);
        }
        else if (stateRbpi == MainState::chargingPaused)
        {
            if (rfidBoquilla.isTagDetected())
            {
                if (strTagVehicle == rfidBoquilla.getTagId())
                {
                    mainState.processResumeFueling();
                    electroValve.openValve();
                }
                else
                {
                    electroValve.closeValve();
                    fuelRegister.finishTransaction(flowmeter.getAcumFlow());
                    mainState.processFinishFueling();
                }
            }
        }
        if (bCancelTransaction)
        {
            electroValve.closeValve();
            fuelRegister.finishTransaction(flowmeter.getAcumFlow());
            //fuelTransactionTimeOut();
            keepAlive.stopTimer(tmrFuelTransaction);
            mainState.processFuelingTimeOut();
            bCancelTransaction = false;
        }
        /*else if (stateRbpi == MainState::finishingTransaction)
        {
        }*/
}
