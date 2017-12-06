###
### The mCRL2 libraries
###

set(MCRL2_LIBRARIES
  atermpp
  bes
  core
  data
  gui
  lps
  lts
  modal_formula
  pbes
  pg
  process
  smt
  trace
  utilities
)

foreach(LIB ${MCRL2_LIBRARIES})
  add_subdirectory(${LIB})
endforeach()

