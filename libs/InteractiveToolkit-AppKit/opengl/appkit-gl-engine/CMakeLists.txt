project (appkit-gl-engine)

if (NOT TARGET InteractiveToolkit-Extension)
    add_subdirectory(../../../InteractiveToolkit-Extension "${CMAKE_BINARY_DIR}/lib/InteractiveToolkit-Extension")
endif()

if (NOT TARGET appkit-window)
    add_subdirectory(../../common/appkit-window "${CMAKE_BINARY_DIR}/lib/appkit-window")
endif()

if (NOT APPKIT_WINDOW_GL)
    message(FATAL_ERROR "You need to set APPKIT_WINDOW_GL to ON in order to compile the appkit-gl-engine lib.")
endif()

if (NOT TARGET appkit-gl-base)
    add_subdirectory(../appkit-gl-base "${CMAKE_BINARY_DIR}/lib/appkit-gl-base")
endif()

# set( MINI_GL_ENGINE_PBR_MODE Optimized CACHE STRING "Choose the PBR mode." )
# set_property(CACHE MINI_GL_ENGINE_PBR_MODE PROPERTY STRINGS LearnOpengl_Com Optimized)

FILE( GLOB_RECURSE PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

tool_define_source_group(${PUBLIC_HEADERS} ${SRC})

add_library( ${PROJECT_NAME} STATIC ${PUBLIC_HEADERS} ${SRC} )

# if(MINI_GL_ENGINE_PBR_MODE STREQUAL LearnOpengl_Com)
#     target_compile_options(${PROJECT_NAME} PUBLIC -DPBR_MODE_LEARN_OPENGL)
# elseif(MINI_GL_ENGINE_PBR_MODE STREQUAL Optimized)
#     target_compile_options(${PROJECT_NAME} PUBLIC -DPBR_MODE_OPTIMIZED)
# endif()

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "LIBS")

#link libraries
target_link_libraries(${PROJECT_NAME}
PUBLIC 
    InteractiveToolkit-Extension
    appkit-gl-base
    appkit-window
)

target_include_directories(
    ${PROJECT_NAME}
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# copy_headers_to_include_directory(${PROJECT_NAME} ${PUBLIC_HEADERS})
# configure_include_file( ${PROJECT_NAME} "${PROJECT_NAME}.h.in" "${PROJECT_NAME}.h" ${PUBLIC_HEADERS} )

# tool_show_current_directories()
