#cmake_minimum_required(VERSION 3.25.0)
#project(neoleo)

# run when you do cmake .. :
message("Hello from tests/CmakeLists.txt")

#enable_testing()
file(GLOB *.sh)
#set_property(WORKING_DIRECTORY TEST ${CMAKE_CURRENT_LIST_DIR})
#set_property(DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
#set_tests_properties( basic1 DIRECTORY tests)

macro(tst name cmd)
	add_test(NAME ${name} 
		COMMAND ${CMAKE_CURRENT_LIST_DIR}/${cmd}
		WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
endmacro()

tst(basic1, basic.sh hello)
#add_test(NAME basic1 COMMAND basic.sh)
#add_test(NAME basic COMMAND ${CMAKE_CURRENT_LIST_DIR}/basic.sh 
#	WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})


