# Emulator Engine

A CHIP8, Bytepusher and Gameboy (WIP) emulator.

## How to build

First you need to install *vcpkg* and download the *sdl2* and *sdl2-ttf* packages from it.

Depending on your platform, make a build file called *build.sh* or *build.bat* with the following contents, changing the contents in <> to fit your requirements:

```shell
mkdir build
cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=<Path to vcpkg>/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=<Release \ Debug>
cmake --build build
```

After executing this, the executable should be in the */build/* file along with the */build/data/* folder.

## Currently supported platforms:
- Bytepusher
    - works perfectly, open an issue if something unexpected happens.
- CHIP8 
    - might have some unsuported SCHIP or XOCHIP opcodes, open an issue if so.
- Gameboy
    - no APU is implemented.
    - only MBC1, MBC3 (partially) and MBC5 are implemented.
- NES (soon)