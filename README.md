# GameJoy
GameJoy is a Game Boy emulator being developed in C++.  
The gb/individual folder contains test ROMs created by Blargg. They can be downloaded from https://gbdev.gg8.se/files/roms/blargg-gb-tests/ under cpu_instrs.zip. The source code for the ROMs is also included in gb/source.

Currently I am implementing all the opcodes for the CPU. After this is completed, I can move onto implementing the graphics and input, with further work on the CPU if required.

Note: The project makes use of SDL2. The files need to be included under gb/SDL.
