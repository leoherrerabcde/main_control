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

};

#endif // MAINSTATE_H
