#file(GLOB_RECURSE sources src/*.cc)
#project(src)

set(CMAKE_BUILD_TYPE Debug)
#add_compile_definitions("NDEBUG") # debugging
set(CMAKE_VERBOSE_MAKEFILE TRUE)

set(CMAKE_CXX_COMPILER c++)
#set(CMAKE_CXX_COMPILER clang++)
#set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_STANDARD 23)
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
	     io-headless.cc       tests.cc
	io-2019.cc           oleofile.cc   
)

add_executable(neoleo)
target_sources(neoleo PUBLIC ${neo_srcs} main.cc)
target_link_libraries(neoleo common_objs ncursesw tinfo)
install(TARGETS neoleo)



	#set(test_srcs     cell.cc parser-2019.cc regions.cc sheet.cc spans.cc utils.cc)
add_executable(testneo testneo.cc)
#target_sources(testneo PUBLIC testneo.cc) # don't install this
target_link_libraries(testneo common_objs)

#install(PROGRAMS neoleo DESTINATION bin)

#target_link_libraries(neoleo ${CURSES_LIBRARIES} panel form)

#target_sources(neoleo PRIVATE src/basic.cc    src/io-curses.cc    src/menu-2025.cc  src/parser-2019.cc  src/spans.cc  src/win.cc
#src/cell.cc     src/io-headless.cc  src/mod.cc        src/regions.cc      src/tests.cc
#src/io-2019.cc  src/main.cc         src/oleofile.cc   src/sheet.cc        src/utils.cc
#)
