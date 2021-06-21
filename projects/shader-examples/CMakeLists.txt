project (shader-examples)

file(GLOB_RECURSE all_src *.cpp *.h )

file(GLOB root_src *.cpp *.h )
file(GLOB shaders_src shaders/*.cpp shaders/*.h )
file(GLOB common_src common/*.cpp common/*.h )
file(GLOB common_gui_src common/GUI/*.cpp common/GUI/*.h )
file(GLOB syshelper_src common/SysHelper/*.cpp common/SysHelper/*.h )
file(GLOB syshelper_tinyxml_src common/SysHelper/TinyXML/*.cpp common/SysHelper/TinyXML/*.h )

source_group("" FILES ${root_src})
source_group("shaders" FILES ${shaders_src})
source_group("common" FILES ${common_src})
source_group("common\\GUI" FILES ${common_gui_src})
source_group("common\\SysHelper" FILES ${syshelper_src})
source_group("common\\SysHelper\\TinyXML" FILES ${syshelper_tinyxml_src})

add_executable(${PROJECT_NAME} ${all_src})

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "TESTS")

target_link_libraries( ${PROJECT_NAME}
    #sfml-network sfml-graphics sfml-window sfml-system sfml-audio
    sfml-window sfml-graphics
    opengl-wrapper
    ${ARIBEIRO_LIBRARIES} )

copy_3rdparty_dll(${PROJECT_NAME})
copy_directory_after_build( ${PROJECT_NAME} resources )
