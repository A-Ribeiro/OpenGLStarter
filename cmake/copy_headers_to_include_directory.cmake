############################################################################
# Copy headers to include directory
############################################################################
macro(copy_headers_to_include_directory projectname)
    file(COPY ${ARGN} DESTINATION ${CMAKE_HOME_DIRECTORY}/include/${projectname}/)
endmacro()

include_directories_global(${CMAKE_HOME_DIRECTORY}/include)