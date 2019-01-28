#ifndef ELECTROVALVCTRL_H
#define ELECTROVALVCTRL_H

#include "device.h"

class ElectroValvCtrl : public Device
{
public:
    ElectroValvCtrl();
    ~ElectroValvCtrl();

    virtual int init();
};

#endif // ELECTROVALVCTRL_H
