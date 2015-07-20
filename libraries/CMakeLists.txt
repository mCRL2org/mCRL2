###
### The mCRL2 libraries
###

set(MCRL2_LIBRARIES
  atermpp
  bes
  core
  data
  lps
  lts
  modal_formula
  pbes
  process
  trace
  utilities
)

foreach(LIB ${MCRL2_LIBRARIES})
  add_subdirectory(${LIB})
endforeach()

