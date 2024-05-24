project(appkit-window-imgui)

if (NOT TARGET appkit-window OR NOT TARGET appkit-gl-base)
    message(ERROR "WindowImGUI needs: appkit-window and appkit-gl-base to be included before using.")
endif()

tool_download_git_package_branch("https://github.com/ocornut/imgui.git" "docking" imgui)

tool_get_dirs(imgui_DOWNLOADED_PATH imgui_BINARY_PATH imgui)

# FIX mouse off center
#
set(StringReplaceSource "const ImVec2 pos = base_pos - offset;")
set(StringReplaceTarget "const ImVec2 pos = base_pos - offset * base_scale;")

file(READ "${imgui_DOWNLOADED_PATH}/imgui.cpp" AUX)
string(FIND "${AUX}" "${StringReplaceTarget}" matchres)
if(${matchres} EQUAL -1)
    string(REPLACE "${StringReplaceSource}" "${StringReplaceTarget}" output "${AUX}")
    file(WRITE "${imgui_DOWNLOADED_PATH}/imgui.cpp" "// Updated By CMake Script:\n// ${CMAKE_CURRENT_LIST_FILE}\n\n${output}")
endif ()


set(IMGUI_SRC
    ${imgui_DOWNLOADED_PATH}/imconfig.h
    ${imgui_DOWNLOADED_PATH}/imgui.h
    ${imgui_DOWNLOADED_PATH}/imgui.cpp

    ${imgui_DOWNLOADED_PATH}/imgui_demo.cpp
    ${imgui_DOWNLOADED_PATH}/imgui_draw.cpp
    ${imgui_DOWNLOADED_PATH}/imgui_internal.h

    ${imgui_DOWNLOADED_PATH}/imgui_tables.cpp
    ${imgui_DOWNLOADED_PATH}/imgui_widgets.cpp

    ${imgui_DOWNLOADED_PATH}/imstb_rectpack.h
    ${imgui_DOWNLOADED_PATH}/imstb_textedit.h
    ${imgui_DOWNLOADED_PATH}/imstb_truetype.h
)

tool_define_source_group_base_path( "${imgui_DOWNLOADED_PATH}" ${IMGUI_SRC} )

FILE( GLOB_RECURSE LOCAL_PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE LOCAL_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

tool_define_source_group(
    ${LOCAL_PUBLIC_HEADERS}
    ${LOCAL_SRC}
)

# copy_headers_to_include_directory(${PROJECT_NAME} ${LOCAL_PUBLIC_HEADERS})

add_library( ${PROJECT_NAME} OBJECT ${IMGUI_SRC} ${IMGUI_IMPL_SRC} ${LOCAL_PUBLIC_HEADERS} ${LOCAL_SRC})

target_compile_definitions( ${PROJECT_NAME} PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_CUSTOM )

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "LIBS")

target_link_libraries(${PROJECT_NAME} PUBLIC 
    appkit-window
    appkit-gl-base
)

target_include_directories(${PROJECT_NAME} 
PUBLIC 
${imgui_DOWNLOADED_PATH} 
"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
