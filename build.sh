mkdir build
cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=MinSizeRel
ninja all
./EmuEngine