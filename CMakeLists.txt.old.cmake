cmake_minimum_required (VERSION 3.5.1)

project(OpenGLStarter)

# "For this is how God loved the world:
# he gave his only Son, so that everyone
# who believes in him may not perish
# but may have eternal life."
#
# John 3:16

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

ENABLE_LANGUAGE(CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# avoid base libs overwrite
option(ARIBEIRO_SKIP_INSTALL_CORE ON)
option(ARIBEIRO_SKIP_INSTALL_PLATFORM ON)
option(ARIBEIRO_SKIP_INSTALL_DATA ON)

if(MSVC)
    #disable size_t warning conversion... on 64build
    add_compile_options( /wd4267 )
endif()

include(cmake/include.cmake)
include(cmake/libs/libaRibeiroCore.cmake)
include(cmake/libs/libaRibeiroPlatform.cmake)
include(cmake/libs/libaRibeiroData.cmake)
include(cmake/libs/libLinkOpenGL.cmake)

#include(cmake/libs/aRibeiroWrappers.cmake)
#tool_include_lib(aRibeiroWrappers ft2-wrapper)

add_subdirectory(src/glad)
add_subdirectory(src/opengl-wrapper)
add_subdirectory(src/window-gl-wrapper)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src/mini-gl-engine)

# add_subdirectory(src/tech-demo-pbr)
# add_subdirectory(src/tech-demo-particle)
# add_subdirectory(src/tech-demo-animation)
# add_subdirectory(src/tech-demo-shadow)

# add_subdirectory(src/ipc_fullscreen_viewer)

option(ARIBEIRO_BUILD_IMGUI_PROJECTS OFF)

if (ARIBEIRO_BUILD_IMGUI_PROJECTS)
    add_subdirectory(src/imgui)
    add_subdirectory(src/imgui_example)
    add_subdirectory(src/imgui_service_ui)
    add_subdirectory(src/imgui_editor)
endif()


add_subdirectory(src/test-pbr-coefs)
