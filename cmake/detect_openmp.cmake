
if(NOT CMAKE_GENERATOR STREQUAL Xcode)

    find_package(OpenMP)
    if (OPENMP_FOUND)
        add_definitions_global(-DOPENMP)
        include_directories_global(${OpenMP_C_INCLUDE_DIRS} ${OpenMP_CXX_INCLUDE_DIRS})

        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

        message(STATUS "[OPENMP ENABLED] OpenMP ready to use!!!")
    else()
        message(STATUS "[OPENMP DISABLED]")
    endif()

else()

    message(STATUS "[OPENMP DISABLED] Xcode does not supports OpenMP")

endif()

