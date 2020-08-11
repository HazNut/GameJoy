// TODO: USE SIGNED VALUES FOR INSTRUCTIONS THAT NEED THEM

#include "gb.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <Windows.h>

using namespace std;

// Initialize the Game Boy by setting the register values.
void gb::initialize()
{
	A = 0x01;
	F = 0xB0;
	B = 0x00;
	C = 0x13;
	D = 0x00;
	E = 0xD8;
	H = 0x01;
	L = 0x4D;
	SP = 0x014D;

	memory[0xFF05] = 0x00;
	memory[0xFF06] = 0x00;
	memory[0xFF07] = 0x00;
	memory[0xFF10] = 0x80;
	memory[0xFF11] = 0xBF;
	memory[0xFF12] = 0xF3;
	memory[0xFF14] = 0xBF;
	memory[0xFF16] = 0x3F;
	memory[0xFF17] = 0x00;
	memory[0xFF19] = 0xBF;
	memory[0xFF1A] = 0x7F;
	memory[0xFF1B] = 0xFF;
	memory[0xFF1C] = 0x9F;
	memory[0xFF1E] = 0xBF;
	memory[0xFF20] = 0xFF;
	memory[0xFF21] = 0x00;
	memory[0xFF22] = 0x00;
	memory[0xFF23] = 0xBF;
	memory[0xFF24] = 0x77;
	memory[0xFF25] = 0xF3;
	memory[0xFF26] = 0xF1;
	memory[0xFF40] = 0x91;
	memory[0xFF42] = 0x00;
	memory[0xFF43] = 0x00;
	memory[0xFF45] = 0x00;
	memory[0xFF47] = 0xFC;
	memory[0xFF48] = 0xFF;
	memory[0xFF49] = 0xFF;
	memory[0xFF4A] = 0x00;
	memory[0xFF4B] = 0x00;
	memory[0xFFFF] = 0x00;

	PC = 0x100;
}

// Load the game.
void gb::loadGame()
{
	streampos size;
	char* memblock;

	ifstream file("tetris.gb", ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char[size];
		file.seekg(0, ios::beg);
		file.read(memblock, size);
		file.close();
		printf("ROM loaded.\n");
		for (int i = 0; i < size; i++)
			memory[i] = memblock[i];
		delete[] memblock;
	}
	else printf("Unable to load ROM.\n");

}

