project (glew)

if (OpenGLStarter_Integration)
    #####################################################
    # ARibeiro Lib Setup
    #####################################################
    # global definitions
    #add_definitions_global(-DGLEW_STATIC)
    #####################################################
endif()


set( PUBLIC_HEADERS
        src/eglew.h
        src/glew.h
        src/glxew.h
        src/wglew.h
        )

set( PRIVATE_HEADERS )

set( SRC src/glew.c )

add_library( ${PROJECT_NAME} STATIC ${PUBLIC_HEADERS} ${PRIVATE_HEADERS} ${SRC} )

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "LIBS")

target_compile_definitions(${PROJECT_NAME} PUBLIC -DGLEW_STATIC)

#if(OS_TARGET STREQUAL win)

    if (ARIBEIRO_RPI)
        set(LIB_TO_LINK_WITH ${GLES_LIBRARY})
    else()
        set(LIB_TO_LINK_WITH ${OPENGL_LIBRARIES})
    endif()

    target_link_libraries(${PROJECT_NAME} PUBLIC ${LIB_TO_LINK_WITH} )

#endif()

if (OpenGLStarter_Integration)
    copy_headers_to_include_directory(${PROJECT_NAME} ${PUBLIC_HEADERS})
endif()
