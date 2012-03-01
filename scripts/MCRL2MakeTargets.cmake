# Authors: Frank Stappers
# Copyright: see the accompanying file COPYING or copy at
# https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

##---------------------------------------------------
## File for additional make targets
##---------------------------------------------------

##---------------------------------------------------
## Target tags (useful editing with Vim)
##---------------------------------------------------

find_program(CTAGS
  ctags
  /usr/local/bin
  /usr/pkg/bin
  /usr/bin
)

if(CTAGS)
  add_custom_target(tags
    ${CTAGS} --languages=C,C++ --recurse=yes --extra=+q --fields=+i --totals=yes ${CMAKE_SOURCE_DIR}
  )
endif(CTAGS)

##---------------------------------------------------
## Target doxy
##---------------------------------------------------

if(UNIX)
  add_custom_target(doxy
     doc/doxy/generate_libref_website.sh
     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
endif(UNIX)

##---------------------------------------------------
## Target latex documentation
##---------------------------------------------------

if(UNIX)
  add_custom_target(tex
     doc/tex/generate_libdocs.py
     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
endif(UNIX)

##---------------------------------------------------
## Target parsers
##---------------------------------------------------

if( MCRL2_ENABLE_PARSERS  )

find_package(BISON)
find_package(FLEX)

if(FLEX_FOUND AND BISON_FOUND)

	message( STATUS "Flex version: ${FLEX_VERSION}")
	message( STATUS "Bison version: ${BISON_VERSION}")

  add_custom_target(mcrl2parser
    ${FLEX_EXECUTABLE} -Pmcrl2 -omcrl2lexer.cpp mcrl2lexer.ll
    COMMAND ${BISON_EXECUTABLE} -p mcrl2 --defines=../include/mcrl2/core/detail/mcrl2parser.h -o mcrl2parser.cpp mcrl2parser.yy
		COMMAND ${CMAKE_COMMAND} -DPARSER="mcrl2" -P ${CMAKE_SOURCE_DIR}/scripts/PostProcessParsers.cmake
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/libraries/core/source
  )

  add_custom_target(liblts_fsmparser
    ${FLEX_EXECUTABLE} -Pfsm -oliblts_fsmlexer.cpp liblts_fsmlexer.ll
    COMMAND ${BISON_EXECUTABLE} -p fsm --defines=../include/mcrl2/lts/detail/liblts_fsmparser.h -o liblts_fsmparser.cpp liblts_fsmparser.yy
		COMMAND ${CMAKE_COMMAND} -DPARSER="fsm" -P ${CMAKE_SOURCE_DIR}/scripts/PostProcessParsers.cmake
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/libraries/lts/source
  )

  add_custom_target(liblts_dotparser
    ${FLEX_EXECUTABLE} -Pdot -oliblts_dotlexer.cpp liblts_dotlexer.ll
    COMMAND ${BISON_EXECUTABLE} -p dot --defines=../include/mcrl2/lts/detail/liblts_dotparser.h -o liblts_dotparser.cpp liblts_dotparser.yy
		COMMAND ${CMAKE_COMMAND} -DPARSER="dot" -P ${CMAKE_SOURCE_DIR}/scripts/PostProcessParsers.cmake
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/libraries/lts/source
  )

  get_filename_component(FLEX_INCLUDE_DIR ${FLEX_EXECUTABLE} PATH)
  string(REPLACE "/bin" "/include" FLEX_INCLUDE_DIR ${FLEX_INCLUDE_DIR})

  add_custom_target(parsers
		 ${CMAKE_COMMAND} -E copy "${FLEX_INCLUDE_DIR}/FlexLexer.h" "${CMAKE_SOURCE_DIR}/build/workarounds"
  )
  add_dependencies( parsers mcrl2parser liblts_fsmparser liblts_dotparser )

endif(FLEX_FOUND AND BISON_FOUND)

endif( MCRL2_ENABLE_PARSERS  )

