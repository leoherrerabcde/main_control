#ifndef MAINSTATE_H
#define MAINSTATE_H


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
        waitForFinishTransaction = 3
    };

    State getLastState();
    int getLastIDTransaction();
    State getCurrentState();

    void processUserAuthorization(bool bAuthorized);
    void processVehicleAuthorization(bool bAuthorized);

private:

    State m_LastState;
    State m_CurrentState;

};

#endif // MAINSTATE_H
