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
        m_bTimerEvent = false;
    }
    //std::chrono::duration<double> m_dInterval;
    std::chrono::milliseconds m_dInterval;
    std::chrono::time_point<std::chrono::steady_clock> m_clkLast;
    bool m_bEnable;
    bool m_bTimerEvent;

    bool isTimerEvent() {bool res(m_bTimerEvent); m_bTimerEvent = false; return res;}
    void resetTimer() {m_clkLast = std::chrono::steady_clock::now();m_bTimerEvent=false; m_bEnable = true;}
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
        void resetTimer(int timerHdl);
        bool isTimerEvent(const int tmrHdl);
        int getUpdate(const int tmrHdl);

        void throwDisable() {m_bThrowEnable = false;}

    protected:

    private:

    const int nullTimer() {return -1;};

    std::chrono::time_point<std::chrono::steady_clock> m_clkLast;
    std::thread* m_pThreadAlive;
    bool m_bStarted;
    //std::chrono::duration<double> m_dMaxDiffTime;
    std::chrono::milliseconds m_dMaxDiffTime;
    std::vector<SCCTimer> m_TimerList;

    bool m_bThrowEnable;
};

#endif // SCCALIVE_H
