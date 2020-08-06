#include "gb.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

void gb::initialize()
{
	PC = 0x0;
}

void gb::loadGame()
{
	streampos size;
	char* memblock;

	ifstream file("cpu_instrs.gb", ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char[size];
		file.seekg(0, ios::beg);
		file.read(memblock, size);
		file.close();
		printf("File loaded\n");
		for (int i = 0; i < size; i++)
			memory[i] = memblock[i];
		delete[] memblock;
	}
	else printf("Unable to open file\n");
}

void gb::emulateCycle()
{
	// Get the opcode and execute the required instructions.
	opcode = memory[PC];
	char opcodeStr[3];
	_itoa_s(opcode, opcodeStr, 16);
	printf("Opcode is %s.\n", opcodeStr);

	switch (opcode & 0xF0) // 8 bit instructions
	{
	case 0x00:
		switch (opcode & 0x0F)
		{
		case 0x00: // NOP
			PC += 1;
			break;

		case 0x01: // LD BC, d16
			B = memory[PC + 1];
			C = memory[PC + 2];
			PC += 3;
			break;

		case 0x02: // LD (BC), A
			memory[(B << 8) | C] = memory[A];
			PC += 1;
			break;

		case 0x03: // INC BC
			BC = (B << 8) | C;
			BC += 1;
			B = BC >> 8 & 0xFF;
			C = BC & 0xFF;
			PC += 1;
			break;

		case 0x04: // INC B
			Hb = checkHalfCarry(B, 0x1, 1);
			B += 1;
			PC += 1;
			Zb = checkZero(B);
			Nb = 0;
			break;

		case 0x05: // DEC B
			Hb = checkHalfCarry(B, 0x1, -1);
			B -= 1;
			PC += 1;
			Zb = checkZero(B);
			Nb = 1;
			break;

		case 0x06: // LD B, d8
			B = memory[PC + 1];
			PC += 2;
			break;

		case 0x07: // RLCA
			Cb = (A >> 7) & 0b1;
			A <<= 1;
			PC += 1;
			Zb = 0;
			Nb = 0;
			Hb = 0;
			break;

		case 0x08: // LD (a16), SP
			memory[(memory[PC + 1] << 8) | memory[PC + 2]] = SP;
			PC += 3;
			break;

		case 0x09: // ADD HL, BC
			HL = (H << 8) | L;
			BC = (B << 8) | C;

			if (HL > (0xFFFF - BC))
				Cb = 1;
			else
				Cb = 0;

			HL += BC;
			H = HL >> 8 & 0xFF;
			L = HL & 0xFF;
			PC += 1;
			Nb = 0;
			Hb = checkHalfCarry(HL, BC, 1);
			break;

		case 0x0A: // LD A, (BC)
			A = memory[(B << 8) | C];
			PC += 1;
			break;

		case 0x0B: // DEC BC
			BC = (B << 8) | C;
			BC -= 1;
			B = BC >> 8 & 0xFF;
			C = BC & 0xFF;
			PC += 1;
			break;

		case 0x0C: // INC C
			C += 1;
			PC += 1;
			Zb = checkZero(C);
			Nb = 0;
			Hb = checkHalfCarry(C, 0x1, 1);
			break;

		case 0x0D: // DEC C
			Hb = checkHalfCarry(C, 1, -1);
			C -= 1;
			PC += 1;
			Zb = 0;
			Nb = 1;
			break;

		case 0x0E: // LD C, d8
			C = memory[PC + 1];
			PC += 2;
			break;

		case 0X0F: // RRCA
			Cb = A & 0b1;
			A >>= 1;
			PC += 1;
			Zb = 0;
			Nb = 0;
			Hb = 0;
			break;
		}

	case 0x10:
		switch (opcode & 0x0F)
		{
		case 0x00: // STOP
			IME = 0;
			PC += 1;
			break;

		case 0x01: // LD DE, d16
			D = memory[PC + 1];
			E = memory[PC + 2];
			PC += 3;
			break;

		case 0x02: // LD (DE), A
			memory[(D << 8) | E] = memory[A];
			PC += 1;
			break;

		case 0x03: // INC DE
			DE = (D << 8) | E;
			DE += 1;
			D = DE >> 8 & 0xFF;
			E = DE & 0xFF;
			PC += 1;
			break;

		case 0x04: // INC D
			Hb = checkHalfCarry(D, 0x1, 1);
			D += 1;
			PC += 1;
			Zb = checkZero(D);
			Nb = 0;
			break;

		case 0x05: // DEC D
			Hb = checkHalfCarry(D, 0x1, -1);
			D -= 1;
			PC += 1;
			Zb = checkZero(D);
			Nb = 1;
			break;

		case 0x06: // LD D, d8
			D = memory[PC + 1];
			PC += 2;
			break;

		case 0x07: // RLA
			oldCb = Cb;
			Cb = (A >> 7) & 0b1;
			A <<= 1;
			if (oldCb == 1)
				A |= 0x1;
			else
				A &= ~0x1;
			PC += 1;
			Zb = 0;
			Nb = 0;
			Hb = 0;
			break;

		case 0x08: // JR r8
			PC += memory[PC + 1];
			break;

		case 0x09: // ADD HL, DE
			HL = (H << 8) | L;
			DE = (D << 8) | E;

			if (HL > (0xFFFF - DE))
				Cb = 1;
			else
				Cb = 0;
			Hb = checkHalfCarry(HL, DE, 1);
			HL += DE;
			H = HL >> 8 & 0xFF;
			L = HL & 0xFF;
			PC += 1;
			Nb = 0;
			break;

		case 0x0A: // LD A, (DE)
			A = memory[(D << 8) | E];
			PC += 1;
			break;

		case 0x0B: // DEC DE
			DE = (D << 8) | E;
			DE -= 1;
			D = DE >> 8 & 0xFF;
			E = DE & 0xFF;
			PC += 1;
			break;

		case 0x0C: // INC E
			Hb = checkHalfCarry(E, 0x1, 1);
			E += 1;
			PC += 1;
			Zb = checkZero(E);
			Nb = 0;
			break;

		case 0x0D: // DEC E
			Hb = checkHalfCarry(E, 0x1, -1);
			E -= 1;
			PC += 1;
			Zb = 0;
			Nb = 1;
			break;

		case 0x0E: // LD E, d8
			E = memory[PC + 1];
			PC += 2;
			break;

		case 0x0F: // RRA
			oldCb = Cb;
			Cb = A & 0b1;
			A >>= 1;
			if (oldCb == 1)
				A |= 0x1 << 7;
			else
				A &= ~(0x1 << 7);
			PC += 1;
			Zb = 0;
			Nb = 0;
			Hb = 0;
			break;
		}

	default:
		1;
		//printf("Unknown opcode.\n");
	}
	setFlags();
}

// Set flag bits in register F.
void gb::setFlags()
{
	switch (Zb)
	{
	case 0:
		F &= ~(0b1 << 7);
		break;
	case 1:
		F |= 0b1 << 7;
		break;
	}

	switch (Nb)
	{
	case 0:
		F &= ~(0b1 << 6);
		break;
	case 1:
		F |= 0b1 << 6;
		break;
	}

	switch (Hb)
	{
	case 0:
		F &= ~(0b1 << 5);
		break;
	case 1:
		F |= 0b1 << 5;
		break;
	}

	switch (Cb)
	{
	case 0:
		F &= ~(0b1 << 4);
		break;
	case 1:
		F |= 0b1 << 4;
		break;
	}
}

// Check if the half-carry bit should be set for an addition/subtraction.
int gb::checkHalfCarry(char a, char b, int mode)
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
}

int gb::checkZero(char a)
{
	if (a == 0)
		return 1;
	else
		return 0;
}

int load()
{

}