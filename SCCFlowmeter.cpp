#include "SCCFlowmeter.h"

#include <sstream>
#include <iostream>


SCCFlowmeter::SCCFlowmeter(const std::string& deviceName, bool bShowdata) : Device(deviceName, bShowdata)
{
    //ctor
}

SCCFlowmeter::~SCCFlowmeter()
{
    //dtor
}

int SCCFlowmeter::init(MainCtrlSettings& settings)
{
    globalLog << "Initializating Flowmeter ..." << std::endl;
    //idVehicleList.init();

    /* Launch Service*/
    std::stringstream sService;
    std::string pathName;
    std::string serviceName;
    const std::string paramPathName(PARAM_PATH_NAME);
    const std::string paramServiceName(PARAM_SERVICE_NAME);

    settings.getValue(m_DeviceName,paramPathName,pathName);
    settings.getValue(m_DeviceName,paramServiceName,serviceName);

    m_strServiceName = serviceName;

    sService << pathName << "/" << serviceName;

    std::stringstream sArgs;

    int comPort, baudRate, remotePort;

    settings.getValue(m_DeviceName, PARAM_COM_PORT, comPort, 0);
    settings.getValue(m_DeviceName, PARAM_BAUD_RATE, baudRate, 0);
    remotePort = settings.serverPort;

    sArgs << comPort << " " << baudRate << " " << remotePort;

    m_iComPort      = comPort;
    m_iBaudRate     = baudRate;
    m_iRemotePort   = remotePort;

    std::string strService(sService.str());
    std::string strArgs(sArgs.str());

    setServicePath(strService);
    setServiceArgs(strArgs);
    //int ret = launchService(strService, strArgs);
    //int ret = 0;

    //SCCLog::print("RFID Nozzle Initiated.");
    m_bLaunchingService = true;
    killService();
    return 0;
}

bool SCCFlowmeter::processDataReceived(const std::string& msg)
{
    pushData(msg);

    while (!isBufferEmpty())
    {
        std::string data;
        data = popFrontMessage();

        if (isAliveMessage(data))
            continue;
        if (!isFrameType(DEVICE_FLOWMETER, data))
            continue;

        std::string strValue;
        std::string strTag;

        FlowRegisters reg;

        bool res = true;
        res = res && getValueMessage(data, VAR_INSTANTFLOWRATE          , reg.m_dInstantFlowRate);
        res = res && getValueMessage(data, VAR_FLUIDSPEED               , reg.m_dFluidSpeed);
        res = res && getValueMessage(data, VAR_MEASUREFLUIDSOUNDSPEED   , reg.m_dMeasureFluidSoundSpeed);
        res = res && getValueMessage(data, VAR_POSACUMFLOWRATE          , reg.m_lPosAcumFlowRate);
        res = res && getValueMessage(data, VAR_POSACUMFLOWRATEDECPART   , reg.m_dPosAcumFlowRateDecPart);
        res = res && getValueMessage(data, VAR_NEGACUMFLOWRATE          , reg.m_lNegAcumFlowRate);
        res = res && getValueMessage(data, VAR_NEGACUMFLOWRATEDECPART   , reg.m_dNegAcumFlowRateDecPart);
        res = res && getValueMessage(data, VAR_NETACUMFLOWRATE          , reg.m_lNetAcumFlowRate);
        res = res && getValueMessage(data, VAR_NETACUMFLOWRATEDECPART   , reg.m_dNetAcumFlowRateDecPart);

        if (res)
        {
            m_flowReg = reg;
            setServiceAlive();
        }
        if (m_bShowData)
            std::cout << data << std::endl;
    }
    return true;
}

double SCCFlowmeter::getAcumFlow()
{
    return 0.0;
}
