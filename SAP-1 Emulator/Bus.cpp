#include "Bus.h"

Bus::Bus()
{
	// Connect CPU to bus
	cpu.ConnectBus(this);

	// Clear RAM on boot
	for (auto& i : ram) i = 0x00;
}

void Bus::write(uint8_t addr, uint8_t data)
{
	if (addr >= 0x00 && addr <= 0x0F)
	{
		ram[addr] = data;
	}
}

uint8_t Bus::read(uint8_t addr)
{
	if (addr >= 0x00 && addr <= 0x0F)
	{
		return ram[addr];
	}
	return 0x00;
}
