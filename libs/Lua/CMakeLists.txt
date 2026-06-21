cmake_minimum_required(VERSION 3.15)

# LuaJIT is incompatible with AddressSanitizer - remove sanitizer flags BEFORE project()
# Save original compile options that might be set in parent
get_directory_property(PARENT_COMPILE_OPTIONS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMPILE_OPTIONS)

# Set the project AFTER saving parent settings
project(Lua C)

# Compiler flags
set(CMAKE_C_STANDARD 99)

# Now remove sanitizer from CMAKE_C_FLAGS and linker flags
# string(REGEX REPLACE "-fsanitize=[^ ]*" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REGEX REPLACE "-fno-omit-frame-pointer" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
# string(REGEX REPLACE "-fsanitize=[^ ]*" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

# Add back frame pointer omission (required by LuaJIT)
if(NOT WIN32 AND NOT MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fomit-frame-pointer")
endif()

# Clear directory-level compile options that might include sanitizer
set_directory_properties(PROPERTIES COMPILE_OPTIONS "")

message(STATUS "Lua: CMAKE_C_FLAGS=${CMAKE_C_FLAGS}")

# get from git
set(LUA_VERSION "lua-5.5.0")

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${LUA_VERSION}/")
    message(STATUS "[lua] downloading...")
    file(DOWNLOAD "https://www.lua.org/ftp/${LUA_VERSION}.tar.gz" "${CMAKE_CURRENT_SOURCE_DIR}/${LUA_VERSION}.tar.gz" SHOW_PROGRESS STATUS result)
    list (GET result 0 error_code)
    if(NOT error_code EQUAL 0)
        file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/${LUA_VERSION}.tar.gz")
        message(FATAL_ERROR "Cannot download: https://www.lua.org/ftp/${LUA_VERSION}.tar.gz")
    endif()
    message(STATUS "[lua] unzip...")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_CURRENT_SOURCE_DIR}/${LUA_VERSION}.tar.gz"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        RESULT_VARIABLE ret
    )
    if(NOT ret EQUAL 0)
        file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/${LUA_VERSION}.tar.gz")
        message( FATAL_ERROR "Cannot unzip ${CMAKE_CURRENT_SOURCE_DIR}/${LUA_VERSION}.tar.gz")
    endif()
    
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${LUA_VERSION}/")
        message(FATAL_ERROR "Error to extract Lua: https://www.lua.org/ftp/${LUA_VERSION}.tar.gz")
    endif()
    message(STATUS "[lua] done")
endif()

set(LUA_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${LUA_VERSION}/src")

# Define source files explicitly (from Makefile CORE_O and LIB_O)
set(LUA_CORE_SRC
    ${LUA_SRC_DIR}/lapi.c
    ${LUA_SRC_DIR}/lcode.c
    ${LUA_SRC_DIR}/lctype.c
    ${LUA_SRC_DIR}/ldebug.c
    ${LUA_SRC_DIR}/ldo.c
    ${LUA_SRC_DIR}/ldump.c
    ${LUA_SRC_DIR}/lfunc.c
    ${LUA_SRC_DIR}/lgc.c
    ${LUA_SRC_DIR}/llex.c
    ${LUA_SRC_DIR}/lmem.c
    ${LUA_SRC_DIR}/lobject.c
    ${LUA_SRC_DIR}/lopcodes.c
    ${LUA_SRC_DIR}/lparser.c
    ${LUA_SRC_DIR}/lstate.c
    ${LUA_SRC_DIR}/lstring.c
    ${LUA_SRC_DIR}/ltable.c
    ${LUA_SRC_DIR}/ltm.c
    ${LUA_SRC_DIR}/lundump.c
    ${LUA_SRC_DIR}/lvm.c
    ${LUA_SRC_DIR}/lzio.c
)

set(LUA_LIB_SRC
    ${LUA_SRC_DIR}/lauxlib.c
    ${LUA_SRC_DIR}/lbaselib.c
    ${LUA_SRC_DIR}/lcorolib.c
    ${LUA_SRC_DIR}/ldblib.c
    ${LUA_SRC_DIR}/liolib.c
    ${LUA_SRC_DIR}/lmathlib.c
    ${LUA_SRC_DIR}/loadlib.c
    ${LUA_SRC_DIR}/loslib.c
    ${LUA_SRC_DIR}/lstrlib.c
    ${LUA_SRC_DIR}/ltablib.c
    ${LUA_SRC_DIR}/lutf8lib.c
    ${LUA_SRC_DIR}/linit.c
)

set(LUA_HEADERS
    ${LUA_SRC_DIR}/lua.h
    ${LUA_SRC_DIR}/luaconf.h
    ${LUA_SRC_DIR}/lualib.h
    ${LUA_SRC_DIR}/lauxlib.h
    ${LUA_SRC_DIR}/lua.hpp
)

# Platform-specific definitions and libraries (from src/Makefile)
set(LUA_DEFINITIONS "")
set(LUA_LIBRARIES "")

if(WIN32)
    if(MSVC)
        # Windows MSVC build
        # list(APPEND LUA_DEFINITIONS -DLUA_BUILD_AS_DLL)
        # Note: Building as static lib, not DLL for this project
    elseif(MINGW)
        # MinGW build (from mingw target in Makefile)
        # list(APPEND LUA_DEFINITIONS -DLUA_BUILD_AS_DLL)
    endif()
elseif(UNIX)
    if(APPLE)
        if(IOS)
            # iOS build
            list(APPEND LUA_DEFINITIONS -DLUA_USE_IOS)
        else()
            # macOS build
            list(APPEND LUA_DEFINITIONS -DLUA_USE_MACOSX -DLUA_USE_READLINE)
            list(APPEND LUA_LIBRARIES readline)
        endif()
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        # Linux build
        list(APPEND LUA_DEFINITIONS -DLUA_USE_LINUX)
        list(APPEND LUA_LIBRARIES dl)
        # Add export-dynamic linker flag equivalent
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-E")
    elseif(CMAKE_SYSTEM_NAME MATCHES "FreeBSD|NetBSD|OpenBSD")
        # FreeBSD/NetBSD/OpenBSD build
        list(APPEND LUA_DEFINITIONS -DLUA_USE_LINUX -DLUA_USE_READLINE)
        list(APPEND LUA_LIBRARIES edit)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-E")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
        # Solaris build
        list(APPEND LUA_DEFINITIONS -DLUA_USE_POSIX -DLUA_USE_DLOPEN -D_REENTRANT)
        list(APPEND LUA_LIBRARIES dl)
    else()
        # Generic POSIX build
        list(APPEND LUA_DEFINITIONS -DLUA_USE_POSIX)
    endif()
endif()

# Create the library
add_library(liblua STATIC ${LUA_CORE_SRC} ${LUA_LIB_SRC} ${LUA_HEADERS})

# Set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(liblua PROPERTIES FOLDER "Lua")

# Apply platform-specific definitions
target_compile_definitions(liblua PRIVATE ${LUA_DEFINITIONS})

# Compiler flags (from Makefile: -O2 -Wall -Wextra)
if(NOT MSVC)
    target_compile_options(liblua PRIVATE -Wall -Wextra)
    # Optimization is controlled by CMAKE_BUILD_TYPE
else()
    target_compile_options(liblua PRIVATE /W3)
endif()

# Link libraries (math library is needed on most platforms)
target_link_libraries(liblua PRIVATE ${LUA_LIBRARIES})

# Math library for non-Windows platforms
if(UNIX)
    target_link_libraries(liblua PRIVATE m)
endif()

target_include_directories(
    liblua
PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/${LUA_VERSION}/src
)

