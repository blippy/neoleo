# cmake v3.28 allows for  C++23 import std;
# trixie has v3.31

cmake_minimum_required(VERSION 3.25)
project(neoleo)

#set(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory(doc)
add_subdirectory(examples)
add_subdirectory(src)
enable_testing()
add_subdirectory(tests)


# Generate an archive using `cpack'
set(CPACK_PACKAGE_NAME "neoleo")
set(CPACK_PACKAGE_VERSION "17.0")
#set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_ARCHIVE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_ARCHIVE_FILE_NAME}")
set(CPACK_ARCHIVE_FILE_EXTENSION "tgz")



add_custom_target(uninstall
	COMMAND xargs rm < install_manifest.txt
)	



# 26/3 desktop file
# There's something not 100% on this. 
# https://discourse.cmake.org/t/how-to-create-pkgconfig-file/10337
# example:
# /usr/share/applications/libreoffice-calc.desktop
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/neoleo.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/neoleo.desktop USE_SOURCE_PERMISSIONS @ONLY)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/neoleo.png DESTINATION share/pixmaps)
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/neoleo.desktop DESTINATION share/applications)
install(CODE "execute_process(COMMAND desktop-file-install --dir=${CMAKE_INSTALL_PREFIX}/share/applications  ${CMAKE_CURRENT_BINARY_DIR}/neoleo.desktop)")
install(CODE "execute_process(COMMAND update-desktop-database ${CMAKE_INSTALL_PREFIX}/share/applications)")
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/docopt.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
