#ifndef COMMGSM_H
#define COMMGSM_H

#include "device.h"

class commGSM : public Device
{
public:
    commGSM();
    ~commGSM();

    virtual int init(const MainCtrlSettings& settings);
};

#endif // COMMGSM_H
