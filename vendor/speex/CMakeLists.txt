include(FetchContent)

FetchContent_Declare(
    speex
    URL https://github.com/xiph/speex/archive/refs/tags/Speex-1.2.1.zip
)
FetchContent_MakeAvailable(speex)
FetchContent_GetProperties(speex)

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

set(DUMMY_TYPES "${CMAKE_CURRENT_BINARY_DIR}/speex_config_types.h")
if(NOT EXISTS ${DUMMY_TYPES})
    file(WRITE ${DUMMY_TYPES} 
        "/* CMake dummy speex_config_types.h */\n"
        "#ifndef __SPEEX_TYPES_H__\n"
        "#define __SPEEX_TYPES_H__\n"
        "#include <stdint.h>\n"
        "typedef int16_t spx_int16_t;\n"
        "typedef uint16_t spx_uint16_t;\n"
        "typedef int32_t spx_int32_t;\n"
        "typedef uint32_t spx_uint32_t;\n"
        "#endif\n"
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
    message(FATAL_ERROR "Speex source files not found!")
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