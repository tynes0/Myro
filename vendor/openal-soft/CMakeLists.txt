include(FetchContent)

FetchContent_Declare(
    openal-soft
    GIT_REPOSITORY https://github.com/kcat/openal-soft.git
    GIT_TAG        master
)

set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)
set(LIBTYPE STATIC CACHE STRING "" FORCE)

FetchContent_MakeAvailable(openal-soft)

# --- HEADER FIX (alc/alcmain.h ve AL/al.h için) ---
if(TARGET OpenAL)
    get_target_property(OAL_SOURCE_DIR OpenAL SOURCE_DIR)
    target_include_directories(OpenAL INTERFACE 
        "${OAL_SOURCE_DIR}/include"
        "${OAL_SOURCE_DIR}"
        "${OAL_SOURCE_DIR}/common"
    )
endif()