set(LIB_DIR "${CMAKE_CURRENT_BINARY_DIR}")

file(DOWNLOAD 
    "https://raw.githubusercontent.com/lieff/minimp3/refs/heads/master/minimp3.h"
    "${LIB_DIR}/minimp3.h"
    SHOW_PROGRESS
)
file(DOWNLOAD 
    "https://raw.githubusercontent.com/lieff/minimp3/refs/heads/master/minimp3_ex.h"
    "${LIB_DIR}/minimp3_ex.h"
    SHOW_PROGRESS
)

set(IMPL_FILE "${LIB_DIR}/minimp3_impl.cpp")
if(NOT EXISTS ${IMPL_FILE})
    file(WRITE ${IMPL_FILE} "#define MINIMP3_IMPLEMENTATION\n#include \"minimp3.h\"\n#include \"minimp3_ex.h\"\n")
endif()

add_library(minimp3 STATIC ${IMPL_FILE})
target_include_directories(minimp3 PUBLIC "${LIB_DIR}")