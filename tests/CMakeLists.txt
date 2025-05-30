# run when you do cmake .. :
#message("Hello from tests/CmakeLists.txt")

set(CTEST_OUTPUT_ON_FAILURE ON)
file(GLOB SHELLS *.sh)
file(COPY ${SHELLS}  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB SCRIPTS *.scr)
file(COPY ${SCRIPTS}  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY certify differ issue25.oleo DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB VERIF verified/*.oleo verified/*.rep)
file(COPY ${VERIF}  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/verified)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/out) 
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/diffs) 


set(NEO ${CMAKE_BINARY_DIR}/neoleo) # need to inform test where exe is

add_test(NAME basic 	COMMAND basic.sh)
add_test(NAME ctime 	COMMAND ${NEO} -s ctime.scr)
add_test(NAME decompile COMMAND ${NEO} -s decompile-01.scr)
add_test(NAME floor 	COMMAND ${NEO} -s floor.scr)
add_test(NAME insert	COMMAND ${NEO} -s insert-01.scr)
add_test(NAME iss16	COMMAND ${NEO} -s issue16.scr)
add_test(NAME iss18	COMMAND ${NEO} -s issue18.scr)
add_test(NAME iss25	COMMAND issue25.sh)
add_test(NAME nohead	COMMAND ${NEO} -s nohead-01.scr)
add_test(NAME fmt  	COMMAND ${NEO} -T fmt)
add_test(NAME parser  	COMMAND ${NEO} -T parser2019)
add_test(NAME pass  	COMMAND ${NEO} -T pass) # a test that always passes
add_test(NAME spans 	COMMAND ${NEO} verified/spans.scr.oleo -s spans.scr)
add_test(NAME write 	COMMAND ${NEO} -s write.scr)


# Temporarily commented out because it will cause tests to hand
#add_test(NAME bug51 COMMAND ${NEO} -s issue51.scr) # 2025-04-20

# Tests that always fail, so dont' run them
# (I made them just for testing purposes)
#add_test(NAME fail COMMAND ${NEO} -T fail)
