############################################################################
# DLL Managment
#
# Use these macros only if your
# library have a 3rdparty dll
# that needs to be copied to
# the executable binary directory
############################################################################
set(DLL_LIST "" CACHE STRING "")
mark_as_internal(DLL_LIST)

set(DLL_LIST_NAMES "" CACHE STRING "")
mark_as_internal(DLL_LIST_NAMES)

macro(register_dll)
    foreach(dll IN ITEMS ${ARGN})
        get_filename_component(dll_full_path "${CMAKE_CURRENT_SOURCE_DIR}/${dll}" ABSOLUTE)
        get_filename_component(dll_filename "${CMAKE_CURRENT_SOURCE_DIR}/${dll}" NAME)
        if (NOT ${dll_full_path} IN_LIST DLL_LIST AND 
            NOT ${dll_filename} IN_LIST DLL_LIST_NAMES)

            if(EXISTS "${dll_full_path}")

                list(APPEND DLL_LIST ${dll_full_path})
                set(DLL_LIST ${DLL_LIST} CACHE INTERNAL "" FORCE)

                list(APPEND DLL_LIST_NAMES ${dll_filename})
                set(DLL_LIST_NAMES ${DLL_LIST_NAMES} CACHE INTERNAL "" FORCE)

            else()

                message(FATAL_ERROR "DLL Not Found: ${dll_full_path}")

            endif()

        endif()
    endforeach()
endmacro()

macro(register_dll_absolute_path)
    foreach(dll IN ITEMS ${ARGN})
        get_filename_component(dll_full_path "${dll}" ABSOLUTE)
        get_filename_component(dll_filename "${dll}" NAME)
        if (NOT ${dll_full_path} IN_LIST DLL_LIST AND 
            NOT ${dll_filename} IN_LIST DLL_LIST_NAMES)

            if(EXISTS "${dll_full_path}")

                list(APPEND DLL_LIST ${dll_full_path})
                set(DLL_LIST ${DLL_LIST} CACHE INTERNAL "" FORCE)

                list(APPEND DLL_LIST_NAMES ${dll_filename})
                set(DLL_LIST_NAMES ${DLL_LIST_NAMES} CACHE INTERNAL "" FORCE)

            else()

                message(FATAL_ERROR "DLL Not Found: ${dll_full_path}")

            endif()

        endif()
    endforeach()
endmacro()
#
# send the variable to the compiler for all projects
#
message(STATUS "dll list: ${DLL_LIST_NAMES}" )
#
# reset global variable after use
#
#cannot reset after use... 
# set(DLL_LIST "" CACHE INTERNAL "" FORCE)
# set(DLL_LIST_NAMES "" CACHE INTERNAL "" FORCE)

macro(copy_3rdparty_dll project_name)
    foreach(dll IN ITEMS ${DLL_LIST})
        get_filename_component(dll_filename ${dll} NAME)
        add_custom_command(
            TARGET ${project_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                    ${dll}
                    $<TARGET_FILE_DIR:${project_name}>/${dll_filename} )
    endforeach()
endmacro()
