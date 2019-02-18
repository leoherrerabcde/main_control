#include "lucesestado.h"
#include "SCCLog.h"

LucesEstado::LucesEstado(const std::string& deviceName)
{

}

LucesEstado::~LucesEstado()
{

}

int LucesEstado::init(MainCtrlSettings& settings)
{
    SCCLog::print("Initializating State Indicators...");

    SCCLog::print("State Indicator ready to use.");

    return 0;
}

void LucesEstado::updateState(const MainState::State newState)
{

}
