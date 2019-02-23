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

void MainState::processUserAuthorization(bool bAuthorized)
{
    m_bUserAuthorized.setVarState(bAuthorized);
    if (m_CurrentState == State.waitForInitTransaction)
    {
        if (bAuthorized && m_bvehicleAuthorized)
        {
            m_LastState = m_CurrentState;
            m_CurrentState = State.startingTransaction;
        }
    }
}

void MainState::processVehicleAuthorization(bool bAuthorized)
{
    m_bVehicleAuthorized.setVarState(bAuthorized);
    if (m_CurrentState == State.waitForInitTransaction)
    {
        if (bAuthorized && m_bUserAuthorized)
        {
            m_LastState = m_CurrentState;
            m_CurrentState = State.startingTransaction;
        }
    }
}

