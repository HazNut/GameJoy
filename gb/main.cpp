#include "gb.h"
#include "SDL.h"
#include <bitset>

gb myGB;

void drawTile(SDL_Renderer* renderer, unsigned int memLocation, unsigned char x, unsigned char y, bool wrap)
{
	unsigned char scrollY = myGB.memory[0xFF42];
	unsigned char scrollX = myGB.memory[0xFF43];

	for (int byte = 0; byte < 15; byte += 2)
	{
		unsigned char lowByte = myGB.memory[memLocation + byte];
		unsigned char highByte = myGB.memory[memLocation + byte + 1];
		
		if (wrap)
		{
			if (y + (byte / 2) + scrollY >= 144)
				y -= 144;
		}

		for (int bit = 0; bit < 8; bit++)
		{
			unsigned char lowBit = (lowByte >> (bit)) & 0x1;
			unsigned char highBit = (highByte >> (bit)) & 0x1;
			unsigned int total = lowBit + (highBit * 2);
			unsigned char shade = myGB.memory[0xFF47] >> (total * 2);

			SDL_SetRenderDrawColor(renderer, shade, shade, shade, SDL_ALPHA_OPAQUE);

			if (wrap)
			{
				if (x + bit + scrollX >= 160)
					x -= 160;
				
			}
			SDL_RenderDrawPoint(renderer, (x + bit) - scrollX, (y + (byte / 2)) - scrollY);
		}
	}
}

void displayBackground(SDL_Renderer* renderer)
{
	int mapBaseVal;

	// Get the base value for the tile map.
	if ((myGB.memory[0xFF40] >> 3) & 0x1)
		mapBaseVal = 0x9C00;
	else
		mapBaseVal = 0x9800;

	for (int tile = 0; tile < 32 * 32; tile++)
	{
		unsigned char tileNumber = myGB.memory[mapBaseVal + tile]; // Get the tile number from the tile map.
		
		unsigned int tileStartByte; // Stores the start of the tile info in tile data.
	

		// Get start byte of tile data.
		if ((myGB.memory[0xFF40] >> 4) & 0x1)
			tileStartByte = 0x8000 + (tileNumber * 16);
		
		else
			tileStartByte = 0x9000 + (signed char(tileNumber) * 16);
		
		drawTile(renderer, tileStartByte, (tile % 32) * 8, (tile / 32) * 8, true);
	}
}

void displayWindow(SDL_Renderer* renderer)
{
	int windowX = myGB.memory[0xFF4B];
	int windowY = myGB.memory[0xFF4A];

	int mapBaseVal;

	// Get the base value for the tile map.
	if ((myGB.memory[0xFF40] >> 6) & 0x1)
		mapBaseVal = 0x9C00;
	else
		mapBaseVal = 0x9800;

	for (int tile = 0; tile < 32 * 32; tile++)
	{
		char tileNumber = myGB.memory[mapBaseVal + tile]; // Get the tile number from the tile map.
		char tileStartByte; // Stores the start of the tile info in tile data.

		// Get start byte of tile data.
		if ((myGB.memory[0xFF40] >> 4) & 0x1)
			tileStartByte = 0x8800 + signed char(tileNumber);
		else
			tileStartByte = 0x8000 + tileNumber;

		drawTile(renderer, tileStartByte, windowX + (tile * 8), windowY + ((tile / 32) * 8), false);
	}
}

void drawSprites(SDL_Renderer* renderer)
{

	for (int sprite = 0; sprite < 40; sprite++)
	{
		char spriteNumber = myGB.memory[0xFE00 + (sprite * 4) + 2];
		int spriteX = myGB.memory[0xFE00 + (sprite * 4) + 1] + 8;
		int spriteY = myGB.memory[0xFE00 + (sprite * 4)] + 16;
		char tileStartByte = 0x8000 + spriteNumber;

		drawTile(renderer, tileStartByte, spriteX, spriteY, false);
	}
}

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* win = SDL_CreateWindow("GameJoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * 4, 144 * 4, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetScale(renderer, 4, 4);
	SDL_Event e;

	// Set up the Game Boy and load the game.
	myGB.initialize();
	myGB.loadGame();

	myGB.memory[0xFF44] = 0x90;

	// Keep emulating CPU cycles.
	for (;;)
	{
		SDL_RenderClear(renderer);
		myGB.modifyBit(myGB.memory[0xFF0F], 1, 0);
		for (int i = 0; i < 10000; i++)
			myGB.emulateCycle();
		displayBackground(renderer);
		
		//displayWindow(renderer);
		//drawSprites(renderer);

		SDL_RenderPresent(renderer);

		while (SDL_PollEvent(&e))
		{
			1;
		}

		/*LY = myGB.memory[0xFF44];

		if (LY == 0x00)
		{

		}
		else if (LY < 0x90)
		{
			SDL_Rect r{ 0, LY, 160, 1 };
			SDL_RenderCopy(renderer, texture, &r, &r);
			SDL_RenderPresent(renderer);
		}



		myGB.memory[0xFF44] += 1;

		if (myGB.memory[0xFF44] == 0x99)
			myGB.memory[0xFF44] = 0x0;
			*/
	}
}
