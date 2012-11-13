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
