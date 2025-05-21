find_package(PythonRequirements)

find_program(DVISVGM_EXECUTABLE dvisvgm)
if (NOT DVISVGM_EXECUTABLE)
    message(FATAL_ERROR "dvisvgm not found, cannot build documentation.")
endif()

if (MCRL2_ENABLE_DOC_DOXYGEN)
    find_package(Doxygen 1.9.6 REQUIRED)
endif()

if (MCRL2_ENABLE_DOC_PDFLATEX)
    find_package(LATEX COMPONENTS BIBTEX REQUIRED)
endif()

add_subdirectory(sphinx)
