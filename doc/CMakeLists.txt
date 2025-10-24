# 25/10 creates neoleo.info successfully. Not sure about Dir

project(doc)

find_program(MAKEINFO_EXECUTABLE makeinfo)

set(info_in 
	${CMAKE_CURRENT_SOURCE_DIR}/neoleo.texi
	#${CMAKE_CURRENT_SOURCE_DIR}/headless.texi
)
set(info_out ${CMAKE_CURRENT_BINARY_DIR}/neoleo.info)
add_custom_command(OUTPUT ${info_out}
	COMMAND ${MAKEINFO_EXECUTABLE} -o ${info_out} ${info_in}
	DEPENDS ${info_in}
	COMMENT "Creating info file $(info_out}"
	VERBATIM
)


add_custom_target(info ALL DEPENDS ${info_out})

install(FILES ${info_out} DESTINATION share/info)
