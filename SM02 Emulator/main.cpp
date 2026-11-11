#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <bitset>
#include <SFML/Graphics.hpp>
#include <mutex>
#include "CPU.h"

std::mutex cpuMutex;

using json = nlohmann::json;

const double targetHz = 10000;
const double nanosecondsPerSec = 1e9;
const auto intervalNs = std::chrono::nanoseconds(
    static_cast<long long>(nanosecondsPerSec / targetHz)
);

static uint16_t stringToUint16(const std::string& str){
    size_t pos;
    unsigned long val = std::stoul(str, &pos);

    if (pos != str.size() || val > UINT16_MAX) {
        throw std::out_of_range("Value out of uint16_t range or invalid format");
    }
    return static_cast<uint16_t>(val);
}

static void cpuThread(CPU cpu)
{
    auto last = std::chrono::steady_clock::now();
    double pending = 0.0;

    while (cpu.state != CPUState::halted) {
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - last).count();
        last = now;

        pending += elapsed * targetHz;

        while (pending >= 1.0) {
            pending--;
            cpu.clockPulse();
        }

        if (pending > 0) {
            std::this_thread::sleep_for(std::chrono::microseconds((int)(pending * 1000000 / targetHz)));
        }
    }
    return;
}

int main() {
    CPU cpu;

    // load program data into memory

    std::ifstream file("data.json");
    json j;
    file >> j;

    for (const auto& [key, value] : j["data"].items()) {
        cpu.memory.write(stringToUint16(key), value);
    }
    std::thread t(cpuThread, cpu);
    t.join();
    return 0;
}