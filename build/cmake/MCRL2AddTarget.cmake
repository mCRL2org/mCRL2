# This script provides several standard functions to add mCRL2 specific (gui) tools and libraries

# Adds a target for an mCRL2 library.
function(mcrl2_add_library TARGET_NAME)  
  set(OPTION_KW)
  set(VALUE_K)
  set(LIST_KW EXCLUDE_HEADERTEST DPARSER_SOURCES SOURCES DEPENDS RESOURCES)
  cmake_parse_arguments("ARG" "${OPTION_KW}" "${VALUE_KW}" "${LIST_KW}" ${ARGN})

  # Finds header files, can be glob since it is only used to show headers in MSVC
  file(GLOB_RECURSE TARGET_INCLUDE_FILES "include/*.h")

  # Install the header files for mCRL2 libraries
  mcrl2_install_header_files(${TARGET_INCLUDE_FILES})

  # For dparser grammer files we need to generate a .c file using make_dparser, and include that file as dependency.
  foreach(GRAMMER_FILE ${ARG_DPARSER_SOURCES})

    # Get the absolute path to be able to run make_dparser    
    get_filename_component(GRAMMER_ABS ${GRAMMER_FILE} ABSOLUTE)

    # Determine the output file, and grammar identifier
    get_filename_component(GRAMMER_NAME ${GRAMMER_FILE} NAME_WE)
    
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/source)
    set(OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/source/${GRAMMER_NAME}.c)
    string(REGEX REPLACE "(.*)_syntax" "\\1" GRAMMAR_IDENT ${GRAMMER_NAME})

    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND make_dparser -A -H0 -i${GRAMMAR_IDENT} -o${OUTPUT_FILE} ${GRAMMER_ABS}
        DEPENDS make_dparser ${GRAMMER_ABS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating ${GRAMMAR_IDENT} parser"
      )

    add_custom_target(make_${GRAMMER_NAME} SOURCES ${GRAMMER_FILE})

    # include the generated syntax file into the source for the library, and depend on this target
    list(APPEND ARG_SOURCES ${OUTPUT_FILE})
    list(APPEND DEPENDS make_${GRAMMER_NAME})
  endforeach()
  
  if(MCRL2_ENABLE_HEADER_TESTS)
    mcrl2_add_header_tests(${TARGET_NAME} "include" ${EXCLUDE_HEADERTEST})
  endif()
  
  add_library(${TARGET_NAME} ${ARG_SOURCES} ${TARGET_INCLUDE_FILES})

  target_link_libraries(${TARGET_NAME} ${ARG_DEPENDS})
  target_include_directories(${TARGET_NAME} PUBLIC "include/")

endfunction()

# Adds an executable target for an mCRL2 tool.
function(mcrl2_add_tool TARGET_NAME)

  # Add application to the MCRL2_TOOLS
  get_property(MCRL2_TOOLS GLOBAL PROPERTY MCRL2_TOOLS)
  set_property(GLOBAL PROPERTY MCRL2_TOOLS "${MCRL2_TOOLS},${TARGET_NAME}")
    
  if(MCRL2_MAN_PAGES)
    mcrl2_add_man_page(${TARGET_NAME})
  endif()

endfunction()

