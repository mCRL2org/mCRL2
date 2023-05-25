find_package(PythonModules COMPONENTS sphinx REQUIRED)

if (MCRL2_ENABLE_DOC_DOXYGEN)
    find_package(Doxygen REQUIRED)
endif()

if (MCRL2_ENABLE_DOC_PDFLATEX)
    find_package(LATEX COMPONENTS BIBTEX REQUIRED)
endif()

add_subdirectory(sphinx)