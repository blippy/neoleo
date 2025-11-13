# 25/11 added
# puzzling out the whole swig mess

# NB A custom target is always considered to be out of date, so its command always run.
# However, that does not extend to its dependencies. You should think of custom target as a .PHONY make rule ... it's there to help you sequence things

find_package(SWIG REQUIRED)


# generate tickle_wrap..c into the build/src directory
set(wrapper_cc ${CMAKE_CURRENT_BINARY_DIR}/tickle_wrap.cc)
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tickle_wrap.o
	COMMAND swig -c++ -tcl  -o ${wrapper_cc} ${CMAKE_CURRENT_SOURCE_DIR}/tickle.i
	COMMAND c++ -I${TCL_INCLUDE_DIRS} -I${CMAKE_CURRENT_SOURCE_DIR}  -c 	${wrapper_cc}
	BYPRODUCTS ${wrapper_cc} # needed??
	COMMENT "Generating swig wrapper"
)

add_custom_target(obj_wrapper
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/tickle_wrap.o
)
#target_include_directories(obj_wrapper PUBLIC ${TCL_INCLUDE_DIRS})
#add_custom_target(swigly
#	#PRE_BUILD
#	COMMAND swig -c++ -tcl  -o ${wrapper_cc} ${CMAKE_CURRENT_SOURCE_DIR}/tickle.i
#	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tickle.i
#	BYPRODUCTS ${wrapper_cc}
#	VERBATIM
#	USES_TERMINAL
#	COMMENT "Generating swig wrapper source file"
#) 

#add_library(ploppy SHARED ${wrapper_cc} ${common_srcs})
add_library(ploppy SHARED ${common_srcs}  ${CMAKE_CURRENT_BINARY_DIR}/tickle_wrap.o)
add_dependencies(ploppy obj_wrapper)
#target_link_libraries(ploppy common)
install(TARGETS ploppy DESTINATION lib)
set(libloc ${CMAKE_INSTALL_PREFIX}/lib) # different distros have different dirs
#set(libloc ${CMAKE_INSTALL_PREFIX}/lib64/libploppy.so) # different distros have different dirs
install(CODE "message(\"Updating library cache ${libloc}/libploppy.so\")")
install(CODE "message(\"Set LD_LIBRARY_PATH if necessary.\")")
#install(CODE "execute_process(COMMAND ldconfig -l ${libloc})")
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

