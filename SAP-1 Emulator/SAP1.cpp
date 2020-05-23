#include "SAP1.h"
#include "Bus.h"

SAP1::SAP1()
{
	using s = SAP1;
	lookup =
	{
		{ "NOP", &s::NOP }, { "LDA", &s::LDA }, { "ADD", &s::ADD }, { "SUB", &s::SUB },
		{ "STA", &s::STA }, { "LDI", &s::LDI }, { "JMP", &s::JMP }, { "JC", &s::JC },
		{ "JZ", &s::JZ }, { "INC", &s::INC }, { "DEC", &s::DEC }, { "CMP", &s::CMP },
		{ "NOP", &s::NOP }, { "NOP", &s::NOP }, { "OUT", &s::OUT }, { "HLT", &s::HLT },
	};
}

std::map<uint8_t, std::string> SAP1::disassemble()
{
	std::map<uint8_t, std::string> mapLines;

	auto hex = [](uint32_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
		{
			s[i] = "0123456789ABCDEF"[n & 0xF];
			return s;
		}
	};

	for (uint8_t address = 0x00; address <= 0x0F; address++)
	{
		std::string sInst = "$" + hex(address, 1) + ": ";
		uint8_t instruct = bus->read(address);
		uint8_t code = (instruct & 0xF0) >> 4;
		uint8_t arg = (instruct & 0x0F);
		sInst += lookup[code].name + " ";
		sInst += "$" + hex(arg, 1);
		mapLines[address] = sInst;
	}

	return mapLines;
}

bool SAP1::complete()
{
	return cycles == 0;
}

bool SAP1::GetFlag(SAP1::FLAGS_SAP1 f) 
{
	return (flags & f) > 0;
}

void SAP1::SetFlag(SAP1::FLAGS_SAP1 f, bool v)
{
	if (v)
	{
		flags |= f;
	}
	else
	{
		flags &= ~f;
	}
}

void SAP1::clock()
{
	if (!halted)
	{
		if (cycles == 0)
		{
			uint8_t instruction = read(pc);
			opcode = (instruction & 0xF0) >> 4;
			operand = (instruction & 0x0F);
			pc++;
			cycles = 5; // Hardcoded number of clock cycles per instruction
			(this->*lookup[opcode].operate)();
		}

		cycles--;
	}
}

void SAP1::reset()
{
	halted = false;
	opcode = 0x00;
	operand = 0x00;

	a = 0x00;
	b = 0x00;
	out = 0x00;
	pc = 0x00;
	flags = 0x00;
}

uint8_t SAP1::read(uint8_t addr)
{
	return bus->read(addr);
}

void SAP1::write(uint8_t addr, uint8_t data)
{
	bus->write(addr, data);
}

void SAP1::NOP()
{
	// Do nothing :P
}

void SAP1::LDA()
{
	a = read(operand);
}

void SAP1::ADD()
{
	b = read(operand);
	uint16_t temp = (uint16_t)a + (uint16_t)b;
	SetFlag(Z, temp == 0x00);
	SetFlag(C, temp > 255);
	a = temp & 0x00FF;
}

void SAP1::SUB()
{
	b = read(operand);
	uint8_t value = (b ^ 0xFF) + 0x01;
	uint16_t temp = (uint16_t)a + (uint16_t)value;
	SetFlag(Z, temp == 0x00);
	SetFlag(C, temp > 255);
	a = temp & 0x00FF;
}

void SAP1::STA()
{
	write(operand, a);
}

void SAP1::LDI()
{
	a = operand;
}

void SAP1::JMP()
{
	pc = operand;
}

void SAP1::JC()
{
	if (GetFlag(C))
	{
		pc = operand;
	}
}

void SAP1::JZ()
{
	if (GetFlag(Z))
	{
		pc = operand;
	}
}

void SAP1::INC()
{
	a += operand;
	SetFlag(Z, a == 0x00);
	SetFlag(C, a == 0x00);
}

void SAP1::DEC()
{
	a -= operand;
	SetFlag(Z, a == 0x00);
	SetFlag(C, a != 0xFF);
}

void SAP1::CMP()
{
	b = read(operand);
	uint8_t value = (b ^ 0xFF) + 0x01;
	uint16_t temp = (uint16_t)a + (uint16_t)value;
	SetFlag(Z, temp == 0x00);
	SetFlag(C, temp > 255);
}

void SAP1::OUT()
{
	out = a;
}

void SAP1::HLT()
{
	pc--; // To display address halted on correctly
	halted = true;
}
