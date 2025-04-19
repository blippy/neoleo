cmake_minimum_required(VERSION 3.25.0)
project(neoleo)

set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fmodules-ts")
file(GLOB_RECURSE sources src/*.cc)
#add_library(lncurses++ STATIC IMPORTED)



add_executable(neoleo ${sources})
target_link_libraries(neoleo cursesw panel form)