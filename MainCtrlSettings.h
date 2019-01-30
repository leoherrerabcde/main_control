#ifndef MAINCTRLSETTINGS_H
#define MAINCTRLSETTINGS_H

#include <stdio.h>

class MainCtrlSettings
{
    public:
        MainCtrlSettings();
        virtual ~MainCtrlSettings();

        int serverPort;
        int noResponseTimeMilli;
        int mainTimerInterval;
        size_t sckBufferSize;

    protected:

    private:
};

#endif // MAINCTRLSETTINGS_H
