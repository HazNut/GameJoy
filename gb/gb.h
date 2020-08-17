#pragma once
#include <stdio.h>
class gb
{
private:
	unsigned char opcode; // Current opcode.
	unsigned char A, B, C, D, E, F, H, L; // CPU registers.
	unsigned int SP, PC; // Stack pointer and program counter.
	unsigned int HL, BC, DE; // Some pairs of registers can be used together as a 16-bit register.
	unsigned char Zb, Nb, Hb, Cb; // Store flag register values.
	unsigned char IME; // Flag to disable or enable interrupts.
	bool scheduleIME; // If IME is scheduled to be set or not.
	int cyclesBeforeEnableIME = 1; // Cycles before IME update.
	unsigned char intVectors[5] = { 0x40, 0x48, 0x50, 0x58, 0x60 }; // Jump vectors for interrupts.

public:
	FILE* pFile; // Pointer for log file.
	unsigned char memory[65536]; // 16-bit address bus means 2^16 bytes can be addressed.
	void initialize(); // Initialize the Game Boy's state.
	void loadGame(); // Load the game into memory.
	void emulateCycle(); // Emulate an instruction.
	void updateFlagReg(); // Alter the flags in the flag register.
	void modifyBit(unsigned char &r, int val, int pos); // Modify a bit in a byte.
	int checkHalfCarry(unsigned char val1, unsigned char val2, char mode); // Check half-carry for adding/subtracting 2 bytes.
	int checkHalfCarry(unsigned int val1, unsigned int val2, char mode); // Check half-carry for adding/subtracting 2 16-bit values.
	int checkZero(unsigned char a); // Check if the zero bit should be set.
	unsigned int combineReg(unsigned char r1, unsigned char r2); // Combine 2 registers into 1 16-bit register.
	void splitReg(unsigned char& r1, unsigned char& r2, unsigned int r3); // Put a combined register's values back into the originals.

	void INC(unsigned char &r); // Increment a byte.
	void INC(unsigned int &val); // Increment a 16-bit value.
	void DEC(unsigned char &val); // Decrement a byte.
	void DEC(unsigned int &val); // Decrement a 16-bit value.
	void ADD(unsigned char &val1, unsigned char val2, bool carry); // Adds a byte to another byte.
	void ADD(unsigned int &r1, unsigned int r2); // Adds a 16-bit value to another 16-bit value.
	void ADD(unsigned int &val1, signed char val2); // Adds a 16-bit signed value to an unsigned 16-bit value.
	void SUB(unsigned char val, bool carry);
	void AND(unsigned char val); // ANDs a value with A.
	void XOR(unsigned char val); // XORs a value with A.
	void OR(unsigned char val);  // ORs a value with A.
	void CP(unsigned char val);  // Sets flags for subtracting from A, without saving the result.
	void ROT(unsigned char dir, bool carry, unsigned char &r); // Rotate a byte normally or through carry.
	void SHIFT(unsigned char dir, unsigned char& r); // Shifts a bit left or right, or right logically.
	void BIT(unsigned char pos, unsigned char r); // Sets the zero flag if a given bit equals 0.
	void SWAP(unsigned char& val); // Swaps the upper and lower nibble of a byte.
	void CALL(); // Calls the next memory address.
	void RST(unsigned char vec); // Calls a given memory address.
	void EI();
	void DI();
};