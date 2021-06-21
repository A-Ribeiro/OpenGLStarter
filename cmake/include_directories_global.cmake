############################################################################
# Global include directories
############################################################################

set(GLOBAL_DIRECTORIES "" CACHE STRING "")
mark_as_internal(GLOBAL_DIRECTORIES)

macro(include_directories_global)
    foreach(directory IN ITEMS ${ARGN})
        if (NOT ${directory} IN_LIST GLOBAL_DIRECTORIES)
            list(APPEND GLOBAL_DIRECTORIES ${directory})
            set(GLOBAL_DIRECTORIES ${GLOBAL_DIRECTORIES} CACHE INTERNAL "" FORCE)
        endif() 
    endforeach()
endmacro()
#
# send the variable to the compiler for all projects
#
message(STATUS "global include directories: ${GLOBAL_DIRECTORIES}" )
include_directories( ${GLOBAL_DIRECTORIES} )
#
# reset global variable after use
#
set(GLOBAL_DIRECTORIES "" CACHE INTERNAL "" FORCE)
#
# initial setup
#

