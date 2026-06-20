# LuaJIT CMake Build

This CMakeLists.txt provides a complete CMake build system for LuaJIT that builds only the static library for use as a dependency in other projects.

**Note:** This CMakeLists.txt expects the LuaJIT source to be in a `luajit` subdirectory (e.g., cloned from `git clone https://luajit.org/git/luajit.git`).

## Features

- ✅ Builds LuaJIT as a static library (`libluajit.a`)
- ✅ Cross-platform support (Windows, Linux, macOS)
- ✅ Architecture detection (x86, x64, ARM, ARM64, PPC, MIPS)
- ✅ Automatic generation of all required files using minilua and buildvm
- ✅ Proper CMake package configuration for easy integration
- ✅ No external dependencies required for build

## Building

### Directory Structure

This assumes you have the following structure:
```
libs/LuaJIT/
  ├── CMakeLists.txt          (this CMake file)
  ├── CMAKE_BUILD.md          (this documentation)
  ├── INTEGRATION_EXAMPLE.cmake
  └── luajit/                 (LuaJIT source from git)
      ├── src/
      ├── dynasm/
      ├── Makefile
      └── ...
```

To get the LuaJIT source:
```bash
cd libs/LuaJIT
git clone https://luajit.org/git/luajit.git
```

### Standalone Build

```bash
cd libs/LuaJIT
mkdir build
cd build
cmake ..
cmake --build .
cmake --install . --prefix /usr/local
```

### As a Subdirectory in Another Project

Add to your parent CMakeLists.txt:

```cmake
# Add LuaJIT as a subdirectory
add_subdirectory(libs/LuaJIT)

# Link against LuaJIT in your target
target_link_libraries(your_target PRIVATE libluajit)
```

### Using find_package

After installation, you can find LuaJIT in other projects:

```cmake
find_package(LuaJIT REQUIRED)
target_link_libraries(your_target PRIVATE LuaJIT::libluajit)
```

## CMake Options

Currently, the build is configured to only build the static library. No additional options are exposed as this is designed for use as a dependency.

## How It Works

The build process follows these steps:

1. **Build minilua** - A minimal Lua interpreter used as a host tool
2. **Generate version info** - Creates version headers from git or release files
3. **Detect architecture** - Tests the compiler to determine target architecture features
4. **Generate buildvm_arch.h** - Uses DynASM (via minilua) to generate architecture-specific code
5. **Build buildvm** - The LuaJIT VM code generator tool
6. **Generate VM code** - Creates assembly/object files and headers for the VM
7. **Build static library** - Compiles all LuaJIT source files into `libluajit.a`

## Architecture Support

- x86 (32-bit)
- x64 (64-bit)
- ARM (32-bit)
- ARM64 (AArch64)
- PowerPC
- MIPS

## Platform Support

- **Windows**: MSVC, MinGW
- **Linux**: GCC, Clang
- **macOS**: Apple Clang
- **BSD**: Should work with system compilers

## Requirements

- CMake 3.15 or higher
- C compiler (GCC, Clang, MSVC)
- Git (optional, for version detection)

## Output

The build produces:

- `libluajit.a` - Static library
- Header files: `lua.h`, `lualib.h`, `lauxlib.h`, `luaconf.h`, `lua.hpp`, `luajit.h`
- JIT library files: `jit/*.lua`

## Differences from Original Makefile

This CMakeLists.txt:

- Only builds the static library (no dynamic library or executable)
- Uses CMake's cross-platform capabilities instead of Make
- Automatically handles all platform-specific details
- Provides proper CMake package configuration
- Integrates cleanly with other CMake projects

## Troubleshooting

### Build Fails on Windows

Make sure you're using either:
- Visual Studio (MSVC) - Use "Visual Studio" generator
- MinGW - Use "MinGW Makefiles" or "Ninja" generator

### Architecture Detection Issues

The build automatically detects architecture from the compiler. If you need to force a specific architecture, you may need to adjust compiler flags through CMake:

```bash
cmake -DCMAKE_C_FLAGS="-m32" ..  # Force 32-bit
```

### Missing Generated Files

If the build fails due to missing generated files, clean the build directory and rebuild:

```bash
rm -rf build
mkdir build
cd build
cmake ..
cmake --build .
```

## Integration Example

Example of integrating LuaJIT into your project:

```cmake
cmake_minimum_required(VERSION 3.15)
project(MyProject)

# Add LuaJIT
add_subdirectory(libs/LuaJIT/LuaJIT)

# Your executable
add_executable(myapp main.cpp)

# Link LuaJIT
target_link_libraries(myapp PRIVATE libluajit)

# On Linux, you may also need libdl and libm (handled automatically by libluajit)
```

In your code:

```cpp
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

int main() {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    
    // Use LuaJIT...
    
    lua_close(L);
    return 0;
}
```

## License

LuaJIT is licensed under the MIT license. See the COPYRIGHT file in the LuaJIT directory for details.
