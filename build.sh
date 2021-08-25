mkdir build
cmake -S . -B build -G "Ninja"
cmake --build build
cd build
./EmuEngine