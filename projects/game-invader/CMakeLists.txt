project (game-invader)

file(GLOB_RECURSE all_src *.cpp *.h )

file(GLOB root_src *.cpp *.h )
file(GLOB gamestate_src gamestate/*.cpp gamestate/*.h )
file(GLOB gamestate_gameplay_src gamestate/gameplay/*.cpp gamestate/gameplay/*.h )
file(GLOB gamestate_gameplay_objects_src gamestate/gameplay/objects/*.cpp gamestate/gameplay/objects/*.h )


set( SRC
    ${root_src}
    ${gamestate_src}
    ${gamestate_gameplay_src}
    ${gamestate_gameplay_objects_src}
)

source_group("" FILES ${root_src})
source_group("gamestate" FILES ${gamestate_src})
source_group("gamestate\\gameplay" FILES ${gamestate_gameplay_src})
source_group("gamestate\\gameplay\\objects" FILES ${gamestate_gameplay_objects_src})

add_executable(${PROJECT_NAME} ${all_src})

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "GAMES")

target_link_libraries( ${PROJECT_NAME}
    #sfml-network sfml-graphics sfml-window sfml-system sfml-audio
    sfml-window sfml-graphics sfml-audio
    opengl-wrapper
    ${ARIBEIRO_LIBRARIES} )

copy_3rdparty_dll(${PROJECT_NAME})
copy_directory_after_build( ${PROJECT_NAME} resources )
