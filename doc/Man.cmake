# 26/1 Created

message("Processing Man.cmake") #output when doing cmake ..

#ADD_CUSTOM_TARGET(man ALL)

#ADD_CUSTOM_COMMAND(
#	TARGET man
#	SOURCE man
#	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/neoleo.1
#)

#INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/neoleo.1 DESTINATION ${CMAKE_INSTALL_PREFIX}/man/man1)

install(FILES neoleo.1 TYPE MAN)
