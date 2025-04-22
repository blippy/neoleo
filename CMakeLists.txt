# cmake v3.28 allows for  C++23 import std;
cmake_minimum_required(VERSION 3.28)
project(neoleo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fmodules-ts")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
set(CMAKE_BUILD_TYPE Debug)
file(GLOB_RECURSE sources src/*.cc)

add_executable(neoleo ${sources})
target_link_libraries(neoleo cursesw panel form)

enable_testing()
add_subdirectory(tests)

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

