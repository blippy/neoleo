#file(GLOB_RECURSE sources src/*.cc)
project(neoleo)

set(CMAKE_BUILD_TYPE Debug)
#add_compile_definitions("NDEBUG") # debugging
set(CMAKE_VERBOSE_MAKEFILE TRUE)
#set(CMAKE_CXX_COMPILER c++)
set(CMAKE_CXX_COMPILER clang++)
#set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_CXX_MODULE_STD 1)

set(neo_srcs 
	basic.cc    io-curses.cc    menu-2025.cc  parser-2019.cc  spans.cc  win.cc
	cell.cc     io-headless.cc  mod.cc        regions.cc      tests.cc
	io-2019.cc  main.cc         oleofile.cc   sheet.cc        utils.cc
	#PARENT_SCOPE
)

add_executable(neoleo ${neo_srcs})
target_link_libraries(neoleo ncursesw tinfo)
install(TARGETS neoleo)

#target_link_libraries(neoleo ${CURSES_LIBRARIES} panel form)

#target_sources(neoleo PRIVATE src/basic.cc    src/io-curses.cc    src/menu-2025.cc  src/parser-2019.cc  src/spans.cc  src/win.cc
#src/cell.cc     src/io-headless.cc  src/mod.cc        src/regions.cc      src/tests.cc
#src/io-2019.cc  src/main.cc         src/oleofile.cc   src/sheet.cc        src/utils.cc
#)