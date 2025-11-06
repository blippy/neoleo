#project(tests)
# run when you do cmake .. :
#message("Hello from tests/CmakeLists.txt. Current binary dir: ${CMAKE_CURRENT_BINARY_DIR}")

set(CTEST_OUTPUT_ON_FAILURE ON)
file(GLOB SHELLS *.sh)
file(COPY ${SHELLS}  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB SCRIPTS *.scr)
file(COPY ${SCRIPTS}  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB OTHER *.bas *.oleo *.tcl)
file(COPY ${OTHER}  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY certify differ issue25.oleo DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB VERIF verified/*.oleo verified/*.rep)
file(COPY ${VERIF}  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/verified)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/out) 
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/diffs) 


set(NEO ${CMAKE_BINARY_DIR}/src/neoleo) # need to inform test where exe is
set(NEOT ${CMAKE_BINARY_DIR}/src/testneo) # need to inform test where exe is

add_test(NAME basic 	COMMAND basic.sh)
add_test(NAME badrc 	COMMAND badrc.sh)
add_test(NAME blang 	COMMAND blang-01.sh)
add_test(NAME pass1		COMMAND ${NEOT} pass)
#add_test(NAME fail		COMMAND ${NEOT} fail) # a deliberate fail, which is generally not what you want
add_test(NAME insert 	COMMAND insert-01.sh)
add_test(NAME iss16 	COMMAND issue16.sh)
add_test(NAME iss18 	COMMAND issue18.sh)
add_test(NAME iss51 	COMMAND issue51.sh)
add_test(NAME hless 	COMMAND nohead-01.sh)
add_test(NAME parser 	COMMAND parser.sh)
add_test(NAME span 		COMMAND spans.sh)
add_test(NAME tcl1 		COMMAND tcl1.sh)
add_test(NAME tcl2 		COMMAND tcl2.sh)
#add_test(NAME tcl2 		COMMAND ${NEO} -m 0 -t tcl2.tcl)
#add_test(NAME tcl3 		COMMAND ${NEO} -m 0 sht1.oleo -t tcl3.tcl)
add_test(NAME tcl3 		COMMAND tcl3.sh)
#add_test(NAME tcl4 		COMMAND ${NEO} -m 0 sht1.oleo -t tcl4.tcl)
add_test(NAME tcl4 		COMMAND tcl4.sh)
#add_test(NAME tcl5 		COMMAND ${NEO} -m 0 -t tcl5.tcl) # test of load-oleo
add_test(NAME tcl5 		COMMAND tcl5.sh) # test of load-oleo
#add_test(NAME tcl6 		COMMAND ${NEO} -m 0 -t tcl6.tcl) # test of set-cell formula
add_test(NAME tcl6 		COMMAND tcl6.sh) # test of set-cell
add_test(NAME tcl7 		COMMAND tcl7.sh) # test of save-oleo-as
add_test(NAME writ 		COMMAND write.sh)


#add_test(NAME tcf 		COMMAND ${NEO} -m 0 -t tcl-fail.tcl) # deliberatebly engineered to fail. So don't use it normally


# Temporarily commented out because it will cause tests to hand
#add_test(NAME bug51 COMMAND ${NEO} -s issue51.scr) # 2025-04-20

# Tests that always fail, so dont' run them
# (I made them just for testing purposes)
#add_test(NAME fail COMMAND ${NEO} -T fail)

# 25/10 Tests of yesteryear
#add_test(NAME decompile COMMAND ${NEO} -s decompile-01.scr)
#add_test(NAME floor 	COMMAND ${NEO} -s floor.scr)
#add_test(NAME iss16	COMMAND ${NEO} -s issue16.scr)
#add_test(NAME iss25	COMMAND issue25.sh)
#add_test(NAME fmt  	COMMAND ${NEO} -T fmt)
#add_test(NAME pass  	COMMAND ${NEO} -T pass) # a test that always passes
