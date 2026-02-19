set(LIB_DIR "${CMAKE_CURRENT_BINARY_DIR}")

file(DOWNLOAD 
    "https://raw.githubusercontent.com/mackron/miniaudio/refs/heads/master/miniaudio.h"
    "${LIB_DIR}/miniaudio.h"
    SHOW_PROGRESS
)

set(IMPL_FILE "${LIB_DIR}/miniaudio_impl.cpp")
if(NOT EXISTS ${IMPL_FILE})
    file(WRITE ${IMPL_FILE} "#define MINIAUDIO_IMPLEMENTATION\n#include \"miniaudio.h\"\n")
endif()

add_library(miniaudio STATIC ${IMPL_FILE})
target_include_directories(miniaudio PUBLIC "${LIB_DIR}")