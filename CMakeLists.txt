# cmake v3.28 allows for  C++23 import std;
cmake_minimum_required(VERSION 3.25)
#cmake_minimum_required(VERSION 4.0)
project(neoleo)

set(CMAKE_BUILD_TYPE Debug)
#add_compile_definitions("NDEBUG") # debugging
set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_CXX_COMPILER c++)
set(CMAKE_CXX_COMPILER clang++)
#set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_MODULE_STD 1)

#set(CMAKE_CXX_FLAGS "-fmodules -fsearch-include-path bits/std.cc")
#set(CMAKE_CXX_FLAGS "-fmodules")

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
set(CMAKE_BUILD_TYPE Debug)
file(GLOB_RECURSE sources src/*.cc)

add_executable(neoleo ${sources})
find_package(Curses REQUIRED)
target_link_libraries(neoleo ncursesw tinfo)
#target_link_libraries(neoleo ${CURSES_LIBRARIES} panel form)

enable_testing()
#add_subdirectory(tests)

# Generate an archive using `cpack'
set(CPACK_PACKAGE_NAME "neoleo")
set(CPACK_PACKAGE_VERSION "16.0")
#set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_ARCHIVE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_ARCHIVE_FILE_NAME}")
set(CPACK_ARCHIVE_FILE_EXTENSION "tgz")
install(DIRECTORY src DESTINATION .)
#set(CPACK_SOURCE_IGNORE_FILES
#	\\.git/
#	build/
#)
include(CPack)

