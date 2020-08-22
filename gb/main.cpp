#include "gb.h"
#include "SDL.h"

gb myGB; // The Game Boy's CPU is stored as an object.

// Checks to see if the Game Boy is requesting the input state, then returns the currently pressed keys.
void processInputs(const Uint8* kb)
{

	// GB wants to check buttons.
	if ((myGB.memory[JOYP] >> 5) == 0)
	{
		if (kb[SDL_SCANCODE_P])
			myGB.modifyBit(myGB.memory[JOYP], 0, 0); // If pressing A, set the input state in memory.
		else
			myGB.modifyBit(myGB.memory[JOYP], 1, 0); // Else reset the input state (1 = off).

		if (kb[SDL_SCANCODE_O])
			myGB.modifyBit(myGB.memory[JOYP], 0, 1);
		else
			myGB.modifyBit(myGB.memory[JOYP], 1, 1);

		if (kb[SDL_SCANCODE_K])
			myGB.modifyBit(myGB.memory[JOYP], 0, 2);
		else
			myGB.modifyBit(myGB.memory[JOYP], 1, 2);

		if (kb[SDL_SCANCODE_L])
			myGB.modifyBit(myGB.memory[JOYP], 0, 3);
		else
			myGB.modifyBit(myGB.memory[JOYP], 1, 3);
	}

	// GB wants to check D-pad.
	else if ((myGB.memory[JOYP] >> 4) == 0)
	{
		if (kb[SDL_SCANCODE_D])
			myGB.modifyBit(myGB.memory[JOYP], 0, 0);
		else
			myGB.modifyBit(myGB.memory[JOYP], 1, 0);

		if (kb[SDL_SCANCODE_A])
			myGB.modifyBit(myGB.memory[JOYP], 0, 1);
		else
			myGB.modifyBit(myGB.memory[JOYP], 1, 1);

		if (kb[SDL_SCANCODE_W])
			myGB.modifyBit(myGB.memory[JOYP], 0, 2);
		else
			myGB.modifyBit(myGB.memory[JOYP], 1, 2);

		if (kb[SDL_SCANCODE_S])
			myGB.modifyBit(myGB.memory[JOYP], 0, 3);
		else
			myGB.modifyBit(myGB.memory[JOYP], 1, 3);
	}
}

// Draw a line of the currently selected tile.
void drawLineOfTile(unsigned int(&gfxArray)[160 * 144], unsigned int memLocation, int x, int y)
{
	// Pixel info is stored across 2 bytes per row.
	int byte = (y % 8) * 2;
	unsigned char lowByte = myGB.memory[memLocation + byte];
	unsigned char highByte = myGB.memory[memLocation + byte + 1];

	// 2 bits per pixel, stored across the 2 bytes.
	unsigned char lowBit = (lowByte >> (7 - (x % 8))) & 0x1;
	unsigned char highBit = (highByte >> (7 - (x % 8))) & 0x1;

	// The high and low bit are added together.
	unsigned int total = lowBit + (highBit * 2);

	// The total gives the shade of grey to be used for the pixel.
	unsigned char shade = myGB.memory[BGP] >> (total * 2);

	// Now add this grey shade to the RGB array.
	gfxArray[(x % 160) + (y * 160)] = ((shade << 16) | (shade << 8) | shade);
}

