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
// Also handles the spacebar to toggle logging.
void processInputs(const Uint8 kb[], SDL_GameController *controller)
{
	// Toggle logging.
	if (kb[SDL_SCANCODE_SPACE])
		myGB.logging = !myGB.logging;

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

// Draw a pixel of an 8x8 tile.
// memLocation = the start of tile data in memory.
// x = column in tile (0-7).
// y = row in tile (0-7).
void drawPixelOfTile(uint32_t gfxArray[], uint16_t memLocation, int x, int y)
{
	// Pixel info is stored across 2 bytes per row.
	int byte = (y % 8) * 2;
	uint8_t lowByte = myGB.memory[memLocation + byte];
	uint8_t highByte = myGB.memory[memLocation + byte + 1];

	// 2 bits per pixel, stored across the 2 bytes.
	uint8_t lowBit = (lowByte >> (7 - (x % 8))) & 0x1;
	uint8_t highBit = (highByte >> (7 - (x % 8))) & 0x1;

	// The high and low bit are added together. High bit is multiplied as
	// the high byte is treated as bits 8-15 rather than 0-7.
	uint8_t total = lowBit + (highBit * 2);

	// The total gives the shade of grey to be used for the pixel.
	uint8_t shade_index = (myGB.memory[BGP] >> (total * 2)) & 0x3;

	// Calculate the RGB value for the grey shade from this index.
	uint8_t shade = 0xFF - (shade_index * 85);

	// Now add this grey shade to the RGB array (same number for R, G and B).
	gfxArray[(x % 160) + (y * 160)] = (shade << 16 | shade << 8 | shade);
	
}

// Draw a pixel of a given sprite.
void drawPixelOfSprite(uint32_t gfxArray[], uint16_t memLocation, int x, int y)
{
	int byte = (y % 8) * 2;
	uint8_t lowByte = myGB.memory[memLocation + byte];
	uint8_t highByte = myGB.memory[memLocation + byte + 1];

	for (int i = 0; i < 8; i++)
	{
		uint8_t lowBit = (lowByte >> (7 - i)) & 0x1;
		uint8_t highBit = (highByte >> (7 - i)) & 0x1;

		uint8_t total = lowBit + (highBit * 2);

		uint8_t shade_index = (myGB.memory[BGP] >> (total * 2)) & 0x3;
		uint8_t shade = 0xFF - (shade_index * 85);

		gfxArray[((x + i) % 160) + (y * 160)] = (shade << 16 | shade << 8 | shade);
	}
	
}

// Draw the background, the lowest layer on the screen. The background has the ability to scroll.
void drawBackground(uint32_t gfxArray[])
{
	uint8_t scrollX, scrollY;  // Where the background is in relation to the rendering window.
	uint8_t tileNum;		   // The number of the tile to be rendered. Treated as unsigned or signed.

	// tileMap is the base memory location to retrieve tiles from.
	// mapOffset is the offset from the base location.
	// The sum of the above gives the memory location containing tileNum.
	// tileStartLoc is where data for the current tile starts.
	uint16_t tileMap, mapOffset, tileStart;		

	scrollX = myGB.memory[SCROLLX];
	scrollY = myGB.memory[SCROLLY];

	// Get the tile map base pointer to use.
	if (((myGB.memory[LCDC] >> 3) & 0x1))
		tileMap = 0x9C00;
	else
		tileMap = 0x9800;


	for (int x = 0; x < 160; x++)
	{
		// Y wrapping - if reached bottom of background, wrap back to the top.
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

		tileNum = myGB.memory[tileMap + mapOffset];

		// Gets the memory location of the start of the tile data using the correct method.
		if ((myGB.memory[LCDC] >> 4) & 0x1)
			tileStart = 0x8000 + (tileNum * 16);			  // 8000 addressing (unsigned).
		else
		{
			tileStart = 0x9000 + ((int8_t) tileNum * 16);  // 8800 addressing (signed).
		}

		drawPixelOfTile(gfxArray, tileStart, x, myGB.memory[LY]);
	}
}

// Draw the window, which is above the background and cannot scroll. See drawBackground().
void drawWindow(uint32_t gfxArray[])
{
	uint8_t winX, winY;  // Position of the window.
	uint8_t currTile;
	uint16_t tileMap, mapOffset, tileStart;

	winX = myGB.memory[WX] - 7;
	winY = myGB.memory[WY];

	if (((myGB.memory[LCDC] >> 6) & 0x1))
		tileMap = 0x9C00;

	else
		tileMap = 0x9800;

	for (int x = 0; x < 160; x++)
	{
		// Get offset based on window position.
		mapOffset = (((x + winX) / 8) % 32) + (((myGB.memory[LY] + winY) / 8) * 32);

		currTile = myGB.memory[tileMap + mapOffset];

		if ((myGB.memory[LCDC] >> 4) & 0x1)
			tileStart = 0x8000 + (currTile * 16);
		else
		{
			tileStart = 0x9000 + ((int8_t) currTile * 16);
		}

		drawPixelOfTile(gfxArray, tileStart, x, myGB.memory[LY]);
	}
}

// Draw all sprites, which can be anywhere on the screen.
void drawSprites(uint32_t gfxArray[])
{
	uint8_t x, y;
	uint8_t tileNum;
	uint16_t tileStart;

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
					tileNum = myGB.memory[0xFE00 + (sprite * 4) + 2];
					tileStart = 0x8000 + (tileNum * 16);
					if ((myGB.memory[0xFE00 + (sprite * 4) + 3] >> 7) & 0x1)
						drawPixelOfSprite(gfxArray, tileStart, x, myGB.memory[LY]);
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
	const int scale = 2; // How much to scale the graphics by.

	// Set up the graphics environment.
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
	SDL_Window* win = SDL_CreateWindow("GameJoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * scale, 144 * scale, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetScale(renderer, 2, 2);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, SDL_CreateRGBSurface(0, 160, 144, 24, 0, 0, 0, 0));
	
	const Uint8* kb = SDL_GetKeyboardState(NULL);  // Stores the state of currently pressed keys.

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
	
	myGB.initialize();  // Set up the Game Boy.

	// Open the file dialog and let the user select the ROM they want to play, and store its path.
	char filepath[100];
	setPathUsingFileDialog(filepath);

	// Load the game and update the window title.
	char gameTitle[16];
	myGB.loadGame(filepath, gameTitle);
	std::string windowTitle = "GameJoy - " + std::string(gameTitle, 16);
	SDL_SetWindowTitle(win, windowTitle.c_str());

	uint32_t gfxArray[160 * 144];  // Stores the RGB value of each pixel.
	
	int cyclesSinceLastUpdate = 0;  // Every 100 cycles of the CPU, update the keyboard state.
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
				// Update the event queue and controller state.
				SDL_PumpEvents();
				SDL_GameControllerUpdate();
				cyclesSinceLastUpdate = 0;
			}
			processInputs(kb, controller);
		}
		
		// If not in VBLANK, draw to the screen.
		if (myGB.memory[LY] < 0x90)
		{
			drawBackground(gfxArray);

			// Only draw window and sprites if enabled.
			if ((myGB.memory[LCDC] >> 5) & 0x1)
				drawWindow(gfxArray);
			if ((myGB.memory[LCDC] >> 1) & 0x1)
				drawSprites(gfxArray);

		}
	
		myGB.memory[LY] += 1; // Increment LY as a scanline has been drawn.

		if (myGB.memory[LY] == myGB.memory[LYC])
		{
			if ((myGB.memory[STAT] >> 6) & 0x1)
				myGB.modifyBit(myGB.memory[IF], 1, 1);	// STAT interrupt.
		}
			
		// Now in HBLANK.
		myGB.modifyBit(myGB.memory[STAT], 0, 0);
		myGB.modifyBit(myGB.memory[STAT], 0, 1);
		if ((myGB.memory[STAT] >> 3) & 0x1)
			myGB.modifyBit(myGB.memory[IF], 1, 1);	// STAT interrupt.
		
		// Once all scanlines have been drawn, render to the screen and set the V-Blank interrupt.
		if (myGB.memory[LY] == 0x90)
		{
			SDL_UpdateTexture(texture, NULL, gfxArray, 160 * 4);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			myGB.modifyBit(myGB.memory[IF], 1, 0);	// VBLANK interrupt.

			// Set mode flag in STAT for VBLANK.
			myGB.modifyBit(myGB.memory[STAT], 1, 0);
			myGB.modifyBit(myGB.memory[STAT], 0, 1);
			if ((myGB.memory[STAT] >> 4) & 0x1)
				myGB.modifyBit(myGB.memory[IF], 1, 1);	// STAT interrupt.
		}
			
		// If LY exceeds its maximum value, reset it (end of V-Blank period).
		if (myGB.memory[LY] > 0x99)
		{
			myGB.memory[LY] = 0x00;  // Reset LY.

			// Set mode flag in STAT for HBLANK.
			myGB.modifyBit(myGB.memory[STAT], 0, 0);
			myGB.modifyBit(myGB.memory[STAT], 0, 1);
			if ((myGB.memory[STAT] >> 3) & 0x1)
				myGB.modifyBit(myGB.memory[IF], 1, 1);	// STAT interrupt.
		}
	}
}