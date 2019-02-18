#ifndef LUCESESTADO_H
#define LUCESESTADO_H

#include "device.h"
#include "mainstate.h"

class LucesEstado : public Device
{
public:
    LucesEstado(const std::string& deviceName = "");
    ~LucesEstado();

    virtual int init(MainCtrlSettings& settings);
    void updateState(const MainState::State newState);
};

#endif // LUCESESTADO_H
