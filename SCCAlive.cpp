#include "SCCAlive.h"
#include "SCCLog.h"

SCCAlive::SCCAlive() : m_pThreadAlive(NULL), m_bStarted(false)
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
        auto clkNow = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = clkNow - m_clkLast;
        //std::chrono::milliseconds diff = clkNow - m_clkLast;

        /*if (diff > m_dMaxDiffTime)
            throw("Rutine is not Alive.");*/

        for (unsigned int i = 0; i < m_TimerList.size(); ++i)
        {
            if (m_TimerList[i].m_bEnable == true)
            {
                std::chrono::duration<double> diff = clkNow - m_TimerList[i].m_clkLast;
                //std::chrono::milliseconds diff = clkNow - m_TimerList[i].m_clkLast;
                if (diff >= m_TimerList[i].m_dInterval)
                {
                    m_TimerList[i].m_clkLast = clkNow;
                    std::string tmrMsg("Timer ");
                    tmrMsg += std::to_string(i) + " event.";
                    SCCLog::print(tmrMsg);
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
        SCCLog::print("Timer created");
    }
    else
        SCCLog::print("Interval too small. Timerisnot created");
    return tmr;
}

void SCCAlive::stopTimer(int timerHdl)
{
    if (timerHdl < 0)
    {
        if ((unsigned int)timerHdl < m_TimerList.size())
        {
            m_TimerList[timerHdl].m_bEnable = false;
        }
        else
            SCCLog::print("Timer Handler out of scope.");
    }
    else
        SCCLog::print("Timer Handler out of scope.");
}

