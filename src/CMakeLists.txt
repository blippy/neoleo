#file(GLOB_RECURSE sources src/*.cc)
#project(src)

set(CMAKE_VERBOSE_MAKEFILE ON)

#include(Sanitise.cmake)

set(CMAKE_BUILD_TYPE Debug)
#add_compile_definitions("NDEBUG") # debugging
set(CMAKE_VERBOSE_MAKEFILE TRUE)

set(CMAKE_CXX_COMPILER c++)
#set(CMAKE_CXX_COMPILER clang++)
#set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_CXX_MODULE_STD 1)
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
set(CMAKE_BUILD_TYPE Debug)

# files that we need for creating neoleo, testneo and a shared library
set(common_srcs cell.cc  oleofile.cc parser-2019.cc regions.cc sheet.cc spans.cc tickle.cc utils.cc)


find_package(PkgConfig)
pkg_check_modules(TCL REQUIRED tcl)
include_directories(AFTER ${TCL_INCLUDE_DIRS}) # for all targets
link_libraries(${TCL_LIBRARIES}) # required by everything

find_package(SWIG REQUIRED)
include(UseSWIG)
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}) # for all targets
set_property(SOURCE tickle.i PROPERTY CPLUSPLUS ON)
swig_add_library(ploppy
	TYPE SHARED
	#TYPE STATIC
	LANGUAGE  tcl
	#OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/Generated"
	#OUTFILE_DIR "${CMAKE_CURRENT_BINARY_DIR}/Wrapper"
	SOURCES tickle.i ${common_srcs}
)

#add_custom_command(
#	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tickle_wrap.cxx
#	COMMAND  swig -c++ -tcl -o ${CMAKE_CURRENT_BINARY_DIR}/tickle_wrap.cxx ${CMAKE_CURRENT_SOURCE_DIR}/tickle.i 
#	DEPENDS tickle.i
#	COMMENT "Generate swig wrapper code"
#)


# files used by both neoleo and testneo
#add_library(common_objs STATIC ${common_srcs})

#add_library(oleo SHARED ${common_srcs})
#target_link_libraries(oleo)
#target_link_libraries(oleo PUBLIC  ploppy)
#install(TARGETS oleo) # We'll want this
#target_include_directories(oleo PUBLIC ${TCL_INCLUDE_DIRS})

#target_compile_options(oleo PRIVATE "-fsanitize=address")
#target_link_libraries(oleo PRIVATE "-fsanitize=address")


# sources exclusive to building neoleo
set(neo_srcs 
	basic.cc    blang2.cc blx.cc io-curses.cc    menu-2025.cc    spans.cc  win.cc
	     io-headless.cc    
	io-2019.cc          
)

add_executable(neoleo)
target_sources(neoleo PUBLIC ${neo_srcs} main.cc) #  Wrapper/tickleTCL_wrap.cc)
target_link_libraries(neoleo ploppy ncursesw tinfo dl)
install(TARGETS neoleo)


#target_link_libraries(neoleo ${TCL_LIBRARIES})
#target_include_directories(neoleo PUBLIC ${TCL_INCLUDE_DIRS})
#target_compile_options(neoleo PUBLIC ${TCL_CFLAGS_OTHER})



# build testneo, which you shouldn't install, as it's for testing purposes
add_executable(testneo testneo.cc)
target_link_libraries(testneo ploppy)
