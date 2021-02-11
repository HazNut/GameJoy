#include "gb.h"
#include "SDL.h"
#include <algorithm>
#include <iostream>
#include <shobjidl.h>
#include <string>
#include <windows.h>

gb myGB; // The Game Boy's CPU is stored as an object.

// Checks to see if the CPU wants to check for dpad/button inputs, then sets the JOYP register depending
// on what directions/buttons were pressed.
void processInputs(const Uint8 kb[], SDL_GameController *controller)
{
	// GB wants to check buttons.
	if (((myGB.memory[JOYP] >> 5) & 0x1) == 0)
	{
		if (kb[SDL_SCANCODE_P] || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B))  // A
			myGB.modifyBit(myGB.memory[JOYP], 0, 0); // If pressing A, set the input state in memory.
		else
			myGB.modifyBit(myGB.memory[JOYP], 1, 0); // Else reset the input state (1 = off).

		if (kb[SDL_SCANCODE_O] || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A))  // B
			myGB.modifyBit(myGB.memory[JOYP], 0, 1);
		else
			myGB.modifyBit(myGB.memory[JOYP], 1, 1);

		if (kb[SDL_SCANCODE_L] || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_BACK))  // SELECT
			myGB.modifyBit(myGB.memory[JOYP], 0, 2);
		else
			myGB.modifyBit(myGB.memory[JOYP], 1, 2);

		if (kb[SDL_SCANCODE_K] || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START))  // START
			myGB.modifyBit(myGB.memory[JOYP], 0, 3);
		else
			myGB.modifyBit(myGB.memory[JOYP], 1, 3);
	}

	// GB wants to check D-pad.
	else if (((myGB.memory[JOYP] >> 4) & 0x1) == 0)
	{
		if (kb[SDL_SCANCODE_D] || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))  // RIGHT
			myGB.modifyBit(myGB.memory[JOYP], 0, 0);
		else
			myGB.modifyBit(myGB.memory[JOYP], 1, 0);

		if (kb[SDL_SCANCODE_A] || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT))  // LEFT
			myGB.modifyBit(myGB.memory[JOYP], 0, 1);
		else
			myGB.modifyBit(myGB.memory[JOYP], 1, 1);

		if (kb[SDL_SCANCODE_W] || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP))  // UP
			myGB.modifyBit(myGB.memory[JOYP], 0, 2);
		else
			myGB.modifyBit(myGB.memory[JOYP], 1, 2);

		if (kb[SDL_SCANCODE_S] || SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN))  // DOWN
			myGB.modifyBit(myGB.memory[JOYP], 0, 3);
		else
			myGB.modifyBit(myGB.memory[JOYP], 1, 3);
	}
}

// Draw a pixel of the currently selected tile.
// X = column of tile.
// Y = row of tile.
void drawPixelOfTile(unsigned int gfxArray[160 * 144], unsigned int memLocation, int x, int y)
{
	// Pixel info is stored across 2 bytes per row.
	int byte = (y % 8) * 2;
	unsigned char lowByte = myGB.memory[memLocation + byte];
	unsigned char highByte = myGB.memory[memLocation + byte + 1];

	// 2 bits per pixel, stored across the 2 bytes.
	unsigned char lowBit = (lowByte >> (7 - (x % 8))) & 0x1;
	unsigned char highBit = (highByte >> (7 - (x % 8))) & 0x1;

	// The high and low bit are added together. High bit is multiplied as
	// the high byte is treated as bits 8-15 rather than 0-7.
	unsigned int total = lowBit + (highBit * 2);

	// The total gives the shade of grey to be used for the pixel.
	unsigned char shade = myGB.memory[BGP] >> total;

	// Now add this grey shade to the RGB array.
	gfxArray[(x % 160) + (y * 160)] = ((shade << 16) | (shade << 8) | shade);
}

