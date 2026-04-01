set(edest share/neoleo/examples)
#file(GLOB egs LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/*.oleo" )
file(GLOB egs LIST_DIRECTORIES true "*.oleo" )
list(REMOVE_ITEM egs CMakeLists.txt)
#message("examples are ${egs}")

#add_custom_target(examples  ALL DEPENDS ${egs})
#install(examples ALL DESTINATION share/neoleo)
install(FILES ${egs} DESTINATION ${edest})

install(PROGRAMS cs.py bind-key1.tcl bind-key2.tcl neodsv.py 
	DESTINATION ${edest}
)


install(PROGRAMS accounts/process-1
	DESTINATION ${edest}/accounts
)
install(FILES 
	accounts/accts-plate.oleo  
	accounts/README.md  
	accounts/trans.oleo
	DESTINATION ${edest}/accounts
)

install(PROGRAMS pids/pids.tcl pids/sleepy 
	DESTINATION ${edest}/pids
)
install(FILES pids/README.md
	DESTINATION ${edest}/pids
)

install(FILES
	c++makefile/main.cc
	c++makefile/Makefile
	c++makefile/README.md
	DESTINATION ${edest}/c++makefile
)

install(FILES
	c++cmake/main.cc
	c++cmake/CMakeLists.txt
	c++cmake/README.md
	DESTINATION ${edest}/c++cmake
)