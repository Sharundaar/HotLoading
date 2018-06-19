#pragma once

#include <chrono>

class Timer
{
public:
    Timer();

    void Restart();

    void Tick();
    double Elapsed() const;
    double Total() const;

private:
    std::chrono::duration<double> m_elapsed;    // elapsed time since last tick (seconds)

    std::chrono::time_point<std::chrono::high_resolution_clock> m_start; // time point at clock start
    std::chrono::time_point<std::chrono::high_resolution_clock> m_tick; // time point at last tick
};

