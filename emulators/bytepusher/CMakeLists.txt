add_library(Bytepusher
    "bytepusher.cpp"
    "components/vm.cpp"
)

# SDL2 stuff
find_package(SDL2 REQUIRED)

# Include directories
target_include_directories(Bytepusher PUBLIC ${SDL2_INCLUDE_DIR})

# Link external libraries
target_link_libraries(
    Bytepusher
    SDL2::SDL2
    SDL2::SDL2main
)