#find_package(TCL REQUIRED)
#find_package(Tclsh REQUIRED)

#message("Tclsh_VERSION=${Tclsh_VERSION}") # This was added in cmake version 4.2
#message("Tclsh_VERSION=${Tclsh_VERSION}")
#message("TCLSH_VERSION_STRING=${TCLSH_VERSION_STRING}") # prior to cmake version 4.2

#string(REGEX REPLACE "\\..*" ""  TCL_MAJOR_VERSION ${TCLSH_VERSION_STRING})
#message("TCL_MINOR_VERSION=${TCL_MINOR_VERSION}")
#set(TCL_DEST_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/lib/tcl${TCL_MAJOR_VERSION}/${TCLSH_VERSION_STRING})
#message("Tcl module path set to:  ${TCL_DEST_MODULE_PATH} . Set TCLLIBPATH if necessary")

#message("TCL_LIBRARY=${TCL_LIBRARY}")
#message("TCL_INCLUDE_PATH=${TCL_INCLUDE_PATH}")
#message("TCL_TCLSH=${TCL_TCLSH}")
#install(FILES oleo.tcl pkgIndex.tcl DESTINATION lib/tcl${TCL_MAJOR_VERSION}/${TCLSH_VERSION_STRING})
message("TCLLIBPATH may need the path  ${CMAKE_INSTALL_PREFIX}/lib")
install(FILES oleo.tcl pkgIndex.tcl DESTINATION lib/oleo)