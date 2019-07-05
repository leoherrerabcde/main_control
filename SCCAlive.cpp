
#include "SCCAlive.h"
#include "SCCLog.h"

#include <iostream>

extern SCCLog globalLog;


SCCAlive::SCCAlive() : m_pThreadAlive(NULL), m_bStarted(false), m_bThrowEnable(true)
{
    //ctor
    m_clkLast = std::chrono::steady_clock::now();
}

SCCAlive::~SCCAlive()
{
    //dtor
    stop();
}

void SCCAlive::start(const int maxTime)
{
    if (m_bStarted == false)
    {
        m_dMaxDiffTime = std::chrono::milliseconds(maxTime);
        m_clkLast = std::chrono::steady_clock::now();
        m_bStarted = true;
        if (m_pThreadAlive == NULL)
            m_pThreadAlive = new std::thread(&SCCAlive::run, this);
    }
}

void SCCAlive::run()
{
    while(m_bStarted == true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        auto clkNow = std::chrono::steady_clock::now();
        {
            std::chrono::duration<double> diff = clkNow - m_clkLast;

            if (diff > m_dMaxDiffTime)
            {
                std::cout << "Main Loop was stopped long time" << std::endl;
                m_clkLast = clkNow;
                if (m_bThrowEnable)
                    throw("Rutine is not Alive.");
            }
        }
        for (unsigned int i = 0; i < m_TimerList.size(); ++i)
        {
            if (m_TimerList[i].m_bEnable == true)
            {
                std::chrono::duration<double> diffTmr = clkNow - m_TimerList[i].m_clkLast;
                //std::chrono::milliseconds diff = clkNow - m_TimerList[i].m_clkLast;
                if (diffTmr >= m_TimerList[i].m_dInterval)
                {
                    m_TimerList[i].m_clkLast = clkNow;
                    m_TimerList[i].m_bTimerEvent = true;
                    /*std::string tmrMsg("Timer ");
                    tmrMsg += std::to_string(i) + " event.";
                    std::cout << tmrMsg << std::endl;*/
                    //globalLog << tmrMsg;
                    //globalLog << std::endl;
                }
            }
        }
    }
}

void SCCAlive::stop()
{
    m_bStarted = false;
    if (m_pThreadAlive != NULL)
    {
        m_pThreadAlive->join();
        delete m_pThreadAlive;
    }
}

void SCCAlive::update()
{
    m_clkLast = std::chrono::steady_clock::now();
}

int SCCAlive::addTimer(const int interval)
{
    int tmr= -1;
    if (interval > 0.05)
    {
        tmr = (int)m_TimerList.size();
        SCCTimer newTimer(interval);
        m_TimerList.push_back(newTimer);
        globalLog << "Timer created" << std::endl;
    }
    else
        globalLog << "Interval too small. Timerisnot created" << std::endl;
    return tmr;
}

void SCCAlive::stopTimer(int timerHdl)
{
    if (timerHdl >= 0)
    {
        if ((unsigned int)timerHdl < m_TimerList.size())
        {
            m_TimerList[timerHdl].m_bEnable = false;
        }
        else
            globalLog << "Timer Handler out of scope." << std::endl;
    }
    else
        if (timerHdl != nullTimer())
            globalLog << "Timer Handler out of scope." << std::endl;
}

bool SCCAlive::isTimerEvent(const int tmrHdl)
{
    if (tmrHdl >= 0)
    {
        if ((unsigned int)tmrHdl < m_TimerList.size())
        {
            return m_TimerList[tmrHdl].isTimerEvent();
        }
        else
            globalLog << "Timer Handler out of scope." << std::endl;
    }
    else
    {
        if (tmrHdl != nullTimer())
            globalLog << "Timer Handler out of scope." << std::endl;
    }
    return false;
}

void SCCAlive::resetTimer(const int timerHdl)
{
    if (timerHdl >= 0)
    {
        if ((unsigned int)timerHdl < m_TimerList.size())
        {
            m_TimerList[timerHdl].resetTimer();
        }
        else
            globalLog << "Timer Handler out of scope." << std::endl;
    }
    else
        if (timerHdl != nullTimer())
            globalLog << "Timer Handler out of scope." << std::endl;
}


