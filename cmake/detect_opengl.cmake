############################################################################
# OpenGL include directories and libs
############################################################################

find_package(OpenGL REQUIRED)

include_directories_global(${OPENGL_INCLUDE_DIR})

message(STATUS "[OpenGL Detector] Defined Libraries Variables: " )
message(STATUS "    OPENGL_LIBRARIES")
