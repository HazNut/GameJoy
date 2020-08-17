#include "gb.h"
#include "SDL.h"
#include <bitset>

gb myGB;

void drawTile(SDL_Renderer* renderer, int memLocation, int x, int y, bool wrap, int currLine)
{
	for (int byte = 0; byte < 15; byte += 2)
	{
		unsigned char lowByte = myGB.memory[memLocation + byte];
		unsigned char highByte = myGB.memory[memLocation + byte + 1];
		for (int bit = 0; bit < 8; bit++)
		{
			unsigned char lowBit = (lowByte >> (7 - bit)) & 0x1;
			unsigned char highBit = (highByte >> (7 - bit)) & 0x1;
			unsigned int total = lowBit | (highBit << 1);

			switch (total)
			{
			case 0:
				SDL_SetRenderDrawColor(renderer, 155, 188, 15, SDL_ALPHA_OPAQUE);
				break;
			case 1:
				SDL_SetRenderDrawColor(renderer, 139, 172, 15, SDL_ALPHA_OPAQUE);
				break;

			case 2:
				SDL_SetRenderDrawColor(renderer, 48, 98, 48, SDL_ALPHA_OPAQUE);
				break;

			case 3:
				SDL_SetRenderDrawColor(renderer, 15, 56, 15, SDL_ALPHA_OPAQUE);
				break;
			}

			//SDL_Rect rect;

			//rect.x = (x + bit);
			//rect.y = (y + (byte / 2));
			//rect.w = 3;
			//rect.h = 3;
			/*if (wrap)
			{
				if (rect.x >= (160))
					rect.x = rect.x - (160);
				if (rect.y >= (144))
					rect.y = rect.y - (1443);
			}*/
			SDL_RenderDrawPoint(renderer, (x + bit), (y + (byte / 2)));
			//SDL_RenderFillRect(renderer, &rect);
		}
	}
}

void displayBackground(SDL_Renderer* renderer, int currLine)
{
	int scrollY = myGB.memory[0xFF42];
	int scrollX = myGB.memory[0xFF43];

	int mapBaseVal;

	// Get the base value for the tile map.
	if ((myGB.memory[0xFF40] >> 3) & 0x1)
		mapBaseVal = 0x9C00;
	else
		mapBaseVal = 0x9800;

	for (int tile = 0; tile < 32 * 32; tile++)
	{
		char tileNumber = myGB.memory[mapBaseVal + tile]; // Get the tile number from the tile map.
		char tileStartByte; // Stores the start of the tile info in tile data.

		// Get start byte of tile data.
		if ((myGB.memory[0xFF40] >> 4) & 0x1)
			tileStartByte = 0x8800 + signed char (tileNumber);
		else
			tileStartByte = 0x8000 + tileNumber;
		drawTile(renderer, tileStartByte, tile * 8, (tile / 32) * 8, true, currLine);
	}
}

void displayWindow(SDL_Renderer* renderer, int currLine)
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

		drawTile(renderer, tileStartByte, windowX + (tile * 8), windowY + ((tile / 32) * 8), false, currLine);
	}
}

void drawSprites(SDL_Renderer* renderer, int currLine)
{

	for (int sprite = 0; sprite < 40; sprite++)
	{
		char spriteNumber = myGB.memory[0xFE00 + (sprite * 4) + 2];
		int spriteX = myGB.memory[0xFE00 + (sprite * 4) + 1] + 8;
		int spriteY = myGB.memory[0xFE00 + (sprite * 4)] + 16;
		char tileStartByte = 0x8000 + spriteNumber;

		drawTile(renderer, tileStartByte, spriteX, spriteY, false, currLine);
	}
}

int main(int argc, char** argv)
{/*
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *win = SDL_CreateWindow("GameJoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160, 144, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_Surface* surf = SDL_CreateRGBSurface(0, 256, 256, 32, 0, 0, 0, 0);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_SetRenderTarget(renderer, texture);
	
	SDL_Event e;*/

	// Set up the Game Boy and load the game.
	myGB.initialize();
	myGB.loadGame();


	int ticks = 0; // Every 1000 cycles, get SDL events.
	unsigned char LY;  // Stores LY.
	myGB.memory[0xFF44] = 0x90;
	// Keep emulating CPU cycles.
	for (;;)
	{
		myGB.emulateCycle();
		
		/*LY = myGB.memory[0xFF44];
		
		if (LY < 0x90)
		{
			SDL_Rect r{ 0, LY, 160, 1 };
			SDL_RenderCopy(renderer, texture, &r, &r);
			SDL_RenderPresent(renderer);
		}

		if (myGB.memory[0xFF44] == 0x90)
		{
			displayBackground(renderer, LY);
			displayWindow(renderer, LY);
			drawSprites(renderer, LY);
			
			SDL_RenderClear(renderer);
		}*/

		/*myGB.memory[0xFF44] += 1;

		if (myGB.memory[0xFF44] == 0x99)
			myGB.memory[0xFF44] = 0x0;*/

	/*	ticks += 1;

		if (ticks == 1000)
		{
			while (SDL_PollEvent(&e))
			{
				1;
			}
			ticks = 0;
		}
				*/
	}

}
