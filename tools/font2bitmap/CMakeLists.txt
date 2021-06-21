PROJECT( font2bitmap )

# Getting the source files
FILE( GLOB HEADERS *.h )
FILE( GLOB SRC *.cpp)


if(WIN32)
    set( ADITIONAL_SRC win32/getopt.c win32/getopt.h )
    set( ADITIONAL_DIR win32 )
    
    SOURCE_GROUP(win32 FILES win32/getopt.c win32/getopt.h)
ENDIF()

include_directories(${ADITIONAL_DIR})

add_executable(${PROJECT_NAME}
        ${HEADERS}
        ${SRC}
        ${ADITIONAL_SRC}
)

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "TOOLS")

# Linking libraries
target_link_libraries( ${PROJECT_NAME}
    convertutf
    data-model
    ft2-wrapper
    ${ARIBEIRO_LIBRARIES} )

#copy_3rdparty_dll(${PROJECT_NAME})
copy_file_after_build(${PROJECT_NAME} 
                        charset.utf8 
                        mailrays.ttf 
                        CreateFontWin32.bat 
                        goudy_bookletter_1911.otf
                        Roboto-Light.ttf
                        Roboto-Regular.ttf )
