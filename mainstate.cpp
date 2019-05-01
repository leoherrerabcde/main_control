#include "mainstate.h"
#include "SCCLog.h"
#include "../commPort/SCCRealTime.h"


#include <unordered_map>


extern SCCLog  globalLog;

std::unordered_map<int, std::string> stStateToStringMap =
{
    {MainState::State::chargingFuel, "Charging Fuel"},
    {MainState::State::chargingPaused, "Chargind Paused"},
    {MainState::State::finishingTransaction, "Finished Transaction"},
    {MainState::State::RFIDBoquilla, "Vahicle Tag Detected"},
    {MainState::State::RFIDUser, "User Detected"},
    {MainState::State::RFIDDriver, "Driver Detected"},
    {MainState::State::startingTransaction, "Starting Fuel Register"},
    {MainState::State::waitForFinishTransaction, "Waiting for Finish Fueling"},
    {MainState::State::waitForInitTransaction, "Waiting for Init Fueling"},
};

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

int MainState::getLastState()
{
    return m_LastState;
}

int MainState::getCurrentState()
{
    return m_CurrentState;
}

void MainState::processUserAuthorization(bool bAuthorized)
{
    m_bUserAuthorized.setVarState(bAuthorized);
    if (bAuthorized)
    {
        if ((m_CurrentState == State::waitForInitTransaction) || (m_CurrentState & State::RFIDBoquilla) || (m_CurrentState & State::RFIDDriver))
        {
            m_LastState = m_CurrentState;
            m_CurrentState |= State::RFIDUser;
        }
        /*else if ((m_CurrentState || State::RFIDBoquilla) || (m_CurrentState || State::RFIDDriver))
        {
            m_LastState = m_CurrentState;
            m_CurrentState |= State::RFIDUser;
        }*/
        if (m_CurrentState == (State::RFIDBoquilla | State::RFIDUser | State::RFIDDriver))
        {
            m_CurrentState = State::chargingFuel;
        }
        printStatus();
    }
}

void MainState::processDriverAuthorization(bool bAuthorized)
{
    m_bDriverAuthorized.setVarState(bAuthorized);
    if (bAuthorized)
    {
        if ((m_CurrentState == State::waitForInitTransaction) || (m_CurrentState & State::RFIDBoquilla) || (m_CurrentState & State::RFIDUser))
        {
            m_LastState = m_CurrentState;
            m_CurrentState |= State::RFIDDriver;
        }
        if (m_CurrentState == (State::RFIDBoquilla | State::RFIDUser | State::RFIDDriver))
        {
            m_CurrentState = State::chargingFuel;
        }
        printStatus();
    }
}

void MainState::processVehicleAuthorization(bool bAuthorized)
{
    m_bvehicleAuthorized.setVarState(bAuthorized);
    if (bAuthorized)
    {
        if ((m_CurrentState == State::waitForInitTransaction) || (m_CurrentState & State::RFIDUser) || (m_CurrentState & State::RFIDUser))
        {
            m_LastState = m_CurrentState;
            m_CurrentState |= State::RFIDBoquilla;
        }
        if (m_CurrentState == (State::RFIDBoquilla | State::RFIDUser | State::RFIDDriver))
        {
            m_CurrentState = State::chargingFuel;
        }
        printStatus();
    }
}

void MainState::processLostVehicleTag()
{
    if (m_CurrentState == State::chargingFuel)
    {
        m_CurrentState = State::chargingPaused;
        printStatus();
    }
}

void MainState::processResumeFueling()
{
    if (m_CurrentState == State::chargingPaused)
    {
        m_CurrentState = State::chargingFuel;
        printStatus();
    }
}

void MainState::processFinishFueling()
{
    if (m_CurrentState == chargingPaused || m_CurrentState == chargingPaused)
    {
        m_CurrentState = State::waitForInitTransaction;
        printStatus();
    }
}

void MainState::processFuelingTimeOut()
{
    m_CurrentState = State::waitForInitTransaction;
    printStatus();
}

void MainState::processFuelingCancel()
{
    m_CurrentState = State::waitForInitTransaction;
    printStatus();
}

void MainState::printStatus()
{
    if (getCurrentState()&(State::RFIDBoquilla | State::RFIDDriver | State::RFIDUser))
    {
        int iBoquilla   = getCurrentState() & State::RFIDBoquilla;
        int iUser       = getCurrentState() & State::RFIDUser;
        int iDriver     = getCurrentState() & State::RFIDDriver;

        globalLog << SCCRealTime::getTimeStamp() << "\tCurrent Status:\t";
        if (iBoquilla)
            globalLog << stStateToStringMap[iBoquilla] << "\t";
        if (iUser)
            globalLog << stStateToStringMap[iUser] << "\t";
        if (iDriver)
            globalLog << stStateToStringMap[iDriver] << "\t";
        globalLog << std::endl;
    }
    else
        globalLog << SCCRealTime::getTimeStamp() << "\tCurrent Status:\t" << stStateToStringMap[getCurrentState()] << std::endl;
}
