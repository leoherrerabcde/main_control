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
        RFIDBoquilla = 2,
        startingTransaction = 4,
        chargingFuel = 8,
        chargingPaused = 16,
        finishingTransaction = 32,
        waitForFinishTransaction = 64
    };

    State getLastState();
    int getLastIDTransaction();
    State getCurrentState();

    void processUserAuthorization(bool bAuthorized);
    void processVehicleAuthorization(bool bAuthorized);
    void processLostVehicleTag();
    void processResumeFueling();
    void processFinishFueling();

private:

    State m_LastState;
    State m_CurrentState;

    SCCStateVar m_bUserAuthorized;
    SCCStateVar m_bvehicleAuthorized;
};

#endif // MAINSTATE_H
