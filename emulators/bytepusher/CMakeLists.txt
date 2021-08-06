add_library(BYTEPUSHER
    "bytepusher.cpp"
    "components/vm.cpp"
)

target_include_directories(BYTEPUSHER PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})