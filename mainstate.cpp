#include "mainstate.h"

MainState::MainState() : m_LastState(State::waitForInitTransaction), m_CurrentState(State::waitForInitTransaction)
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
    if (m_CurrentState == State::waitForInitTransaction)
    {
        if (bAuthorized)
        {
            m_LastState = m_CurrentState;
            /*if (m_bvehicleAuthorized.get())
                m_CurrentState = State::startingTransaction;
            else*/
                m_CurrentState = State::RFIDUser;
        }
    }
    else if (m_CurrentState == State::RFIDBoquilla)
    if (m_CurrentState == State::waitForInitTransaction)
    {
        if (bAuthorized)
        {
            m_LastState = m_CurrentState;
            /*if (m_bvehicleAuthorized.get())
                m_CurrentState = State::startingTransaction;
            else*/
                m_CurrentState = State::chargingFuel;
        }
    }
}

void MainState::processVehicleAuthorization(bool bAuthorized)
{
    //m_bVehicleAuthorized.setVarState(bAuthorized);
    m_bvehicleAuthorized.setVarState(bAuthorized);
    if (m_CurrentState == State::waitForInitTransaction)
    {
        if (bAuthorized)
        {
            m_LastState = m_CurrentState;
            if (m_bUserAuthorized.get())
                m_CurrentState = State::startingTransaction;
            else
                m_CurrentState = State::RFIDBoquilla;
        }
    }
}

void MainState::processLostVehicleTag()
{
    if (m_CurrentState == State::chargingFuel)
        m_CurrentState = State::chargingPaused;
}

void MainState::processResumeFueling()
{
    if (m_CurrentState == State::chargingPaused)
        m_CurrentState = State::chargingFuel;
}

void MainState::processFinishFueling()
{
    if (m_CurrentState == chargingPaused || m_CurrentState == chargingPaused)
        m_CurrentState = State::waitForInitTransaction;
}
