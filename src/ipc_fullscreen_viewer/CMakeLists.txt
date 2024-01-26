project (ipc-fullscreen-viewer)

#check if it is linux
if (UNIX)
    set(ARIBEIRO_LINUX_LOADER_USE_USR_LOCAL_PATH OFF CACHE BOOL "Make the loaders to use the system path: /usr/local/etc/OpenMultimedia/.")
endif()

FILE( GLOB_RECURSE PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

set(ADDITIONAL_PARAMETERS)

if (MSVC)
    list(APPEND ADDITIONAL_PARAMETERS appicon.rc)
endif()

add_executable(
    ${PROJECT_NAME} 
    ${PUBLIC_HEADERS} ${SRC}
    ${ADDITIONAL_PARAMETERS}
)

tool_define_source_group(${PUBLIC_HEADERS} ${SRC} ${ADDITIONAL_PARAMETERS})

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "OpenMultimedia")

target_link_libraries(${PROJECT_NAME} 
    appkit-gl-engine
)

if (ARIBEIRO_LINUX_LOADER_USE_USR_LOCAL_PATH)
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DARIBEIRO_LINUX_LOADER_USE_USR_LOCAL_PATH)
endif()

tool_copy_directory_after_build( ${PROJECT_NAME} resources )
