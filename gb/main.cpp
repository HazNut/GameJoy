#include "gb.h"
#include "SDL.h"
#include <bitset>

gb myGB;

void drawTile(int(&gfxArray)[256 * 256], unsigned int memLocation, int x, int y)
{


	int byte = (y % 8) * 2;

	unsigned char lowByte = myGB.memory[memLocation + byte];
	unsigned char highByte = myGB.memory[memLocation + byte + 1];

	unsigned char lowBit = (lowByte >> (7 - (x % 8))) & 0x1;
	unsigned char highBit = (highByte >> (7 - (x % 8))) & 0x1;
	unsigned int total = lowBit + (highBit * 2);
	unsigned char shade = myGB.memory[0xFF47] >> (total * 2);
	/*if (x > 160)
		x -= 160;
	if (y > 144)
		y -= 144;*/
	gfxArray[(x % 256) + (y * 256)] = ((shade << 16) | (shade << 8) | shade);
}


void displayBackground(int (&gfxArray)[256 * 256])
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

void displayWindow(int (&gfxArray)[256 * 256])
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

void drawSprites(int (&gfxArray)[256 * 256])
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
	SDL_Window* win = SDL_CreateWindow("GameJoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 256 * 2, 256 * 2, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetScale(renderer, 2, 2);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, SDL_CreateRGBSurface(0, 256, 256, 32, 0, 0, 0, 0));
	SDL_Event e;

	// Set up the Game Boy and load the game.
	myGB.initialize();
	myGB.loadGame();

	myGB.memory[0xFF44] = 0x94;
	int gfxArray[256 * 256];
	int x = 0;
	int y = 0;
	int mapBaseVal;
	int currTile;
	int tileStartByte;
	// Keep emulating CPU cycles.
	for (;;)
	{
		
		for (int i = 0; i < 100000; i++)
		{
			myGB.modifyBit(myGB.memory[0xFF0F], 1, 0);
			myGB.emulateCycle();
		}
		unsigned char scrollY = myGB.memory[0xFF42];
		unsigned char scrollX = myGB.memory[0xFF43];
		if ((myGB.memory[0xFF40] >> 3) && x >= 160)
			mapBaseVal = 0x9C00;
		else
			mapBaseVal = 0x9800;

		for (int y = 0; y < 256; y++)
		{
			for (int x = 0; x < 256; x++)
			{
				currTile = myGB.memory[mapBaseVal + ((x / 8) % 32) + ((y / 8) * 32)];
				if ((myGB.memory[0xFF40] >> 4) & 0x1)
					tileStartByte = 0x8000 + (currTile * 16);
				else
					tileStartByte = 0x9000 + (signed char(currTile) * 16);
				drawTile(gfxArray, tileStartByte, x, y);
			}
		}

		SDL_UpdateTexture(texture, NULL, gfxArray, 256 * 4);
		SDL_Rect destRect = { -scrollX, -scrollY, 256, 256 };
		printf("%d, %d\n", scrollX, scrollY);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
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