// Emulate a cycle of the Game Boy.
void gb::emulateCycle()
{
	// Get the opcode and execute the required instructions.
	opcode = memory[PC];
	char opcodeStr[3];
	_itoa_s(opcode, opcodeStr, 16);
	printf("\nOpcode is %s, PC is %i. ZNHC = %i%i%i%i\n", opcodeStr, PC, Zb, Nb, Hb, Cb);

	if (opcode == 0xCB)
	{
		PC += 1;
		opcode = memory[PC];
		_itoa_s(opcode, opcodeStr, 16);
		printf("\nOpcode after prefix is %s, PC is %i. ZNHC = %i%i%i%i\n", opcodeStr, PC, Zb, Nb, Hb, Cb);
		switch (opcode & 0xF0)
		{
		case 0x00:
			switch (opcode & 0x0F)
			{
			case 0x00: // RLC B
				ROT('L', false, B);
				PC += 1;
				break;
			
			case 0x01: // RLC C
				ROT('L', false, C);
				PC += 1;
				break;

			case 0x02: // RLC D
				ROT('L', false, D);
				PC += 1;
				break;

			case 0x03: // RLC E
				ROT('L', false, E);
				PC += 1;
				break;

			case 0x04: // RLC H
				ROT('L', false, H);
				PC += 1;
				break;

			case 0x05: // RLC L
				ROT('L', false, L);
				PC += 1;
				break;

			case 0x06: // RLC (HL)
				ROT('L', false, memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x07: // RLC A
				ROT('L', false, A);
				PC += 1;
				break;

			case 0x08: // RRC B
				ROT('R', false, B);
				PC += 1;
				break;

			case 0x09: // RRC C
				ROT('R', false, C);
				PC += 1;
				break;

			case 0x0A: // RRC D
				ROT('R', false, D);
				PC += 1;
				break;

			case 0x0B: // RRC E
				ROT('R', false, E);
				PC += 1;
				break;

			case 0x0C: // RRC H
				ROT('R', false, H);
				PC += 1;
				break;

			case 0x0D: // RRC L
				ROT('R', false, L);
				PC += 1;
				break;

			case 0x0E: // RRC (HL)
				ROT('R', false, memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x0F: // RRC A
				ROT('R', false, A);
				PC += 1;
				break;
			}
			break;

		case 0x10:
			switch (opcode & 0x0F)
			{
			case 0x00: // RL B
				ROT('L', true, B);
				PC += 1;
				break;

			case 0x01: // RL C
				ROT('L', true, C);
				PC += 1;
				break;

			case 0x02: // RL D
				ROT('L', true, D);
				PC += 1;
				break;

			case 0x03: // RL E
				ROT('L', true, E);
				PC += 1;
				break;

			case 0x04: // RL H
				ROT('L', true, H);
				PC += 1;
				break;

			case 0x05: // RL L
				ROT('L', true, L);
				PC += 1;
				break;

			case 0x06: // RL (HL)
				ROT('L', true, memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x07: // RL A
				ROT('L', true, A);
				PC += 1;
				break;

			case 0x08: // RR B
				ROT('R', true, B);
				PC += 1;
				break;

			case 0x09: // RR C
				ROT('R', true, C);
				PC += 1;
				break;

			case 0x0A: // RR D
				ROT('R', true, D);
				PC += 1;
				break;

			case 0x0B: // RR E
				ROT('R', true, E);
				PC += 1;
				break;

			case 0x0C: // RR H
				ROT('R', true, H);
				PC += 1;
				break;

			case 0x0D: // RR L
				ROT('R', true, L);
				PC += 1;
				break;

			case 0x0E: // RR (HL)
				ROT('R', true, memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x0F: // RR A
				ROT('R', true, A);
				PC += 1;
				break;
			}
			break;

		case 0x20:
			switch (opcode & 0x0F)
			{
			case 0x00: // SLA B
				SHIFT('L', B);
				PC += 1;
				break;

			case 0x01: // SLA C
				SHIFT('L', C);
				PC += 1;
				break;

			case 0x02: // SLA D
				SHIFT('L', C);
				PC += 1;
				break;

			case 0x03: // SLA E
				SHIFT('L', E);
				PC += 1;
				break;

			case 0x04: // SLA H
				SHIFT('L', H);
				PC += 1;
				break;

			case 0x05: // SLA L
				SHIFT('L', L);
				PC += 1;
				break;

			case 0x06: // SLA (HL)
				SHIFT('L', memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x07: // SLA A
				SHIFT('L', A);
				PC += 1;
				break;

			case 0x08: // SRA B
				SHIFT('R', B);
				PC += 1;
				break;

			case 0x09: // SRA C
				SHIFT('R', C);
				PC += 1;
				break;

			case 0x0A: // SRA D
				SHIFT('R', D);
				PC += 1;
				break;

			case 0x0B: // SRA E
				SHIFT('R', E);
				PC += 1;
				break;

			case 0x0C: // SRA H
				SHIFT('R', H);
				PC += 1;
				break;

			case 0x0D: // SRA L
				SHIFT('R', L);
				PC += 1;
				break;

			case 0x0E: // SRA (HL)
				SHIFT('R', memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x0F: // SRA A
				SHIFT('R', A);
				PC += 1;
				break;
			}
			break;

		case 0x30:
			switch (opcode & 0x0F)
			{
			case 0x00: // SWAP B
				SWAP(B);
				PC += 1;
				break;

			case 0x01: // SWAP C
				SWAP(C);
				PC += 1;
				break;

			case 0x02: // SWAP D
				SWAP(D);
				PC += 1;
				break;

			case 0x03: // SWAP E
				SWAP(E);
				PC += 1;
				break;

			case 0x04: // SWAP H
				SWAP(H);
				PC += 1;
				break;

			case 0x05: // SWAP L
				SWAP(L);
				PC += 1;
				break;

			case 0x06: // SWAP (HL)
				SWAP(memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x07: // SWAP A
				SWAP(A);
				PC += 1;
				break;

			case 0x08: // SRL B
				SHIFT('RL', B);
				PC += 1;
				break;

			case 0x09: // SRL C
				SHIFT('RL', C);
				PC += 1;
				break;

			case 0x0A: // SRL D
				SHIFT('RL', D);
				PC += 1;
				break;

			case 0x0B: // SRL E
				SHIFT('RL', E);
				PC += 1;
				break;

			case 0x0C: // SRL H
				SHIFT('RL', H);
				PC += 1;
				break;

			case 0x0D: // SRL L
				SHIFT('RL', L);
				PC += 1;
				break;

			case 0x0E: // SRL (HL)
				SHIFT('RL', memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x0F: // SRL A
				SHIFT('RL', A);
				PC += 1;
				break;
			}
			break;

		case 0x40:
			switch (opcode & 0x0F)
			{
			case 0x00: // BIT 0, B
				BIT(0, B);
				PC += 1;
				break;

			case 0x01: // BIT 0, C
				BIT(0, C);
				PC += 1;
				break;

			case 0x02: // BIT 0, D
				BIT(0, D);
				PC += 1;
				break;

			case 0x03: // BIT 0, E
				BIT(0, D);
				PC += 1;
				break;

			case 0x04: // BIT 0, H
				BIT(0, H);
				PC += 1;
				break;

			case 0x05: // BIT 0, L
				BIT(0, L);
				PC += 1;
				break;

			case 0x06: // BIT 0, (HL)
				BIT(0, memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x07: // BIT 0, A
				BIT(0, A);
				PC += 1;
				break;

			case 0x08: // BIT 1, B
				BIT(1, B);
				PC += 1;
				break;

			case 0x09: // BIT 1, C
				BIT(1, C);
				PC += 1;
				break;

			case 0x0A: // BIT 1, D
				BIT(1, D);
				PC += 1;
				break;

			case 0x0B: // BIT 1, E
				BIT(1, E);
				PC += 1;
				break;

			case 0x0C: // BIT 1, H
				BIT(1, H);
				PC += 1;
				break;

			case 0x0D: // BIT 1, L
				BIT(1, L);
				PC += 1;
				break;

			case 0x0E: // BIT 1, (HL)
				BIT(1, memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x0F: // BIT 1, A
				BIT(1, A);
				PC += 1;
				break;
			}
			break;

		case 0x50:
			switch (opcode & 0x0F)
			{
			case 0x00: // BIT 2, B
				BIT(2, B);
				PC += 1;
				break;

			case 0x01: // BIT 2, C
				BIT(2, C);
				PC += 1;
				break;

			case 0x02: // BIT 2, D
				BIT(2, D);
				PC += 1;
				break;

			case 0x03: // BIT 2, E
				BIT(2, D);
				PC += 1;
				break;

			case 0x04: // BIT 2, H
				BIT(2, H);
				PC += 1;
				break;

			case 0x05: // BIT 2, L
				BIT(2, L);
				PC += 1;
				break;

			case 0x06: // BIT 2, (HL)
				BIT(2, memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x07: // BIT 2, A
				BIT(2, A);
				PC += 1;
				break;

			case 0x08: // BIT 3, B
				BIT(2, B);
				PC += 1;
				break;

			case 0x09: // BIT 3, C
				BIT(3, C);
				PC += 1;
				break;

			case 0x0A: // BIT 3, D
				BIT(3, D);
				PC += 1;
				break;

			case 0x0B: // BIT 3, E
				BIT(3, E);
				PC += 1;
				break;

			case 0x0C: // BIT 3, H
				BIT(3, H);
				PC += 1;
				break;

			case 0x0D: // BIT 3, L
				BIT(3, L);
				PC += 1;
				break;

			case 0x0E: // BIT 3, (HL)
				BIT(3, memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x0F: // BIT 3, A
				BIT(3, A);
				PC += 1;
				break;
			}
			break;

		case 0x60:
			switch (opcode & 0x0F)
			{
			case 0x00: // BIT 4, B
				BIT(4, B);
				PC += 1;
				break;

			case 0x01: // BIT 4, C
				BIT(4, C);
				PC += 1;
				break;

			case 0x02: // BIT 4, D
				BIT(4, D);
				PC += 1;
				break;

			case 0x03: // BIT 4, E
				BIT(4, D);
				PC += 1;
				break;

			case 0x04: // BIT 4, H
				BIT(4, H);
				PC += 1;
				break;

			case 0x05: // BIT 4, L
				BIT(4, L);
				PC += 1;
				break;

			case 0x06: // BIT 4, (HL)
				BIT(4, memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x07: // BIT 4, A
				BIT(4, A);
				PC += 1;
				break;

			case 0x08: // BIT 5, B
				BIT(5, B);
				PC += 1;
				break;

			case 0x09: // BIT 5, C
				BIT(5, C);
				PC += 1;
				break;

			case 0x0A: // BIT 5, D
				BIT(5, D);
				PC += 1;
				break;

			case 0x0B: // BIT 5, E
				BIT(5, E);
				PC += 1;
				break;

			case 0x0C: // BIT 5, H
				BIT(5, H);
				PC += 1;
				break;

			case 0x0D: // BIT 5, L
				BIT(5, L);
				PC += 1;
				break;

			case 0x0E: // BIT 5, (HL)
				BIT(5, memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x0F: // BIT 5, A
				BIT(5, A);
				PC += 1;
				break;
			}
			break;

		case 0x70:
			switch (opcode & 0x0F)
			{
			case 0x00: // BIT 6, B
				BIT(6, B);
				PC += 1;
				break;

			case 0x01: // BIT 6, C
				BIT(6, C);
				PC += 1;
				break;

			case 0x02: // BIT 6, D
				BIT(6, D);
				PC += 1;
				break;

			case 0x03: // BIT 6, E
				BIT(6, D);
				PC += 1;
				break;

			case 0x04: // BIT 6, H
				BIT(6, H);
				PC += 1;
				break;

			case 0x05: // BIT 6, L
				BIT(6, L);
				PC += 1;
				break;

			case 0x06: // BIT 6, (HL)
				BIT(6, memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x07: // BIT 6, A
				BIT(6, A);
				PC += 1;
				break;

			case 0x08: // BIT 7, B
				BIT(7, B);
				PC += 1;
				break;

			case 0x09: // BIT 7, C
				BIT(7, C);
				PC += 1;
				break;

			case 0x0A: // BIT 7, D
				BIT(7, D);
				PC += 1;
				break;

			case 0x0B: // BIT 7, E
				BIT(7, E);
				PC += 1;
				break;

			case 0x0C: // BIT 7, H
				BIT(7, H);
				PC += 1;
				break;

			case 0x0D: // BIT 7, L
				BIT(7, L);
				PC += 1;
				break;

			case 0x0E: // BIT 7, (HL)
				BIT(7, memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x0F: // BIT 7, A
				BIT(7, A);
				PC += 1;
				break;
			}
			break;
		
		case 0x80:
			switch (opcode & 0x0F)
			{
			case 0x00: // RES 0, B
				modifyBit(B, 0, 0);
				PC += 1;
				break;

			case 0x01: // RES 0, C
				modifyBit(C, 0, 0);
				PC += 1;
				break;

			case 0x02: // RES 0, D
				modifyBit(D, 0, 0);
				PC += 1;
				break;

			case 0x03: // RES 0, E
				modifyBit(E, 0, 0);
				PC += 1;
				break;

			case 0x04: // RES 0, H
				modifyBit(H, 0, 0);
				PC += 1;
				break;

			case 0x05: // RES 0, L
				modifyBit(L, 0, 0);
				PC += 1;
				break;

			case 0x06: // RES 0, (HL)
				modifyBit(memory[combineReg(H, L)], 0, 0);
				PC += 1;
				break;

			case 0x07: // RES 0, A
				modifyBit(A, 0, 0);
				PC += 1;
				break;

			case 0x08: // RES 1, B
				modifyBit(B, 0, 1);
				PC += 1;
				break;

			case 0x09: // RES 1, C
				modifyBit(B, 0, 1);
				PC += 1;
				break;

			case 0x0A: // RES 1, D
				modifyBit(B, 0, 1);
				PC += 1;
				break;

			case 0x0B: // RES 1, E
				modifyBit(B, 0, 1);
				PC += 1;
				break;

			case 0x0C: // RES 1, H
				modifyBit(B, 0, 1);
				PC += 1;
				break;

			case 0x0D: // RES 1, L
				modifyBit(B, 0, 1);
				PC += 1;
				break;

			case 0x0E: // RES 1, (HL)
				modifyBit(B, 0, 1);
				PC += 1;
				break;

			case 0x0F: // RES 1, A
				modifyBit(B, 0, 1);
				PC += 1;
				break;
			}
			break;

		case 0x90:
			switch (opcode & 0x0F)
			{
			case 0x00: // RES 2, B
				modifyBit(B, 0, 2);
				PC += 1;
				break;

			case 0x01: // RES 2, C
				modifyBit(C, 0, 2);
				PC += 1;
				break;

			case 0x02: // RES 2, D
				modifyBit(D, 0, 2);
				PC += 1;
				break;

			case 0x03: // RES 2, E
				modifyBit(E, 0, 2);
				PC += 1;
				break;

			case 0x04: // RES 2, H
				modifyBit(H, 0, 2);
				PC += 1;
				break;

			case 0x05: // RES 2, L
				modifyBit(L, 0, 2);
				PC += 1;
				break;

			case 0x06: // RES 2, (HL)
				modifyBit(memory[combineReg(H, L)], 0, 2);
				PC += 1;
				break;

			case 0x07: // RES 2, A
				modifyBit(A, 0, 2);
				PC += 1;
				break;

			case 0x08: // RES 3, B
				modifyBit(B, 0, 3);
				PC += 1;
				break;

			case 0x09: // RES 3, C
				modifyBit(B, 0, 3);
				PC += 1;
				break;

			case 0x0A: // RES 3, D
				modifyBit(B, 0, 3);
				PC += 1;
				break;

			case 0x0B: // RES 3, E
				modifyBit(B, 0, 3);
				PC += 1;
				break;

			case 0x0C: // RES 3, H
				modifyBit(B, 0, 3);
				PC += 1;
				break;

			case 0x0D: // RES 3, L
				modifyBit(B, 0, 3);
				PC += 1;
				break;

			case 0x0E: // RES 3, (HL)
				modifyBit(B, 0, 3);
				PC += 1;
				break;

			case 0x0F: // RES 3, A
				modifyBit(B, 0, 3);
				PC += 1;
				break;
			}
			break;

		case 0xA0:
			switch (opcode & 0x0F)
			{
			case 0x00: // RES 4, B
				modifyBit(B, 0, 4);
				PC += 1;
				break;

			case 0x01: // RES 4, C
				modifyBit(C, 0, 4);
				PC += 1;
				break;

			case 0x02: // RES 4, D
				modifyBit(D, 0, 4);
				PC += 1;
				break;

			case 0x03: // RES 4, E
				modifyBit(E, 0, 4);
				PC += 1;
				break;

			case 0x04: // RES 4, H
				modifyBit(H, 0, 4);
				PC += 1;
				break;

			case 0x05: // RES 4, L
				modifyBit(L, 0, 4);
				PC += 1;
				break;

			case 0x06: // RES 4, (HL)
				modifyBit(memory[combineReg(H, L)], 0, 4);
				PC += 1;
				break;

			case 0x07: // RES 4, A
				modifyBit(A, 0, 4);
				PC += 1;
				break;

			case 0x08: // RES 5, B
				modifyBit(B, 0, 5);
				PC += 1;
				break;

			case 0x09: // RES 5, C
				modifyBit(B, 0, 5);
				PC += 1;
				break;

			case 0x0A: // RES 5, D
				modifyBit(B, 0, 5);
				PC += 1;
				break;

			case 0x0B: // RES 5, E
				modifyBit(B, 0, 5);
				PC += 1;
				break;

			case 0x0C: // RES 5, H
				modifyBit(B, 0, 5);
				PC += 1;
				break;

			case 0x0D: // RES 5, L
				modifyBit(B, 0, 5);
				PC += 1;
				break;

			case 0x0E: // RES 5, (HL)
				modifyBit(B, 0, 5);
				PC += 1;
				break;

			case 0x0F: // RES 5, A
				modifyBit(B, 0, 5);
				PC += 1;
				break;
			}
			break;

		case 0xB0:
			switch (opcode & 0x0F)
			{
			case 0x00: // RES 6, B
				modifyBit(B, 0, 6);
				PC += 1;
				break;

			case 0x01: // RES 6, C
				modifyBit(C, 0, 6);
				PC += 1;
				break;

			case 0x02: // RES 6, D
				modifyBit(D, 0, 6);
				PC += 1;
				break;

			case 0x03: // RES 6, E
				modifyBit(E, 0, 6);
				PC += 1;
				break;

			case 0x04: // RES 6, H
				modifyBit(H, 0, 6);
				PC += 1;
				break;

			case 0x05: // RES 6, L
				modifyBit(L, 0, 6);
				PC += 1;
				break;

			case 0x06: // RES 6, (HL)
				modifyBit(memory[combineReg(H, L)], 0, 6);
				PC += 1;
				break;

			case 0x07: // RES 6, A
				modifyBit(A, 0, 6);
				PC += 1;
				break;

			case 0x08: // RES 7, B
				modifyBit(B, 0, 7);
				PC += 1;
				break;

			case 0x09: // RES 7, C
				modifyBit(B, 0, 7);
				PC += 1;
				break;

			case 0x0A: // RES 7, D
				modifyBit(B, 0, 7);
				PC += 1;
				break;

			case 0x0B: // RES 7, E
				modifyBit(B, 0, 7);
				PC += 1;
				break;

			case 0x0C: // RES 7, H
				modifyBit(B, 0, 7);
				PC += 1;
				break;

			case 0x0D: // RES 7, L
				modifyBit(B, 0, 7);
				PC += 1;
				break;

			case 0x0E: // RES 7, (HL)
				modifyBit(B, 0, 7);
				PC += 1;
				break;

			case 0x0F: // RES 7, A
				modifyBit(B, 0, 7);
				PC += 1;
				break;
			}
			break;

		case 0xC0:
			switch (opcode & 0x0F)
			{
			case 0x00: // SET 0, B
				modifyBit(B, 1, 0);
				PC += 1;
				break;

			case 0x01: // SET 0, C
				modifyBit(C, 1, 0);
				PC += 1;
				break;

			case 0x02: // SET 0, D
				modifyBit(D, 1, 0);
				PC += 1;
				break;

			case 0x03: // SET 0, E
				modifyBit(E, 1, 0);
				PC += 1;
				break;

			case 0x04: // SET 0, H
				modifyBit(H, 1, 0);
				PC += 1;
				break;

			case 0x05: // SET 0, L
				modifyBit(L, 1, 0);
				PC += 1;
				break;

			case 0x06: // SET 0, (HL)
				modifyBit(memory[combineReg(H, L)], 1, 0);
				PC += 1;
				break;

			case 0x07: // SET 0, A
				modifyBit(A, 1, 0);
				PC += 1;
				break;

			case 0x08: // SET 1, B
				modifyBit(B, 1, 1);
				PC += 1;
				break;

			case 0x09: // SET 1, C
				modifyBit(B, 1, 1);
				PC += 1;
				break;

			case 0x0A: // SET 1, D
				modifyBit(B, 1, 1);
				PC += 1;
				break;

			case 0x0B: // SET 1, E
				modifyBit(B, 1, 1);
				PC += 1;
				break;

			case 0x0C: // SET 1, H
				modifyBit(B, 1, 1);
				PC += 1;
				break;

			case 0x0D: // SET 1, L
				modifyBit(B, 1, 1);
				PC += 1;
				break;

			case 0x0E: // SET 1, (HL)
				modifyBit(B, 1, 1);
				PC += 1;
				break;

			case 0x0F: // SET 1, A
				modifyBit(B, 1, 1);
				PC += 1;
				break;
			}
			break;

		case 0xD0:
			switch (opcode & 0x0F)
			{
			case 0x00: // SET 2, B
				modifyBit(B, 1, 2);
				PC += 1;
				break;

			case 0x01: // SET 2, C
				modifyBit(C, 1, 2);
				PC += 1;
				break;

			case 0x02: // SET 2, D
				modifyBit(D, 1, 2);
				PC += 1;
				break;

			case 0x03: // SET 2, E
				modifyBit(E, 1, 2);
				PC += 1;
				break;

			case 0x04: // SET 2, H
				modifyBit(H, 1, 2);
				PC += 1;
				break;

			case 0x05: // SET 2, L
				modifyBit(L, 1, 2);
				PC += 1;
				break;

			case 0x06: // SET 2, (HL)
				modifyBit(memory[combineReg(H, L)], 1, 2);
				PC += 1;
				break;

			case 0x07: // SET 2, A
				modifyBit(A, 1, 2);
				PC += 1;
				break;

			case 0x08: // SET 3, B
				modifyBit(B, 1, 3);
				PC += 1;
				break;

			case 0x09: // SET 3, C
				modifyBit(B, 1, 3);
				PC += 1;
				break;

			case 0x0A: // SET 3, D
				modifyBit(B, 1, 3);
				PC += 1;
				break;

			case 0x0B: // SET 3, E
				modifyBit(B, 1, 3);
				PC += 1;
				break;

			case 0x0C: // SET 3, H
				modifyBit(B, 1, 3);
				PC += 1;
				break;

			case 0x0D: // SET 3, L
				modifyBit(B, 1, 3);
				PC += 1;
				break;

			case 0x0E: // SET 3, (HL)
				modifyBit(B, 1, 3);
				PC += 1;
				break;

			case 0x0F: // SET 3, A
				modifyBit(B, 1, 3);
				PC += 1;
				break;
			}
			break;

		case 0xE0:
			switch (opcode & 0x0F)
			{
			case 0x00: // SET 4, B
				modifyBit(B, 1, 4);
				PC += 1;
				break;

			case 0x01: // SET 4, C
				modifyBit(C, 1, 4);
				PC += 1;
				break;

			case 0x02: // SET 4, D
				modifyBit(D, 1, 4);
				PC += 1;
				break;

			case 0x03: // SET 4, E
				modifyBit(E, 1, 4);
				PC += 1;
				break;

			case 0x04: // SET 4, H
				modifyBit(H, 1, 4);
				PC += 1;
				break;

			case 0x05: // SET 4, L
				modifyBit(L, 1, 4);
				PC += 1;
				break;

			case 0x06: // SET 4, (HL)
				modifyBit(memory[combineReg(H, L)], 1, 4);
				PC += 1;
				break;

			case 0x07: // SET 4, A
				modifyBit(A, 1, 4);
				PC += 1;
				break;

			case 0x08: // SET 5, B
				modifyBit(B, 1, 5);
				PC += 1;
				break;

			case 0x09: // SET 5, C
				modifyBit(B, 1, 5);
				PC += 1;
				break;

			case 0x0A: // SET 5, D
				modifyBit(B, 1, 5);
				PC += 1;
				break;

			case 0x0B: // SET 5, E
				modifyBit(B, 1, 5);
				PC += 1;
				break;

			case 0x0C: // SET 5, H
				modifyBit(B, 1, 5);
				PC += 1;
				break;

			case 0x0D: // SET 5, L
				modifyBit(B, 1, 5);
				PC += 1;
				break;

			case 0x0E: // SET 5, (HL)
				modifyBit(B, 1, 5);
				PC += 1;
				break;

			case 0x0F: // SET 5, A
				modifyBit(B, 1, 5);
				PC += 1;
				break;
			}
			break;

		case 0xF0:
			switch (opcode & 0x0F)
			{
			case 0x00: // SET 6, B
				modifyBit(B, 1, 6);
				PC += 1;
				break;

			case 0x01: // SET 6, C
				modifyBit(C, 1, 6);
				PC += 1;
				break;

			case 0x02: // SET 6, D
				modifyBit(D, 1, 6);
				PC += 1;
				break;

			case 0x03: // SET 6, E
				modifyBit(E, 1, 6);
				PC += 1;
				break;

			case 0x04: // SET 6, H
				modifyBit(H, 1, 6);
				PC += 1;
				break;

			case 0x05: // SET 6, L
				modifyBit(L, 1, 6);
				PC += 1;
				break;

			case 0x06: // SET 6, (HL)
				modifyBit(memory[combineReg(H, L)], 1, 6);
				PC += 1;
				break;

			case 0x07: // SET 6, A
				modifyBit(A, 1, 6);
				PC += 1;
				break;

			case 0x08: // SET 7, B
				modifyBit(B, 1, 7);
				PC += 1;
				break;

			case 0x09: // SET 7, C
				modifyBit(B, 1, 7);
				PC += 1;
				break;

			case 0x0A: // SET 7, D
				modifyBit(B, 1, 7);
				PC += 1;
				break;

			case 0x0B: // SET 7, E
				modifyBit(B, 1, 7);
				PC += 1;
				break;

			case 0x0C: // SET 7, H
				modifyBit(B, 1, 7);
				PC += 1;
				break;

			case 0x0D: // SET 7, L
				modifyBit(B, 1, 7);
				PC += 1;
				break;

			case 0x0E: // SET 7, (HL)
				modifyBit(B, 1, 7);
				PC += 1;
				break;

			case 0x0F: // SET 7, A
				modifyBit(B, 1, 7);
				PC += 1;
				break;
			}
			break;

		default:
			printf("Unknown opcode.\n");
			PC += 1;
		}
	}
	else
	{
		switch (opcode & 0xF0) // 8 bit instructions
		{
		case 0x00: // Done.
			switch (opcode & 0x0F)
			{
			case 0x00: // NOP
				PC += 1;
				break;

			case 0x01: // LD BC, d16
				B = memory[PC + 2];
				C = memory[PC + 1];
				PC += 3;
				break;

			case 0x02: // LD (BC), A
				memory[(B << 8) | C] = memory[A];
				PC += 1;
				break;

			case 0x03: // INC BC
				BC = combineReg(B, C);
				INC(BC);
				splitReg(B, C, BC);
				PC += 1;
				break;

			case 0x04: // INC B
				INC(B);
				PC += 1;
				break;

			case 0x05: // DEC B
				DEC(B);
				PC += 1;
				break;

			case 0x06: // LD B, d8
				B = memory[PC + 1];
				PC += 2;
				break;

			case 0x07: // RLCA
				ROT('L', false, A);
				PC += 1;
				break;

			case 0x08: // LD (a16), SP
				memory[(memory[PC + 2] << 8) | memory[PC + 1]] = SP & 0xFF;
				memory[(memory[PC + 2] << 8) | memory[PC + 1] + 1] = SP >> 8;
				PC += 3;
				break;

			case 0x09: // ADD HL, BC
				HL = combineReg(H, L);
				BC = combineReg(B, C);
				ADD(HL, BC);
				splitReg(H, L, HL);
				PC += 1;
				break;

			case 0x0A: // LD A, (BC)
				A = memory[(B << 8) | C];
				PC += 1;
				break;

			case 0x0B: // DEC BC
				BC = combineReg(B, C);
				DEC(BC);
				splitReg(B, C, BC);
				PC += 1;
				break;

			case 0x0C: // INC C
				INC(C);
				PC += 1;
				break;

			case 0x0D: // DEC C
				DEC(C);
				PC += 1;
				break;

			case 0x0E: // LD C, d8
				C = memory[PC + 1];
				PC += 2;
				break;

			case 0X0F: // RRCA
				ROT('R', false, A);
				PC += 1;
				break;
			}
			break;

		case 0x10: // Need to update STOP to do something.
			switch (opcode & 0x0F)
			{
			case 0x00: // STOP
				printf("STOP\n");
				PC += 1;
				break;

			case 0x01: // LD DE, d16
				D = memory[PC + 2];
				E = memory[PC + 1];
				PC += 3;
				break;


			case 0x02: // LD (DE), A
				memory[(D << 8) | E] = A;
				PC += 1;
				break;

			case 0x03: // INC DE
				DE = combineReg(D, E);
				INC(DE);
				splitReg(D, E, DE);
				PC += 1;
				break;

			case 0x04: // INC D
				INC(D);
				PC += 1;
				break;

			case 0x05: // DEC D
				DEC(D);
				PC += 1;
				break;

			case 0x06: // LD D, d8
				D = memory[PC + 1];
				PC += 2;
				break;

			case 0x07: // RLA
				ROT('L', true, A);
				PC += 1;
				break;

			case 0x08: // JR r8
				PC += 1;
				PC += memory[PC];
				break;

			case 0x09: // ADD HL, DE
				HL = combineReg(H, L);
				DE = combineReg(D, E);
				ADD(HL, DE);
				splitReg(H, L, HL);
				PC += 1;
				break;

			case 0x0A: // LD A, (DE)
				A = memory[(D << 8) | E];
				PC += 1;
				break;

			case 0x0B: // DEC DE
				DE = combineReg(D, E);
				DEC(DE);
				splitReg(D, E, DE);
				PC += 1;
				break;

			case 0x0C: // INC E
				INC(E);
				PC += 1;
				break;

			case 0x0D: // DEC E
				DEC(E);
				PC += 1;
				break;

			case 0x0E: // LD E, d8
				E = memory[PC + 1];
				PC += 2;
				break;

			case 0x0F: // RRA
				ROT('R', true, A);
				PC += 1;
				break;
			}
			break;

		case 0x20: // Done.
			switch (opcode & 0x0F)
			{
			case 0x00: // JR NZ, r8
				if (Zb == 0)
				{
					PC += 1;
					PC += memory[PC];
				}
				else
					PC += 2;
				break;

			case 0x01: // LD HL, d16
				H = memory[PC + 2];
				L = memory[PC + 1];
				PC += 3;
				break;

			case 0x02: // LD (HL+), A
				memory[(H << 8) | L] = A;
				HL = combineReg(H, L);
				INC(HL);
				splitReg(H, L, HL);
				PC += 1;
				break;

			case 0x03: // INC HL
				HL = combineReg(H, L);
				INC(HL);
				splitReg(H, L, HL);
				PC += 1;
				break;

			case 0x04: // INC H
				INC(H);
				PC += 1;
				break;

			case 0x05: // DEC H
				DEC(H);
				PC += 1;
				break;

			case 0x06: // LD H, d8
				H = memory[PC + 1];
				PC += 2;
				break;

			case 0x07: // DAA
			{
				if (A > 99)
					Cb = 1;
				else
					Cb = 0;
				Zb = checkZero(A);
				Hb = 0;
				unsigned char d1 = A % 10;
				A /= 10;
				unsigned char d2 = A % 10;
				A = (d2 << 4) | d1;
			}

			case 0x08: // JR Z, r8
				if (Zb == 1)
				{
					PC += 1;
					PC += memory[PC];
				}
				else
					PC += 2;
				break;

			case 0x09: // ADD HL, HL
				HL = combineReg(H, L);
				ADD(HL, HL);
				splitReg(H, L, HL);
				PC += 1;
				break;

			case 0x0A: // LD A, (HL+)
				A = memory[(H << 8) | L];
				HL = combineReg(H, L);
				INC(HL);
				splitReg(H, L, HL);
				PC += 1;
				break;

			case 0x0B: // DEC HL
				HL = combineReg(H, L);
				DEC(HL);
				splitReg(H, L, HL);
				PC += 1;
				break;

			case 0x0C: // INC L
				INC(L);
				PC += 1;
				break;

			case 0x0D: // DEC L
				DEC(L);
				PC += 1;
				break;

			case 0x0E: // LD L, d8
				L = memory[PC + 1];
				PC += 2;
				break;

			case 0x0F: // CPL
				A = ~A;
				Nb = 1;
				Hb = 1;
				PC += 1;
				break;
			}
			break;

		case 0x30: // Done.
			switch (opcode & 0x0F)
			{
			case 0x00: // JR NC, r8
				if (Cb == 0)
				{
					PC += 1;
					PC += memory[PC];
				}
				else
					PC += 2;
				break;

			case 0x01: // LD SP, d16
				SP = (memory[PC + 2] << 8) | memory[PC + 1];
				PC += 3;
				break;

			case 0x02: // LD (HL-), A
				memory[(H << 8) | L] = A;
				HL = combineReg(H, L);
				DEC(HL);
				splitReg(H, L, HL);
				PC += 1;
				break;

			case 0x03: // INC SP
				INC(SP);
				PC += 1;
				break;

			case 0x04: // INC (HL)
				INC(memory[(H << 8) | L]);
				PC += 1;
				break;

			case 0x05: // DEC (HL)
				DEC(memory[(H << 8) | L]);
				PC += 1;
				break;

			case 0x06: // LD (HL), d8
				memory[(H << 8) | L] = memory[PC + 1];
				PC += 2;
				break;

			case 0x07: // SCF
				Cb = 1;
				Nb = 0;
				Hb = 0;
				PC += 1;
				break;

			case 0x08: // JR C, r8
				if (Cb == 1)
				{
					PC += 1;
					PC += memory[PC];
				}
				else
					PC += 2;
				break;

			case 0x09: // ADD HL, SP
				HL = combineReg(H, L);
				ADD(HL, SP);
				splitReg(H, L, HL);
				PC += 1;
				break;

			case 0x0A: // LD A, (HL-)
				A = memory[(H << 8) | L];
				HL = combineReg(H, L);
				DEC(HL);
				splitReg(H, L, HL);
				PC += 1;
				break;

			case 0x0B: // DEC SP
				DEC(SP);
				PC += 1;
				break;

			case 0x0C: // INC A
				INC(A);
				PC += 1;
				break;

			case 0x0D: // DEC A
				DEC(A);
				PC += 1;
				break;

			case 0x0E: // LD A, d8
				A = memory[PC + 1];
				PC += 2;
				break;

			case 0x0F: // CCF
				Cb = ~Cb;
				Nb = 0;
				Hb = 0;
				PC += 1;
				break;
			}
			break;

		case 0x40: // Done.
			switch (opcode & 0x0F)
			{
			case 0x00: // LD B, B
				B = B;
				PC += 1;
				break;

			case 0x01: // LD B, C
				B = C;
				PC += 1;
				break;

			case 0x02: // LD B, D
				B = D;
				PC += 1;
				break;

			case 0x03: // LD B, E
				B = E;
				PC += 1;
				break;

			case 0x04: // LD B, H
				B = H;
				PC += 1;
				break;

			case 0x05: // LD B, L
				B = L;
				PC += 1;
				break;

			case 0x06: // LD B, (HL)
				B = memory[(H << 8) | L];
				PC += 1;
				break;

			case 0x07: // LD B, A
				B = A;
				PC += 1;
				break;

			case 0x08: // LD C, B
				C = B;
				PC += 1;
				break;

			case 0x09: // LD C, C
				C = C;
				PC += 1;
				break;

			case 0x0A: // LD C, D
				C = D;
				PC += 1;
				break;

			case 0x0B: // LD C, E
				C = E;
				PC += 1;
				break;

			case 0x0C: // LD C, H
				C = H;
				PC += 1;
				break;

			case 0x0D: // LD C, L
				C = L;
				PC += 1;
				break;

			case 0x0E: // LD C, (HL)
				C = memory[(H << 8) | L];
				PC += 1;
				break;

			case 0x0F: // LD C, A
				C = A;
				PC += 1;
				break;
			}
			break;

		case 0x50: // Done.
			switch (opcode & 0x0F)
			{
			case 0x00: // LD D, B
				D = B;
				PC += 1;
				break;

			case 0x01: // LD D, C
				D = C;
				PC += 1;
				break;

			case 0x02: // LD D, D
				D = D;
				PC += 1;
				break;

			case 0x03: // LD D, E
				D = E;
				PC += 1;
				break;

			case 0x04: // LD D, H
				D = H;
				PC += 1;
				break;

			case 0x05: // LD D, L
				D = L;
				PC += 1;
				break;

			case 0x06: // LD D, (HL)
				D = memory[(H << 8) | L];
				PC += 1;
				break;

			case 0x07: // LD D, A
				D = A;
				PC += 1;
				break;

			case 0x08: // LD D, B
				D = B;
				PC += 1;
				break;

			case 0x09: // LD D, C
				D = C;
				PC += 1;
				break;

			case 0x0A: // LD D, D
				D = D;
				PC += 1;
				break;

			case 0x0B: // LD D, E
				D = E;
				PC += 1;
				break;

			case 0x0C: // LD D, H
				D = H;
				PC += 1;
				break;

			case 0x0D: // LD D, L
				D = L;
				PC += 1;
				break;

			case 0x0E: // LD D, (HL)
				D = memory[(H << 8) | L];
				PC += 1;
				break;

			case 0x0F: // LD D, A
				D = A;
				PC += 1;
				break;
			}
			break;

		case 0x60: // Done.
			switch (opcode & 0x0F)
			{
			case 0x00: // LD H, B
				H = B;
				PC += 1;
				break;

			case 0x01: // LD H, C
				H = C;
				PC += 1;
				break;

			case 0x02: // LD H, D
				H = D;
				PC += 1;
				break;

			case 0x03: // LD H, E
				H = E;
				PC += 1;
				break;

			case 0x04: // LD H, H
				H = H;
				PC += 1;
				break;

			case 0x05: // LD H, L
				H = L;
				PC += 1;
				break;

			case 0x06: // LD H, (HL)
				H = memory[(H << 8) | L];
				PC += 1;
				break;

			case 0x07: // LD H, A
				H = A;
				PC += 1;
				break;

			case 0x08: // LD H, B
				H = B;
				PC += 1;
				break;

			case 0x09: // LD H, C
				H = C;
				PC += 1;
				break;

			case 0x0A: // LD H, D
				H = D;
				PC += 1;
				break;

			case 0x0B: // LD H, E
				H = E;
				PC += 1;
				break;

			case 0x0C: // LD H, H
				H = H;
				PC += 1;
				break;

			case 0x0D: // LD H, L
				H = L;
				PC += 1;
				break;

			case 0x0E: // LD H, (HL)
				H = memory[(H << 8) | L];
				PC += 1;
				break;

			case 0x0F: // LD H, A
				H = A;
				PC += 1;
				break;
			}
			break;

		case 0x70: // Need to update HALT to do something.
			switch (opcode & 0x0F)
			{
			case 0x00: // LD (HL), B
				memory[(H << 8) | L] = B;
				PC += 1;
				break;

			case 0x01: // LD (HL), C
				memory[(H << 8) | L] = C;
				PC += 1;
				break;

			case 0x02: // LD (HL), D
				memory[(H << 8) | L] = D;
				PC += 1;
				break;

			case 0x03: // LD (HL), E
				memory[(H << 8) | L] = E;
				PC += 1;
				break;

			case 0x04: // LD (HL), H
				memory[(H << 8) | L] = H;
				PC += 1;
				break;

			case 0x05: // LD (HL), L
				memory[(H << 8) | L] = L;
				PC += 1;
				break;

			case 0x06: // HALT
				printf("HALT\n");
				PC += 1;
				break;

			case 0x07: // LD (HL), A
				memory[(H << 8) | L] = A;
				PC += 1;
				break;

			case 0x08: // LD A, B
				A = B;
				PC += 1;
				break;

			case 0x09: // LD A, C
				A = C;
				PC += 1;
				break;

			case 0x0A: // LD A, D
				A = D;
				PC += 1;
				break;

			case 0x0B: // LD A, E
				A = E;
				PC += 1;
				break;

			case 0x0C: // LD A, H
				A = H;
				PC += 1;
				break;

			case 0x0D: // LD A, L
				A = L;
				PC += 1;
				break;

			case 0x0E: // LD A, (HL)
				A = memory[(H << 8) | L];
				PC += 1;
				break;

			case 0x0F: // LD A, A
				A = A;
				PC += 1;
				break;
			}
			break;

		case 0x80: // Done.
			switch (opcode & 0x0F)
			{
			case 0x00: // ADD A, B
				ADD(A, B);
				PC += 1;
				break;

			case 0x01: // ADD A, C
				ADD(A, C);
				PC += 1;
				break;

			case 0x02: // ADD A, D
				ADD(A, D);
				PC += 1;
				break;

			case 0x03: // ADD A, E
				ADD(A, E);
				PC += 1;
				break;

			case 0x04: // ADD A, H
				ADD(A, H);
				PC += 1;
				break;

			case 0x05: // ADD A, L
				ADD(A, L);
				PC += 1;
				break;

			case 0x06: // ADD A, (HL)
				ADD(A, memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x07: // ADD A, A
				ADD(A, A);
				PC += 1;
				break;

			case 0x08: // ADC A, B
				ADD(A, B + Cb);
				PC += 1;
				break;

			case 0x09: // ADC A, C
				ADD(A, C + Cb);
				PC += 1;
				break;

			case 0x0A: // ADC A, D
				ADD(A, D + Cb);
				PC += 1;
				break;

			case 0x0B: // ADC A, E
				ADD(A, E + Cb);
				PC += 1;
				break;

			case 0x0C: // ADC A, H
				ADD(A, H + Cb);
				PC += 1;
				break;

			case 0x0D: // ADC A, L
				ADD(A, L + Cb);
				PC += 1;
				break;

			case 0x0E: // ADC A, (HL)
				ADD(A, memory[combineReg(H, L)] + Cb);
				PC += 1;
				break;

			case 0x0F: // ADC A, A
				ADD(A, A + Cb);
				PC += 1;
				break;
			}
			break;

		case 0x90: // Done.
			switch (opcode & 0x0F)
			{
			case 0x00: // SUB B
				SUB(B);
				PC += 1;
				break;

			case 0x01: // SUB C
				SUB(C);
				PC += 1;
				break;

			case 0x02: // SUB D
				SUB(D);
				PC += 1;
				break;

			case 0x03: // SUB E
				SUB(E);
				PC += 1;
				break;

			case 0x04: // SUB H
				SUB(H);
				PC += 1;
				break;

			case 0x05: // SUB L
				SUB(L);
				PC += 1;
				break;

			case 0x06: // SUB (HL)
				SUB(memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x07: // SUB A
				SUB(A);
				PC += 1;
				break;

			case 0x08: // SBC A, B
				SUB(B + Cb);
				PC += 1;
				break;

			case 0x09: // SBC A, C
				SUB(C + Cb);
				PC += 1;
				break;

			case 0x0A: // SBC A, D
				SUB(D + Cb);
				PC += 1;
				break;

			case 0x0B: // SBC A, E
				SUB(E + Cb);
				PC += 1;
				break;

			case 0x0C: // SBC A, H
				SUB(H + Cb);
				PC += 1;
				break;

			case 0x0D: // SBC A, L
				SUB(L + Cb);
				PC += 1;
				break;

			case 0x0E: // SBC A, (HL)
				SUB(memory[combineReg(H, L)] + Cb);
				PC += 1;
				break;

			case 0x0F: // SBC A, A
				SUB(A + Cb);
				PC += 1;
				break;
			}
			break;

		case 0xA0: // Done.
			switch (opcode & 0x0F)
			{
			case 0x00: // AND B
				AND(B);
				PC += 1;
				break;

			case 0x01: // AND C
				AND(C);
				PC += 1;
				break;

			case 0x02: // AND D
				AND(D);
				PC += 1;
				break;

			case 0x03: // AND E
				AND(E);
				PC += 1;
				break;

			case 0x04: // AND H
				AND(H);
				PC += 1;
				break;

			case 0x05: // AND L
				AND(L);
				PC += 1;
				break;

			case 0x06: // AND (HL)
				AND(memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x07: // AND A
				AND(A);
				PC += 1;
				break;

			case 0x08: // XOR B
				XOR(B);
				PC += 1;
				break;

			case 0x09: // XOR C
				XOR(C);
				PC += 1;
				break;

			case 0x0A: // XOR D
				XOR(D);
				PC += 1;
				break;

			case 0x0B: // XOR E
				XOR(E);
				PC += 1;
				break;

			case 0x0C: // XOR H
				XOR(H);
				PC += 1;
				break;

			case 0x0D: // XOR L
				XOR(L);
				PC += 1;
				break;

			case 0x0E: // XOR (HL)
				XOR(memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x0F: // XOR A
				XOR(A);
				PC += 1;
				break;
			}
			break;

		case 0xB0: // Done.
			switch (opcode & 0x0F)
			{
			case 0x00: // OR B
				OR(B);
				PC += 1;
				break;

			case 0x01: // OR C
				OR(C);
				PC += 1;
				break;

			case 0x02: // OR D
				OR(D);
				PC += 1;
				break;

			case 0x03: // OR E
				OR(E);
				PC += 1;
				break;

			case 0x04: // OR H
				OR(H);
				PC += 1;
				break;

			case 0x05: // OR L
				OR(L);
				PC += 1;
				break;

			case 0x06: // OR (HL)
				OR(memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x07: // OR A
				OR(A);
				PC += 1;
				break;

			case 0x08: // CP B
				CP(B);
				PC += 1;
				break;

			case 0x09: // CP C
				CP(C);
				PC += 1;
				break;

			case 0x0A: // CP D
				CP(D);
				PC += 1;
				break;

			case 0x0B: // CP E
				CP(E);
				PC += 1;
				break;

			case 0x0C: // CP H
				CP(H);
				PC += 1;
				break;

			case 0x0D: // CP L
				CP(L);
				PC += 1;
				break;

			case 0x0E: // CP (HL)
				CP(memory[combineReg(H, L)]);
				PC += 1;
				break;

			case 0x0F: // CP A
				CP(A);
				PC += 1;
				break;
			}
			break;

		case 0xC0: // Done.
			switch (opcode & 0x0F)
			{
			case 0x00: // RET NZ
				if (Zb == 0)
				{
					PC = (memory[SP + 1] << 8) | memory[SP];
					SP += 2;
				}
				else
				{
					PC += 1;
				}
				break;

			case 0x01: // POP BC
				B = memory[SP + 1];
				C = memory[SP];
				SP += 2;
				PC += 1;
				break;

			case 0x02: // JP NZ, a16
				if (Zb == 0)
				{
					PC = (memory[PC + 2] << 8) | memory[PC + 1];
				}
				else
				{
					PC += 3;
				}
				break;

			case 0x03: // JP a16
				PC = (memory[PC + 2] << 8) | memory[PC + 1];
				break;

			case 0x04: // CALL NZ, a16
				if (Zb == 0)
					CALL();
				else
					PC += 3;
				break;

			case 0x05: // PUSH BC
				memory[SP - 1] = B;
				memory[SP - 2] = C;
				SP -= 2;
				PC += 1;
				break;

			case 0x06: // ADD A, d8
				ADD(A, memory[PC + 1]);
				PC += 2;
				break;

			case 0x07: // RST 00H
				memory[SP - 1] = PC >> 8;
				memory[SP - 2] = PC & 0xFF;
				SP -= 2;
				PC = 0x0;
				break;

			case 0x08: // RET Z
				if (Zb == 1)
				{
					PC = (memory[SP + 1] << 8) | memory[SP];
					SP += 2;
				}
				else
				{
					PC += 1;
				}
				break;

			case 0x09: // RET
				PC = (memory[SP + 1] << 8) | memory[SP];
				SP += 2;
				break;

			case 0x0A: // JP Z, a16
				if (Zb == 1)
				{
					PC = (memory[PC + 2] << 8) | memory[PC + 1];
				}
				else
				{
					PC += 3;
				}
				break;

			case 0x0C: // CALL Z, a16
				if (Zb == 1)
					CALL();
				else
					PC += 3;
				break;

			case 0x0D: // CALL a16
				CALL();
				break;

			case 0x0E: // ADC A, d8
				ADD(A, memory[PC + 1] + Cb);
				PC += 2;
				break;

			case 0x0F: // RST 08H
				memory[SP - 1] = PC >> 8;
				memory[SP - 2] = PC & 0xFF;
				SP -= 2;
				PC = 0x08;
				break;
			}
			break;

		case 0xD0: // Done.
			switch (opcode & 0x0F)
			{
			case 0x00: // RET NC
				if (Nb == 0)
				{
					PC = (memory[SP + 1] << 8) | memory[SP];
					SP += 2;
				}
				else
				{
					PC += 1;
				}
				break;

			case 0x01: // POP DE
				D = memory[SP + 1];
				E = memory[SP];
				SP += 2;
				PC += 1;
				break;

			case 0x02: // JP NC, a16
				if (Cb == 0)
				{
					PC = (memory[PC + 2] << 8) | memory[PC + 1];
				}
				else
				{
					PC += 3;
				}
				break;

			case 0x04: // CALL NC, a16
				if (Cb == 0)
					CALL();
				else
					PC += 3;
				break;

			case 0x05: // PUSH DE
				memory[SP - 1] = D;
				memory[SP - 2] = E;
				SP -= 2;
				PC += 1;
				break;

			case 0x06: // SUB d8
				SUB(memory[PC + 1]);
				PC += 2;
				break;

			case 0x07: // RST 10H
				memory[SP - 1] = PC >> 8;
				memory[SP - 2] = PC & 0xFF;
				SP -= 2;
				PC = 0x10;
				break;

			case 0x08: // RET C
				if (Cb == 1)
				{
					PC = (memory[SP + 1] << 8) | memory[SP];
					SP += 2;
				}
				else
				{
					PC += 1;
				}
				break;

			case 0x09: // RETI
				PC = (memory[SP + 1] << 8) | memory[SP];
				SP += 2;
				IME = 1;
				break;

			case 0x0A: // JP C, a16
				if (Cb == 1)
				{
					PC = (memory[PC + 2] << 8) | memory[PC + 1];
				}
				else
				{
					PC += 3;
				}
				break;

			case 0x0C: // CALL C, a16
				if (Cb == 1)
					CALL();
				else
					PC += 3;
				break;

			case 0x0E: // SBC A, d8
				SUB(memory[PC + 1] + Cb);
				PC += 2;
				break;

			case 0x0F: // RST 18H
				memory[SP - 1] = PC >> 8;
				memory[SP - 2] = PC & 0xFF;
				PC = 0x18;
				break;
			}
			break;

		case 0xE0: // Done.
			switch (opcode & 0x0F)
			{
			case 0x00: // LDH (a8), A
				memory[memory[PC + 1] + 0xFF00] = A;
				PC += 2;
				break;

			case 0x01: // POP HL
				H = memory[SP + 1];
				L = memory[SP];
				SP += 2;
				PC += 1;
				break;

			case 0x02: // LD (C), A
				memory[C] = A;
				PC += 1;
				break;

			case 0x05: // PUSH HL
				memory[SP - 1] = H;
				memory[SP - 2] = L;
				SP -= 2;
				PC += 1;
				break;

			case 0x06: // AND d8
				AND(memory[PC + 1]);
				PC += 2;
				break;

			case 0x07: // RST 20H
				memory[SP - 1] = PC >> 8;
				memory[SP - 2] = PC & 0xFF;
				SP -= 2;
				PC = 0x20;
				break;

			case 0x08: // ADD SP, r8
				ADD(SP, memory[PC + 1]);
				PC += 2;
				break;

			case 0x09: // JP HL
				PC = (H << 8) | L;
				break;

			case 0x0A: // LD (a16), A
				memory[memory[PC + 2] | memory[PC + 1]] = A;
				PC += 3;
				break;

			case 0x0E: // XOR d8
				XOR(memory[PC + 1]);
				PC += 2;
				break;

			case 0x0F: // RST 28H
				memory[SP - 1] = PC >> 8;
				memory[SP - 2] = PC & 0xFF;
				SP -= 2;
				PC = 0x28;
				break;
			}
			break;

		case 0xF0: // Done.
			switch (opcode & 0x0F)
			{
			case 0x00: // LDH A, (a8)
				A = memory[memory[PC + 1] + 0xFF00];
				PC += 2;
				break;

			case 0x01: // POP AF
				A = memory[SP + 1];
				F = memory[SP];
				SP += 2;
				PC += 1;
				break;

			case 0x02: // LD A, (C)
				A = memory[C];
				PC += 1;
				break;

			case 0x03: // DI
				printf("DI\n");
				IME = 0;
				PC += 1;
				break;

			case 0x05: // PUSH AF
				memory[SP - 1] = A;
				memory[SP - 2] = F;
				SP -= 2;
				PC += 1;
				break;

			case 0x06: // OR d8
				OR(memory[PC + 1]);
				PC += 2;
				break;

			case 0x07: // RST 30H
				memory[SP - 1] = PC >> 8;
				memory[SP - 2] = PC & 0xFF;
				SP -= 2;
				PC = 0x30;
				break;

			case 0x08: // LD HL, SP + r8
				H = ((SP + memory[PC + 1]) >> 8) & 0xFF;
				L = (SP + memory[PC + 1]) & 0xFF;
				PC += 3;
				Zb = 0;
				Nb = 0;
				Hb = checkHalfCarry(SP, memory[PC + 1], 1);
				if (SP > unsigned int(0xFFFF) - memory[PC + 1])
					Cb = 1;
				else
					Cb = 0;
				PC += 2;
				break;

			case 0x09: // LD SP, HL
				SP = (H << 8) | L;
				PC += 1;
				break;

			case 0x0A: // LD A, (a16)
				A = (memory[PC + 2] << 8) | memory[PC + 1];
				PC += 3;
				break;

			case 0x0B: // EI
				printf("EI\n");
				enableInterrupts = true;
				PC += 1;
				break;

			case 0x0E: // CP d8
				CP(memory[PC + 1]);
				PC += 2;
				break;

			case 0x0F: // RST 38H
				memory[SP - 1] = PC >> 8;
				memory[SP - 2] = PC & 0xFF;
				SP -= 2;
				PC = 0x38;
				break;
			}
			break;

		default:
			printf("Unknown opcode.\n");
			PC += 1;
		}

	}
	
	setFlags();
	if (enableInterrupts && opcode != 0x0B)
	{
		IME = 1;
		enableInterrupts = false;
	}
	//Sleep(1000);
}

// Set the values of the flag bits in register F.
void gb::setFlags()
{
	// Set each flag bit to the value it should be in the register F.
	modifyBit(F, Zb, 7);
	modifyBit(F, Nb, 6);
	modifyBit(F, Hb, 5);
	modifyBit(F, Cb, 4);
}

// Set or reset a bit in a byte, at a given position.
void gb::modifyBit(unsigned char &r, int val, int pos)
{
	if (val == 0)
	{
		r &= ~(0x1 << pos);
	}
	else if (val == 1)
	{
		r |= 0x1 << pos;
	}
}

// Check if the half-carry bit should be set for an addition/subtraction of two bytes.
int gb::checkHalfCarry(unsigned char a, unsigned char b, int mode)
{
	if (mode == 1)
		if ((((a & 0xf) + (b & 0xf)) & 0x10) == 0x10)
			return 1;
		else
			return 0;
	else if (mode == -1)
		if ((((a & 0xf) - (b & 0xf)) < 0))
			return 1;
		else
			return 0;
	return Hb; // If wrong mode entered for function, don't change the bit.
}

// No idea if this is correct :)
int gb::checkHalfCarry(unsigned int a, unsigned char b, int mode)
{
	if (mode == 1)
		if ((((a & 0xfff) + (b & 0xfff)) & 0x1000) == 0x1000)
			return 1;
		else
			return 0;
	else if (mode == -1)
		if ((((a & 0xfff) - (b & 0xfff)) < 0xfff))
			return 1;
		else
			return 0;
	return Hb; // If wrong mode entered for function, don't change the bit.
}

// Check if a value equals 0. Used to set the zero flag bit.
int gb::checkZero(char a)
{
	if (a == 0)
		return 1;
	else
		return 0;
}

// Increment a byte.
void gb::INC(unsigned char &val)
{
	Hb = checkHalfCarry(val, 0x1, 1);
	val += 1;
	Zb = checkZero(val);
	Nb = 0;
}

// Increment a 16-bit value.
void gb::INC(unsigned int &val)
{
	val += 1;
}

// Decrement a byte.
void gb::DEC(unsigned char &r)
{
	Hb = checkHalfCarry(r, 0x1, -1);
	r -= 1;
	Zb = checkZero(r);
	Nb = 1;
}

// Decrement a 16-bit value.
void gb::DEC(unsigned int &val)
{
	val -= 1;
}

// Rotate the contents of a register left or right. Can rotate normally or through carry.
void gb::ROT(char dir, bool carry, unsigned char &r)
{
	unsigned int oldCb = Cb;
	if (dir == 'L')
	{
		Cb = (r >> 7) & 0x1;
		r <<= 1;

		if (carry)
			modifyBit(r, oldCb, 0);
		else
			modifyBit(r, Cb, 0);

		
	}
	else if (dir == 'R')
	{
		Cb = r & 0x1;
		r >>= 1;
		if (carry)
			modifyBit(r, oldCb, 7);
		else
			modifyBit(r, Cb, 7);
	}
	if (r == 0)
		Zb = 1;
	else
		Zb = 0;
	Nb = 0;
	Hb = 0;
}

// Shift the contents of a register left or right.
void gb::SHIFT(char mode, unsigned char& r)
{
	if (mode == 'L')
	{
		Cb = (r >> 7) & 0x1;
		r <<= 1;
	}
	else if (mode == 'R')
	{
		Cb = r & 0x1;
		r >>= 1;
		if ((r >> 6) & 0x1 == 0)
			modifyBit(r, 7, 0);
		else
			modifyBit(r, 7, 1);
	}
	else if (mode == 'RL')
	{
		Cb = r & 0x1;
		r >>= 1;
	}

	if (r == 0)
		Zb = 1;
	else
		Zb = 0;
	Nb = 0;
	Hb = 0;
}

// Adds a byte value to another byte, with or without adding the carry bit.
void gb::ADD(unsigned char &val1, unsigned char val2)
{
	if (val1 > (0xFF - val2))
		Cb = 1;
	else
		Cb = 0;
	Hb = checkHalfCarry(val1, val2, 1);
	val1 += val2;
	Zb = checkZero(val1);
	Nb = 0;
}

// Adds a 16-bit value to another 16-bit value.
// NOTE: Maybe write method for half-carry with two 16-bit values?
void gb::ADD(unsigned int &val1, unsigned int val2)
{
	if (val1 > (unsigned int(0xFFFF) - val2))
		Cb = 1;
	else
		Cb = 0;

	if (val1 > unsigned int(0xFFF) - val2)
		Hb = 1;
	else
		Hb = 0;
	val1 += val2;
	Nb = 0;
}

void gb::SUB(unsigned char val)
{
	if (val > A)
		Cb = 1;
	else
		Cb = 0;

	Hb = checkHalfCarry(A, val, -1);
	A -= val;
	Zb = checkZero(A);
	Nb = 1;
}

void gb::AND(unsigned char val)
{
	A &= val;
	Zb = checkZero(A);
	Nb = 0;
	Hb = 1;
	Cb = 0;
}

void gb::XOR(unsigned char val)
{
	A ^= val;
	Zb = checkZero(A);
	Nb = 0;
	Hb = 0;
	Cb = 0;
}

void gb::OR(unsigned char val)
{
	A |= val;
	Zb = checkZero(A);
	Nb = 0;
	Hb = 0;
	Cb = 0;
}

void gb::CP(unsigned char val)
{
	if (val > A)
		Cb = 1;
	else
		Cb = 0;

	Hb = checkHalfCarry(A, val, -1);
	Zb = checkZero(A - val);
	Nb = 1;
}

void gb::CALL()
{
	memory[SP - 1] = memory[PC + 3];
	memory[SP - 2] = memory[PC + 4];
	PC = (memory[PC + 2] << 8) | memory[PC + 1];
	SP -= 2;
}

void gb::SWAP(unsigned char &val)
{
	val = val << 4 | val >> 4;
	if (val == 0)
		Zb = 1;
	else
		Zb = 0;
	Nb = 0;
	Hb = 0;
	Cb = 0;
}

void gb::BIT(char pos, unsigned char r)
{
	char bit = (r >> pos) & 0x1;
	if (bit == 0)
		Zb = 1;
	else
		Zb = 0;
	Nb = 0;
	Hb = 1;
}

// Returns the result of combining two 8-bit register values together.
// NOTE: Maybe use for combining values in memory as well?
int gb::combineReg(unsigned char r1, unsigned char r2)
{
	return (r1 << 8) | r2;
}

// Splits a 16-bit register into two, and stores the resulting bytes in two other registers.
void gb::splitReg(unsigned char &r1, unsigned char &r2, unsigned int r3)
{
	r1 = r3 >> 8 & 0xFF;
	r2 = r3 & 0xFF;
}