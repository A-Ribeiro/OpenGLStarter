project (md5)

FILE( GLOB PUBLIC_HEADERS *.h)
FILE( GLOB SRC *.cpp)

add_library( ${PROJECT_NAME} STATIC ${PUBLIC_HEADERS} ${SRC} )

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "LIBS")

if (OpenGLStarter_Integration)
    copy_headers_to_include_directory(${PROJECT_NAME} ${PUBLIC_HEADERS})
endif()
