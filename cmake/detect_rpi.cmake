############################################################################
# Detect Raspberry PI environment
############################################################################
if(OS_TARGET STREQUAL linux)

    if(EXISTS "/opt/vc/include/bcm_host.h")
        MESSAGE(STATUS "bcm_host.h found")
        set( ARIBEIRO_RPI true CACHE BOOL "Set this if you are buinding for Raspberry PI" )
    endif()

    if(ARIBEIRO_RPI)
        message(STATUS "*************************************")
        message(STATUS "* Building project for Raspberry Pi *")
        message(STATUS "*************************************")

        #RPI OMX Compilation Flag: -DOMX_SKIP64BIT
        add_definitions_global(-DARIBEIRO_RPI -DOMX_SKIP64BIT)

        link_directories(/opt/vc/lib/)

        include_directories_global(
            /opt/vc/include/interface/vcos/pthreads
            /opt/vc/include/interface/vmcs_host/linux
            /opt/vc/include
        )

        set(EGL_INCLUDE_DIR /opt/vc/include)
        set(EGL_LIBRARY /opt/vc/lib/libbrcmEGL.so)
        set(GLES_INCLUDE_DIR /opt/vc/include)
        set(GLES_LIBRARY /opt/vc/lib/libbrcmGLESv2.so)

        set(SFML_OPENGL_ES 1)

        set(RPI_LIBRARIES bcm_host openmaxil vcos vchiq_arm)

        message(STATUS "[RPI Detector] Defined Libraries Variables: " )
        message(STATUS "    RPI_LIBRARIES")
        message(STATUS "    GLES_LIBRARY")
        message(STATUS "    EGL_LIBRARY")

    endif()

endif()
