# 25/11 added
# puzzling out the whole swig mess

# NB A custom target is always considered to be out of date, so its command always run.
# However, that does not extend to its dependencies. You should think of custom target as a .PHONY make rule ... it's there to help you sequence things

find_package(SWIG REQUIRED)
include(UseSWIG)

#set(CMAKE_VERBOSE_MAKEFILE ON)
message("CMAKE_CURRENT_BINARY_DIR : ${CMAKE_CURRENT_BINARY_DIR}")

# generate tickle_wrap..c into the build/src directory
set(wrapper_cc ${CMAKE_CURRENT_BINARY_DIR}/tickle_wrap.cc)
add_custom_target(tickle_wrap_cc DEPENDS ${wrapper_cc})
add_custom_command(
	OUTPUT ${wrapper_cc}
	#COMMAND c++ -I${TCL_INCLUDE_DIRS} -I${CMAKE_CURRENT_SOURCE_DIR} -c 	${wrapper_cc}
	COMMAND swig -c++ -tcl  -o ${wrapper_cc} ${CMAKE_CURRENT_SOURCE_DIR}/tickle.i
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tickle.i
	COMMENT "Generating tickle_wrap.cc"
)
	
#set(wrapper_o 	${CMAKE_CURRENT_BINARY_DIR}/tickle_wrap.o)
#add_custom_target(tickle_wrap_o DEPENDS ${wrapper_cc}) 
#add_custom_command(
#	OUTPUT ${wrapper_o}
#	#COMMAND swig -c++ -tcl  -o ${wrapper_cc} ${CMAKE_CURRENT_SOURCE_DIR}/tickle.i
#	#COMMAND c++ -I${TCL_INCLUDE_DIRS} -I${CMAKE_CURRENT_SOURCE_DIR} -c 	${wrapper_cc}
#	COMMAND c++  -I${TCL_INCLUDE_DIRS} -I${CMAKE_CURRENT_SOURCE_DIR}  -c 	${wrapper_cc}
#	#COMMAND c++  -I${TCL_INCLUDE_DIRS}  -c 	${wrapper_cc}
#	#BYPRODUCTS ${wrapper_cc} # needed??
#	COMMENT "Generating swig $(wrapper_o}"
#)

#add_custom_target(obj_wrapper
#	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/tickle_wrap.o
#)

set(libloc ${CMAKE_INSTALL_PREFIX}/lib) # different distros have different dirs
install(CODE "message(\"Updating library cache ${libloc}/libploppy.so\")")
install(CODE "message(\"Set LD_LIBRARY_PATH if necessary.\")")
#install(CODE "execute_process(COMMAND ldconfig -l ${libloc})")
install(CODE "execute_process(COMMAND ldconfig)")

