cmake_minimum_required(VERSION 3.15)

# LuaJIT is incompatible with AddressSanitizer - remove sanitizer flags BEFORE project()
# Save original compile options that might be set in parent
get_directory_property(PARENT_COMPILE_OPTIONS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMPILE_OPTIONS)

# Set the project AFTER saving parent settings
project(LuaJIT C)

# Now remove sanitizer from CMAKE_C_FLAGS and linker flags
string(REGEX REPLACE "-fsanitize=[^ ]*" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REGEX REPLACE "-fno-omit-frame-pointer" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REGEX REPLACE "-fsanitize=[^ ]*" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

# Add back frame pointer omission (required by LuaJIT)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fomit-frame-pointer")

# Clear directory-level compile options that might include sanitizer
set_directory_properties(PROPERTIES COMPILE_OPTIONS "")

message(STATUS "LuaJIT: CMAKE_C_FLAGS=${CMAKE_C_FLAGS}")

# get from git
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/luajit/")
    message(STATUS "[luajit] cloning...")
    find_package(Git REQUIRED)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" clone --recurse-submodules "https://luajit.org/git/luajit.git" "${CMAKE_CURRENT_SOURCE_DIR}/luajit/"
        OUTPUT_VARIABLE result
        #COMMAND_ERROR_IS_FATAL ANY
    )
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/luajit/")
        message(FATAL_ERROR "Error to clone repository: https://luajit.org/git/luajit.git")
    endif()
    message(STATUS "[luajit] done")
endif()

# LuaJIT version information
set(LUAJIT_MAJVER 2)
set(LUAJIT_MINVER 1)
set(LUAJIT_ABIVER 5.1)
set(LUAJIT_NODOTABIVER 51)
set(LUAJIT_MMVERSION "${LUAJIT_MAJVER}.${LUAJIT_MINVER}")

# Build configuration
set(LUAJIT_BUILD_STATIC ON CACHE BOOL "Build static library")

# Platform detection
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(TARGET_SYS "Windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(TARGET_SYS "Darwin")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(TARGET_SYS "Linux")
else()
    set(TARGET_SYS "${CMAKE_SYSTEM_NAME}")
endif()

# Compiler flags
set(CMAKE_C_STANDARD 99)

# Base compiler options (conditional based on compiler)
if(MSVC)
    # MSVC flags - use CMake's optimization settings, don't override
    set(CCOPT_FLAGS "")
    set(CCWARN_FLAGS "/W3")
    set(TARGET_LFSFLAGS "")
else()
    # GCC/Clang flags
    set(CCOPT_FLAGS "-O2;-fomit-frame-pointer")
    set(CCWARN_FLAGS "-Wall")
    set(TARGET_LFSFLAGS "-D_FILE_OFFSET_BITS=64;-D_LARGEFILE_SOURCE")
endif()

# Architecture-specific flags
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|amd64")
    set(TARGET_LJARCH "x64")
    set(CCOPT_ARCH "")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i.86|x86|X86")
    set(TARGET_LJARCH "x86")
    if(MSVC)
        set(CCOPT_ARCH "/arch:SSE2")
    else()
        set(CCOPT_ARCH "-march=i686;-msse;-msse2;-mfpmath=sse")
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64|ARM64|AARCH64")
    set(TARGET_LJARCH "arm64")
    set(CCOPT_ARCH "")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|ARM")
    set(TARGET_LJARCH "arm")
    set(CCOPT_ARCH "")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "ppc|powerpc|PPC|POWERPC")
    set(TARGET_LJARCH "ppc")
    set(CCOPT_ARCH "")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "mips")
    set(TARGET_LJARCH "mips")
    set(CCOPT_ARCH "")
else()
    set(TARGET_LJARCH "x64")
    set(CCOPT_ARCH "")
endif()

