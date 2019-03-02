#ifndef SCCFLOWMETER_H
#define SCCFLOWMETER_H

#include "device.h"


class SCCFlowmeter : public Device
{
    public:
        SCCFlowmeter(const std::string& deviceName = "");
        virtual ~SCCFlowmeter();

    virtual int init(MainCtrlSettings& settings);
    virtual bool processDataReceived(const std::string& msg = "");

    protected:

    private:
};

#endif // SCCFLOWMETER_H
