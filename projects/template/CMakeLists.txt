project (template)

#file(GLOB_RECURSE  gamelogic_src GameLogic/*.cpp GameLogic/*.h )

set( SRC
    main.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
#set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "EXE")

target_link_libraries( ${PROJECT_NAME}
    #sfml-network sfml-graphics sfml-window sfml-system sfml-audio
    sfml-window sfml-graphics
    opengl-wrapper
    ${ARIBEIRO_LIBRARIES} )


copy_3rdparty_dll(${PROJECT_NAME})
copy_directory_after_build( ${PROJECT_NAME} resources )
