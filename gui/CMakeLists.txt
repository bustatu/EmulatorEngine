add_library(GUI
    "text/font.cpp"
    "text/text.cpp"
)

# SDL2 stuff
find_package(SDL2 REQUIRED)
find_package(sdl2-ttf REQUIRED)

# Include directories
target_include_directories(GUI PUBLIC ${SDL2_INCLUDE_DIR})

# Link external libraries
target_link_libraries(
    GUI
    PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    SDL2::SDL2_ttf
)