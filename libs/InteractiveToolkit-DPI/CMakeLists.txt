project (InteractiveToolkit-DPI)

if (NOT TARGET InteractiveToolkit-Extension)
    add_subdirectory(../InteractiveToolkit-Extension "${CMAKE_BINARY_DIR}/lib/InteractiveToolkit-Extension")
endif()



set(ADD_LIBS)
set(ADD_DIRS)

if(WIN32)

    

elseif(APPLE)
elseif(UNIX)

    find_package(X11 REQUIRED QUIET)

    list(APPEND ADD_LIBS ${X11_LIBRARIES} )
    list(APPEND ADD_DIRS ${X11_INCLUDE_DIR} )

endif()

FILE( GLOB_RECURSE PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

tool_define_source_group(${PUBLIC_HEADERS} ${SRC})

add_library( ${PROJECT_NAME} STATIC ${PUBLIC_HEADERS} ${SRC} )

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "LIBS")

#link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC 
    InteractiveToolkit-Extension
    ${ADD_LIBS}
)

target_include_directories(
    ${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ADD_DIRS}
)
