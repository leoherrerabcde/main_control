#ifndef COMMGSM_H
#define COMMGSM_H

#include "device.h"

class commGSM : public Device
{
public:
    commGSM(const std::string& deviceName = "", bool bShowdata = false);
    ~commGSM();

    virtual int init(MainCtrlSettings& settings);
};

#endif // COMMGSM_H
