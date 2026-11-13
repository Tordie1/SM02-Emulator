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

const double targetHz = 10000000; // 10 MHz

const unsigned int scale = 24;

static uint16_t stringToUint16(const std::string& str){
    size_t pos;
    unsigned long val = std::stoul(str, &pos);

    if (pos != str.size() || val > UINT16_MAX) {
        throw std::out_of_range("Value out of uint16_t range or invalid format");
    }
    return static_cast<uint16_t>(val);
}

static void cpuThread(CPU& cpu) {
    auto start = std::chrono::steady_clock::now();
    const uint64_t cyclesPerMillisecond = static_cast<uint64_t>(targetHz / 1000);
    uint64_t targetCycle = 0;
    uint64_t cycles = 0;

    while (true) {
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - start).count();

        targetCycle = static_cast<uint64_t>(elapsed * targetHz);

        while (cycles < targetCycle) {
            if (cpu.state == CPUState::halted || cpu.forceHLT) {
                //std::cout << "Halted";
                return;
            }
            cycles++;
            cpu.clockPulse();
			if (cpu.state == CPUState::fetch && cpu.instructionStep == 0) {
                //std::cout << "\na:   " << std::bitset<16>(cpu.a) << "\nb:   " << std::bitset<16>(cpu.b) << "\npc:  " << std::bitset<16>(cpu.pc) << "\nptr: " << std::bitset<16>(cpu.ptr) << "\n";
			}
        }
        /* IDK how useful these lines are.
        const auto nextCycleBatch = cycles / cyclesPerMillisecond + 1;
        std::this_thread::sleep_until(start + std::chrono::milliseconds(nextCycleBatch));
        */
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
    sf::RenderWindow window(sf::VideoMode({ 32 * scale, 32 * scale }), "SM02 Screen");
    window.setFramerateLimit(60);

    sf::View view(sf::FloatRect({0.f, 0.f}, {32.f, 32.f}));
    window.setView(view);

    sf::RectangleShape pixelRect({1.f, 1.f});
    pixelRect.setFillColor(sf::Color::Green);

    /*Trying to set the icon gives an error, and I don't know why.
    sf::Image icon;
    if (icon.loadFromFile("icon.png")) {
        window.setIcon(icon.getSize(), icon.getPixelsPtr());
    }*/

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
    std::thread t2(graphicsThread, std::ref(cpu));
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::thread t1(cpuThread, std::ref(cpu));
    t2.join();
    {
        std::lock_guard<std::mutex> lock(cpu.cpuMutex);
        cpu.forceHLT = true;
    }
    t1.join();
    return 0;
}