#include <iostream>
#include <sstream>

#include "Bus.h"
#include "SAP1.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class Demo_SAP1 : public olc::PixelGameEngine
{
public:
	Demo_SAP1()
	{
		sAppName = "SAP-1 Breadboard Computer Emulator";
	}

	Bus computer;
	std::map<uint8_t, std::string> mapAsm;
	bool isExecuting = false;
	float residualTime = 0.0;
	int frequency = 60;

	std::string hex(uint32_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
		{
			s[i] = "0123456789ABCDEF"[n & 0xF];
		}
		return s;
	}

	std::string decimal(uint32_t n, uint8_t d)
	{
		std::string number = std::to_string(n);
		std::string result = std::string(d - number.length(), '0') + number;
		return result;
	}

	void DrawCpu(int x, int y)
	{
		DrawString(x, y, "Flags:", olc::WHITE);
		DrawString(x + 64, y, "C", computer.cpu.flags & SAP1::C ? olc::GREEN : olc::RED);
		DrawString(x + 80, y, "Z", computer.cpu.flags & SAP1::Z ? olc::GREEN : olc::RED);
		DrawString(x + 110, y, "Frequency:", olc::WHITE);
		DrawString(x + 110, y + 10, std::to_string(frequency) + "Hz", olc::WHITE);
		DrawString(x, y + 10, "PC: $" + hex(computer.cpu.pc, 1));
		DrawString(x, y + 20, "A: $" + hex(computer.cpu.a, 2) + " [" + std::to_string(computer.cpu.a) + "]");
		DrawString(x, y + 30, "B: $" + hex(computer.cpu.b, 2) + " [" + std::to_string(computer.cpu.b) + "]");
		DrawString(x, y + 40, "OUT: $" + hex(computer.cpu.out, 2) + " [" + std::to_string(computer.cpu.out) + "]");
	}

	void DrawRAM(int x, int y)
	{
		auto curr = mapAsm.find(computer.cpu.pc);
		for (uint8_t addr = 0x00; addr <= 0x0F; addr++)
		{
			DrawString(x, y, "Assembly:", olc::WHITE);
			DrawString(x + 100, y, "Memory:", olc::WHITE);
			auto line = mapAsm.find(addr);
			if ((*line).second == (*curr).second)
			{
				DrawString(x, y + 10 * addr + 15, (*line).second, olc::CYAN);
				DrawString(x + 100, y + 10 * addr + 15, "$" + hex(computer.read(addr), 2) + "  [" + decimal(computer.read(addr), 3) + "]", olc::CYAN);
			}
			else
			{
				DrawString(x, y + 10 * addr + 15, (*line).second, olc::WHITE);
				DrawString(x + 100, y + 10 * addr + 15, "$" + hex(computer.read(addr), 2) + "  [" + decimal(computer.read(addr), 3) + "]", olc::WHITE);
			}
		}
	}

	bool OnUserCreate()
	{
		// Load program
		std::stringstream ss;
		ss << "91 E0 BF 75 ";
		ss << "60 F0 00 00 ";
		ss << "00 00 00 00 ";
		ss << "00 00 00 1E";
		//ss << "00";
		uint8_t offset = 0x00;
		while (!ss.eof())
		{
			std::string b;
			ss >> b;
			computer.ram[offset] = (uint8_t)std::stoul(b, nullptr, 16);
			offset++;
		}

		// Extract disassembly
		mapAsm = computer.cpu.disassemble();

		// Reset
		computer.cpu.reset();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		Clear(olc::DARK_BLUE);

		// Extract disassembly every frame since we have such a small amount of RAM that it will inevitably change
		mapAsm = computer.cpu.disassemble();

		if (isExecuting)
		{
			if (residualTime > 0.0f)
			{
				residualTime -= fElapsedTime;
			}
			else
			{
				residualTime += (1.0f / (float)frequency) - fElapsedTime;
				if (!computer.cpu.halted)
				{
					computer.cpu.clock();
				}
			}
		}
		else
		{
			if (GetKey(olc::Key::C).bPressed)
			{
				do
				{
					computer.cpu.clock();
				} while (!computer.cpu.complete() && !computer.cpu.halted);
			}
		}

		if (GetKey(olc::Key::SPACE).bPressed)
		{
			isExecuting = !isExecuting;
		}

		if (GetKey(olc::Key::R).bPressed)
		{
			computer.cpu.reset();
		}

		if (GetKey(olc::Key::F).bPressed)
		{
			if (frequency >= GetFPS())
			{

			}
			else if (frequency >= 1000)
			{
				if (frequency + 1000 < GetFPS())
				{
					frequency += 1000;
				}
			}
			else if (frequency >= 100)
			{
				if (frequency + 100 < GetFPS())
				{
					frequency += 100;
				}
			}
			else if (frequency >= 10)
			{
				if (frequency + 10 < GetFPS())
				{
					frequency += 10;
				}
			}
			else
			{
				if (frequency + 1 < GetFPS())
				{
					frequency += 1;
				}
			}
		}

		if (GetKey(olc::Key::G).bPressed)
		{
			if (frequency <= 1)
			{
				
			}
			else if (frequency <= 10)
			{
				frequency -= 1;
			}
			else if (frequency <= 100)
			{
				frequency -= 10;
			}
			else if (frequency <= 1000)
			{
				frequency -= 100;
			}
			else
			{
				frequency -= 1000;
			}
		}

		if (frequency > GetFPS())
		{
			if (frequency <= 1)
			{

			}
			else if (frequency <= 10)
			{
				frequency -= 1;
			}
			else if (frequency <= 100)
			{
				frequency -= 10;
			}
			else if (frequency <= 1000)
			{
				frequency -= 100;
			}
			else
			{
				frequency -= 1000;
			}
		}

		DrawString(80, 70, decimal(computer.cpu.out, 3), olc::WHITE, 10U);
		DrawCpu(448, 2);
		DrawRAM(448, 72);
		DrawString(7, 227, "  C - Advance one instruction     R - Reset computer  ");
		if (isExecuting)
		{
			DrawString(7, 237, "SPACE - Stop Execution    F - Speed up   G - Slow down");
		}
		else
		{
			DrawString(7, 237, "SPACE - Start Execution   F - Speed up   G - Slow down");
		}

		return true;
	}
};

int main()
{
	Demo_SAP1 demo;
	if (demo.Construct(640, 250, 2, 2))
	{
		demo.Start();
	}

	return 0;
}