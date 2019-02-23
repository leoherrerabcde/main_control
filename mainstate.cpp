#include "mainstate.h"

MainState::MainState()
{

}

MainState::~MainState()
{

}

int MainState::getLastIDTransaction()
{
    return 0;
}

MainState::State MainState::getLastState()
{
    return m_LastState;
}

MainState::State MainState::getCurrentState()
{
    return m_CurrentState;
}

