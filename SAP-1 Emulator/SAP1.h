#pragma once
#include <cstdint>
#include <string>
#include <map>
#include <vector>

class Bus;

class SAP1
{
public:
	SAP1();

public :
	enum FLAGS_SAP1
	{
		Z = (1 << 0),	// Zero bit
		C = (1 << 1),	// Carry bit
	};

	uint8_t a = 0x00;		// A register
	uint8_t b = 0x00;		// B register
	uint8_t out = 0x00;		// Output register
	uint8_t pc = 0x00;		// Program counter
	uint8_t flags = 0x00;	// Flags register (only bottom 2 bits are used)

	bool halted = false;

	std::map<uint8_t, std::string> disassemble();

	bool complete();

	void ConnectBus(Bus* n) { bus = n; }

	void clock();
	void reset();

private:
	uint8_t opcode = 0x00;
	uint8_t operand = 0x00;
	uint8_t cycles = 0;

	Bus* bus = nullptr;
	uint8_t read(uint8_t addr);
	void write(uint8_t addr, uint8_t data);

	bool GetFlag(FLAGS_SAP1 f);
	void SetFlag(FLAGS_SAP1 f, bool v);

	struct INSTRUCTION
	{
		std::string name;
		void (SAP1::*operate)(void) = nullptr;
	};

	std::vector<INSTRUCTION> lookup;

private:
	// Opcodes
	void NOP();	// 0x0
	void LDA();	// 0x1
	void ADD();	// 0x2
	void SUB();	// 0x3
	void STA();	// 0x4
	void LDI();	// 0x5
	void JMP();	// 0x6
	void JC();	// 0x7
	void JZ();	// 0x8
	void INC();	// 0x9
	void DEC();	// 0xA
	void CMP();	// 0xB
				// 0xC
				// 0xD
	void OUT();	// 0xE
	void HLT(); // 0xF


};