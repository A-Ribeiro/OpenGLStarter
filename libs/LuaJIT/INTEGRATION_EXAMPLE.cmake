# Example: How to integrate LuaJIT into your CMakeLists.txt

# Directory structure:
# libs/LuaJIT/
#   ├── CMakeLists.txt          (LuaJIT wrapper)
#   └── luajit/                 (LuaJIT source from git)
#       ├── src/
#       └── ...

# Option 1: Add as subdirectory (recommended for this setup)
# Add this to your main CMakeLists.txt:

if (NOT TARGET libluajit)
    add_subdirectory(../../libs/LuaJIT "${CMAKE_BINARY_DIR}/lib/LuaJIT")
endif()
#add_subdirectory(libs/LuaJIT)


# Then link it to your target:
add_executable(myapp src/main.cpp)
target_link_libraries(myapp PRIVATE libluajit)

# The libluajit target already includes:
# - All necessary include directories
# - Platform-specific libraries (libm on Unix, libdl on Linux)
# - Proper compile definitions

# ============================================================================

# Option 2: Use as an external project (if installed)
find_package(LuaJIT REQUIRED)
target_link_libraries(myapp PRIVATE LuaJIT::libluajit)

# ============================================================================

# Full example CMakeLists.txt for a project using LuaJIT:

cmake_minimum_required(VERSION 3.15)
project(MyLuaJITProject)

# Add LuaJIT
add_subdirectory(libs/LuaJIT)

# Your application
add_executable(myapp
    src/main.cpp
    src/script_engine.cpp
)

# Link LuaJIT
target_link_libraries(myapp PRIVATE libluajit)

# If you need C++17 or higher
target_compile_features(myapp PRIVATE cxx_std_17)

# ============================================================================

# Example C++ code (src/main.cpp):

/*
#include <iostream>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

int main() {
    std::cout << "Initializing LuaJIT..." << std::endl;
    
    // Create Lua state
    lua_State *L = luaL_newstate();
    if (!L) {
        std::cerr << "Failed to create Lua state" << std::endl;
        return 1;
    }
    
    // Load standard libraries
    luaL_openlibs(L);
    
    // Execute a simple Lua script
    const char* script = R"(
        print("Hello from LuaJIT!")
        print("Version: " .. jit.version)
        
        -- Test JIT functionality
        local function factorial(n)
            if n <= 1 then return 1 end
            return n * factorial(n-1)
        end
        
        print("Factorial(10) = " .. factorial(10))
    )";
    
    if (luaL_dostring(L, script) != LUA_OK) {
        std::cerr << "Lua error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }
    
    // Clean up
    lua_close(L);
    
    std::cout << "Done!" << std::endl;
    return 0;
}
*/
