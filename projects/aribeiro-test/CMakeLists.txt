project (aribeiro-test)

set( SRC
    main.cpp
)

add_executable( ${PROJECT_NAME} ${SRC} )

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "TESTS")

target_link_libraries( ${PROJECT_NAME} ${ARIBEIRO_LIBRARIES} )

copy_3rdparty_dll(${PROJECT_NAME})
