project (appkit-window)

find_package(InteractiveToolkit REQUIRED QUIET)

set( APPKIT_WINDOW_GL ON CACHE BOOL "Make Available GLWindow" )
set( APPKIT_WINDOW_VULKAN OFF CACHE BOOL "Make Available VulkanWindow" )

set( APPKIT_WINDOW AutoDetect CACHE STRING "Choose the base window library." )
set_property(CACHE APPKIT_WINDOW PROPERTY STRINGS None AutoDetect SFML GLFW Raspberry_Pi)

if (ITK_RPI)
    message(STATUS "[appkit-window] Forcing using the Raspberry Pi wrapper")
    set( APPKIT_WINDOW Raspberry_Pi CACHE STRING "Choose the base window library." FORCE )
elseif (APPKIT_WINDOW STREQUAL AutoDetect)

    message(FATAL_ERROR "You need to set APPKIT_WINDOW to SFML, GLFW or Raspberry_Pi")

    # message(STATUS "[appkit-window] detecting window system")
    # find_package(SFML 2 COMPONENTS system window graphics audio network QUIET)
    # if (SFML_FOUND)
    #     message(STATUS "[appkit-window] SFML DETECTED")
    # endif()
    # find_package(glfw3 QUIET)
    # if (TARGET glfw)
    #     message(STATUS "[appkit-window] GLFW DETECTED")
    # endif()
    # if (SFML_FOUND AND TARGET glfw)
    #     message(FATAL_ERROR "You need to set APPKIT_WINDOW to SFML, GLFW or Raspberry_Pi")
    # endif()

    # if (SFML_FOUND)
    #     set( APPKIT_WINDOW SFML CACHE STRING "Choose the base window library." FORCE )
    # elseif (GLFW3_FOUND)
    #     set( APPKIT_WINDOW GLFW CACHE STRING "Choose the base window library." FORCE )
    # else()
    #     set( APPKIT_WINDOW SFML CACHE STRING "Choose the base window library." FORCE )
    #     message(STATUS "[appkit-window] no system libs found... forcing use of SFML")
    # endif()
endif()

set(WINDOW_COMPILE_DEFS)
set(WINDOW_LINK_LIBS)

if (APPKIT_WINDOW_GL)
    list(APPEND WINDOW_COMPILE_DEFS APPKIT_WINDOW_GL)
endif()

if (APPKIT_WINDOW_VULKAN)
    list(APPEND WINDOW_COMPILE_DEFS APPKIT_WINDOW_VULKAN)
endif()

if (APPKIT_WINDOW STREQUAL SFML)

    include(../../../InteractiveToolkit-Extension/cmake/libsfmlFromSource.cmake)

    message(STATUS "[appkit-window] USING SFML")

    list(APPEND WINDOW_COMPILE_DEFS APPKIT_WINDOW_SFML)

    if (APPKIT_WINDOW_GL)
        include(../../../InteractiveToolkit-Extension/cmake/libLinkOpenGL.cmake)
        list(APPEND WINDOW_LINK_LIBS LinkOpenGL sfml-graphics)
    endif()
    
    if (APPKIT_WINDOW_VULKAN)
        #list(APPEND WINDOW_LINK_LIBS)
    endif()

    list(APPEND WINDOW_LINK_LIBS sfml-window sfml-system)
    

elseif (APPKIT_WINDOW STREQUAL GLFW)
    message(STATUS "[appkit-window] USING GLFW")
    list(APPEND WINDOW_COMPILE_DEFS APPKIT_WINDOW_GLFW)
    list(APPEND WINDOW_LINK_LIBS glfw)
elseif (APPKIT_WINDOW STREQUAL Raspberry_Pi)
    message(STATUS "[appkit-window] USING CUSTOM RPI")
    list(APPEND WINDOW_COMPILE_DEFS APPKIT_WINDOW_RPI)
    list(APPEND WINDOW_LINK_LIBS ${EGL_LIBRARY} ${GLES_LIBRARY} ${RPI_LIBRARIES})
    #message(FATAL_ERROR "APPKIT_WINDOW=Raspberry_Pi not implemented yet")
endif()

FILE( GLOB_RECURSE PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE PUBLIC_INL RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.inl)
FILE( GLOB_RECURSE SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

add_library( ${PROJECT_NAME} STATIC ${PUBLIC_HEADERS} ${PUBLIC_INL} ${SRC} )

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "LIBS")

target_compile_definitions(${PROJECT_NAME} 
PUBLIC
    ${WINDOW_COMPILE_DEFS}
)

target_compile_options(${PROJECT_NAME} 
PUBLIC
    ${INTERACTIVETOOLKIT_COMPILE_OPTIONS}
)

target_link_options(${PROJECT_NAME} 
PUBLIC
    ${INTERACTIVETOOLKIT_LINK_OPTIONS}
)

target_include_directories(${PROJECT_NAME} 
PUBLIC
    ${INTERACTIVETOOLKIT_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${PROJECT_NAME} 
PUBLIC 
    ${INTERACTIVETOOLKIT_LIBRARIES}
    ${WINDOW_LINK_LIBS}
)

configure_build_flags( ${PROJECT_NAME} "include/AppKit/window/buildFlags.h.in" "include/AppKit/window/buildFlags.h")
