project(appkit-window-imguizmo)

if (NOT TARGET appkit-window-imgui)
    message(ERROR "appkit-window-imguizmo needs: appkit-window-imgui.")
endif()

tool_download_git_package("https://github.com/CedricGuillemet/ImGuizmo.git" imguizmo)
tool_get_dirs(imguizmo_DOWNLOADED_PATH imguizmo_BINARY_PATH imguizmo)

tool_replace_in_file(
    ${imguizmo_DOWNLOADED_PATH}/GraphEditor.h
    "bool mSelected{ false };" # Search
    "bool mSelected;" # Replace
)

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

# # compile examples
# project(appkit-window-imguizmo-example)

# message(STATUS "IMGUIZMO examples at: ${imguizmo_BINARY_PATH}/example")
# if(NOT EXISTS "${imguizmo_BINARY_PATH}/example/")
#     # file(MAKE_DIRECTORY "${imguizmo_BINARY_PATH}/example")
#     # message(STATUS "[${imguizmo_BINARY_PATH}/example] created")
#     tool_copy_to_dir(
#         ${imguizmo_BINARY_PATH}/example

#         ${imguizmo_DOWNLOADED_PATH}/example/main.cpp
#         ${imguizmo_DOWNLOADED_PATH}/example/stb_image.h
#         ${imguizmo_DOWNLOADED_PATH}/example/ImApp.h
#     )
# endif()

# set(SRC
#     ${imguizmo_BINARY_PATH}/example/main.cpp
#     ${imguizmo_BINARY_PATH}/example/stb_image.h
#     ${imguizmo_BINARY_PATH}/example/ImApp.h
# )

# tool_define_source_group_base_path( "${imguizmo_DOWNLOADED_PATH}/example" ${SRC} )

# add_executable( ${PROJECT_NAME} ${SRC})

# set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "EXAMPLE")

# target_link_libraries(${PROJECT_NAME} PUBLIC 
#     appkit-window-imguizmo
# )




