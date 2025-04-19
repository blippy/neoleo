cmake_minimum_required(VERSION 3.25.0)
project(neoleo)

set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fmodules-ts")
file(GLOB_RECURSE sources src/*.cc)
#add_library(lncurses++ STATIC IMPORTED)



add_executable(neoleo ${sources})
target_link_libraries(neoleo cursesw panel form)

#add_test(NAME ctime COMMAND neoleo -H <tests/ctime.scr)
#add_test(NAME fails COMMAND neoleo -T fail)
add_test(NAME pass COMMAND neoleo -T pass)
enable_testing()