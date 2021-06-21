#
# To use this toolchain file you need to run the folowing brew installation:
#
# brew install --with-toolchain llvm
#

set(COMPILER_PATH /usr/local/opt/llvm)

set(CMAKE_C_COMPILER ${COMPILER_PATH}/bin/clang)
set(CMAKE_CXX_COMPILER ${COMPILER_PATH}/bin/clang++)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I${COMPILER_PATH}/include -I${COMPILER_PATH}/include/c++/v1/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${COMPILER_PATH}/include -I${COMPILER_PATH}/include/c++/v1/")

SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -L${COMPILER_PATH}/lib -Wl,-rpath,${COMPILER_PATH}/lib")


