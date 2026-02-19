include(FetchContent)

FetchContent_Declare(
    speex
    URL https://github.com/xiph/speex/archive/refs/tags/Speex-1.2.1.zip
)
FetchContent_MakeAvailable(speex)
FetchContent_GetProperties(speex)

# --- DUMMY CONFIG.H ---
set(DUMMY_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/config.h")
if(NOT EXISTS ${DUMMY_CONFIG})
    file(WRITE ${DUMMY_CONFIG} 
        "/* CMake dummy config.h */\n"
        "#define HAVE_STRING_H 1\n"
        "#define HAVE_MEMORY_H 1\n"
        "#ifdef _MSC_VER\n"
        "#define inline __inline\n"
        "#endif\n"
        "#define EXPORT\n"
    )
endif()

file(GLOB SPEEX_SOURCES 
    "${speex_SOURCE_DIR}/libspeex/*.c"
    "${speex_SOURCE_DIR}/libspeex/*.h"
)

list(FILTER SPEEX_SOURCES EXCLUDE REGEX "/test[^/]*\\.c$")
list(FILTER SPEEX_SOURCES EXCLUDE REGEX "/client[^/]*\\.c$")
list(FILTER SPEEX_SOURCES EXCLUDE REGEX "/mdf\\.c$")

if(NOT SPEEX_SOURCES)
    message(FATAL_ERROR "Speex kaynak dosyalari bulunamadi!")
endif()

add_library(speex STATIC ${SPEEX_SOURCES})

set_target_properties(speex PROPERTIES LINKER_LANGUAGE C)

target_include_directories(speex PUBLIC 
    "${speex_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_compile_definitions(speex PRIVATE
    FLOATING_POINT
    _CRT_SECURE_NO_WARNINGS
    HAVE_CONFIG_H
    USE_SMALLFT
)

if(TARGET Ogg::ogg)
    target_link_libraries(speex PRIVATE Ogg::ogg)
endif()