int main(int argc, char** argv)
{

	// Set up the graphics environment.
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* win = SDL_CreateWindow("GameJoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * 2, 144 * 2, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetScale(renderer, 2, 2);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, SDL_CreateRGBSurface(0, 160, 144, 24, 0, 0, 0, 0));
	
	const Uint8* kb = SDL_GetKeyboardState(NULL); // Stores the state of currently pressed keys.
	
	// Set up the Game Boy and load the game.
	myGB.initialize();
	myGB.loadGame();

	unsigned int gfxArray [160 * 144]; // Stores the RGB value of each pixel.
	int x = 0; // The x coordinate on the current scanline to be rendered.
	unsigned char scrollX, scrollY; // Where the background is in relation to the rendering window.
	unsigned char currTile; // The current tile to have a line rendered.
	unsigned int mapBaseVal, mapOffset, tileStartByte; // Pointers to memory locations related to tiles.
	int cyclesSinceLastUpdate = 0; // Every 100 cycles of the CPU, update the keyboard state.
	myGB.memory[LY] = 0x0; // LY set to 0; this means we are rendering the first scanline.

	// Keep emulating until the end of time itself.
	for (;;)
	{
		
		// A scanline is drawn every 456 cycles, so run CPU for 456 cycles.
		for (int i = 0; i < 456; i++)
		{
			myGB.emulateCycle();

			// Update input state every 100 cycles to prevent slowdown.
			if (cyclesSinceLastUpdate == 100)
			{
				SDL_PumpEvents(); // This updates the keyboard state.
				cyclesSinceLastUpdate = 0;
			}
			cyclesSinceLastUpdate += 1;
			
			processInputs(kb); // Processes keyboard inputs if the input state is requested by the Game Boy.
			
		}
		
		// After setting the V-Blank interrupt, it can be reset.
		if (myGB.memory[IF] & 0x1)
			myGB.modifyBit(myGB.memory[IF], 0, 0);

		// Get scroll values.
		scrollX = myGB.memory[SCROLLX];
		scrollY = myGB.memory[SCROLLY];
	
		// Get the tile map base pointer to use.
		if (((myGB.memory[LCDC] >> 3) & 0x1))
			mapBaseVal = 0x9C00;
			
		else
			mapBaseVal = 0x9800;
			
		// If not in V-Blank, render a scanline. This is done by drawing a line of pixels from consecutive tiles.
		if (myGB.memory[LY] < 0x90)
		{
			for (int x = 0; x < 160; x++)
			{

				// Here we calculate the current tile to have its line drawn (very ugly). Background wraps around.

				if (myGB.memory[LY] + scrollY >= 256) // If reached bottom of background, wrap back to the top.
					mapOffset = (((x + scrollX) / 8) % 32) + (((myGB.memory[LY] + scrollY - 256) / 8) * 32);
				else if (x + scrollX >= 256) // X wrapping - right wraps back to left.
					mapOffset = (((x + scrollX - 256) / 8) % 32) + (((myGB.memory[LY] + scrollY) / 8) * 32);
				else if ((x + scrollX >= 256) && (myGB.memory[LY] + scrollY >= 256)) // X and Y wrap.
					mapOffset = (((x + scrollX - 256) / 8) % 32) + (((myGB.memory[LY] + scrollY - 256) / 8) * 32);
				else // Regular, no wrapping.
					mapOffset = (((x + scrollX) / 8) % 32) + (((myGB.memory[LY] + scrollY) / 8) * 32);
				
				currTile = myGB.memory[mapBaseVal + mapOffset]; // Gets the tile number from the tile map.
			
				// Gets the memory location of the start of the tile data using the correct method.
				if ((myGB.memory[LCDC] >> 4) & 0x1)
					tileStartByte = 0x8000 + (currTile * 16); // 8000 addressing (unsigned).
				else
				{
					tileStartByte = 0x9000 + (signed char(currTile) * 16); // 8800 addressing (signed).
				}
					
				drawLineOfTile(gfxArray, tileStartByte, x, myGB.memory[LY]); // Draw a line of the current tile.
			}
		}
	
		
		myGB.memory[LY] += 1; // Increment LY as a scanline has been drawn.

		// Once all scanlines have been drawn, render to the screen and set the V-Blank interrupt.
		if (myGB.memory[LY] == 0x90)
		{
			SDL_UpdateTexture(texture, NULL, gfxArray, 160 * 4);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			myGB.modifyBit(myGB.memory[IF], 1, 0);
		}
			
		// If LY exceeds its maximum value, reset it (end of V-Blank period).
		if (myGB.memory[LY] > 0x99)
			myGB.memory[LY] = 0x00;
		
	}
}