# Adds an executable target for an mCRL2 gui tool, which depend on Qt.
# This will do the same as mcrl_add_tool, but in addition to that also
# prepare a desktop application with desired icons.
function(mcrl2_add_gui_tool TARGET_NAME)

  if(MCRL2_MAN_PAGES)
    mcrl2_add_man_page(${TARGET_NAME})
  endif()

  if(MSVC)
    # Change to WIN32 application to avoid spawning a terminal
    set_target_properties(${TARGET_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
  elseif(UNIX AND NOT APPLE)
    # Add the desktop file
    set(DESKTOP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.desktop)
    configure_file(${CMAKE_SOURCE_DIR}/build/packaging/desktop.in ${DESKTOP_FILE} @ONLY)
    install(FILES ${DESKTOP_FILE} DESTINATION share/applications)
  endif()

  # Add application to the MCRL2_QT_APPS
  set(MCRL2_QT_APPS "${MCRL2_QT_APPS};${TARGET_NAME}" CACHE INTERNAL "")
  
  # Add application to the MCRL2_TOOLS
  get_property(MCRL2_TOOLS GLOBAL PROPERTY MCRL2_TOOLS)
  set_property(GLOBAL PROPERTY MCRL2_TOOLS "${MCRL2_TOOLS},${TARGET_NAME}")

  # Enable the CMake build system to automatically run MOC/UIC/RCC on source files that need it.
  set_target_properties(${TARGET_NAME} PROPERTIES AUTOMOC TRUE)
  set_target_properties(${TARGET_NAME} PROPERTIES AUTOUIC TRUE)
  set_target_properties(${TARGET_NAME} PROPERTIES AUTORCC TRUE)
    
endfunction()

# Install the given headers into the MCRL2_INCLUDE_PATH
function(mcrl2_install_header_files)
  foreach(HEADER ${ARGN})
    get_filename_component(HEADER_ABS "${HEADER}" ABSOLUTE)
    file(RELATIVE_PATH RELPATH "${CMAKE_CURRENT_SOURCE_DIR}/include" "${HEADER_ABS}")
    get_filename_component(RELPATH ${RELPATH} PATH)
    install(FILES ${HEADER} DESTINATION ${MCRL2_INCLUDE_PATH}/${RELPATH} COMPONENT Headers)
  endforeach()
endfunction()

# Call the tool to generate a man page and install it.
function(mcrl2_add_man_page TARGET_NAME)
  add_custom_command(
    TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${TARGET_NAME} --generate-man-page | gzip --best > ${TARGET_NAME}.1.gz
    COMMENT "Generating man page for ${TARGET_NAME}"
    USES_TERMINAL
  )
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.1.gz DESTINATION share/man/man1 COMPONENT Documentation)
endfunction()

# Finds all .cpp files in the test/ subdirectory and generates one test executable for each that
# is subsequently added as a test executed in the Testing directory.
function(mcrl2_add_tests TARGET_NAME TEST_CATEGORY)
  file(GLOB tests "test/*.cpp")

  foreach(test ${tests})

    get_filename_component(base ${test} NAME_WE)
    set(testname ${TEST_CATEGORY}_${TARGET_NAME}_${base})

    # Add a compilation target for test
    add_executable(${testname} ${test})
    set_target_properties(${testname} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_CATEGORY})
    target_link_libraries(${testname} ${TARGET_NAME})

    # Execute the test with ctest
    if(CMAKE_CONFIGURATION_TYPES)
      add_test(NAME ${testname} 
        COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_CATEGORY}/$<CONFIG>/${testname}" 
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/Testing/")
    else()
      add_test(NAME ${testname} 
        COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_CATEGORY}/${testname}" 
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/Testing/")
    endif()

    # Some tests need jittyc so ensure that the compile rewriter script is available.
    set_tests_properties(${testname} PROPERTIES
      LABELS ${category}
      ENVIRONMENT "MCRL2_COMPILEREWRITER=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mcrl2compilerewriter")
  endforeach()  
endfunction()

# Finds all header files in the include/ directory and generates a test consisting of just
# compiling with that file included.
function(mcrl2_add_header_tests TARGET_NAME INCLUDE_DIR EXCLUDE_FILES)
  file(GLOB_RECURSE headers "${INCLUDE_DIR}/*.h")
  
  foreach(header ${headers})
    file(RELATIVE_PATH cppname ${CMAKE_CURRENT_SOURCE_DIR}/include ${header})
    file(TO_CMAKE_PATH ${cppname} cppname)
    list(FIND EXCLUDE_FILES ${cppname} found)
    if(${found} LESS 0)
      string(REGEX REPLACE "\\.|/" "_" testname "${cppname}")
      set(testname headertest_${testname})

      if(NOT TARGET ${testname})
        # In test.cpp we define BOOST_UNITS_HEADER_NAME to be the current header
        add_executable(${testname} EXCLUDE_FROM_ALL "${CMAKE_SOURCE_DIR}/build/cmake/test.cpp")
        target_link_libraries(${testname} ${TARGET_NAME})
        set_target_properties(${testname} PROPERTIES COMPILE_DEFINITIONS "BOOST_UNITS_HEADER_NAME=${cppname}")

        # Build the resulting executable, this should succeed for the test to pass
        add_test(NAME ${testname} COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${testname})
        set_tests_properties(${testname} PROPERTIES LABELS headertest)
      endif()

    endif()
  endforeach()
endfunction()
