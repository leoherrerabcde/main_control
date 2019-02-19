#ifndef MAINCTRLSETTINGS_H
#define MAINCTRLSETTINGS_H

#include <stdio.h>
#include <unordered_map>

#include "SCCDeviceNames.h"

#include "../commPort/SCCArgumentParser.h"

#define PARAM_SERVICE_NAME  "ServiceName"
#define PARAM_PATH_NAME     "PathName"
#define PARAM_COM_PORT      "COM"
#define PARAM_BAUD_RATE     "BaudRate"

class MainCtrlSettings
{
    public:
        MainCtrlSettings();
        virtual ~MainCtrlSettings();

        int serverPort;
        int noResponseTimeMilli;
        int mainTimerInterval;
        size_t sckBufferSize;

        bool getValue(const std::string& deviceName, const std::string& argName, int& val, const int& valDefault = 0);
        bool getValue(const std::string& deviceName, const std::string& argName, std::string& val, const std::string& valDefault = "");

    protected:

    private:
};

#endif // MAINCTRLSETTINGS_H