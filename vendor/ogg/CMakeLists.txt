FetchContent_Declare(
    ogg
    GIT_REPOSITORY https://github.com/xiph/ogg.git
    GIT_TAG        master
)
set(INSTALL_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(ogg)