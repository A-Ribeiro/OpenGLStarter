# The name of this library uses undescore '_' 
# to appear in the top of the filesystem listing.
project (appkit-gl_glad)

include(../../../InteractiveToolkit-Extension/cmake/libLinkOpenGL.cmake)

FILE( GLOB_RECURSE HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
#FILE( GLOB_RECURSE SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.c)

SET( SRC src/gl.c)

#if(ITK_RPI)
#    list(APPEND SRC src/egl.c)
#endif()

add_library( ${PROJECT_NAME} STATIC ${SRC} ${HEADERS} )
# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "LIBS")
#target_compile_definitions(${PROJECT_NAME} PUBLIC -DGLAD_GL_STATIC -DGLAD_GL_NO_GLU)

set(ADD_LIBS)
if(ITK_RPI)
    list(APPEND ADD_LIBS ${EGL_LIBRARY} ${GLES_LIBRARY} ${RPI_LIBRARIES})
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DGLAD_GLES2)
endif()

#check if it is linux
if (UNIX)
    list(APPEND ADD_LIBS dl)
endif()

target_link_libraries(${PROJECT_NAME} 
PUBLIC
    LinkOpenGL
PRIVATE
    ${ADD_LIBS}
)



target_include_directories(${PROJECT_NAME} 
PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

# copy_headers_to_include_directory(glad ${CMAKE_CURRENT_SOURCE_DIR}/include/)
# copy_headers_to_include_directory(KHR ${CMAKE_CURRENT_SOURCE_DIR}/include/KHR/)