// Draw a pixel of a given sprite.
void drawPixelOfSprite(unsigned int gfxArray[160 * 144], unsigned int memLocation, int x, int y)
{
	// Pixel info is stored across 2 bytes per row.
	int byte = (y % 8) * 2;
	unsigned char lowByte = myGB.memory[memLocation + byte];
	unsigned char highByte = myGB.memory[memLocation + byte + 1];

	for (int i = 0; i < 8; i++)
	{
		// 2 bits per pixel, stored across the 2 bytes.
		unsigned char lowBit = (lowByte >> (7 - i)) & 0x1;
		unsigned char highBit = (highByte >> (7 - i)) & 0x1;

		// The high and low bit are added together.
		unsigned int total = lowBit + (highBit * 2);

		// The total gives the shade of grey to be used for the pixel.
		unsigned char shade = myGB.memory[BGP] >> total;

		// Now add this grey shade to the RGB array.
		gfxArray[((x + i) % 160) + (y * 160)] = ((shade << 16) | (shade << 8) | shade);
	}
	
}

// Draw the background.
void drawBackground(unsigned int gfxArray[160 * 144])
{
	unsigned char scrollX, scrollY;						// Where the background is in relation to the rendering window.
	unsigned char currTile;								// The current tile to have a line rendered.
	unsigned int mapBaseVal, mapOffset, tileStartByte;	// Pointers to memory locations related to tiles.

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

			// If reached bottom of background, wrap back to the top.
			if (myGB.memory[LY] + scrollY >= 256)
				mapOffset = (((x + scrollX) / 8) % 32) + (((myGB.memory[LY] + scrollY - 256) / 8) * 32);

			// X wrapping - right wraps back to left.
			else if (x + scrollX >= 256)
				mapOffset = (((x + scrollX - 256) / 8) % 32) + (((myGB.memory[LY] + scrollY) / 8) * 32);

			// X and Y wrap.
			else if ((x + scrollX >= 256) && (myGB.memory[LY] + scrollY >= 256))
				mapOffset = (((x + scrollX - 256) / 8) % 32) + (((myGB.memory[LY] + scrollY - 256) / 8) * 32);

			// Regular, no wrapping.
			else
				mapOffset = (((x + scrollX) / 8) % 32) + (((myGB.memory[LY] + scrollY) / 8) * 32);

			currTile = myGB.memory[mapBaseVal + mapOffset];	// Gets the tile number from the tile map.


			// Gets the memory location of the start of the tile data using the correct method.
			if ((myGB.memory[LCDC] >> 4) & 0x1)
				tileStartByte = 0x8000 + (currTile * 16);					// 8000 addressing (unsigned).
			else
			{
				tileStartByte = 0x9000 + (signed char(currTile) * 16);		// 8800 addressing (signed).
			}

			drawPixelOfTile(gfxArray, tileStartByte, x, myGB.memory[LY]);	// Draw a pixel of the current tile.
		}
	}
}

// Draw the window.
void drawWindow(unsigned int gfxArray[160 * 144])
{
	unsigned char winX, winY;							// Where the background is in relation to the rendering window.
	unsigned char currTile;								// The current tile to have a line rendered.
	unsigned int mapBaseVal, mapOffset, tileStartByte;	// Pointers to memory locations related to tiles.

	// Get scroll values.
	winX = myGB.memory[WINX] - 7;
	winY = myGB.memory[WINY];

	// Get the tile map base pointer to use.
	if (((myGB.memory[LCDC] >> 6) & 0x1))
		mapBaseVal = 0x9C00;

	else
		mapBaseVal = 0x9800;

	// If not in V-Blank, render a scanline. This is done by drawing a line of pixels from consecutive tiles.
	if (myGB.memory[LY] < 0x90)
	{
		for (int x = 0; x < 160; x++)
		{

			// Here we calculate the current tile to have its line drawn (very ugly). Background wraps around.

			// If reached bottom of background, wrap back to the top.
			mapOffset = (((x + winX) / 8) % 32) + (((myGB.memory[LY] + winY) / 8) * 32);

			currTile = myGB.memory[mapBaseVal + mapOffset];	// Gets the tile number from the tile map.


			// Gets the memory location of the start of the tile data using the correct method.
			if ((myGB.memory[LCDC] >> 4) & 0x1)
				tileStartByte = 0x8000 + (currTile * 16);					// 8000 addressing (unsigned).
			else
			{
				tileStartByte = 0x9000 + (signed char(currTile) * 16);		// 8800 addressing (signed).
			}

			drawPixelOfTile(gfxArray, tileStartByte, x, myGB.memory[LY]);	// Draw a line of the current tile.
		}
	}
}

