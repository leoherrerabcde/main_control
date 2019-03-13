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
    {MainState::State::startingTransaction, "Statrting Fuel Register"},
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
            printStatus();
        }
    }
    else if (m_CurrentState == State::RFIDBoquilla)
    //if (m_CurrentState == State::waitForInitTransaction)
    {
        if (bAuthorized)
        {
            m_LastState = m_CurrentState;
            /*if (m_bvehicleAuthorized.get())
                m_CurrentState = State::startingTransaction;
            else*/
                m_CurrentState = State::chargingFuel;
            printStatus();
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
            printStatus();
        }
    }
    else if (m_CurrentState == State::RFIDUser)
    //if (m_CurrentState == State::waitForInitTransaction)
    {
        if (bAuthorized)
        {
            m_LastState = m_CurrentState;
            /*if (m_bvehicleAuthorized.get())
                m_CurrentState = State::startingTransaction;
            else*/
                m_CurrentState = State::chargingFuel;
            printStatus();
        }
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

void MainState::printStatus()
{
    globalLog << SCCRealTime::getTimeStamp() << "\tCurrent Status:\t" << stStateToStringMap[getCurrentState()] << std::endl;
}
