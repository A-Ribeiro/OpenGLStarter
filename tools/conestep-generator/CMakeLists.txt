PROJECT( conestep-generator )

# Getting the source files
FILE( GLOB HEADERS *.h )
FILE( GLOB SRC *.cpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SRC})

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "TOOLS")

# Linking libraries
target_link_libraries( ${PROJECT_NAME} ${ARIBEIRO_LIBRARIES} )

copy_3rdparty_dll(${PROJECT_NAME})
copy_directory_after_build( ${PROJECT_NAME} resources )
