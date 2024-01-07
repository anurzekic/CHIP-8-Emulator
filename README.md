# CHIP-8-Emulator
A CHIP-8 Emulator written in C++. It utilizes SDL2 to provide video and audio.

"CHIP-8 is an interpreted programming language, developed by Joseph Weisbecker made on his 1802 Microprocessor. It was initially used on the COSMAC VIP and Telmac 1800 8-bit microcomputers in the mid-1970s. CHIP-8 programs are run on a CHIP-8 virtual machine. It was made to allow video games to be more easily programmed for these computers." 

## Building
Requires CMake and SDL2: </br>
Ubuntu:
```
$ sudo apt-get install cmake libsdl2-dev
```
Arch:
```
$ sudo pacman -S sdl2
```
Fedora:
```
$ sudo dnf install cmake SDL2 SDL2-devel
```

To build the project:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```
## Running

To run the program do:
```
$ ./Chip8 <ROM file>
```
The repository includes 19 (public domain) ROMs of which 9 tests and 10 games.

Currently it suports only regular CHIP-8 ROMS. But I plan to add support for other variations in the future.

## TODO
- Fix issues with movement

## References
- https://en.wikipedia.org/wiki/CHIP-8
- http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#memmap
- https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
