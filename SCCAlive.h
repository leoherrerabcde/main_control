#ifndef SCCALIVE_H
#define SCCALIVE_H

#include <chrono>
#include <thread>
#include <vector>

struct SCCTimer
{
    SCCTimer(int interval)
    {
        m_dInterval = std::chrono::milliseconds(interval);
        m_clkLast = std::chrono::steady_clock::now();
        m_bEnable = true;
    }
    //std::chrono::duration<double> m_dInterval;
    std::chrono::milliseconds m_dInterval;
    std::chrono::time_point<std::chrono::steady_clock> m_clkLast;
    bool m_bEnable;
};

class SCCAlive
{
    public:
        SCCAlive();
        virtual ~SCCAlive();

        void update();
        void start(const int maxTime);
        void stop();
        void run();
        int addTimer(const int interval);
        void stopTimer(int timerHdl);

    protected:

    private:

    std::chrono::time_point<std::chrono::steady_clock> m_clkLast;
    std::thread* m_pThreadAlive;
    bool m_bStarted;
    //std::chrono::duration<double> m_dMaxDiffTime;
    std::chrono::milliseconds m_dMaxDiffTime;
    std::vector<SCCTimer> m_TimerList;
};

#endif // SCCALIVE_H
