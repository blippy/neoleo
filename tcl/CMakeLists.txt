#find_package(TCL)
find_package(Tclsh)

message("Tclsh_VERSION=${Tclsh_VERSION}") # This was added in cmake version 4.2
message("Tclsh_VERSION=${Tclsh_VERSION}")
message("TCLSH_VERSION_STRING=${TCLSH_VERSION_STRING}") # prior to cmake version 4.2


#message("TCL_LIBRARY=${TCL_LIBRARY}")
#message("TCL_INCLUDE_PATH=${TCL_INCLUDE_PATH}")
#message("TCL_TCLSH=${TCL_TCLSH}")