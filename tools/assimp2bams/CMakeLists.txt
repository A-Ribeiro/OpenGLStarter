PROJECT( assimp2bams )

# Getting the source files
FILE( GLOB sources *.cpp)
FILE( GLOB headers *.h )

add_executable(${PROJECT_NAME} ${sources} ${headers})

source_group("" FILES ${sources} ${headers})

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "TOOLS")

# Linking libraries
target_link_libraries(${PROJECT_NAME}
    assimp # assimp needs zlib to be included after this lib
           # This is why this configuration needs ARIBEIRO_LIBRARIES to be included below
    ${ARIBEIRO_LIBRARIES}
)

copy_3rdparty_dll(${PROJECT_NAME})
copy_file_after_build(${PROJECT_NAME} 
                        assimp2bams-no-animations.bat
                        assimp2bams-only-animations.bat )
