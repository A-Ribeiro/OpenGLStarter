############################################################################
# Detect NEON compatible environment
############################################################################
if( ARCH_TARGET STREQUAL arm32 OR ARCH_TARGET STREQUAL arm64 )
    set( ARIBEIRO_NEON ON CACHE BOOL "Turn on NEON optimizations." )
    if (ARIBEIRO_NEON)
        message(STATUS "**************")
        message(STATUS "* USING NEON *")
        message(STATUS "**************")
		add_definitions_global(-DARIBEIRO_NEON -mfpu=neon -finline-functions)
    endif()
endif()
