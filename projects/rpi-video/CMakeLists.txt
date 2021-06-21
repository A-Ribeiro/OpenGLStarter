project (rpi-video)

FILE( GLOB src *.h *.cpp)
FILE( GLOB src_openmax openmax/*.h openmax/*.cpp)
FILE( GLOB src_v4l2 v4l2/*.h v4l2/*.cpp)

add_executable(${PROJECT_NAME}
	${src}
	${src_openmax}
	${src_v4l2}
)

source_group("" FILES ${src})
source_group("openmax" FILES ${src_openmax})
source_group("v4l2" FILES ${src_v4l2})

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "RPI")

target_link_libraries( ${PROJECT_NAME} ${ARIBEIRO_LIBRARIES} )

