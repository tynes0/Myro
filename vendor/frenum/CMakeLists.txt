set(LIB_DIR "${CMAKE_CURRENT_BINARY_DIR}")

file(DOWNLOAD 
    "https://raw.githubusercontent.com/tynes0/Frenum/refs/heads/main/frenum.h"
    "${LIB_DIR}/frenum.h"
    SHOW_PROGRESS
)

add_library(frenum INTERFACE)
target_include_directories(frenum INTERFACE "${LIB_DIR}")