project(appkit-window-imguizmo)

if (NOT TARGET appkit-window-imgui)
    message(ERROR "appkit-window-imguizmo needs: appkit-window-imgui.")
endif()

tool_download_git_package("https://github.com/CedricGuillemet/ImGuizmo.git" imguizmo)
tool_get_dirs(imguizmo_DOWNLOADED_PATH imguizmo_BINARY_PATH imguizmo)

set(IMGUIZMO_SRC

    ${imguizmo_DOWNLOADED_PATH}/GraphEditor.cpp
    ${imguizmo_DOWNLOADED_PATH}/GraphEditor.h

    ${imguizmo_DOWNLOADED_PATH}/ImCurveEdit.cpp
    ${imguizmo_DOWNLOADED_PATH}/ImCurveEdit.h

    ${imguizmo_DOWNLOADED_PATH}/ImGradient.cpp
    ${imguizmo_DOWNLOADED_PATH}/ImGradient.h

    ${imguizmo_DOWNLOADED_PATH}/ImGuizmo.cpp
    ${imguizmo_DOWNLOADED_PATH}/ImGuizmo.h

    ${imguizmo_DOWNLOADED_PATH}/ImSequencer.cpp
    ${imguizmo_DOWNLOADED_PATH}/ImSequencer.h

    ${imguizmo_DOWNLOADED_PATH}/ImZoomSlider.h
)

tool_define_source_group_base_path( "${imguizmo_DOWNLOADED_PATH}" ${IMGUIZMO_SRC} )

FILE( GLOB_RECURSE LOCAL_PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE LOCAL_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

tool_define_source_group(
    ${LOCAL_PUBLIC_HEADERS}
    ${LOCAL_SRC}
)

# copy_headers_to_include_directory(${PROJECT_NAME} ${LOCAL_PUBLIC_HEADERS})

add_library( ${PROJECT_NAME} STATIC ${IMGUIZMO_SRC} ${IMGUIZMO_IMPL_SRC} ${LOCAL_PUBLIC_HEADERS} ${LOCAL_SRC})

# target_compile_definitions( ${PROJECT_NAME} PUBLIC -DIMGUIZMO_IMPL_OPENGL_LOADER_CUSTOM )

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "LIBS")

target_link_libraries(${PROJECT_NAME} PUBLIC 
    appkit-window-imgui
)

target_include_directories(${PROJECT_NAME} 
PUBLIC 
${imguizmo_DOWNLOADED_PATH} 
"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# compile examples
