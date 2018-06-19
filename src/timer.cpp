#include "timer.h"

Timer::Timer()
{
    Restart();
}

void Timer::Restart()
{
    m_start = std::chrono::high_resolution_clock::now();
    m_tick = m_start;
    m_elapsed = std::chrono::duration<double>::zero();
}

void Timer::Tick()
{
    auto last = m_tick;
    m_tick = std::chrono::high_resolution_clock::now();

    m_elapsed = m_tick - last;
}

double Timer::Total() const
{
    return static_cast<double>( std::chrono::duration_cast<std::chrono::milliseconds>( ( std::chrono::high_resolution_clock::now() - m_start ) ).count() ) / 1000.0;
}

double Timer::Elapsed() const
{
    return m_elapsed.count();
}