#include "gb.h"
#include "SDL.h"

gb myGB;

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *win = SDL_CreateWindow("GameJoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * 5, 144 * 5, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_Event e;
	myGB.initialize();
	myGB.loadGame();

	for (;;)
	{
		myGB.emulateCycle();
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

		for (int tile = 0; tile < 32 * 32; tile++)
		{
			int tileNumber = myGB.memory[0x9800 + tile];
			for (int byte = 0; byte < 15; byte += 2)
			{
				char lowByte = myGB.memory[(tileNumber * 16) + 0x8000 + byte];
				char highByte = myGB.memory[(tileNumber * 16) + 0x8000 + byte + 1];
				for (int bit = 0; bit < 8; bit++)
				{
					char lowBit = lowByte >> bit & 0x1;
					char highBit = highByte >> bit & 0x1;
					int total = (lowBit * 1) + (highBit * 2);
					if (total > 0)
					{
						SDL_Rect rect;
						rect.x = ((tile * 32) + (byte * 8) + bit) * 5;
						rect.y = tile * 5;
						rect.w = 5;
						rect.h = 5;
						SDL_RenderFillRect(renderer, &rect);
					}
				}
			}
		}
		SDL_RenderPresent(renderer);	
		while (SDL_PollEvent(&e) != 0)
		{
			0;
		}
	}
		
}