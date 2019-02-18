#include "lucesestado.h"
#include "SCCLog.h"

LucesEstado::LucesEstado()
{

}

LucesEstado::~LucesEstado()
{

}

int LucesEstado::init(const MainCtrlSettings& settings)
{
    SCCLog::print("Initializating State Indicators...");

    SCCLog::print("State Indicator ready to use.");

    return 0;
}

void LucesEstado::updateState(const MainState::State newState)
{

}
