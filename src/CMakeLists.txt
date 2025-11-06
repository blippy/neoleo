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

find_package(PkgConfig)
pkg_check_modules(TCL REQUIRED tcl)
include_directories(${TCL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}) # for all targets
#link_libraries(${TCL_LIBRARIES}) # required by everything

# files that we need for creating neoleo, testneo and a shared library
set(common_srcs cell.cc  oleofile.cc parser-2019.cc regions.cc sheet.cc spans.cc tickle.cc utils.cc)
#add_library(common SHARED ${common_srcs})


include(swig.cmake)


# sources exclusive to building neoleo
set(neo_srcs 
	basic.cc    blang2.cc blx.cc io-curses.cc    menu-2025.cc    win.cc
	     io-headless.cc    
	io-2019.cc        
)

add_executable(neoleo)
target_sources(neoleo PUBLIC ${neo_srcs} main.cc ${wrapper_cc})
add_dependencies(neoleo ploppy)
target_link_libraries(neoleo ploppy)
target_link_libraries(neoleo ${TCL_LIBRARIES})
#target_sources(neoleo PUBLIC ${neo_srcs} main.cc) #  Wrapper/tickleTCL_wrap.cc)
target_link_libraries(neoleo ncursesw tinfo)
install(TARGETS neoleo)




# build testneo, which you shouldn't install, as it's for testing purposes
add_executable(testneo testneo.cc)
add_dependencies(testneo ploppy)
target_link_libraries(testneo ploppy)
target_link_libraries(testneo ${TCL_LIBRARIES})
#add_dependencies(testneo common)
#target_link_libraries(testneo common)
#target_link_libraries(testneo ${TCL_LIBRARIES})
