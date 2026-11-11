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
#include "CPU.h"

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
// The cpu thread is running way slower than it should be. Pls fix.
static void cpuThread(CPU& cpu) {
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
}

static bool getScreenPixel(int x, int y, CPU& cpu) {
    std::lock_guard<std::mutex> lock(cpu.cpuMutex);
    if (x >= 0 && x < 32 && y >= 0 && y < 32) {
        if (x < 16) {
            if (((cpu.screen[-y + 63]) & (0b1000000000000000 >> x)) >= 1) {
				return 1;
			}
		}
		else {
			if (((cpu.screen[-y + 31]) & (0b1000000000000000 >> (x - 16))) >= 1) {
				return 1;
			}
        }
    }
    return false;
}

static void graphicsThread(CPU& cpu) {
    const unsigned int scale = 16;
    sf::RenderWindow window(sf::VideoMode({ 32 * scale, 32 * scale }), "SM02 Screen");
    window.setFramerateLimit(60);

    sf::View view(sf::FloatRect({0.f, 0.f}, {32.f, 32.f}));
    window.setView(view);

    sf::RectangleShape pixelRect({1.f, 1.f});
    pixelRect.setFillColor(sf::Color::Green);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);

        for (int y = 0; y < 32; ++y) {
            for (int x = 0; x < 32; ++x) {
                pixelRect.setPosition({static_cast<float>(x), static_cast<float>(y)});
                {
                    if (getScreenPixel(x, y, std::ref(cpu))) {
                        window.draw(pixelRect);
                    }
                }
            }
        }

        window.display();
    }
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
    std::thread t1(cpuThread, std::ref(cpu));
    std::thread t2(graphicsThread, std::ref(cpu));
    t2.join();
    {
        std::lock_guard<std::mutex> lock(cpu.cpuMutex);
        cpu.state = CPUState::halted;
    }
    t1.join();
    return 0;
}