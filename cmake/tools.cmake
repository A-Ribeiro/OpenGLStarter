macro ( mark_as_internal _var )
  set ( ${_var} ${${_var}} CACHE INTERNAL "hide this!" FORCE )
endmacro( mark_as_internal _var )

macro(list_to_string list str)
    set(${str} "")
    foreach(entry ${list})
        string(LENGTH "${${str}}" length)
        if( ${length} EQUAL 0 )
            string(APPEND ${str} "${entry}" )
        else()
            string(APPEND ${str} " ${entry}" )
        endif()
    endforeach()
endmacro()

macro(create_missing_cmake_build_type)
    #if( NOT DEFINED CMAKE_BUILD_TYPE ) #the variable need to be check with empty content
    if( NOT CMAKE_BUILD_TYPE )
        set( CMAKE_BUILD_TYPE Release CACHE STRING
                "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
                FORCE )
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS None Debug Release RelWithDebInfo MinSizeRel)
    endif()
endmacro()

macro(configure_include_file projectname inputfile outputfile)
    set(cmake_includes "")
    foreach(filename IN ITEMS ${ARGN})
        get_filename_component(FILENAME_WITHOUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${filename}" NAME)
        set(cmake_includes "${cmake_includes}#include <${projectname}/${FILENAME_WITHOUT_PATH}>\n")
    endforeach()
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/${inputfile}"
        "${CMAKE_HOME_DIRECTORY}/include/${projectname}/${outputfile}"
        @ONLY
    )
endmacro()

macro(define_source_group )
    foreach(entry IN ITEMS ${ARGN})
        get_filename_component(dirname "${entry}" DIRECTORY )
        if (dirname)
            string(REPLACE "/" "\\" dirname_replaced ${dirname})
            source_group(${dirname_replaced} FILES ${entry})
        else()
            source_group("" FILES ${entry})
        endif()
    endforeach()
endmacro()

macro(define_source_group_base_path base_path )
    foreach(entry IN ITEMS ${ARGN})
        get_filename_component(dirname "${entry}" DIRECTORY )
        if (dirname)
            
            string(FIND "${dirname}" "${base_path}" found)

            if (found VERSION_EQUAL 0)
                string(LENGTH "${base_path}" base_path_length)
                string(LENGTH "${dirname}" dirname_length)
                math(EXPR new_length "${dirname_length} - ${base_path_length}")
                string(SUBSTRING "${dirname}" 
                        "${base_path_length}" 
                        "${new_length}" dirname)
            endif()

            string(FIND "${dirname}" "/" found)
            if (found VERSION_EQUAL 0)
                string(LENGTH "${dirname}" dirname_length)
                math(EXPR new_length "${dirname_length} - 1")
                string(SUBSTRING "${dirname}" 
                        "1" 
                        "${new_length}" dirname)
            endif()

            string(LENGTH "${dirname}" dirname_length)
            if (dirname_length VERSION_GREATER 0)
                string(REPLACE "/" "\\" dirname_replaced ${dirname})
                source_group(${dirname_replaced} FILES ${entry})
            else()
                source_group("" FILES ${entry})
            endif()

        else()
            source_group("" FILES ${entry})
        endif()
    endforeach()
endmacro()

macro(copy_file_after_build PROJECT_NAME)
    foreach(FILENAME IN ITEMS ${ARGN})
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}")
			get_filename_component(FILENAME_WITHOUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}" NAME)
            add_custom_command(
                TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                        ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}
                        $<TARGET_FILE_DIR:${PROJECT_NAME}>/${FILENAME_WITHOUT_PATH} )
        else()
            message(FATAL_ERROR "File Does Not Exists: ${FILENAME}")
        endif()
    endforeach()
endmacro()

macro(copy_directory_after_build PROJECT_NAME)
    foreach(DIRECTORY IN ITEMS ${ARGN})
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}")
            add_custom_command(
                TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}
                        $<TARGET_FILE_DIR:${PROJECT_NAME}>/${DIRECTORY} )
        else()
            message(FATAL_ERROR "Directory Does Not Exists: ${DIRECTORY}")
        endif()
    endforeach()
endmacro()

macro(copy_alessandro_ribeiro_content_after_build PROJECT_NAME DIRECTORY)
	add_custom_command(
		TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
				${CMAKE_HOME_DIRECTORY}/AlessandroRibeiro
				 $<TARGET_FILE_DIR:${PROJECT_NAME}>/${DIRECTORY}/AlessandroRibeiro )
endmacro()
