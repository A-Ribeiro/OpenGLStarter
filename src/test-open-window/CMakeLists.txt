project (test-open-window)

find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND REQUIRED wayland-client)

# include(${CMAKE_SOURCE_DIR}/libs/InteractiveToolkit-Extension/cmake/libLinkOpenGL.cmake)

FILE( GLOB_RECURSE PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

define_source_group(${PUBLIC_HEADERS} ${SRC})

add_executable( ${PROJECT_NAME} ${PUBLIC_HEADERS} ${SRC} )

target_link_libraries(${PROJECT_NAME}
PUBLIC 
    appkit-window
    InteractiveToolkit-DPI
)


target_include_directories(${PROJECT_NAME} PRIVATE ${WAYLAND_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${WAYLAND_LIBRARIES})
target_compile_options(${PROJECT_NAME} PRIVATE ${WAYLAND_CFLAGS_OTHER})

