project (test-pbr-coefs)

FILE( GLOB_RECURSE PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

define_source_group(${PUBLIC_HEADERS} ${SRC})

add_executable( ${PROJECT_NAME} ${PUBLIC_HEADERS} ${SRC} )

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
#set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "TOOL")

#link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC 
    InteractiveToolkit-Extension
    appkit-gl-engine
)