// Draw sprites.
void drawSprites(unsigned int gfxArray[160 * 144])
{
	unsigned char x, y, currTile;
	unsigned int tileStartByte;

	for (int sprite = 0; sprite < 40; sprite++)
	{
		y = myGB.memory[0xFE00 + (sprite * 4)] - 16;
		
		if ((myGB.memory[LY] <= y + 7) && (myGB.memory[LY] >= y))
		{
			if ((y < 144) && (y > 0))
			{
				x = myGB.memory[0xFE00 + (sprite * 4) + 1] - 8;
				if ((x < 160) && (x > 0))
				{
					currTile = myGB.memory[0xFE00 + (sprite * 4) + 2];
					tileStartByte = 0x8000 + (currTile * 16);
					if ((myGB.memory[0xFE00 + (sprite * 4) + 3] >> 7) & 0x1)
						drawPixelOfSprite(gfxArray, tileStartByte, x, myGB.memory[LY]);
				}
			}
		}
	}
}

// Open a file dialog to select a ROM, then store the ROM's path.
void setPathUsingFileDialog(char* filepath)
{

	// Stores the result of certain operations.
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	// pszFilePath stores the file path to the opened file. Has to be initialized in a weird way.
	wchar_t wcharTemp[] = L"";
	PWSTR pszFilePath = wcharTemp;

	if (SUCCEEDED(hr))
	{
		// Create interface for a Common Item Dialog object, then create the object itself, passing the interface.
		IFileOpenDialog* pFileOpen;
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			// Create and set the file dialog filter to only Game Boy ROMs.
			COMDLG_FILTERSPEC rgSpec[] =
			{
				L"Game Boy ROM", L"*.gb"
			};
			hr = pFileOpen->SetFileTypes(1, rgSpec);

			if (SUCCEEDED(hr))
			{
				hr = pFileOpen->Show(NULL); // Show the Open dialog box.
				if (SUCCEEDED(hr))
				{
					// Get the file the user selected.
					IShellItem* pItem;
					hr = pFileOpen->GetResult(&pItem);

					if (SUCCEEDED(hr))
					{
						// Get the file path, then convert it to the right format and store it.
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
						if (pszFilePath)
							wcstombs(filepath, pszFilePath, 100);

						CoTaskMemFree(pszFilePath);
						pItem->Release();
					}
					pFileOpen->Release();
				}
				CoUninitialize();
			}
		}
	}
}

