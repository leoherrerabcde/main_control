#ifndef ELECTROVALVCTRL_H
#define ELECTROVALVCTRL_H

#include "device.h"

class ElectroValvCtrl : public Device
{
public:
    ElectroValvCtrl(const std::string& deviceName = "", bool bShowdata = false);
    ~ElectroValvCtrl();

    virtual int init(MainCtrlSettings& settings);
};

#endif // ELECTROVALVCTRL_H
