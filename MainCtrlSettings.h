#ifndef MAINCTRLSETTINGS_H
#define MAINCTRLSETTINGS_H


class MainCtrlSettings
{
    public:
        MainCtrlSettings();
        virtual ~MainCtrlSettings();

        int serverPort;
        int noResponseTimeMilli;
        int mainTimerInterval;

    protected:

    private:
};

#endif // MAINCTRLSETTINGS_H
