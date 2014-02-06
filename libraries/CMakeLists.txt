###
### The mCRL2 libraries
###
#
# Every mCRL2 library has the same structure. This is enforced by the build system. To add a new
# mCRL2 library called <NAME>, do the following (paths are to be read as relative to the repository
# root):
#
# - Use the following directory structure:
#      /libraries
#        /<NAME>
#          CMakeLists.txt
#          /source
#            sources.cpp
#          /include
#            /mcrl2
#              /<NAME>
#                includes.h
#
# - In the CMakeLists.txt, add a call to
#
#     mcrl2_library(<NAME>
#       [SOURCES <SRC1 ... SRCN>]
#       [DEPENDS <LIB1 [HEADER_ONLY] ... LIBN [HEADER_ONLY]>]
#       [INCLUDE <DIR1 ... DIRN>])
#
#   This function creates a target mcrl2_<NAME> that represents the (shared) library.
#   SRC1 ... SRCN are paths of source files to compile, relative to the source directory of
#   the library. Source files may be .c, .cpp or .g (a DParser grammar).
#   LIB1 ... LIBN are the libraries you want to use. When linking to libraries with a name that
#   matches mcrl2_*, the appropriate include directory is automatically added. If HEADER_ONLY is
#   specified, then the library is not linked, but only the include directory is added (this only
#   makes sense for mcrl2_* libraries).
#   DIR1 ... DIRN are the additional include directories you need for the library. This path should
#   only contain include paths for external libraries (not in the repository). DParser headers are
#   included automatically when a grammar file is in the source list.
#
# - Add <NAME> to the MCRL2_LIBRARIES:

set(MCRL2_LIBRARIES
  atermpp
  bes
  core
  data
  lps
  lts
  modal_formula
  svc
  pbes
  process
  trace
  utilities
)

#
# That's it! From here on, you find the implementation.
#
foreach(LIB ${MCRL2_LIBRARIES})
  add_subdirectory(${LIB})
endforeach()
