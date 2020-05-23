#pragma once
#include <array>
#include <cstdint>

#include "SAP1.h"

class Bus
{
public:
	Bus();

public: // Devices on bus
	SAP1 cpu;

	std::array<uint8_t, 16> ram;

public: // Bus read and write
	void write(uint8_t addr, uint8_t data);
	uint8_t read(uint8_t addr);
};

