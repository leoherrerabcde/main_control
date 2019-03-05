#ifndef SCCFLOWMETER_H
#define SCCFLOWMETER_H

#include "device.h"
#include "../flowmCom/SCCFlowProtocol.h"


class SCCFlowmeter : public Device
{
    public:
        SCCFlowmeter(const std::string& deviceName = "", bool bShowdata = false);
        virtual ~SCCFlowmeter();

    virtual int init(MainCtrlSettings& settings);
    virtual bool processDataReceived(const std::string& msg = "");
    double getAcumFlow();

    protected:

    private:

        FlowRegisters   m_flowReg;
};

#endif // SCCFLOWMETER_H
