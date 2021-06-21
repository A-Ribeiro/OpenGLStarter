project (rtvideo)

FILE( GLOB src *.cpp)
FILE( GLOB header *.h)

set( PUBLIC_HEADERS ${header} )
set( PRIVATE_HEADERS "" )
set( SRC ${src} )

if(OS_TARGET STREQUAL linux)

    FILE( GLOB src_v4l2 v4l2/*.cpp)
    FILE( GLOB header_v4l2 v4l2/*.h)

    set( PUBLIC_HEADERS ${PUBLIC_HEADERS} ${header_v4l2} )
    set( SRC ${SRC} ${src_v4l2} )

elseif( OS_TARGET STREQUAL mac )
    
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/avfoundation/ )
    
    FILE( GLOB src_baseclasses avfoundation/*.mm)
    FILE( GLOB header_baseclasses avfoundation/*.h)

    set( PUBLIC_HEADERS ${PUBLIC_HEADERS} ${header_baseclasses} )
    set( SRC ${SRC} ${src_baseclasses} )
    
endif()


set( ALL_FILES ${SRC} ${PUBLIC_HEADERS} ${PRIVATE_HEADERS})

add_library(${PROJECT_NAME} STATIC	${ALL_FILES})

define_source_group_base_path( ${CMAKE_CURRENT_SOURCE_DIR} ${ALL_FILES})

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "PROJECTLIBS")



if( OS_TARGET STREQUAL win )
	#dshow libs
	target_link_libraries( ${PROJECT_NAME} PUBLIC ${ARIBEIRO_LIBRARIES} strmiids)
elseif( OS_TARGET STREQUAL mac )
    target_link_libraries( ${PROJECT_NAME} PUBLIC ${ARIBEIRO_LIBRARIES}
            "-framework AVFoundation"
            "-framework CoreMedia"
            "-framework CoreVideo"
            "-framework ServiceManagement"
            "-framework Foundation"
            "-framework Cocoa"
            "-framework VideoToolbox")
else()
	target_link_libraries( ${PROJECT_NAME} PUBLIC ${ARIBEIRO_LIBRARIES})
endif()

copy_headers_to_include_directory(${PROJECT_NAME} ${PUBLIC_HEADERS})
configure_include_file( ${PROJECT_NAME} "${PROJECT_NAME}.h.in" "${PROJECT_NAME}.h" ${PUBLIC_HEADERS} )
