# Additional install scripts
##---------------------------------------------------
## LTSMIN configure, build and install in CMAKE_INSTALL_PREFIX
##---------------------------------------------------

#Set root to source LTSmin
 set(MCRL2_LTSMIN_INSTALL_PATH ""
      CACHE PATH "Build and install the LTSmin toolset if path to the root to source is provided." )

 message( STATUS "MCRL2_LTSMIN_INSTALL_PATH: ${MCRL2_LTSMIN_INSTALL_PATH}" )

 if( NOT MCRL2_LTSMIN_INSTALL_PATH STREQUAL "" )
   
   # Show warning when building with shared libraries
   if( BUILD_SHARED_LIBS )
     message( STATUS "")
     message( STATUS "Warning: the mCRL2 and LTSmin builds do not always build reliably with BUILD_SHARED_LIBS=ON." )
     message( STATUS "Warning: we recommend rebuilding with BUILD_SHARED_LIBS=OFF. LTSmin will NOT be built!" )
     message( STATUS "")
   endif( BUILD_SHARED_LIBS )
   
 
   message( STATUS "Configure, build and install LTSmin toolset, after installing mCRL2" )
   configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/InstallLTSmin.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/InstallLTSmin.cmake" @ONLY  )
   install( SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/InstallLTSmin.cmake" )	 
 endif( NOT MCRL2_LTSMIN_INSTALL_PATH STREQUAL "" )

