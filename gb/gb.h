#pragma once
class gb
{
private:
	unsigned char opcode;
	unsigned char memory[65536]; // 16-bit address bus means 64kB can be addressed (??)
	unsigned char A, B, C, D, E, F, H, L; // Registers
	unsigned int SP, PC; // Stack pointer and program counter
	unsigned int HL, BC, DE;
	int Zb, Nb, Hb, Cb; // Flags in flag register
	int IME;

public:
	void initialize();
	void loadGame();
	void emulateCycle();
	void setFlags();
	int checkHalfCarry(char a, char b, int mode);
	int checkZero(char a);
	void inc(unsigned char &r);
	void inc(unsigned char &r1, unsigned char &r2);
	void dec(unsigned char &r);
	void dec(unsigned char &r1, unsigned char &r2);
	void rot(char dir, bool carry, unsigned char &r);
	void modifyBit(unsigned char &r, int val, int pos);
};