# Source directories
set(LUAJIT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/luajit/src")
set(LUAJIT_DYNASM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/luajit/dynasm")
set(LUAJIT_HOST_DIR "${CMAKE_CURRENT_BINARY_DIR}/host")

# Create host build directory
file(MAKE_DIRECTORY "${LUAJIT_HOST_DIR}")

# For MSVC: Remove /RTC1 from Debug flags to avoid conflict with /O2 for host tools
if(MSVC)
    string(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
endif()

###############################################################################
# Step 1: Build minilua (host tool)
###############################################################################

add_executable(minilua
    "${LUAJIT_SRC_DIR}/host/minilua.c"
)

# Optimization for host tools  
if(MSVC)
    set_target_properties(minilua PROPERTIES
        COMPILE_OPTIONS "/O2"
    )
else()
    # Explicitly set compile options, overriding directory-level options  
    set_target_properties(minilua PROPERTIES
        COMPILE_OPTIONS "-O2;-fomit-frame-pointer"
    )
endif()

if(NOT WIN32)
    target_link_libraries(minilua m)
endif()

set_target_properties(minilua PROPERTIES FOLDER "LuaJIT")

###############################################################################
# Step 2: Generate version information
###############################################################################

# Read release version from file or git
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/luajit/.git")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} show -s --format=%ct
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/luajit"
        OUTPUT_VARIABLE LUAJIT_RELVER
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
else()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/luajit/.relver")
        file(READ "${CMAKE_CURRENT_SOURCE_DIR}/luajit/.relver" LUAJIT_RELVER)
        string(STRIP "${LUAJIT_RELVER}" LUAJIT_RELVER)
    elseif(EXISTS "${LUAJIT_SRC_DIR}/luajit_relver.txt")
        file(READ "${LUAJIT_SRC_DIR}/luajit_relver.txt" LUAJIT_RELVER)
        string(STRIP "${LUAJIT_RELVER}" LUAJIT_RELVER)
    else()
        set(LUAJIT_RELVER "rolling")
    endif()
endif()

set(LUAJIT_VERSION "${LUAJIT_MMVERSION}.${LUAJIT_RELVER}")

# Generate luajit_relver.txt
file(WRITE "${LUAJIT_SRC_DIR}/luajit_relver.txt" "${LUAJIT_RELVER}")

# Generate luajit.h with version
add_custom_command(
    OUTPUT "${LUAJIT_SRC_DIR}/luajit.h"
    COMMAND minilua "${LUAJIT_SRC_DIR}/host/genversion.lua"
    WORKING_DIRECTORY "${LUAJIT_SRC_DIR}"
    DEPENDS minilua "${LUAJIT_SRC_DIR}/host/genversion.lua" "${LUAJIT_SRC_DIR}/luajit_rolling.h"
    COMMENT "Generating luajit.h"
)

###############################################################################
# Step 3: Detect architecture features
###############################################################################

# Create a test file to detect architecture features
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/lj_arch_test.c"
    "#include \"lj_arch.h\"\n"
)

# Run preprocessor to get architecture defines
if(MSVC)
    # For MSVC, use a simpler approach based on CMake variables
    # since MSVC preprocessor flags are different
    set(TARGET_TESTARCH "")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        string(APPEND TARGET_TESTARCH "LJ_ARCH_BITS 64\n")
        string(APPEND TARGET_TESTARCH "LJ_LE 1\n")
    else()
        string(APPEND TARGET_TESTARCH "LJ_ARCH_BITS 32\n")
        string(APPEND TARGET_TESTARCH "LJ_LE 1\n")
    endif()
    string(APPEND TARGET_TESTARCH "LJ_HASJIT 1\n")
    string(APPEND TARGET_TESTARCH "LJ_HASFFI 1\n")
    string(APPEND TARGET_TESTARCH "LJ_ARCH_HASFPU 1\n")
    # Check for GC64 mode - default is enabled on x64
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        string(APPEND TARGET_TESTARCH "LJ_FR2 1\n")  # GC64 mode uses FR2
    endif()
else()
    # GCC/Clang preprocessor
    execute_process(
        COMMAND ${CMAKE_C_COMPILER} -E -dM -I"${LUAJIT_SRC_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/lj_arch_test.c"
        OUTPUT_VARIABLE TARGET_TESTARCH
        ERROR_VARIABLE TARGET_TESTARCH_ERROR
        RESULT_VARIABLE TARGET_TESTARCH_RESULT
    )
    
    # Check if preprocessor command failed
    if(NOT TARGET_TESTARCH_RESULT EQUAL 0)
        message(WARNING "Architecture detection failed: ${TARGET_TESTARCH_ERROR}")
        message(STATUS "Using fallback architecture detection")
        # Fallback to CMake's detection
        set(TARGET_TESTARCH "")
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            string(APPEND TARGET_TESTARCH "LJ_ARCH_BITS 64\n")
            string(APPEND TARGET_TESTARCH "LJ_TARGET_X64 1\n")
        else()
            string(APPEND TARGET_TESTARCH "LJ_ARCH_BITS 32\n")
            string(APPEND TARGET_TESTARCH "LJ_TARGET_X86 1\n")
        endif()
        string(APPEND TARGET_TESTARCH "LJ_LE 1\n")
        string(APPEND TARGET_TESTARCH "LJ_HASJIT 1\n")
        string(APPEND TARGET_TESTARCH "LJ_HASFFI 1\n")
        string(APPEND TARGET_TESTARCH "LJ_ARCH_HASFPU 1\n")
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            string(APPEND TARGET_TESTARCH "LJ_FR2 1\n")
            string(APPEND TARGET_TESTARCH "LJ_GC64 1\n")
        endif()
    endif()
endif()

# Debug: Show key architecture detections
message(STATUS "Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "TARGET_TESTARCH length: ${CMAKE_MATCH_0}")
if(TARGET_TESTARCH MATCHES "LJ_ARCH_BITS 64")
    message(STATUS "Detected: 64-bit architecture")
else()
    message(STATUS "Detected: 32-bit architecture")
endif()
if(TARGET_TESTARCH MATCHES "LJ_LE 1")
    message(STATUS "Detected: Little-endian")
else()
    message(STATUS "Detected: Big-endian or unknown")
endif()
if(TARGET_TESTARCH MATCHES "LJ_TARGET_X64")
    message(STATUS "Detected: X64 target")
elseif(TARGET_TESTARCH MATCHES "LJ_TARGET_X86")
    message(STATUS "Detected: X86 target")
endif()

# Parse architecture flags for DynASM
set(DASM_AFLAGS "")

if(TARGET_TESTARCH MATCHES "LJ_LE 1")
    list(APPEND DASM_AFLAGS "-D" "ENDIAN_LE")
else()
    list(APPEND DASM_AFLAGS "-D" "ENDIAN_BE")
endif()

if(TARGET_TESTARCH MATCHES "LJ_ARCH_BITS 64")
    list(APPEND DASM_AFLAGS "-D" "P64")
endif()

if(TARGET_TESTARCH MATCHES "LJ_HASJIT 1")
    list(APPEND DASM_AFLAGS "-D" "JIT")
endif()

if(TARGET_TESTARCH MATCHES "LJ_HASFFI 1")
    list(APPEND DASM_AFLAGS "-D" "FFI")
endif()

if(TARGET_TESTARCH MATCHES "LJ_DUALNUM 1")
    list(APPEND DASM_AFLAGS "-D" "DUALNUM")
endif()

if(TARGET_TESTARCH MATCHES "LJ_ARCH_HASFPU 1")
    list(APPEND DASM_AFLAGS "-D" "FPU")
endif()

if(NOT TARGET_TESTARCH MATCHES "LJ_ABI_SOFTFP 1")
    list(APPEND DASM_AFLAGS "-D" "HFABI")
endif()

if(TARGET_TESTARCH MATCHES "LJ_NO_UNWIND 1")
    list(APPEND DASM_AFLAGS "-D" "NO_UNWIND")
endif()

if(TARGET_SYS STREQUAL "Windows")
    list(APPEND DASM_AFLAGS "-D" "WIN")
endif()

# Detect architecture version
string(REGEX MATCH "LJ_ARCH_VERSION [0-9]+" ARCH_VERSION_MATCH "${TARGET_TESTARCH}")
if(ARCH_VERSION_MATCH)
    string(REGEX REPLACE "LJ_ARCH_VERSION " "" ARCH_VERSION "${ARCH_VERSION_MATCH}")
    list(APPEND DASM_AFLAGS "-D" "VER=${ARCH_VERSION}")
endif()

# For x64, check if we should use x86 mode
set(DASM_ARCH "${TARGET_LJARCH}")
if(TARGET_LJARCH STREQUAL "x64")
    if(NOT TARGET_TESTARCH MATCHES "LJ_FR2 1")
        set(DASM_ARCH "x86")
    endif()
endif()

set(DASM_DASC "${LUAJIT_SRC_DIR}/vm_${DASM_ARCH}.dasc")

# Debug: Show DASM flags
message(STATUS "DASM Architecture: ${DASM_ARCH}")
message(STATUS "DASM Flags: ${DASM_AFLAGS}")
message(STATUS "DASM File: ${DASM_DASC}")

###############################################################################
# Step 4: Generate buildvm_arch.h using DynASM
###############################################################################

add_custom_command(
    OUTPUT "${LUAJIT_HOST_DIR}/buildvm_arch.h"
    COMMAND minilua "${LUAJIT_DYNASM_DIR}/dynasm.lua"
        ${DASM_AFLAGS}
        -o "${LUAJIT_HOST_DIR}/buildvm_arch.h"
        "${DASM_DASC}"
    DEPENDS minilua "${DASM_DASC}"
    WORKING_DIRECTORY "${LUAJIT_SRC_DIR}"
    COMMENT "Generating buildvm_arch.h with DynASM (Flags: ${DASM_AFLAGS})"
)

###############################################################################
# Step 5: Build buildvm (host tool)
###############################################################################

add_executable(buildvm
    "${LUAJIT_SRC_DIR}/host/buildvm.c"
    "${LUAJIT_SRC_DIR}/host/buildvm_asm.c"
    "${LUAJIT_SRC_DIR}/host/buildvm_peobj.c"
    "${LUAJIT_SRC_DIR}/host/buildvm_lib.c"
    "${LUAJIT_SRC_DIR}/host/buildvm_fold.c"
    "${LUAJIT_HOST_DIR}/buildvm_arch.h"
    "${LUAJIT_SRC_DIR}/luajit.h"  # Add dependency on generated header
)

target_include_directories(buildvm PRIVATE
    "${LUAJIT_SRC_DIR}"
    "${LUAJIT_HOST_DIR}"
    "${LUAJIT_DYNASM_DIR}"
)

# Define target architecture for buildvm
if(TARGET_LJARCH STREQUAL "x64")
    target_compile_definitions(buildvm PRIVATE LUAJIT_TARGET=LUAJIT_ARCH_X64)
elseif(TARGET_LJARCH STREQUAL "x86")
    target_compile_definitions(buildvm PRIVATE LUAJIT_TARGET=LUAJIT_ARCH_X86)
elseif(TARGET_LJARCH STREQUAL "arm")
    target_compile_definitions(buildvm PRIVATE LUAJIT_TARGET=LUAJIT_ARCH_ARM)
elseif(TARGET_LJARCH STREQUAL "arm64")
    target_compile_definitions(buildvm PRIVATE LUAJIT_TARGET=LUAJIT_ARCH_ARM64)
elseif(TARGET_LJARCH STREQUAL "ppc")
    target_compile_definitions(buildvm PRIVATE LUAJIT_TARGET=LUAJIT_ARCH_PPC)
elseif(TARGET_LJARCH STREQUAL "mips")
    target_compile_definitions(buildvm PRIVATE LUAJIT_TARGET=LUAJIT_ARCH_MIPS32)
endif()

if(MSVC)
    set_target_properties(buildvm PROPERTIES
        COMPILE_OPTIONS "/O2"
    )
else()
    # Explicitly set compile options, overriding directory-level options
    set_target_properties(buildvm PROPERTIES
        COMPILE_OPTIONS "-O2;-fomit-frame-pointer"
    )
endif()

set_target_properties(buildvm PROPERTIES FOLDER "LuaJIT")

###############################################################################
# Step 6: Generate headers and VM assembly using buildvm
###############################################################################

# List of library source files for buildvm
set(LJLIB_C
    "${LUAJIT_SRC_DIR}/lib_base.c"
    "${LUAJIT_SRC_DIR}/lib_math.c"
    "${LUAJIT_SRC_DIR}/lib_bit.c"
    "${LUAJIT_SRC_DIR}/lib_string.c"
    "${LUAJIT_SRC_DIR}/lib_table.c"
    "${LUAJIT_SRC_DIR}/lib_io.c"
    "${LUAJIT_SRC_DIR}/lib_os.c"
    "${LUAJIT_SRC_DIR}/lib_package.c"
    "${LUAJIT_SRC_DIR}/lib_debug.c"
    "${LUAJIT_SRC_DIR}/lib_jit.c"
    "${LUAJIT_SRC_DIR}/lib_ffi.c"
    "${LUAJIT_SRC_DIR}/lib_buffer.c"
)

# Determine VM mode and output file
if(WIN32)
    set(LJVM_MODE "peobj")
    set(LJVM_BOUT "${CMAKE_CURRENT_BINARY_DIR}/lj_vm.o")
elseif(APPLE)
    set(LJVM_MODE "machasm")
    set(LJVM_BOUT "${CMAKE_CURRENT_BINARY_DIR}/lj_vm.S")
else()
    set(LJVM_MODE "elfasm")
    set(LJVM_BOUT "${CMAKE_CURRENT_BINARY_DIR}/lj_vm.S")
endif()

# Generate lj_vm.S or lj_vm.o
add_custom_command(
    OUTPUT "${LJVM_BOUT}"
    COMMAND buildvm -m ${LJVM_MODE} -o "${LJVM_BOUT}"
    DEPENDS buildvm
    WORKING_DIRECTORY "${LUAJIT_SRC_DIR}"
    COMMENT "Generating VM assembly"
)

# Generate header files
add_custom_command(
    OUTPUT "${LUAJIT_SRC_DIR}/lj_bcdef.h"
    COMMAND buildvm -m bcdef -o "${LUAJIT_SRC_DIR}/lj_bcdef.h" ${LJLIB_C}
    DEPENDS buildvm ${LJLIB_C}
    WORKING_DIRECTORY "${LUAJIT_SRC_DIR}"
    COMMENT "Generating lj_bcdef.h"
)

add_custom_command(
    OUTPUT "${LUAJIT_SRC_DIR}/lj_ffdef.h"
    COMMAND buildvm -m ffdef -o "${LUAJIT_SRC_DIR}/lj_ffdef.h" ${LJLIB_C}
    DEPENDS buildvm ${LJLIB_C}
    WORKING_DIRECTORY "${LUAJIT_SRC_DIR}"
    COMMENT "Generating lj_ffdef.h"
)

add_custom_command(
    OUTPUT "${LUAJIT_SRC_DIR}/lj_libdef.h"
    COMMAND buildvm -m libdef -o "${LUAJIT_SRC_DIR}/lj_libdef.h" ${LJLIB_C}
    DEPENDS buildvm ${LJLIB_C}
    WORKING_DIRECTORY "${LUAJIT_SRC_DIR}"
    COMMENT "Generating lj_libdef.h"
)

add_custom_command(
    OUTPUT "${LUAJIT_SRC_DIR}/lj_recdef.h"
    COMMAND buildvm -m recdef -o "${LUAJIT_SRC_DIR}/lj_recdef.h" ${LJLIB_C}
    DEPENDS buildvm ${LJLIB_C}
    WORKING_DIRECTORY "${LUAJIT_SRC_DIR}"
    COMMENT "Generating lj_recdef.h"
)

add_custom_command(
    OUTPUT "${LUAJIT_SRC_DIR}/lj_folddef.h"
    COMMAND buildvm -m folddef -o "${LUAJIT_SRC_DIR}/lj_folddef.h" "${LUAJIT_SRC_DIR}/lj_opt_fold.c"
    DEPENDS buildvm "${LUAJIT_SRC_DIR}/lj_opt_fold.c"
    WORKING_DIRECTORY "${LUAJIT_SRC_DIR}"
    COMMENT "Generating lj_folddef.h"
)

add_custom_command(
    OUTPUT "${LUAJIT_SRC_DIR}/jit/vmdef.lua"
    COMMAND buildvm -m vmdef -o "${LUAJIT_SRC_DIR}/jit/vmdef.lua" ${LJLIB_C}
    DEPENDS buildvm ${LJLIB_C}
    WORKING_DIRECTORY "${LUAJIT_SRC_DIR}"
    COMMENT "Generating jit/vmdef.lua"
)

###############################################################################
# Step 7: Build LuaJIT static library
###############################################################################

# Core source files
set(LJCORE_C
    "${LUAJIT_SRC_DIR}/lj_assert.c"
    "${LUAJIT_SRC_DIR}/lj_gc.c"
    "${LUAJIT_SRC_DIR}/lj_err.c"
    "${LUAJIT_SRC_DIR}/lj_char.c"
    "${LUAJIT_SRC_DIR}/lj_bc.c"
    "${LUAJIT_SRC_DIR}/lj_obj.c"
    "${LUAJIT_SRC_DIR}/lj_buf.c"
    "${LUAJIT_SRC_DIR}/lj_str.c"
    "${LUAJIT_SRC_DIR}/lj_tab.c"
    "${LUAJIT_SRC_DIR}/lj_func.c"
    "${LUAJIT_SRC_DIR}/lj_udata.c"
    "${LUAJIT_SRC_DIR}/lj_meta.c"
    "${LUAJIT_SRC_DIR}/lj_debug.c"
    "${LUAJIT_SRC_DIR}/lj_prng.c"
    "${LUAJIT_SRC_DIR}/lj_state.c"
    "${LUAJIT_SRC_DIR}/lj_dispatch.c"
    "${LUAJIT_SRC_DIR}/lj_vmevent.c"
    "${LUAJIT_SRC_DIR}/lj_vmmath.c"
    "${LUAJIT_SRC_DIR}/lj_strscan.c"
    "${LUAJIT_SRC_DIR}/lj_strfmt.c"
    "${LUAJIT_SRC_DIR}/lj_strfmt_num.c"
    "${LUAJIT_SRC_DIR}/lj_serialize.c"
    "${LUAJIT_SRC_DIR}/lj_api.c"
    "${LUAJIT_SRC_DIR}/lj_profile.c"
    "${LUAJIT_SRC_DIR}/lj_lex.c"
    "${LUAJIT_SRC_DIR}/lj_parse.c"
    "${LUAJIT_SRC_DIR}/lj_bcread.c"
    "${LUAJIT_SRC_DIR}/lj_bcwrite.c"
    "${LUAJIT_SRC_DIR}/lj_load.c"
    "${LUAJIT_SRC_DIR}/lj_ir.c"
    "${LUAJIT_SRC_DIR}/lj_opt_mem.c"
    "${LUAJIT_SRC_DIR}/lj_opt_fold.c"
    "${LUAJIT_SRC_DIR}/lj_opt_narrow.c"
    "${LUAJIT_SRC_DIR}/lj_opt_dce.c"
    "${LUAJIT_SRC_DIR}/lj_opt_loop.c"
    "${LUAJIT_SRC_DIR}/lj_opt_split.c"
    "${LUAJIT_SRC_DIR}/lj_opt_sink.c"
    "${LUAJIT_SRC_DIR}/lj_mcode.c"
    "${LUAJIT_SRC_DIR}/lj_snap.c"
    "${LUAJIT_SRC_DIR}/lj_record.c"
    "${LUAJIT_SRC_DIR}/lj_crecord.c"
    "${LUAJIT_SRC_DIR}/lj_ffrecord.c"
    "${LUAJIT_SRC_DIR}/lj_asm.c"
    "${LUAJIT_SRC_DIR}/lj_trace.c"
    "${LUAJIT_SRC_DIR}/lj_gdbjit.c"
    "${LUAJIT_SRC_DIR}/lj_ctype.c"
    "${LUAJIT_SRC_DIR}/lj_cdata.c"
    "${LUAJIT_SRC_DIR}/lj_cconv.c"
    "${LUAJIT_SRC_DIR}/lj_ccall.c"
    "${LUAJIT_SRC_DIR}/lj_ccallback.c"
    "${LUAJIT_SRC_DIR}/lj_carith.c"
    "${LUAJIT_SRC_DIR}/lj_clib.c"
    "${LUAJIT_SRC_DIR}/lj_cparse.c"
    "${LUAJIT_SRC_DIR}/lj_lib.c"
    "${LUAJIT_SRC_DIR}/lj_alloc.c"
    "${LUAJIT_SRC_DIR}/lib_aux.c"
    ${LJLIB_C}
    "${LUAJIT_SRC_DIR}/lib_init.c"
)

# Create static library
add_library(libluajit STATIC
    ${LJCORE_C}
    "${LJVM_BOUT}"
)

# Set library output name
if(MSVC)
    set_target_properties(libluajit PROPERTIES
        OUTPUT_NAME "luajit"
        FOLDER "LuaJIT"
    )
else()
    set_target_properties(libluajit PROPERTIES
        OUTPUT_NAME "luajit"
        PREFIX "lib"
        FOLDER "LuaJIT"
    )
endif()

# Include directories
target_include_directories(libluajit PUBLIC
    $<BUILD_INTERFACE:${LUAJIT_SRC_DIR}>
    $<INSTALL_INTERFACE:include/luajit-${LUAJIT_MMVERSION}>
)

# Detect unwind support (required for proper x64 exception handling)
set(LUAJIT_UNWIND_EXTERNAL OFF)
if(NOT MSVC AND NOT WIN32)
    if(APPLE)
        # macOS always needs LUAJIT_UNWIND_EXTERNAL
        set(LUAJIT_UNWIND_EXTERNAL ON)
    elseif(TARGET_TESTARCH MATCHES "LJ_NO_UNWIND 1")
        # Architecture has no unwind support
        set(LUAJIT_UNWIND_EXTERNAL OFF)
    else()
        # For Linux and other POSIX systems with GCC/Clang, enable LUAJIT_UNWIND_EXTERNAL
        # The toolchain generates unwind tables by default
        set(LUAJIT_UNWIND_EXTERNAL ON)
    endif()
endif()

message(STATUS "LuaJIT UNWIND_EXTERNAL: ${LUAJIT_UNWIND_EXTERNAL}")

# Compile options - use set_target_properties to override directory-level options
if(MSVC)
    set_target_properties(libluajit PROPERTIES
        COMPILE_OPTIONS "${CCOPT_FLAGS};${CCOPT_ARCH};${CCWARN_FLAGS};${TARGET_LFSFLAGS}"
    )
else()
    set_target_properties(libluajit PROPERTIES
        COMPILE_OPTIONS "${CCOPT_FLAGS};${CCOPT_ARCH};${CCWARN_FLAGS};${TARGET_LFSFLAGS};-U_FORTIFY_SOURCE;-fno-stack-protector"
    )
    
    # Add unwind external flag if needed
    if(LUAJIT_UNWIND_EXTERNAL)
        target_compile_definitions(libluajit PRIVATE LUAJIT_UNWIND_EXTERNAL)
    endif()
    
    # Add linker flag for export dynamic symbols
    if(NOT APPLE AND NOT CMAKE_SYSTEM_NAME MATCHES "SunOS")
        target_link_options(libluajit PRIVATE -Wl,-E)
    endif()
endif()

# Platform-specific settings
if(NOT WIN32)
    target_link_libraries(libluajit PUBLIC m)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_link_libraries(libluajit PUBLIC dl)
    endif()
endif()

# Handle assembly file compilation
if(NOT WIN32)
    set_source_files_properties("${LJVM_BOUT}" PROPERTIES LANGUAGE C)
endif()

# Make sure generated files are created before compilation
add_custom_target(luajit_generated_files
    DEPENDS
        "${LUAJIT_SRC_DIR}/luajit.h"
        "${LUAJIT_SRC_DIR}/lj_bcdef.h"
        "${LUAJIT_SRC_DIR}/lj_ffdef.h"
        "${LUAJIT_SRC_DIR}/lj_libdef.h"
        "${LUAJIT_SRC_DIR}/lj_recdef.h"
        "${LUAJIT_SRC_DIR}/lj_folddef.h"
        "${LUAJIT_SRC_DIR}/jit/vmdef.lua"
        "${LJVM_BOUT}"
)

set_target_properties(luajit_generated_files PROPERTIES FOLDER "LuaJIT")

add_dependencies(libluajit luajit_generated_files)

###############################################################################
# Installation
###############################################################################

# Install library
install(TARGETS libluajit
    EXPORT LuaJITTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(FILES
    "${LUAJIT_SRC_DIR}/lua.h"
    "${LUAJIT_SRC_DIR}/lualib.h"
    "${LUAJIT_SRC_DIR}/lauxlib.h"
    "${LUAJIT_SRC_DIR}/luaconf.h"
    "${LUAJIT_SRC_DIR}/lua.hpp"
    "${LUAJIT_SRC_DIR}/luajit.h"
    DESTINATION include/luajit-${LUAJIT_MMVERSION}
)

# Install JIT library files
file(GLOB JITLIB_FILES "${LUAJIT_SRC_DIR}/jit/*.lua")
install(FILES ${JITLIB_FILES}
    DESTINATION share/luajit-${LUAJIT_MMVERSION}/jit
)

# Export targets
install(EXPORT LuaJITTargets
    FILE LuaJITTargets.cmake
    NAMESPACE LuaJIT::
    DESTINATION lib/cmake/LuaJIT
)

# Create config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/LuaJITConfigVersion.cmake"
    VERSION ${LUAJIT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Simple config file
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/LuaJITConfig.cmake"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/LuaJITTargets.cmake\")

set(LuaJIT_FOUND TRUE)
set(LuaJIT_LIBRARIES LuaJIT::libluajit)
set(LuaJIT_INCLUDE_DIRS \"\${CMAKE_CURRENT_LIST_DIR}/../../include/luajit-${LUAJIT_MMVERSION}\")
"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/LuaJITConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/LuaJITConfigVersion.cmake"
    DESTINATION lib/cmake/LuaJIT
)

###############################################################################
# Summary
###############################################################################

message(STATUS "LuaJIT Configuration:")
message(STATUS "  Version: ${LUAJIT_VERSION}")
message(STATUS "  Target System: ${TARGET_SYS}")
message(STATUS "  Target Architecture: ${TARGET_LJARCH}")
message(STATUS "  DynASM Architecture: ${DASM_ARCH}")
message(STATUS "  VM Mode: ${LJVM_MODE}")
message(STATUS "  Static Library: ${LUAJIT_BUILD_STATIC}")
