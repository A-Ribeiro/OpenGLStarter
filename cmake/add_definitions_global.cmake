############################################################################
# Global compiler flags
############################################################################

set(GLOBAL_DEFINITIONS "" CACHE STRING "")
mark_as_internal(GLOBAL_DEFINITIONS)

macro(add_definitions_global)
    foreach(definition IN ITEMS ${ARGN})
        if (NOT ${definition} IN_LIST GLOBAL_DEFINITIONS)
            list(APPEND GLOBAL_DEFINITIONS ${definition})
            set(GLOBAL_DEFINITIONS ${GLOBAL_DEFINITIONS} CACHE INTERNAL "" FORCE)
        endif() 
    endforeach()
endmacro()
#
# send the variable to the compiler for all projects
#
list_to_string( "${GLOBAL_DEFINITIONS}" GLOBAL_DEFINITIONS_STR )
message(STATUS "global definitions: ${GLOBAL_DEFINITIONS_STR}" )
add_definitions( ${GLOBAL_DEFINITIONS_STR} )
#
# reset global variable after use
#
set(GLOBAL_DEFINITIONS "" CACHE INTERNAL "" FORCE)
#
# initial setup
#
add_definitions_global(-DOS_TARGET_${OS_TARGET}
                       -DARCH_TARGET_${ARCH_TARGET})
