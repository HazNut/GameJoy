#pragma once
class gb
{
private:
	unsigned char opcode;
	unsigned char memory[65535]; // 16-bit address bus means 64kB can be addressed (??)
	unsigned char A, B, C, D, E, F, H, L; // Registers
	unsigned int SP, PC; // Stack pointer and program counter
	unsigned int HL, BC, DE;
	unsigned int oldCb;
	int Zb, Nb, Hb, Cb; // Flags in flag register
	int IME;

public:
	void initialize();
	void loadGame();
	void emulateCycle();
	void setFlags();
	int checkHalfCarry(char a, char b, int mode);
	int checkZero(char a);
};