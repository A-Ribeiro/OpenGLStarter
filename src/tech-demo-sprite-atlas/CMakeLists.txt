project (tech-demo-sprite-atlas)

FILE( GLOB_RECURSE PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

add_executable(
    ${PROJECT_NAME} 
    ${PUBLIC_HEADERS} ${SRC}
)

tool_define_source_group(${PUBLIC_HEADERS} ${SRC})

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "BIN")

target_link_libraries(${PROJECT_NAME} 
    appkit-gl-engine
)

tool_copy_directory_after_build( ${PROJECT_NAME} resources )
