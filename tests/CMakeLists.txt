
# run when you do cmake .. :
#message("Hello from tests/CmakeLists.txt")

file(GLOB SHELLS *.sh)
file(COPY ${SHELLS}  DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB VERIF verified/*.oleo verified/*.rep)
file(COPY ${VERIF}  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/verified)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/out) 
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/diffs) 


set(NEO ${CMAKE_BINARY_DIR}/neoleo) # need to inform test where exe is

add_test(NAME basic COMMAND basic.sh)
add_test(NAME ctime COMMAND ctime.sh ${NEO})
add_test(NAME pass  COMMAND ${NEO} -T pass) # a test that always passes
add_test(NAME write COMMAND ${NEO} -s write.scr)

# Tests that always fail, so dont' run them
# (I made them just for testing purposes)
#add_test(NAME fail COMMAND ${NEO} -T fail)
