#ifndef GB_H
#define GB_H

#include <stdio.h>
#include <cstdint>

// Named registers in memory.
constexpr uint16_t JOYP = 0xFF00;
constexpr uint16_t DIV = 0xFF04;
constexpr uint16_t TIMA = 0xFF05;
constexpr uint16_t TMA = 0xFF06;
constexpr uint16_t TAC = 0xFF07;
constexpr uint16_t IF = 0xFF0F;
constexpr uint16_t LCDC = 0xFF40;
constexpr uint16_t STAT = 0xFF41;
constexpr uint16_t SCROLLY = 0xFF42;
constexpr uint16_t SCROLLX = 0xFF43;
constexpr uint16_t LY = 0xFF44;
constexpr uint16_t LYC = 0xFF45;
constexpr uint16_t BGP = 0xFF47;
constexpr uint16_t OBP1 = 0xFF48;
constexpr uint16_t OBP2 = 0xFF49;
constexpr uint16_t WY = 0xFF4A;
constexpr uint16_t WX = 0xFF4B;
constexpr uint16_t IE = 0xFFFF;

class gb
{
public:
	void initialize();														
	void loadGame(char filename[], char* gameTitle);						
	void emulateCycle();													
	void modifyBit(uint8_t &r, int val, int pos);						

	uint8_t memory[65536];													// 2^16 bytes can be addressed.
	bool logging = false;													// Set to log CPU state to output.txt.

private:
	// General functions.
	void incTimer();
	void updateFlagReg();													
	bool checkHalfCarry(uint8_t val1, uint8_t val2, char mode);
	bool checkHalfCarry(uint16_t val1, uint16_t val2, char mode);
	bool checkZero(uint8_t a);
	uint16_t combineReg(uint8_t r1, uint8_t r2);
	void splitReg(uint8_t &r1, uint8_t &r2, uint16_t r3);
	void writeToMemory(uint16_t addr, uint8_t data);

	// Implementations of some opcodes. Capitalised as some names are keywords in C++ e.g. xor.
	void INC(uint8_t &r);												
	void INC(uint16_t &val);
	void DEC(uint8_t &val);
	void DEC(uint16_t &val);
	void ADD(uint8_t &val1, uint8_t val2, bool carry);
	void ADD(uint16_t &r1, uint16_t r2);
	void ADD(uint16_t &val1, int8_t val2);
	void SUB(uint8_t val, bool carry);
	void AND(uint8_t val);
	void XOR(uint8_t val);
	void OR(uint8_t val);
	void CP(uint8_t val);
	void ROT(char dir, bool carry, uint8_t &r);
	void SHIFT(char dir, uint8_t &r);
	void BIT(int pos, uint8_t r);
	void SWAP(uint8_t &val);
	void CALL();
	void RST(uint8_t vec);
	void EI();
	void DI();

	FILE* pFile;															// Pointer for log file.
	uint8_t opcode;
	uint8_t A, B, C, D, E, F, H, L;											// CPU registers.
	uint16_t SP, PC;														// Stack pointer and program counter.
	uint16_t HL, BC, DE;													// Some registers can be combined.
	uint8_t Zb, Nb, Hb, Cb;													// Store flag register values.
	bool IME;																// Flag to disable/enable interrupts.
	bool scheduleIME;														// Set if IME is scheduled to be enabled.
	int cyclesBeforeEnableIME = 1;
	uint8_t intVectors[5] = { 0x40, 0x48, 0x50, 0x58, 0x60 };				// Jump vectors for interrupts.
	unsigned int counter;													// Counts the number of machine cycles passed.
};
#endif GB_H