set(edest share/neoleo/examples)
#file(GLOB egs LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/*.oleo" )
file(GLOB egs LIST_DIRECTORIES true "*.oleo" )
list(REMOVE_ITEM egs CMakeLists.txt)
#message("examples are ${egs}")

#add_custom_target(examples  ALL DEPENDS ${egs})
#install(examples ALL DESTINATION share/neoleo)
install(FILES ${egs} DESTINATION ${edest})

install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/cs.py neodsv.py neog.py 
	DESTINATION ${edest}
)


install(PROGRAMS accounts/process
	DESTINATION ${edest}/accounts
)
install(FILES accounts/accts-plate.oleo  accounts/README.md  accounts/trans.oleo
	DESTINATION ${edest}/accounts
)

install(PROGRAMS procs/neopids procs/pids.py procs/sleepy
	DESTINATION ${edest}/procs
)
install(FILES procs/pids.bas procs/youtube-script.txt
	DESTINATION ${edest}/procs
)
