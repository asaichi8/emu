Untitled NES Emulator
========================================================

NES emulator

Linux/MacOS
------------
### Dependencies
apt: ```sudo apt install cmake ninja g++ libsdl2-dev```

pacman: ```sudo pacman -S cmake ninja gcc sdl2```

Homebrew: ```brew install cmake ninja sdl2``` // TODO: check this

### Usage
1. Install dependencies.
2. Clone the repo: ```git clone https://github.com/asaichi8/emu/```
3. Enter the directory: ```cd emu```
4. Create a build directory: ```mkdir build && cd build```
5. Set up cmake: ```cmake ..``` or ```cmake .. -G "Ninja"```
5. Compile: ```make``` or ```ninja```
6. Run: ```./emu <rom_path>```


Windows // TODO: update this using cmake
------------
### Dependencies
1. MinGW: follow the instructions at: https://code.visualstudio.com/docs/cpp/config-mingw
2. SDL2: download the latest development MinGW version in SDL releases from: https://www.libsdl.org/

### Usage
1. Install/download dependencies.
2. Clone the repo.
3. Extract the SDL2 `lib` and `include` folders from the SDL2 mingw32 folder (most likely called something like `i686-w64-mingw32`) into a folder called `SDL2` into the `include` folder in the repository.
4. Compile with ```make``` in a terminal.
5. Run `emu.exe` in the `bin` folder.
