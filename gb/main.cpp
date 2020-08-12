#include "gb.h"
#include "SDL.h"
#include <bitset>

gb myGB;

void displayBackground(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 155, 188, 15, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	int scrollY = myGB.memory[0xFF42];
	int scrollX = myGB.memory[0xFF43];

	for (int tile = 0; tile < 32 * 32; tile++)
	{
		int tileNumber = myGB.memory[0x9000 + tile];
		
		for (int byte = 0; byte < 15; byte += 2)
		{
			char lowByte = myGB.memory[(tileNumber * 16) + 0x8000 + byte];
			char highByte = myGB.memory[(tileNumber * 16) + 0x8000 + byte + 1];
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
				for (int i = 0; i < 5; i++)
				{
					rect.x = (((tile * 8) + bit) * 5) + i;
					rect.y = ((byte / 2) * 5) + i;
					rect.w = 5;
					rect.h = 5;
					if (rect.x > 160 * 5)
						rect.x = ((rect.x * 5) - (160 * 5));
					if (rect.y > 144 * 5)
						rect.x = ((rect.y * 5) - (144 * 5));
					SDL_RenderFillRect(renderer, &rect);
				}
				
			}
		}
	}
	SDL_RenderPresent(renderer);
}

void drawSprites(SDL_Renderer *renderer)
{
	for (int sprite = 0; sprite < 40; sprite++)
	{
		char y = myGB.memory[0xFA00 + (sprite * 4)];
		char x = myGB.memory[0xFE00 + (sprite * 4) + 1];
		char tileNumber = myGB.memory[0xFE00 + (sprite * 4) + 2];

		for (int byte = 0; byte < 15; byte += 2)
		{
			char lowByte = myGB.memory[(tileNumber * 16) + 0x8000 + byte];
			char highByte = myGB.memory[(tileNumber * 16) + 0x8000 + byte + 1];
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
				for (int i = 0; i < 5; i++)
				{
					rect.x = x * 5;
					rect.y = y * 5;
					rect.w = 5;
					rect.h = 5;
					
					SDL_RenderFillRect(renderer, &rect);
				}

			}
		}










	}
	SDL_RenderPresent(renderer);
}
int main(int argc, char** argv)
{
	//SDL_Init(SDL_INIT_VIDEO);
	//SDL_Window *win = SDL_CreateWindow("GameJoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * 5, 144 * 5, SDL_WINDOW_SHOWN);
	//SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	//SDL_Event e;
	myGB.initialize();
	myGB.loadGame();

	for (;;)
	{
		
		myGB.emulateCycle();
		//displayBackground(renderer);
		//drawSprites(renderer);
		//while (SDL_PollEvent(&e) != 0)
		//{
		//	break;
		//}
	}
		
}
