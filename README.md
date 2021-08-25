# Emulator Engine

## How to build

Depending on your platform, make a build file called *build.sh* or *build.bat* with the following contents:

> mkdir build
> cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=<path to vcpkg>/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=<Release \ Debug>
> cmake --build build

After executing this, the executable should be in the */build/* file along with the */build/data/* folder.

## Currently supported platforms:
- Bytepusher
    - works perfectly, open an issue if something unexpected happens.
- CHIP8 
    - might have some unsuported SCHIP or XOCHIP opcodes, open an issue if so.
- Gameboy
    - boot ROM checksum fails.
    - boot ROM is not displayed properly.
    - lots of opcodes missing.
- NES (soon)

## Screenshots from supported platforms:

### CHIP8:

![CHIP8 Tetris](https://raw.githubusercontent.com/bustatu/EmulatorEngine/main/github-images/chip8-tetris.png?raw=true)

### Bytepusher:

![Bytepusher Nyan Cat](https://raw.githubusercontent.com/bustatu/EmulatorEngine/main/github-images/bp-nyan.png?raw=true)

### Gameboy (work in progress):

![Gameboy Boot ROM](https://raw.githubusercontent.com/bustatu/EmulatorEngine/main/github-images/gb-bootrom.png?raw=true)