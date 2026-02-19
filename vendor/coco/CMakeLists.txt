set(LIB_DIR "${CMAKE_CURRENT_BINARY_DIR}")

file(DOWNLOAD 
    "https://raw.githubusercontent.com/tynes0/coco/refs/heads/main/coco.h"
    "${LIB_DIR}/coco.h"
    SHOW_PROGRESS
)

add_library(coco INTERFACE)
target_include_directories(coco INTERFACE "${LIB_DIR}")