cmake_minimum_required(VERSION 3.25)
project(app)

set(CMAKE_CXX_STANDARD 23)

find_package(PkgConfig REQUIRED)
pkg_check_modules(NEOLEO REQUIRED neoleo)

message("libraries are ${NEOLEO_LIBRARIES}")

add_executable(app main.cc)
target_link_libraries(app ${NEOLEO_LIBRARIES})
