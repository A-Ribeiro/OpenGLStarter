############################################################################
# Global linker flags
############################################################################

set(GLOBAL_LINKER_FLAGS "" CACHE STRING "")
mark_as_internal(GLOBAL_LINKER_FLAGS)

macro(add_linker_flag_global)
    foreach(flag IN ITEMS ${ARGN})
        if (NOT ${flag} IN_LIST GLOBAL_LINKER_FLAGS)
            list(APPEND GLOBAL_LINKER_FLAGS ${flag})
            set(GLOBAL_LINKER_FLAGS ${GLOBAL_LINKER_FLAGS} CACHE INTERNAL "" FORCE)
        endif()
    endforeach()
endmacro()
#
# send the variable to the compiler for all projects
#
list_to_string( "${GLOBAL_LINKER_FLAGS}" GLOBAL_LINKER_FLAGS_STR )
message(STATUS "global linker flags: ${GLOBAL_LINKER_FLAGS_STR}" )
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GLOBAL_LINKER_FLAGS_STR}")
#
# reset global variable after use
#
set(GLOBAL_LINKER_FLAGS "" CACHE INTERNAL "" FORCE)
#
# initial setup
#
