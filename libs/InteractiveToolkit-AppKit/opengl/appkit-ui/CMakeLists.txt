project (appkit-ui)

if (NOT TARGET InteractiveToolkit-Extension)
    add_subdirectory(../../../InteractiveToolkit-Extension "${CMAKE_BINARY_DIR}/lib/InteractiveToolkit-Extension")
endif()

if (NOT TARGET appkit-gl-engine)
    add_subdirectory(../appkit-gl-engine "${CMAKE_BINARY_DIR}/lib/appkit-gl-engine")
endif()

if (NOT TARGET InteractiveToolkit-DPI)
    add_subdirectory(../../../InteractiveToolkit-DPI "${CMAKE_BINARY_DIR}/lib/InteractiveToolkit-DPI")
endif()

FILE( GLOB_RECURSE PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

tool_define_source_group(${PUBLIC_HEADERS} ${SRC})

add_library( ${PROJECT_NAME} STATIC ${PUBLIC_HEADERS} ${SRC} )

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "LIBS")

#link libraries
target_link_libraries(${PROJECT_NAME}
PUBLIC 
    InteractiveToolkit-Extension
    appkit-gl-engine
    InteractiveToolkit-DPI
)

target_include_directories(
    ${PROJECT_NAME}
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# copy_headers_to_include_directory(${PROJECT_NAME} ${PUBLIC_HEADERS})
# configure_include_file( ${PROJECT_NAME} "${PROJECT_NAME}.h.in" "${PROJECT_NAME}.h" ${PUBLIC_HEADERS} )

# tool_show_current_directories()
