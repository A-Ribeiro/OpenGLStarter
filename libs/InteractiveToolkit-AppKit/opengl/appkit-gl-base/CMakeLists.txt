project (appkit-gl-base)

# already included in appkit-gl_glad
# include(../../../InteractiveToolkit-Extension/cmake/libLinkOpenGL.cmake)

if (NOT TARGET InteractiveToolkit-Extension)
    add_subdirectory(../../../InteractiveToolkit-Extension "${CMAKE_BINARY_DIR}/lib/InteractiveToolkit-Extension")
endif()

if (NOT TARGET appkit-gl_glad)
    add_subdirectory(../appkit-gl_glad "${CMAKE_BINARY_DIR}/lib/appkit-gl_glad")
endif()

FILE( GLOB_RECURSE PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE PUBLIC_INL RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.inl)
FILE( GLOB_RECURSE SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

tool_define_source_group(${PUBLIC_HEADERS} ${SRC})

add_library( ${PROJECT_NAME} STATIC ${PUBLIC_HEADERS} ${SRC} ${PUBLIC_INL})

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "LIBS")

target_link_libraries(
    ${PROJECT_NAME}
PUBLIC
    appkit-gl_glad
    InteractiveToolkit-Extension
)

target_include_directories(
    ${PROJECT_NAME}
PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# copy_headers_to_include_directory(${PROJECT_NAME} ${PUBLIC_HEADERS})
# configure_include_file( ${PROJECT_NAME} "${PROJECT_NAME}.h.in" "${PROJECT_NAME}.h" ${PUBLIC_HEADERS} )
