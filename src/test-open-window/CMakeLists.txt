project (test-open-window)

# include(${CMAKE_SOURCE_DIR}/libs/InteractiveToolkit-Extension/cmake/libLinkOpenGL.cmake)

FILE( GLOB_RECURSE PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

define_source_group(${PUBLIC_HEADERS} ${SRC})

add_executable( ${PROJECT_NAME} ${PUBLIC_HEADERS} ${SRC} )

target_link_libraries(${PROJECT_NAME}
PUBLIC 
    appkit-window
)

