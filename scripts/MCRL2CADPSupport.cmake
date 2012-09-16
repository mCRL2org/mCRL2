# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

##---------------------------------------------------
## Setup dependency to CADP
##---------------------------------------------------

message(STATUS "MCRL2_ENABLE_CADP_SUPPORT: ${MCRL2_ENABLE_CADP_SUPPORT}" )
if(MCRL2_ENABLE_CADP_SUPPORT )
  # Set location to root of CADP install path
  set(MCRL2_CADP_INSTALL_PATH ""
      CACHE PATH "Path to CADP install path." )
  message( STATUS "MCRL2_CADP_INSTALL_PATH: ${MCRL2_CADP_INSTALL_PATH}" )

  # Set location to root of CADP install path
  if( MCRL2_CADP_INSTALL_PATH )
  execute_process(COMMAND ${MCRL2_CADP_INSTALL_PATH}/com/arch
    OUTPUT_VARIABLE MCRL2_CADP_ARCHITECTURE
    OUTPUT_STRIP_TRAILING_WHITESPACE 
  )
  endif( MCRL2_CADP_INSTALL_PATH )

  if( MCRL2_CADP_ARCHITECTURE )

    message( STATUS "---------------------------------------------------------------------------")
    message( STATUS "Make sure that:")
    message( STATUS " * the build architecture matches the ${MCRL2_CADP_ARCHITECTURE}-architecture detected by CADP")                  
    message( STATUS " * the variable BUILD_SHARED_LIBS = OFF ")
    message( STATUS "---------------------------------------------------------------------------")

    set(USE_BCG "")
    add_definitions( "-DUSE_BCG" )

    set(MCRL2_CADP_BINARY_DIR  "${MCRL2_CADP_INSTALL_PATH}/bin.${MCRL2_CADP_ARCHITECTURE}" )
    set(MCRL2_CADP_INCLUDE_DIR "${MCRL2_CADP_INSTALL_PATH}/incl" )

    include_directories( ${MCRL2_CADP_INCLUDE_DIR} )	
    link_directories(${MCRL2_CADP_BINARY_DIR})
    
    add_library(imp_bcg STATIC IMPORTED)
    set_property(TARGET imp_bcg PROPERTY
                IMPORTED_LOCATION ${MCRL2_CADP_BINARY_DIR}/libBCG.a)
    add_library(imp_bcg_io STATIC IMPORTED)
    set_property(TARGET imp_bcg_io PROPERTY
    				    IMPORTED_LOCATION ${MCRL2_CADP_BINARY_DIR}/libBCG_IO.a)
    message( STATUS "CADP found: ${MCRL2_CADP_BINARY_DIR}")
 
  else( MCRL2_CADP_ARCHITECTURE  )
    message( FATAL_ERROR "CADP not found")
    set( USE_BCG "//" )
endif( MCRL2_CADP_ARCHITECTURE )

else(MCRL2_ENABLE_CADP_SUPPORT)
  set( USE_BCG "//" )
endif(MCRL2_ENABLE_CADP_SUPPORT)
