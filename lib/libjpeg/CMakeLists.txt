project ( libjpeg )
cmake_minimum_required ( VERSION 2.8.12 )

set( LIBJPEG_BUILD_EXECUTABLES OFF CACHE BOOL "Build libjpeg executables.")

include ( CheckIncludeFile )
check_include_file ( stddef.h HAVE_STDDEF_H )
check_include_file ( stdlib.h HAVE_STDLIB_H )

if ( WIN32 AND NOT CYGWIN )
    set ( TWO_FILE_COMMANDLINE true )
endif ( )

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/jconfig.h.cmake"
    "${CMAKE_HOME_DIRECTORY}/include/${projectname}/jconfig.h"
    @ONLY
)

#jconfig.h
include_directories ( ${CMAKE_HOME_DIRECTORY}/include/${projectname} )

set ( PUBLIC_HEADERS src/jerror.h src/jmorecfg.h src/jpeglib.h )

set ( SRC src/jmemnobs.c src/jaricom.c src/jcapimin.c src/jcapistd.c src/jcarith.c src/jccoefct.c src/jccolor.c 
src/jcdctmgr.c src/jchuff.c src/jcinit.c src/jcmainct.c src/jcmarker.c src/jcmaster.c src/jcomapi.c src/jcparam.c 
src/jcprepct.c src/jcsample.c src/jctrans.c src/jdapimin.c src/jdapistd.c src/jdarith.c src/jdatadst.c src/jdatasrc.c 
src/jdcoefct.c src/jdcolor.c src/jddctmgr.c src/jdhuff.c src/jdinput.c src/jdmainct.c src/jdmarker.c src/jdmaster.c 
src/jdmerge.c src/jdpostct.c src/jdsample.c src/jdtrans.c src/jerror.c src/jfdctflt.c src/jfdctfst.c src/jfdctint.c 
src/jidctflt.c src/jidctfst.c src/jidctint.c src/jquant1.c src/jquant2.c src/jutils.c src/jmemmgr.c )
  
  #cderror.h cdjpeg.h jdct.h jinclude.h jmemsys.h jpegint.h jversion.h transupp.h

add_library ( ${PROJECT_NAME} STATIC ${SRC} ${PUBLIC_HEADERS} )

# set the target's folder (for IDEs that support it, e.g. Visual Studio)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "LIBS")

if ( LIBJPEG_BUILD_EXECUTABLES )
    add_executable ( cjpeg 
        examples/cjpeg/cdjpeg.c 
        examples/cjpeg/cjpeg.c 
        examples/cjpeg/rdbmp.c 
        examples/cjpeg/rdgif.c 
        examples/cjpeg/rdppm.c 
        examples/cjpeg/rdrle.c 
        examples/cjpeg/rdtarga.c 
        examples/cjpeg/rdswitch.c )

    add_executable ( djpeg 
        examples/djpeg/cdjpeg.c 
        examples/djpeg/djpeg.c 
        examples/djpeg/wrbmp.c 
        examples/djpeg/wrgif.c 
        examples/djpeg/wrppm.c 
        examples/djpeg/wrrle.c 
        examples/djpeg/wrtarga.c 
        examples/djpeg/rdcolmap.c )

    add_executable ( jpegtran 
        examples/jpegtran/jpegtran.c 
        examples/jpegtran/cdjpeg.c 
        examples/jpegtran/rdswitch.c 
        examples/jpegtran/transupp.c )

    add_executable ( rdjpgcom examples/rdjpgcom/rdjpgcom.c )

    add_executable ( wrjpgcom examples/wrjpgcom/wrjpgcom.c )

    target_link_libraries ( cjpeg libjpeg )
    target_link_libraries ( djpeg libjpeg )
    target_link_libraries ( jpegtran libjpeg )
    target_link_libraries ( rdjpgcom libjpeg )
    target_link_libraries ( wrjpgcom libjpeg )
endif ()

# arguments for testing
# djpeg -dct int -ppm -outfile testout.ppm testorig.jpg
# djpeg -dct int -bmp -colors 256 -outfile testout.bmp testorig.jpg
# cjpeg -dct int -outfile testout.jpg testimg.ppm
# djpeg -dct int -ppm -outfile testoutp.ppm testprog.jpg
# cjpeg -dct int -progressive -opt -outfile testoutp.jpg testimg.ppm
# jpegtran -outfile testoutt.jpg testprog.jpg

copy_headers_to_include_directory(${PROJECT_NAME} ${PUBLIC_HEADERS})
