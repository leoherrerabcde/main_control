#ifndef LUCESESTADO_H
#define LUCESESTADO_H

#include "device.h"
#include "mainstate.h"

class LucesEstado : public Device
{
public:
    LucesEstado();
    ~LucesEstado();

    virtual int init();
    void updateState(const MainState::State newState);
};

#endif // LUCESESTADO_H
