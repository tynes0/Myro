file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADERS 
    "src/*.h" "src/*.hpp" "src/*.inl"
    "include/*.h" "include/*.hpp" "include/*.inl"
)

add_library(Myro STATIC ${SOURCES} ${HEADERS})

target_include_directories(Myro
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_compile_definitions(Myro PUBLIC
    AL_LIBTYPE_STATIC
    FLAC__NO_DLL
    _CRT_SECURE_NO_WARNINGS
    $<$<CONFIG:Debug>:MYRO_DEBUG>
    $<$<CONFIG:Release>:MYRO_RELEASE>
    $<$<CONFIG:Dist>:MYRO_DIST>
)

target_link_libraries(Myro PUBLIC
    OpenAL
    Ogg::ogg
    Vorbis::vorbis
    Vorbis::vorbisenc
    vorbisfile
    Opus::opus
    opusfile
    FLAC::FLAC
    speex
    Myro::lame

    miniaudio
    minimp3
    coco
    dtlog
    frenum
)

if(WIN32)
    target_link_libraries(Myro PRIVATE winmm)
endif()