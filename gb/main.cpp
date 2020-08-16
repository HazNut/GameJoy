#include "gb.h"
#include "SDL.h"
#include <bitset>

gb myGB;

void displayBackground(SDL_Renderer* renderer)
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

		for (int byte = 0; byte < 15; byte += 2)
		{
			char lowByte = myGB.memory[tileStartByte + byte];
			char highByte = myGB.memory[tileStartByte + byte + 1];
			for (int bit = 0; bit < 8; bit++)
			{
				char lowBit = lowByte >> (7 - bit) & 0x1;
				char highBit = highByte >> (7 - bit) & 0x1;
				int total = (lowBit * 1) + (highBit * 2);

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
			
				SDL_Rect rect;
				
				rect.x = (((tile * 8) + bit)) * 3;
				rect.y = (((tile / 32) * 8) + (byte / 2)) * 3;
				rect.w = 3;
				rect.h = 3;
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}
}

void displayWindow(SDL_Renderer* renderer)
{
	int scrollY = myGB.memory[0xFF42];
	int scrollX = myGB.memory[0xFF43];

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

		for (int byte = 0; byte < 15; byte += 2)
		{
			char lowByte = myGB.memory[tileStartByte + byte];
			char highByte = myGB.memory[tileStartByte + byte + 1];
			for (int bit = 0; bit < 8; bit++)
			{
				char lowBit = lowByte >> (7 - bit) & 0x1;
				char highBit = highByte >> (7 - bit) & 0x1;
				int total = (lowBit * 1) + (highBit * 2);

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

				SDL_Rect rect;

				rect.x = (((tile * 8) + bit)) * 3;
				rect.y = (((tile / 32) * 8) + (byte / 2)) * 3;
				rect.w = 3;
				rect.h = 3;
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}
}

void drawSprites(SDL_Renderer *renderer)
{

	for (int sprite = 0; sprite < 40; sprite++)
	{
		char spriteNumber = myGB.memory[0xFE00 + (sprite * 4) + 2];
		int spriteX = myGB.memory[0xFE00 + (sprite * 4) + 1] + 8;
		int spriteY = myGB.memory[0xFE00 + (sprite * 4)] + 16;
		char tileStartByte = 0x8000 + spriteNumber;



		for (int byte = 0; byte < 15; byte += 2)
		{
			char lowByte = myGB.memory[tileStartByte + byte];
			char highByte = myGB.memory[tileStartByte + byte + 1];
			for (int bit = 0; bit < 8; bit++)
			{
				char lowBit = lowByte >> (7 - bit) & 0x1;
				char highBit = highByte >> (7 - bit) & 0x1;
				int total = (lowBit * 1) + (highBit * 2);

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

				SDL_Rect rect;

				rect.x = (((spriteX) + bit)) * 3;
				rect.y = (((spriteY) * 8) + (byte / 2)) * 3;
				rect.w = 3;
				rect.h = 3;
				SDL_RenderFillRect(renderer, &rect);
			}
		}





	}
}

int main(int argc, char** argv)
{
	/*SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *win = SDL_CreateWindow("GameJoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 256 * 3, 256 * 3, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_Event e;*/

	// Set up the Game Boy and load the game.
	myGB.initialize();
	myGB.loadGame();

	// Keep emulating CPU cycles.
	for (;;)
	{
		myGB.emulateCycle();
		/*SDL_RenderClear(renderer);
		displayBackground(renderer);
		displayWindow(renderer);
		drawSprites(renderer);
		SDL_RenderPresent(renderer);*/
	
		/*while (SDL_PollEvent(&e) != 0)
		{
			break;
		}*/
		myGB.memory[0xFF44] = 0x94; // Set VBLANK to required value for tetris.
		myGB.modifyBit(myGB.memory[0xFF0F], 1, 0); // Set VBLANK interrupt.
	}
		
}
