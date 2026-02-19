include(FetchContent)

FetchContent_Declare(
    opusfile
    URL https://github.com/xiph/opusfile/archive/refs/tags/v0.12.zip
)
FetchContent_MakeAvailable(opusfile)

file(GLOB OPUSFILE_SOURCES "${opusfile_SOURCE_DIR}/src/*.c")

list(FILTER OPUSFILE_SOURCES EXCLUDE REGEX "http.c")
list(FILTER OPUSFILE_SOURCES EXCLUDE REGEX "wincerts.c")

add_library(opusfile STATIC ${OPUSFILE_SOURCES})

target_include_directories(opusfile PUBLIC 
    "${opusfile_SOURCE_DIR}/include"
)

target_compile_definitions(opusfile PRIVATE 
    OP_DISABLE_HTTP          # HTTP kapat
    _CRT_SECURE_NO_WARNINGS  # Uyarıları kapat
)

target_link_libraries(opusfile PRIVATE 
    Ogg::ogg 
    Opus::opus
)