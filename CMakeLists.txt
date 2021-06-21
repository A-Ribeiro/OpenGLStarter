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

# make all libs that are included,
# to use the framework comands
set( OpenGLStarter_Integration ON )

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake-modules)

include(cmake/tools.cmake)
include(cmake/DetectOSAndArchitecture.cmake)
include(cmake/add_definitions_global.cmake)
include(cmake/include_directories_global.cmake)
include(cmake/add_linker_flag_global.cmake)
include(cmake/dll_management.cmake)
include(cmake/copy_headers_to_include_directory.cmake)
include(cmake/ide_setup.cmake)
include(cmake/detect_rpi.cmake)
include(cmake/detect_sse2.cmake)
include(cmake/detect_neon.cmake)
include(cmake/detect_opengl.cmake)
include(cmake/detect_openmp.cmake)

#
# 3rdParty libraries
#
add_subdirectory( lib/assimp )
add_subdirectory( lib/convertutf )
add_subdirectory( lib/freetype )
add_subdirectory( lib/glew )
add_subdirectory( lib/libjpeg )
add_subdirectory( lib/libpng )
#add_subdirectory( lib/libxml )
#add_subdirectory( lib/ogg )
add_subdirectory( lib/sfml )
add_subdirectory( lib/zlib )
add_subdirectory( lib/rtaudio )
add_subdirectory( lib/randomlib )
add_subdirectory( lib/md5 )

#
# MainLibrary
#
add_subdirectory( lib/aribeiro )

#
# Project Libs
#
add_subdirectory( projects-lib/rtaudio-wrapper )
add_subdirectory( projects-lib/md5-wrapper )
add_subdirectory( projects-lib/opengl-wrapper )
add_subdirectory( projects-lib/zlib-wrapper )
add_subdirectory( projects-lib/data-model )
add_subdirectory( projects-lib/ft2-wrapper )
add_subdirectory( projects-lib/mini-gl-engine )
add_subdirectory( projects-lib/rtvideo )

#
# Projects
#
add_subdirectory( projects/template )
add_subdirectory( projects/aribeiro-test )
add_subdirectory( projects/aribeiro-test-easing-eq )
add_subdirectory( projects/game-invader )
add_subdirectory( projects/game-tetris )
add_subdirectory( projects/shader-examples )
add_subdirectory( projects/post-processing )
add_subdirectory( projects/post-processing-power-of-two )
add_subdirectory( projects/collision )
add_subdirectory( projects/puc2019 )
add_subdirectory( projects/raytracer )
add_subdirectory( projects/boid )

if (ARIBEIRO_RPI)
    add_subdirectory( projects/rpi-video )
endif()

add_subdirectory( projects/test-glfont2 )

add_subdirectory( projects/tech-demo-pbr )
add_subdirectory( projects/tech-demo-particle )
add_subdirectory( projects/tech-demo-animation )


#
# Tools
#
add_subdirectory( tools/font2bitmap )
add_subdirectory( tools/conestep-generator )
add_subdirectory( tools/assimp2bams )

#add_subdirectory( projects/crazytwirl )
#add_subdirectory( src/steamworks )

