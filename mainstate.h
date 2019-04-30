#ifndef MAINSTATE_H
#define MAINSTATE_H

#include "SCCStateVar.h"

class MainState
{
public:
    MainState();
    ~MainState();

    enum State
    {
        waitForInitTransaction = 0,
        RFIDUser = 1,
        RFIDDriver = 2,
        RFIDBoquilla = 4,
        startingTransaction = 8,
        chargingFuel = 16,
        chargingPaused = 32,
        finishingTransaction = 64,
        waitForFinishTransaction = 128
    };

    int getLastState();
    int getLastIDTransaction();
    int getCurrentState();

    void processUserAuthorization(bool bAuthorized);
    void processDriverAuthorization(bool bAuthorized);
    void processVehicleAuthorization(bool bAuthorized);
    void processLostVehicleTag();
    void processResumeFueling();
    void processFinishFueling();
    void processFuelingTimeOut();

private:

    /*int operator |=(MainState::State lVal, const MainState::State& rVal)
    {
        int iResult = (int)lVal | (int)rVal;
        //MainState::State lNewVal = (MainState::State)iResult ;

        return iResult;
    }*/

    void printStatus();

    int m_LastState;
    int m_CurrentState;

    SCCStateVar m_bUserAuthorized;
    SCCStateVar m_bDriverAuthorized;
    SCCStateVar m_bvehicleAuthorized;
};


#endif // MAINSTATE_H