int main(int argc, char* args[])
{
	// Set up the graphics environment.
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
	SDL_Window* win = SDL_CreateWindow("GameJoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * 2, 144 * 2, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetScale(renderer, 2, 2);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, SDL_CreateRGBSurface(0, 160, 144, 24, 0, 0, 0, 0));
	
	const Uint8* kb = SDL_GetKeyboardState(NULL); // Stores the state of currently pressed keys.

	// Set up controller (must be connected before running).
	SDL_GameController* controller = nullptr;
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		if (SDL_IsGameController(i))
		{
			std::cout << "CONTROLLER CONNECTED\n";
			controller = SDL_GameControllerOpen(i);
			break;
		}
	}
	
	myGB.initialize(); // Set up the Game Boy.

	// Open the file dialog and let the user select the ROM they want to play, and store its path.
	char filepath[100];
	setPathUsingFileDialog(filepath);

	// Load the game and update the window title.
	char gameTitle[16];
	myGB.loadGame(filepath, gameTitle);
	std::string windowTitle = "GameJoy - " + std::string(gameTitle, 16);
	SDL_SetWindowTitle(win, windowTitle.c_str());

	unsigned int* gfxArray = new unsigned int [160 * 144];	// Stores the RGB value of each pixel.
	
	int cyclesSinceLastUpdate = 0;							// Every 100 cycles of the CPU, update the keyboard state.
	myGB.modifyBit(myGB.memory[LCDC], 1, 7);
	// Keep emulating until the end of time itself.
	for (;;)
	{
		
		// A scanline is drawn every 456 cycles, so run CPU for 456 cycles.
		for (int i = 0; i < 456; i++)
		{
			myGB.emulateCycle();
			cyclesSinceLastUpdate += 1;
			// Update input state every 100 cycles to prevent slowdown.
			if (cyclesSinceLastUpdate == 100)
			{
				SDL_PumpEvents();	// This updates the keyboard state.
				SDL_GameControllerUpdate();
				cyclesSinceLastUpdate = 0;
				if (kb[SDL_SCANCODE_SPACE])
					myGB.logging = !myGB.logging;
			}
			
			processInputs(kb, controller);		// Processes keyboard inputs if the input state is requested by the Game Boy.
			
		}
		
		// If not in VBLANK, draw to the screen.
		if (myGB.memory[LY] < 0x90)
		{
			drawBackground(gfxArray);

			// Draw window if enabled.
			if ((myGB.memory[LCDC] >> 5) & 0x1)
				drawWindow(gfxArray);
			if ((myGB.memory[LCDC] >> 1) & 0x1)
				drawSprites(gfxArray);

		}
		
			
		myGB.memory[LY] += 1; // Increment LY as a scanline has been drawn.

		// Check LY = LYC.
		if (myGB.memory[LY] == myGB.memory[0xFF45])
		{
			myGB.modifyBit(myGB.memory[IF], 1, 2);	// STAT interrupt.
			if ((myGB.memory[0xFF41] >> 6) & 0x1)
				myGB.modifyBit(myGB.memory[IF], 1, 1);	// STAT interrupt.
		}
			

		// Now in HBLANK.
		myGB.modifyBit(myGB.memory[0xFF41], 0, 0);
		myGB.modifyBit(myGB.memory[0xFF41], 0, 1);
		if ((myGB.memory[0xFF41] >> 3) & 0x1)
			myGB.modifyBit(myGB.memory[IF], 1, 1);	// STAT interrupt.
		

		// Once all scanlines have been drawn, render to the screen and set the V-Blank interrupt.
		if (myGB.memory[LY] == 0x90)
		{
			SDL_UpdateTexture(texture, NULL, gfxArray, 160 * 4);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			myGB.modifyBit(myGB.memory[IF], 1, 0);	// VBLANK interrupt.

			// Set mode flag in STAT for VBLANK.
			myGB.modifyBit(myGB.memory[0xFF41], 1, 0);
			myGB.modifyBit(myGB.memory[0xFF41], 0, 1);
			if ((myGB.memory[0xFF41] >> 4) & 0x1)
				myGB.modifyBit(myGB.memory[IF], 1, 1);	// STAT interrupt.

		}
			
		// If LY exceeds its maximum value, reset it (end of V-Blank period).
		if (myGB.memory[LY] > 0x99)
		{
			myGB.memory[LY] = 0x00;					// Reset LY.

			// Set mode flag in STAT for HBLANK.
			myGB.modifyBit(myGB.memory[0xFF41], 0, 0);
			myGB.modifyBit(myGB.memory[0xFF41], 0, 1);
			if ((myGB.memory[0xFF41] >> 3) & 0x1)
				myGB.modifyBit(myGB.memory[IF], 1, 1);	// STAT interrupt.
		}
	}
}