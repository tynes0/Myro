file(GLOB_RECURSE EXAMPLE_SOURCES "src/*.cpp")

add_executable(Myro-Examples ${EXAMPLE_SOURCES})

target_link_libraries(Myro-Examples PRIVATE Myro)

set_target_properties(Myro-Examples PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

set_target_properties(Myro-Examples PROPERTIES 
    VS_DEBUGGER_ENVIRONMENT "ALSOFT_LOGLEVEL=0"
)

# Assets kopyalama (Varsa)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    add_custom_command(TARGET Myro-Examples POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Myro-Examples>/assets"
    )
endif()