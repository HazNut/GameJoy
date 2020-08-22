#include "gb.h"
#include "SDL.h"
#include <bitset>
#include <Windows.h>

gb myGB;

void drawTile(unsigned int(&gfxArray)[160 * 144], unsigned int memLocation, int x, int y)
{
	int byte = (y % 8) * 2;

	unsigned char lowByte = myGB.memory[memLocation + byte];
	unsigned char highByte = myGB.memory[memLocation + byte + 1];

	unsigned char lowBit = (lowByte >> (7 - (x % 8))) & 0x1;
	unsigned char highBit = (highByte >> (7 - (x % 8))) & 0x1;
	unsigned int total = lowBit + (highBit * 2);
	unsigned char shade = myGB.memory[0xFF47] >> (total * 2);
	gfxArray[(x % 160) + (y * 160)] = ((shade << 16) | (shade << 8) | shade);
}


void displayBackground(unsigned int (&gfxArray)[160 * 144])
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
		drawTile(gfxArray, tileStartByte, tile * 8, true);
	}
}

void displayWindow(unsigned int (&gfxArray)[160 * 144])
{
	int windowX = myGB.memory[0xFF4B] - 0x7;
	int windowY = myGB.memory[0xFF4A];
	int mapBaseVal;

	// Get the base value for the tile map.
	if ((myGB.memory[0xFF40] >> 6) & 0x1)
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

		drawTile(gfxArray, tileStartByte, tile, false);
	}
}

void drawSprites(unsigned int (&gfxArray)[160 * 144])
{

	for (int sprite = 0; sprite < 40; sprite++)
	{
		unsigned char spriteNumber = myGB.memory[0xFE00 + (sprite * 4) + 2];
		unsigned int spriteX = myGB.memory[0xFE00 + (sprite * 4) + 1] + 8;
		unsigned int spriteY = myGB.memory[0xFE00 + (sprite * 4)] + 16;
		unsigned int tileStartByte = 0x8000 + spriteNumber;
		drawTile(gfxArray, tileStartByte, spriteX + spriteY, false);
	}
}

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* win = SDL_CreateWindow("GameJoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * 2, 144 * 2, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetScale(renderer, 2, 2);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, SDL_CreateRGBSurface(0, 160, 144, 24, 0, 0, 0, 0));
	SDL_Event e;
	const Uint8* kb = SDL_GetKeyboardState(NULL);
	// Set up the Game Boy and load the game.
	myGB.initialize();
	myGB.loadGame();

	unsigned int gfxArray [160 * 144];
	int x = 0;
	unsigned char scrollX, scrollY, currTile;
	unsigned int mapBaseVal, mapOffset, tileStartByte;
	int cycles = 0;
	myGB.memory[0xFF44] = 0x0;

	// Keep emulating CPU cycles.
	for (;;)
	{
		
		for (int i = 0; i < 456; i++)
		{
			myGB.emulateCycle();
			cycles += 1;
			if (cycles == 100)
			{
				SDL_PumpEvents();
				cycles = 0;
			}
				
			if ((myGB.memory[0xFF00] >> 5) == 0)
			{
				if (kb[SDL_SCANCODE_P])
					myGB.modifyBit(myGB.memory[0xFF00], 0, 0);
				else
					myGB.modifyBit(myGB.memory[0xFF00], 1, 0);
				if (kb[SDL_SCANCODE_O]) 
					myGB.modifyBit(myGB.memory[0xFF00], 0, 1);
				else
					myGB.modifyBit(myGB.memory[0xFF00], 1, 1);
				if (kb[SDL_SCANCODE_K]) 
					myGB.modifyBit(myGB.memory[0xFF00], 0, 2);
				else
					myGB.modifyBit(myGB.memory[0xFF00], 1, 2);
				if (kb[SDL_SCANCODE_L]) 
					myGB.modifyBit(myGB.memory[0xFF00], 0, 3);
				else
					myGB.modifyBit(myGB.memory[0xFF00], 1, 3);
			}

			else if ((myGB.memory[0xFF00] >> 4) == 0)
			{
				if (kb[SDL_SCANCODE_D]) 
					myGB.modifyBit(myGB.memory[0xFF00], 0, 0);
				else
					myGB.modifyBit(myGB.memory[0xFF00], 1, 0);
				if (kb[SDL_SCANCODE_A]) 
					myGB.modifyBit(myGB.memory[0xFF00], 0, 1);
				else
					myGB.modifyBit(myGB.memory[0xFF00], 1, 1);
				if (kb[SDL_SCANCODE_W]) 
					myGB.modifyBit(myGB.memory[0xFF00], 0, 2);
				else
					myGB.modifyBit(myGB.memory[0xFF00], 1, 2);
				if (kb[SDL_SCANCODE_S])
					myGB.modifyBit(myGB.memory[0xFF00], 0, 3);
				else
					myGB.modifyBit(myGB.memory[0xFF00], 1, 3);
			}
		}
		
	
		scrollY = myGB.memory[0xFF42];
		scrollX = myGB.memory[0xFF43];
	
		if (((myGB.memory[0xFF40] >> 3) & 0x1))
			mapBaseVal = 0x9C00;
			
		else
			mapBaseVal = 0x9800;
			
	
		if (myGB.memory[0xFF44] < 0x90)
		{
			
				
			for (int x = 0; x < 160; x++)
			{
				if (myGB.memory[0xFF44] + scrollY >= 256)
					mapOffset = (((x + scrollX) / 8) % 32) + (((myGB.memory[0xFF44] + scrollY - 256) / 8) * 32);
				else if (x + scrollX >= 256)
					mapOffset = (((x + scrollX - 256) / 8) % 32) + (((myGB.memory[0xFF44] + scrollY) / 8) * 32);
				else if ((x + scrollX >= 256) && (myGB.memory[0xFF44] + scrollY >= 256))
					mapOffset = (((x + scrollX - 256) / 8) % 32) + (((myGB.memory[0xFF44] + scrollY - 256) / 8) * 32);
				else
					mapOffset = (((x + scrollX) / 8) % 32) + (((myGB.memory[0xFF44] + scrollY) / 8) * 32);
				
				currTile = myGB.memory[mapBaseVal + mapOffset];
			
				if ((myGB.memory[0xFF40] >> 4) & 0x1)
					tileStartByte = 0x8000 + (currTile * 16);
				else
				{
					tileStartByte = 0x9000 + (signed char(currTile) * 16);
				}
					
			
				drawTile(gfxArray, tileStartByte, x, myGB.memory[0xFF44]);

			}
		}
	
		
		myGB.memory[0xFF44] += 1;
	
		if (myGB.memory[0xFF44] == 0x90)
		{
			SDL_UpdateTexture(texture, NULL, gfxArray, 160 * 4);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			myGB.modifyBit(myGB.memory[0xFF0F], 1, 0);
		}
			

		if (myGB.memory[0xFF44] > 0x99)
		{
			myGB.modifyBit(myGB.memory[0xFF0F], 0, 0);
			myGB.memory[0xFF44] = 0x00;
			
		}
		
		if (SDL_PollEvent(&e) != 0)
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
