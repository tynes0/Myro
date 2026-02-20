cmake_minimum_required(VERSION 3.15)
project(vendor_lame)

include(FetchContent)
FetchContent_Declare(
    lame
    GIT_REPOSITORY https://github.com/Iunusov/libmp3lame-CMAKE.git
    GIT_TAG        master
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build LAME as a static library" FORCE)

FetchContent_MakeAvailable(lame)


if(TARGET mp3lame)

    target_include_directories(mp3lame INTERFACE 
        $<BUILD_INTERFACE:${lame_SOURCE_DIR}/include>
    )
        add_library(Myro::lame ALIAS mp3lame)
endif()