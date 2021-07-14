# Force the usage of the clang compiler
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

# Build using ninja generator
cmake -Bbuild -G Ninja

# Make the files
cd build

ninja all

# Run the resulting file
./EmuEngine