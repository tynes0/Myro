set(LIB_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# Header ve Source indir
file(DOWNLOAD 
    "https://raw.githubusercontent.com/tynes0/dtlog/refs/heads/main/dtlog.h"
    "${LIB_DIR}/dtlog.h"
    SHOW_PROGRESS
)
file(DOWNLOAD 
    "https://raw.githubusercontent.com/tynes0/dtlog/refs/heads/main/dtlog.cpp"
    "${LIB_DIR}/dtlog.cpp"
    SHOW_PROGRESS
)

add_library(dtlog STATIC "${LIB_DIR}/dtlog.cpp" "${LIB_DIR}/dtlog.h")
target_include_directories(dtlog PUBLIC "${LIB_DIR}")