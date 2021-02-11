#ifndef GB_H
#define GB_H

#include <stdio.h>

// Named registers in memory.
constexpr unsigned int JOYP = 0xFF00;
constexpr unsigned int DIV = 0xFF04;
constexpr unsigned int TIMA = 0xFF05;
constexpr unsigned int TMA = 0xFF06;
constexpr unsigned int TAC = 0xFF07;
constexpr unsigned int IF = 0xFF0F;
constexpr unsigned int LCDC = 0xFF40;
constexpr unsigned int SCROLLY = 0xFF42;
constexpr unsigned int SCROLLX = 0xFF43;
constexpr unsigned int LY = 0xFF44;
constexpr unsigned int BGP = 0xFF47;
constexpr unsigned int WINY = 0xFF4A;
constexpr unsigned int WINX = 0xFF4B;
constexpr unsigned int IE = 0xFFFF;

class gb
{
public:
	void initialize();														
	void loadGame(char filename[], char* gameTitle);						
	void emulateCycle();													
	void modifyBit(unsigned char& r, int val, int pos);						

	unsigned char memory[65536];											// 2^16 bytes can be addressed.
	bool logging = false;													// Set to log CPU state to output.txt.

private:
	// General functions.
	void incTimer();
	void updateFlagReg();													
	bool checkHalfCarry(unsigned char val1, unsigned char val2, char mode);
	bool checkHalfCarry(unsigned int val1, unsigned int val2, char mode);
	bool checkZero(unsigned char a);										
	unsigned int combineReg(unsigned char r1, unsigned char r2);			
	void splitReg(unsigned char& r1, unsigned char& r2, unsigned int r3);	
	void writeToMemory(unsigned int addr, unsigned char data);				

	// Implementations of some opcodes. Capitalised as some names are keywords in C++ e.g. xor.
	void INC(unsigned char& r);												
	void INC(unsigned int& val);											
	void DEC(unsigned char& val);											
	void DEC(unsigned int& val);											
	void ADD(unsigned char& val1, unsigned char val2, bool carry);			
	void ADD(unsigned int& r1, unsigned int r2);							
	void ADD(unsigned int& val1, signed char val2);							
	void SUB(unsigned char val, bool carry);								
	void AND(unsigned char val);											
	void XOR(unsigned char val);											
	void OR(unsigned char val);												
	void CP(unsigned char val);												
	void ROT(unsigned char dir, bool carry, unsigned char& r);
	void SHIFT(unsigned char dir, unsigned char& r);
	void BIT(unsigned char pos, unsigned char r);
	void SWAP(unsigned char& val);
	void CALL();
	void RST(unsigned char vec);
	void EI();
	void DI();

	FILE* pFile;															// Pointer for log file.
	unsigned char opcode;
	unsigned char A, B, C, D, E, F, H, L;									// CPU registers.
	unsigned int SP, PC;													// Stack pointer and program counter.
	unsigned int HL, BC, DE;												// Some registers can be combined.
	unsigned char Zb, Nb, Hb, Cb;											// Store flag register values.
	unsigned char IME;														// Flag to disable/enable interrupts.
	bool scheduleIME;														// Set if IME is scheduled to be enabled.
	int cyclesBeforeEnableIME = 1;
	unsigned char intVectors[5] = { 0x40, 0x48, 0x50, 0x58, 0x60 };			// Jump vectors for interrupts.
	unsigned int counter;													// Counts the number of machine cycles passed.
	
};
#endif GB_H