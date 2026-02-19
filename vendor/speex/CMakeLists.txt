include(FetchContent)

# Speex Reposunu Çek
FetchContent_Declare(
    speex
    GIT_REPOSITORY https://github.com/xiph/speex.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(speex)

# --- DUMMY CONFIG.H ---
set(DUMMY_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/config.h")
if(NOT EXISTS ${DUMMY_CONFIG})
    file(WRITE ${DUMMY_CONFIG} 
        "/* CMake dummy config.h */\n"
        "#define HAVE_STRING_H 1\n"
        "#define HAVE_MEMORY_H 1\n"
    )
endif()

# Kaynak kodları topla
file(GLOB SPEEX_SOURCES 
    "${speex_SOURCE_DIR}/libspeex/*.c"
    "${speex_SOURCE_DIR}/libspeex/*.h"
)

# Gereksiz dosyaları filtrele
list(FILTER SPEEX_SOURCES EXCLUDE REGEX "test.*\\.c")
list(FILTER SPEEX_SOURCES EXCLUDE REGEX "client.*\\.c")
list(FILTER SPEEX_SOURCES EXCLUDE REGEX "mdf\\.c")

add_library(speex STATIC ${SPEEX_SOURCES})

target_include_directories(speex PUBLIC 
    "${speex_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# --- DEFINITIONS GÜNCELLEMESİ ---
target_compile_definitions(speex PRIVATE
    FLOATING_POINT
    EXPORT=
    _CRT_SECURE_NO_WARNINGS
    HAVE_CONFIG_H
    USE_SMALLFT  # <-- SORUNU ÇÖZEN SATIR: Speex'in kendi FFT'sini kullan
)

if(TARGET Ogg::ogg)
    target_link_libraries(speex PRIVATE Ogg::ogg)
endif()