#pragma once
class gb
{
private:
	unsigned char opcode;
	unsigned char memory[65536]; // 16-bit address bus means 2^16 bytes can be addressed.
	unsigned char A, B, C, D, E, F, H, L; // Registers.
	unsigned int SP, PC; // Stack pointer and program counter.
	unsigned int HL, BC, DE; // Some pairs of registers can be used together as a 16-bit register.
	int Zb, Nb, Hb, Cb; // Flags in flag register.
	int IME; // Flag to disable or enable interrupts.

public:
	void initialize(); // Initialize the Game Boy's state.
	void loadGame(); // Load the game into memory.
	void emulateCycle(); // Emulate an instruction.
	void setFlags(); // Alter the flags in the flag register.
	int checkHalfCarry(unsigned char a, unsigned char b, int mode); // Check if a half-carry will occur when adding/subtracting two bytes.
	int checkZero(char a); // Check if a value equals zero.
	
	// Increment and decrement 8 and 16-bit values.
	void INC(unsigned char &r);
	void INC(unsigned int &val);
	void DEC(unsigned char &val);
	void DEC(unsigned int &val);

	void ROT(char dir, bool carry, unsigned char &r); // Rotate a byte normally or through carry.
	void modifyBit(unsigned char &r, int val, int pos); // Modify a bit in a byte.
	void ADD(unsigned char &val1, unsigned char val2, bool carry); // Adds a byte to another byte, with or without the carry bit.
	void ADD(unsigned int &r1, unsigned int r2); // Adds a 16-bit value to another 16-bit value.
	void ADD(unsigned int &val1, unsigned char val2);
	void SUB(unsigned char val, bool carry);
	void AND(unsigned char val);
	void XOR(unsigned char val);
	void OR(unsigned char val);
	void CP(unsigned char val);

	int combineReg(unsigned char r1, unsigned char r2);
	void splitReg(unsigned char r1, unsigned char r2, unsigned int r3);
};