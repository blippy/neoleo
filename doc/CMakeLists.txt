# 25/10 creates neoleo.info successfully. Not sure about Dir

project(doc)

find_program(MAKEINFO_EXECUTABLE makeinfo)


#################################################################
# build neoleo.info

set(info_in 
	${CMAKE_CURRENT_SOURCE_DIR}/neoleo.texi
	#${CMAKE_CURRENT_SOURCE_DIR}/headless.texi
)
set(p "${CMAKE_INSTALL_PREFIX}")
set(info_out ${CMAKE_CURRENT_BINARY_DIR}/neoleo.info)
add_custom_command(OUTPUT ${info_out}
	COMMAND ${MAKEINFO_EXECUTABLE} -D "PREFIX ${p}" -o ${info_out} ${info_in}
	DEPENDS ${info_in}
	COMMENT "Creating info file ${info_out} with ${p}"
	VERBATIM
)


add_custom_target(info ALL DEPENDS ${info_out})

install(FILES ${info_out} DESTINATION share/info)


#################################################################
# build html files

set(H ${CMAKE_CURRENT_BINARY_DIR}/html)
set(html_out 
	${H}/index.html
	${H}/Command-Index.html
	${H}/Function-Index.html
	${H}/Options-Index.html
	${H}/Concept-Index.html  
	${H}/Headless-mode.html
	${H}/Key-Index.html

)
add_custom_command(OUTPUT ${html_out}
	COMMAND ${MAKEINFO_EXECUTABLE} --html  -D "PREFIX ${p}" -o html ${info_in}
        DEPENDS ${info_in}
        COMMENT "Creating html files"
        VERBATIM
)

add_custom_target(html ALL DEPENDS ${html_out})
install(FILES ${html_out}  DESTINATION share/neoleo/html)
