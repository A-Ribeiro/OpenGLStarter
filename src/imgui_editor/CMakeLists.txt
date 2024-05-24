project (imgui_editor)

#check if it is linux
if (UNIX)
    set(ARIBEIRO_LINUX_LOADER_USE_USR_LOCAL_PATH OFF CACHE BOOL "Make the loaders to use the system path: /usr/local/etc/OpenMultimedia/.")
endif()

FILE( GLOB_RECURSE PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

tool_define_source_group(${PUBLIC_HEADERS} ${SRC})

add_executable( ${PROJECT_NAME} ${PUBLIC_HEADERS} ${SRC} )

tool_download_git_package("https://github.com/Tencent/rapidjson" rapidjson)
tool_get_dirs(rapidjson_DOWNLOADED_PATH rapidjson_BINARY_PATH rapidjson)

target_include_directories( ${PROJECT_NAME} PUBLIC "${rapidjson_DOWNLOADED_PATH}/include" )

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "TOOL")

#link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC 
    # appkit-gl-base
    appkit-gl-engine
    appkit-window-imgui
    InteractiveToolkit-DPI
)

if (ARIBEIRO_LINUX_LOADER_USE_USR_LOCAL_PATH)
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DARIBEIRO_LINUX_LOADER_USE_USR_LOCAL_PATH)
endif()

#copy_file_after_build( ${PROJECT_NAME} service_list.json )
tool_copy_directory_after_build( ${PROJECT_NAME} resources )