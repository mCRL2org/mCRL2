add_mcrl2_library(svc
  INSTALL_HEADERS FALSE
  SOURCES
    huffman.cpp
    lz.cpp
    compress.cpp
    blocklist.cpp
    code.cpp
    ptable.cpp
    svc1.cpp
    svc2.cpp
    hashtable.cpp
  DEPENDS
    mcrl2_atermpp
    mcrl2_utilities
)

