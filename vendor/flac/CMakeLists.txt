include(FetchContent)

FetchContent_Declare(
    flac
    URL https://github.com/xiph/flac/archive/refs/tags/1.4.3.zip
)

# Gereksizleri Kapat
set(BUILD_PROGRAMS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(INSTALL_MANPAGES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(flac)

# --- FLAC SUSTURUCU ---
# FLAC hedeflerine "NDEBUG" (No Debug) tanımını ekliyoruz.
# Bu, debug modunda bile olsa FLAC'ın sessiz kalmasını sağlar.
if(TARGET FLAC)
    target_compile_definitions(FLAC PRIVATE NDEBUG)
endif()
if(TARGET flac)
    target_compile_definitions(flac PRIVATE NDEBUG)
endif()
if(TARGET flac_static)
    target_compile_definitions(flac_static PRIVATE NDEBUG)
endif()