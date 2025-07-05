cmake_minimum_required (VERSION 3.10)

project(OpenGLStarter)

# "For this is how God loved the world:
# he gave his only Son, so that everyone
# who believes in him may not perish
# but may have eternal life."
#
# John 3:16

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

ENABLE_LANGUAGE(CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# reset global variables
unset (CMAKE_MODULE_PATH CACHE)
unset (CMAKE_PREFIX_PATH CACHE)

# create CMAKE_BUILD_TYPE for non-windows platform
if( NOT CMAKE_BUILD_TYPE AND (NOT WIN32 OR CMAKE_COMPILER_IS_GNUCXX) )
    set( CMAKE_BUILD_TYPE Release CACHE STRING
            "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
            FORCE )
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS None Debug Release RelWithDebInfo MinSizeRel)
endif()

# configure compile vars
if(WIN32)
    #
    # Windows
    #
    if(CMAKE_COMPILER_IS_GNUCXX)

        if (CMAKE_BUILD_TYPE STREQUAL Release)
            add_definitions(-DNDEBUG)
            add_compile_options(-O3 -fno-strict-aliasing)
        endif()

    elseif(MSVC)
        # change the multithread debug DLL to multithread debug static compilation
        # avoid link with another VS DLL...
        foreach(flag
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO

                CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
                CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO )
            if(${flag} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
                SET ( ${flag} "${${flag}}" CACHE STRING "" FORCE)
            endif()
            set("${flag}" "${${flag}} /MP")
        endforeach()

        #SET(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} /O2")
        #SET(CMAKE_C_FLAGS_RELEASE  "${CMAKE_C_FLAGS_RELEASE} /O2")
    endif()
elseif(APPLE)
    #
    # MacOS X, iOS, watchOS, tvOS (since 3.10.3)
    #
    if (CMAKE_BUILD_TYPE STREQUAL Release)
        add_definitions(-DNDEBUG)
        add_compile_options(-O3 -fno-strict-aliasing)
    endif()
    #SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -all_load")
    #SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -v")
    #SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -ObjC")
elseif(UNIX)
    #
    # Linux, BSD, Solaris, Minix
    #
    if (CMAKE_BUILD_TYPE STREQUAL Release)
        add_definitions(-DNDEBUG)
        add_compile_options(-O3 -fno-strict-aliasing)
    elseif( CMAKE_BUILD_TYPE STREQUAL Debug)
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address") # -static-libasan
    endif()
else()
    message(FATAL_ERROR "Cmake not configured for this OS")
endif()

add_subdirectory(libs/InteractiveToolkit "${CMAKE_BINARY_DIR}/lib/InteractiveToolkit")

find_package(InteractiveToolkit REQUIRED)

if (NOT TARGET InteractiveToolkit-Extension)
    add_subdirectory(libs/InteractiveToolkit-Extension "${CMAKE_BINARY_DIR}/lib/InteractiveToolkit-Extension")
endif()

if (NOT TARGET InteractiveToolkit-DPI)
    add_subdirectory(libs/InteractiveToolkit-DPI "${CMAKE_BINARY_DIR}/lib/InteractiveToolkit-DPI")
endif()

# include window source
if (NOT TARGET appkit-window)
    add_subdirectory(libs/InteractiveToolkit-AppKit/common/appkit-window "${CMAKE_BINARY_DIR}/lib/appkit-window")
endif()

if (NOT TARGET appkit-gl_glad)
    add_subdirectory(libs/InteractiveToolkit-AppKit/opengl/appkit-gl_glad "${CMAKE_BINARY_DIR}/lib/appkit-gl_glad")
endif()

if (NOT TARGET appkit-gl-base)
    add_subdirectory(libs/InteractiveToolkit-AppKit/opengl/appkit-gl-base "${CMAKE_BINARY_DIR}/lib/appkit-gl-base")
endif()

if (NOT TARGET appkit-gl-engine)
    add_subdirectory(libs/InteractiveToolkit-AppKit/opengl/appkit-gl-engine "${CMAKE_BINARY_DIR}/lib/appkit-gl-engine")
endif()

if (NOT TARGET appkit-window-imgui)
    add_subdirectory(libs/InteractiveToolkit-AppKit/common/appkit-window-imgui "${CMAKE_BINARY_DIR}/lib/appkit-window-imgui")
endif()

if (NOT TARGET appkit-window-imguizmo)
    add_subdirectory(libs/InteractiveToolkit-AppKit/common/appkit-window-imguizmo "${CMAKE_BINARY_DIR}/lib/appkit-window-imguizmo")
endif()

add_subdirectory(src/test-open-window)

add_subdirectory(src/tech-demo-pbr)

add_subdirectory(src/tech-demo-particle)

add_subdirectory(src/tech-demo-animation)

add_subdirectory(src/tech-demo-shadow)

add_subdirectory(src/imgui_example)

add_subdirectory(src/imguizmo_example)

add_subdirectory(src/imgui_editor)

add_subdirectory(src/imgui_service_ui)

add_subdirectory(src/ipc_fullscreen_viewer)

# option(ARIBEIRO_BUILD_IMGUI_PROJECTS OFF)
# if (ARIBEIRO_BUILD_IMGUI_PROJECTS)
#     add_subdirectory(src/imgui)
#     add_subdirectory(src/imgui_example)
#     add_subdirectory(src/imgui_service_ui)
#     add_subdirectory(src/imgui_editor)
# endif()

add_subdirectory(src/test-pbr-coefs)

add_subdirectory(src/opening)

add_subdirectory(src/font-with-bitmap)

add_subdirectory(src/tech-demo-line-rendering)
