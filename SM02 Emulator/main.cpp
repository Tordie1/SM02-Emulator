#include <chrono>
#include <thread>
#include <iostream>
#include "CPU.h"


const double targetHz = 1.0;
const double nanosecondsPerSec = 1e9;
const auto intervalNs = std::chrono::nanoseconds(
    static_cast<long long>(nanosecondsPerSec / targetHz)
);

int main() {
    CPU cpu;
    auto last = std::chrono::high_resolution_clock::now();

    while (!cpu.halted) {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = now - last;

        if (elapsed >= intervalNs) {
            last = now;
            cpu.clockPulse();
            std::this_thread::yield();
        }
    }
    return 0;
}