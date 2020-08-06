#include "gb.h"

gb myGB;

int main(int argc, char** argv)
{
	myGB.initialize();
	myGB.loadGame();

	for (;;)
		myGB.emulateCycle();
}