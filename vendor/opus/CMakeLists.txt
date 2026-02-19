include(FetchContent)

FetchContent_Declare(
    opus
    URL https://github.com/xiph/opus/archive/refs/tags/v1.5.1.zip
)

set(OPUS_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(OPUS_BUILD_PROGRAMS OFF CACHE BOOL "" FORCE)

set(OPUS_BUILD_SHARED_LIBRARY OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(opus)