#include "MainCtrlSettings.h"

MainCtrlSettings::MainCtrlSettings()
{
    //ctor
    serverPort = 4000;
    noResponseTimeMilli = 5000;
    mainTimerInterval = 15000;
}

MainCtrlSettings::~MainCtrlSettings()
{
    //dtor
}
