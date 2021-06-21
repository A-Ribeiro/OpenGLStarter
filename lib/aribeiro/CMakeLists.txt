project (aribeiro)

#
# General Configuration
#

set(TARGET_LIBS libjpeg png_static zlib-wrapper randomlib )

# include RPI libs
if (ARIBEIRO_RPI)
    set(TARGET_LIBS ${RPI_LIBRARIES} ${TARGET_LIBS})
endif()

#
# The default behaviour of the linker is to
# load symbols just when they are needed.
#
# The code below forces the load of all defined symbols
#
if(OS_TARGET STREQUAL win)
    list(APPEND TARGET_LIBS
                shlwapi iphlpapi #path operations
                winmm ) #multimedia calls
    if (MSVC)
        #Visual Studio
        set(ARIBEIRO_LIBRARIES -WHOLEARCHIVE:aribeiro aribeiro)
    else()
        #MingW...
        set(ARIBEIRO_LIBRARIES -Wl,--whole-archive aribeiro -Wl,--no-whole-archive)
        #set(ARIBEIRO_LIBRARIES aribeiro)
    endif()
elseif(OS_TARGET STREQUAL mac)
    set(ARIBEIRO_LIBRARIES -Wl,-all_load aribeiro)
elseif(OS_TARGET STREQUAL linux)
    list(APPEND TARGET_LIBS pthread )
    set(ARIBEIRO_LIBRARIES -Wl,--whole-archive aribeiro -Wl,--no-whole-archive)
else()
    message(FATAL_ERROR "Platform not tested with aribeiro lib")
endif ()

# export global variables
set(ARIBEIRO_LIBRARIES ${ARIBEIRO_LIBRARIES} PARENT_SCOPE)


FILE( GLOB_RECURSE PUBLIC_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
FILE( GLOB_RECURSE SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)

define_source_group_base_path("src" ${PUBLIC_HEADERS} ${SRC})

add_library( ${PROJECT_NAME} STATIC ${PUBLIC_HEADERS} ${SRC} )

target_link_libraries(${PROJECT_NAME} PUBLIC ${TARGET_LIBS})

copy_headers_to_include_directory( ${PROJECT_NAME} ${PUBLIC_HEADERS} )

configure_include_file( ${PROJECT_NAME} "src/${PROJECT_NAME}.h.in" "${PROJECT_NAME}.h" ${PUBLIC_HEADERS} )

