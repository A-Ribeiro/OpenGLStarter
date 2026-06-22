if (TARGET liblua OR TARGET libluajit)
    return()
endif()

if( NOT ITK_LUA_MODE )
    set( ITK_LUA_MODE USE_LUA CACHE STRING
                "Choose the type of Lua implementation."
                FORCE )
endif()
set_property(CACHE ITK_LUA_MODE PROPERTY STRINGS USE_LUA USE_LUA_JIT)


if (ITK_LUA_MODE STREQUAL "USE_LUA_JIT")
    if (NOT TARGET libluajit)
        add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/LuaJIT" "${CMAKE_BINARY_DIR}/lib/LuaJIT")
        # alias for LuaJIT
        add_library(liblua ALIAS libluajit)
    endif()
else()
    if (NOT TARGET liblua)
        add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Lua" "${CMAKE_BINARY_DIR}/lib/Lua")
    endif()
endif()

message(STATUS "[ITK_LUA_MODE   ${ITK_LUA_MODE}]")
