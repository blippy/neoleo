project(doc)

add_custom_target(neoleo.info ALL
	COMMAND makeinfo neoleo.texi headless.texi
)

target_sources(neoleo.info
	PRIVATE neoleo.texi headless.texi
)


#add_dependencies(neoleo.info neoleo.texi headless.texi)

#add_custom_command(
#	TARGET neoleo.info
#	POST_BUILD
#	COMMAND makeinfo neoleo.texi headless.info
#)
