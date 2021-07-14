:: Build using ninja generator
cmake -Bbuild -G Ninja

:: Make the files
cd build

ninja all

:: Run the resulting file
EmuEngine.exe