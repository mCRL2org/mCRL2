# Additional install scripts
##---------------------------------------------------
## LTSMIN configure, build and install in CMAKE_INSTALL_PREFIX
##---------------------------------------------------

#Set root to source LTSmin
 set(MCRL2_LTSMIN_INSTALL_PATH ""
      CACHE PATH "Build and install the LTSmin toolset if path to the root to source is provided." )

 message( STATUS "MCRL2_LTSMIN_INSTALL_PATH: ${MCRL2_LTSMIN_INSTALL_PATH}" )

 if( NOT MCRL2_LTSMIN_INSTALL_PATH STREQUAL "" )
	 message( STATUS "Configure, build and install LTSmin toolset, after installing mCRL2" )
   configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/InstallLTSmin.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/InstallLTSmin.cmake" @ONLY  )
   install( SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/InstallLTSmin.cmake" )	 
 endif( NOT MCRL2_LTSMIN_INSTALL_PATH STREQUAL "" )

