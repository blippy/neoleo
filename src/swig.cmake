# 25/11 added
# puzzling out the whole swig mess

find_package(SWIG REQUIRED)


# generate tickle_wrap..c into the build/src directory
set(wrapper_cc ${CMAKE_CURRENT_BINARY_DIR}/tickle_wrap.cc)
add_custom_target(swigly
	#PRE_BUILD
	COMMAND swig -c++ -tcl  -o ${wrapper_cc} ${CMAKE_CURRENT_SOURCE_DIR}/tickle.i
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tickle.i
	BYPRODUCTS ${wrapper_cc}
	VERBATIM
	USES_TERMINAL
	COMMENT "Generating swig wrapper source file"
) 

add_library(ploppy SHARED ${wrapper_cc} ${common_srcs})
#target_link_libraries(ploppy common)
install(TARGETS ploppy DESTINATION lib)
#install(CODE "message(\"Remember to run 'sudo ldconfig' to update library libploppy.so\")")
install(CODE "execute_process(COMMAND ldconfig)")


#find_package(SWIG REQUIRED)
#include(UseSWIG)
#include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}) # for all targets
#set_property(SOURCE tickle.i PROPERTY CPLUSPLUS ON)
#set(PLOP libploppy)
#add_library(ploppy_do SHARED ${common_srcs})
#swig_add_library(ploppy
#	TYPE SHARED
#	#TYPE STATIC
#	LANGUAGE  tcl
	#OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/Generated"
	#OUTFILE_DIR "${CMAKE_CURRENT_BINARY_DIR}/Wrapper"
#	SOURCES tickle.i 
#)
#swig_link_libraries(ploppy ploppy_do)
#add_library(libploppy STATIC ploppy.so)
#install(TARGETS ${PLOP} DESTINATION lib)

