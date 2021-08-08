add_library(GUI
    "font.cpp"
    "text.cpp"
)

target_include_directories(GUI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})