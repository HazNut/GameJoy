#include "gb.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <Windows.h>

using namespace std;

// Initialize the Game Boy.
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

// Emulate a cycle of the Game Boy.
void gb::emulateCycle()
{
	// Get the opcode and execute the required instructions.
	opcode = memory[PC];
	char opcodeStr[3];
	_itoa_s(opcode, opcodeStr, 16);
	printf("Opcode is %s, PC is %i.\n", opcodeStr, PC);

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
			inc(B, C);
			PC += 1;
			break;

		case 0x04: // INC B
			inc(B);
			PC += 1;
			break;

		case 0x05: // DEC B
			dec(B);
			PC += 1;
			break;

		case 0x06: // LD B, d8
			B = memory[PC + 1];
			PC += 2;
			break;

		case 0x07: // RLCA
			rot('L', false, A);
			PC += 1;
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
			dec(B, C);
			PC += 1;
			break;

		case 0x0C: // INC C
			inc(C);
			break;

		case 0x0D: // DEC C
			dec(C);
			PC += 1;
			break;

		case 0x0E: // LD C, d8
			C = memory[PC + 1];
			PC += 2;
			break;

		case 0X0F: // RRCA
			rot('R', false, A);
			PC += 1;
			break;
		}
		break;

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
			inc(D, E);
			PC += 1;
			break;

		case 0x04: // INC D
			inc(D);
			PC += 1;
			break;

		case 0x05: // DEC D
			dec(D);
			PC += 1;
			break;

		case 0x06: // LD D, d8
			D = memory[PC + 1];
			PC += 2;
			break;

		case 0x07: // RLA
			rot('L', true, A);
			PC += 1;
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
			dec(D, E);
			PC += 1;
			break;

		case 0x0C: // INC E
			inc(E);
			PC += 1;
			break;

		case 0x0D: // DEC E
			dec(E);
			PC += 1;
			break;

		case 0x0E: // LD E, d8
			E = memory[PC + 1];
			PC += 2;
			break;

		case 0x0F: // RRA
			rot('R', true, A);
			PC += 1;
			break;
		}
		break;

	case 0x20:
		switch (opcode & 0x0F)
		{
		case 0x00: // JR NZ, r8
			if (Zb == 0)
				PC += memory[PC + 1];
			break;
		
		case 0x01: // LD HL, d16
			H = memory[PC + 1];
			L = memory[PC + 2];
			PC += 3;
			break;

		case 0x02: // LD (HL+), A
			memory[(H << 8) | L] = memory[A];
			inc(H, L);
			PC += 1;
			break;

		case 0x03: // INC HL
			inc(H, L);
			PC += 1;
			break;

		case 0x04: // INC H
			inc(H);
			PC += 1;
			break;

		case 0x05: // DEC H
			dec(H);
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
			Zb = checkZero(A);
			unsigned char d1 = A % 10;
			A /= 10;
			unsigned char d2 = A % 10;
			A = (d2 << 4) | d1;
		}

		case 0x08: // JR Z, r8
			if (Zb == 1)
				PC += memory[PC + 1];
			break;

		case 0x09: // ADD HL, HL
			HL = (H << 8) | L;

			if (HL > (0xFFFF - HL))
				Cb = 1;
			else
				Cb = 0;
			Hb = checkHalfCarry(HL, HL, 1);
			HL += HL;
			H = HL >> 8 & 0xFF;
			L = HL & 0xFF;
			PC += 1;
			Nb = 0;
			break;

		case 0x0A: // LD A, (HL+)
			A = memory[(H << 8) | L];
			inc(H, L);
			PC += 1;
			break;

		case 0x0B: // DEC HL
			dec(H, L);
			PC += 1;
			break;

		case 0x0C: // INC L
			inc(L);
			PC += 1;
			break;

		case 0x0D: // DEC L
			dec(L);
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
		}

	default:
		1;
		//printf("Unknown opcode.\n");
	}
	setFlags();
	Sleep(1000);
	
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
	return Hb;
}

// Check if a value equals 0. Used to set the zero flag bit.
int gb::checkZero(char a)
{
	if (a == 0)
		return 1;
	else
		return 0;
}

// Increment the value stored in a register.
void gb::inc(unsigned char &r)
{
	Hb = checkHalfCarry(r, 0x1, 1);
	r += 1;
	Zb = checkZero(r);
	Nb = 0;
}

// Increment the value stored across two registers.
void gb::inc(unsigned char &r1, unsigned char &r2)
{
	int r1r2 = (r1 << 8) | r2;
	r1r2 += 1;
	r1 = r1r2 >> 8 & 0xFF;
	r2 = r1r2 & 0xFF;
}

// Decrement the value in a register.
void gb::dec(unsigned char &r)
{
	Hb = checkHalfCarry(r, 0x1, -1);
	r -= 1;
	Zb = checkZero(r);
	Nb = 1;
}

// Decrement the value stored across two registers.
void gb::dec(unsigned char &r1, unsigned char &r2)
{
	int r1r2 = (r1 << 8) | r2;
	r1r2 -= 1;
	r1 = r1r2 >> 8 & 0xFF;
	r2 = r1r2 & 0xFF;
}

// Rotate the contents of a register left or right. Can rotate normally or through carry.
void gb::rot(char dir, bool carry, unsigned char &r)
{
	unsigned int oldCb = Cb;
	if (dir == 'L')
	{
		Cb = (r >> 7) & 0x1;
		r <<= 1;

		if (carry)
			if (oldCb == 1)
				r |= 0x1;
			else
				r &= ~0x1;
	}
	else if (dir == 'R')
	{
		Cb = r & 0x1;
		r >>= 1;
		if (carry)
			if (oldCb == 1)
				r |= 0x1 << 7;
			else
				r &= ~(0x1 << 7);
	}
	Zb = 0;
	Nb = 0;
	Hb = 0;
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