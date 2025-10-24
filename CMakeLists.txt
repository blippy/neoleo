# cmake v3.28 allows for  C++23 import std;
# trixie has v3.31

cmake_minimum_required(VERSION 3.25)
project(neoleo)
add_subdirectory(doc)

#set(CMAKE_BUILD_TYPE Debug)
#add_compile_definitions("NDEBUG") # debugging
#set(CMAKE_VERBOSE_MAKEFILE TRUE)
#set(CMAKE_CXX_COMPILER c++)
#set(CMAKE_CXX_COMPILER clang++)
#set(CMAKE_CXX_COMPILER g++)
#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_CXX_MODULE_STD 1)

#set(CMAKE_CXX_FLAGS "-fmodules -fsearch-include-path bits/std.cc")
#set(CMAKE_CXX_FLAGS "-fmodules")


#file(GLOB_RECURSE sources src/*.cc)
#target_sources(neoleo PRIVATE src/basic.cc    src/io-curses.cc    src/menu-2025.cc  src/parser-2019.cc  src/spans.cc  src/win.cc
#src/cell.cc     src/io-headless.cc  src/mod.cc        src/regions.cc      src/tests.cc
#src/io-2019.cc  src/main.cc         src/oleofile.cc   src/sheet.cc        src/utils.cc
#)
#add_subdirectory(src)

#add_executable(neoleo ${sources})
#find_package(Curses REQUIRED)
#target_link_libraries(neoleo ncursesw tinfo)
#target_link_libraries(neoleo ${CURSES_LIBRARIES} panel form)

#enable_testing()
#add_subdirectory(tests)

# Generate an archive using `cpack'
set(CPACK_PACKAGE_NAME "neoleo")
set(CPACK_PACKAGE_VERSION "16.0")
#set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_ARCHIVE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_ARCHIVE_FILE_NAME}")
set(CPACK_ARCHIVE_FILE_EXTENSION "tgz")
#install(DIRECTORY src DESTINATION .)
#set(CPACK_SOURCE_IGNORE_FILES
#	\\.git/
#	build/
#)
#include(CPack)

