# cmake v3.28 allows for  C++23 import std;
# trixie has v3.31

cmake_minimum_required(VERSION 3.25)
project(neoleo)



add_subdirectory(doc)
add_subdirectory(examples)
add_subdirectory(src)
enable_testing()
add_subdirectory(tests)


# Generate an archive using `cpack'
set(CPACK_PACKAGE_NAME "neoleo")
set(CPACK_PACKAGE_VERSION "16.0")
#set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_ARCHIVE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_ARCHIVE_FILE_NAME}")
set(CPACK_ARCHIVE_FILE_EXTENSION "tgz")
#install(DIRECTORY src DESTINATION .)
#set(CPACK_SOURCE_IGNORE_FILES
#	\\.git/
#	build/
#)
#include(CPack)


add_custom_target(uninstall
	COMMAND xargs rm < install_manifest.txt
)	
