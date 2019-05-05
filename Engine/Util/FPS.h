#pragma once

#include <chrono>

class FPSCounter
{
public:
    typedef std::chrono::time_point<std::chrono::system_clock> time;
    typedef std::chrono::duration<float, std::milli> ms;

    FPSCounter() : m_fpsCurrent(0), m_fpsAvgSec(0), m_oneSecTicks(0), m_oneSecDuration(std::chrono::milliseconds::zero()){}
    ~FPSCounter() = default;

    int GetFPSCurrent() { return m_fpsCurrent; }
    int GetFPSAvgSec() { return m_fpsAvgSec; }

    void BeginTick()
    {
        m_tickBeginTime = std::chrono::system_clock::now();
    }

    void EndTick()
    {
        m_tickEndTime = std::chrono::system_clock::now();

        m_oneTickDuration = std::chrono::duration_cast<ms>(m_tickEndTime - m_tickBeginTime);
        m_oneSecDuration += m_oneTickDuration;

        m_fpsCurrent = (int)(1000 / m_oneTickDuration.count());
        if (m_oneSecDuration.count() >= 1000)
        {
            m_fpsAvgSec = (int)(m_oneSecTicks / (m_oneSecDuration.count() / 1000));
            m_oneSecDuration = std::chrono::milliseconds::zero();
            m_oneSecTicks = 0;
        }

        m_oneSecTicks++;
    }

private:
    int m_fpsCurrent;
    int m_fpsAvgSec;

    int m_oneSecTicks;

    time m_tickBeginTime;
    time m_tickEndTime;

    ms m_oneTickDuration;
    ms m_oneSecDuration;

};