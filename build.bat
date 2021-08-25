mkdir build
cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=D:/Tools/VCPKG/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build
cd build
EmuEngine.exe