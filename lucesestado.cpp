#include "lucesestado.h"
#include "SCCLog.h"

extern SCCLog globalLog;

LucesEstado::LucesEstado(const std::string& deviceName) : Device(deviceName)
{

}

LucesEstado::~LucesEstado()
{

}

int LucesEstado::init(MainCtrlSettings& settings)
{
    globalLog << "Initializating State Indicators..." << std::endl;

    globalLog  << "State Indicator ready to use." << std::endl;

    return 0;
}

void LucesEstado::updateState(const MainState::State newState)
{

}
