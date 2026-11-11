#include <cstdint>
#include <vector>

class Memory {
private:
	std::vector<uint16_t> data = std::vector<uint16_t>(65536, 0);

public:
    uint16_t read(uint16_t address) const;
    void write(uint16_t address, uint16_t value);
    void dump(bool skipZeroes = true) const;
};