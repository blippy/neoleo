#file(GLOB_RECURSE sources src/*.cc)
#project(src)

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


# files used by both neoleo and testneo
add_library(common_objs STATIC
	cell.cc parser-2019.cc regions.cc sheet.cc spans.cc utils.cc
)

# sources exclusive to building neoleo
set(neo_srcs 
	basic.cc    blang2.cc blx.cc io-curses.cc    menu-2025.cc    spans.cc  win.cc
	     io-headless.cc    
	io-2019.cc           oleofile.cc   tickle.cc
)

add_executable(neoleo)
target_sources(neoleo PUBLIC ${neo_srcs} main.cc)
target_link_libraries(neoleo common_objs ncursesw tinfo)
install(TARGETS neoleo)


find_package(PkgConfig)
pkg_check_modules(TCL REQUIRED tcl)
target_link_libraries(neoleo ${TCL_LIBRARIES})
target_include_directories(neoleo PUBLIC ${TCL_INCLUDE_DIRS})
target_compile_options(neoleo PUBLIC ${TCL_CFLAGS_OTHER})
#message("tcl inc dirs are ${TCL_INCLUDE_DIRS}")



# build testneo, which you shouldn't install, as it's for testing purposes
add_executable(testneo testneo.cc tests.cc)
target_link_libraries(testneo common_objs)
