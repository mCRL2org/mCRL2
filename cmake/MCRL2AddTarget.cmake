# This script provides several standard functions to add mCRL2 specific (gui) tools and libraries

# Adds a target for an mCRL2 library.
function(mcrl2_add_library TARGET_NAME)  
  set(OPTION_KW)
  set(VALUE_K)
  set(LIST_KW EXCLUDE_HEADERTEST DPARSER_SOURCES INCLUDE_DIRS SOURCES DEPENDS)
  cmake_parse_arguments("ARG" "${OPTION_KW}" "${VALUE_KW}" "${LIST_KW}" ${ARGN})

  # Finds header files, can be glob since it is only used to show headers in MSVC
  file(GLOB_RECURSE TARGET_INCLUDE_FILES "include/*.h")


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
    mcrl2_add_header_tests(${TARGET_NAME} "include" "${ARG_EXCLUDE_HEADERTEST}")
  endif()
  
  add_library(${TARGET_NAME} ${ARG_SOURCES} ${TARGET_INCLUDE_FILES})

  target_link_libraries(${TARGET_NAME} PUBLIC ${ARG_DEPENDS})
  target_include_directories(${TARGET_NAME} PUBLIC "include/" ${ARG_INCLUDE_DIRS})

  if(MCRL2_ENABLE_TESTS)
    mcrl2_add_tests(${TARGET_NAME} "test/" "librarytest")
  endif()

  # Install the header files for mCRL2 libraries
  mcrl2_install_header_files(${TARGET_INCLUDE_FILES})

  # Install the libraries only when we are using jittyc, or shared libraries.
  install(TARGETS ${TARGET_NAME}
    COMPONENT "Libraries"
    LIBRARY DESTINATION ${MCRL2_LIBRARY_PATH}
    ARCHIVE DESTINATION ${MCRL2_ARCHIVE_PATH}
    FRAMEWORK DESTINATION ${MCRL2_LIBRARY_PATH})

endfunction()

# Adds an executable target for an mCRL2 tool.
function(mcrl2_add_tool TARGET_NAME)
  set(OPTION_KW)
  set(VALUE_K)
  set(LIST_KW SOURCES DEPENDS)
  cmake_parse_arguments("ARG" "${OPTION_KW}" "${VALUE_KW}" "${LIST_KW}" ${ARGN})

  # Add application to the MCRL2_TOOLS
  get_property(MCRL2_TOOLS GLOBAL PROPERTY MCRL2_TOOLS)
  set_property(GLOBAL PROPERTY MCRL2_TOOLS "${MCRL2_TOOLS},${TARGET_NAME}")
    
  # Finds header files, can be glob since it is only used to show headers in MSVC
  file(GLOB_RECURSE TARGET_INCLUDE_FILES "*.h")

  add_executable(${TARGET_NAME} ${ARG_SOURCES} ${TARGET_INCLUDE_FILES})

  target_link_libraries(${TARGET_NAME} ${ARG_DEPENDS})
  target_include_directories(${TARGET_NAME} PUBLIC "." "include/")

  if(MCRL2_MAN_PAGES)
    mcrl2_add_man_page(${TARGET_NAME})
  endif()
  
  install(TARGETS ${TARGET_NAME}
    COMPONENT "Stable"
    RUNTIME DESTINATION ${MCRL2_RUNTIME_PATH}
    BUNDLE DESTINATION ${MCRL2_BUNDLE_PATH})

endfunction()

# Adds an executable target for an mCRL2 gui tool, which depend on Qt.
# This will do the same as mcrl_add_tool, but in addition to that also
# prepare a desktop application with desired icons.
function(mcrl2_add_gui_tool TARGET_NAME)
  set(OPTION_KW)
  set(VALUE_KW MENUNAME DESCRIPTION ICON)
  set(LIST_KW SOURCES DEPENDS RESOURCES)
  
  cmake_parse_arguments("ARG" "${OPTION_KW}" "${VALUE_KW}" "${LIST_KW}" ${ARGN})

  if(NOT MCRL2_ENABLE_GUI_TOOLS)
    return()
  endif()

  # Add application to the MCRL2_QT_APPS
  set(MCRL2_QT_APPS "${MCRL2_QT_APPS};${TARGET_NAME}" CACHE INTERNAL "")
  
  # Add application to the MCRL2_TOOLS
  get_property(MCRL2_TOOLS GLOBAL PROPERTY MCRL2_TOOLS)
  set_property(GLOBAL PROPERTY MCRL2_TOOLS "${MCRL2_TOOLS},${TARGET_NAME}")

  # Finds header files, can be glob since it is only used to show headers in MSVC
  file(GLOB_RECURSE TARGET_INCLUDE_FILES "*.h")

  # This sets version numbers and icons for the executable.
  mcrl2_add_resource_files(${TARGET_NAME} ${ARG_MENUNAME} ${ARG_DESCRIPTION} "${ARG_ICON}" ARG_SOURCES)

  add_executable(${TARGET_NAME} ${ARG_SOURCES} ${TARGET_INCLUDE_FILES})

  target_link_libraries(${TARGET_NAME} ${ARG_DEPENDS})
  target_include_directories(${TARGET_NAME} PUBLIC "." "include/")

  if(MCRL2_MAN_PAGES)
    mcrl2_add_man_page(${TARGET_NAME})
  endif()

  if(MSVC)
    # Change to WIN32 application to avoid spawning a terminal
    set_target_properties(${TARGET_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
  endif()

  # Enable the CMake build system to automatically run MOC/UIC/RCC on source files that need it.
  set_target_properties(${TARGET_NAME} PROPERTIES AUTOMOC TRUE)
  set_target_properties(${TARGET_NAME} PROPERTIES AUTOUIC TRUE)
  set_target_properties(${TARGET_NAME} PROPERTIES AUTORCC TRUE)
    
  install(TARGETS ${TARGET_NAME}
    COMPONENT "Stable"
    RUNTIME DESTINATION ${MCRL2_RUNTIME_PATH}
    BUNDLE DESTINATION ${MCRL2_BUNDLE_PATH})

  if(WIN32)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
      COMMAND ${WINDEPLOYQT_EXECUTABLE} --no-translations --no-system-d3d-compiler --no-quick-import $<TARGET_FILE:${TARGET_NAME}>
      WORKING_DIRECTORY ${MCRL2_TOOL_PATH})
  endif()

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
function(mcrl2_add_tests TARGET_NAME TEST_DIR TEST_CATEGORY)
  file(GLOB tests "${TEST_DIR}/*.cpp")

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
      LABELS ${TEST_CATEGORY}
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
        add_executable(${testname} "${CMAKE_SOURCE_DIR}/cmake/headertest.cpp")
        target_link_libraries(${testname} ${TARGET_NAME})
        set_target_properties(${testname} PROPERTIES COMPILE_DEFINITIONS "BOOST_UNITS_HEADER_NAME=${cppname}")
      endif()

    endif()
  endforeach()
endfunction()

# This function adds versions and icon meta data to the executable by adapting the source files.
# On windows this function adds a .rc file to the input source files.
function(mcrl2_add_resource_files TARGET_NAME TOOLNAME DESCRIPTION ICON SOURCE_FILES)
  if(MSVC)
    set(ORIGFILENAME ${TARGET_NAME}.exe)
    set(RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}_icon.rc)

    # Destructure the version number.
    string(SUBSTRING ${MCRL2_MAJOR_VERSION} 0 4 VERSION_HIHI)
    string(SUBSTRING ${MCRL2_MAJOR_VERSION} 4 -1 VERSION_HILO)
    string(LENGTH ${VERSION_HILO} _vlen)
    if(_vlen GREATER 4)
      string(REPLACE "." "" VERSION_HILO ${VERSION_HILO})
    else()
      string(REPLACE "." "0" VERSION_HILO ${VERSION_HILO})
    endif()
    string(REPLACE "M" "" REVISION ${MCRL2_MINOR_VERSION})

    if(NOT MCRL2_PACKAGE_RELEASE)
      set(FILEFLAGS "VS_FF_PRERELEASE")
    else()
      set(FILEFLAGS "VER_DBG")
    endif()

    set(ICOFILE ${CMAKE_SOURCE_DIR}/cmake/packaging/icons/${ICON}.ico)
    get_filename_component(ORIGFILENAME ${ORIGFILENAME} NAME)
    configure_file(${CMAKE_SOURCE_DIR}/cmake/packaging/icon.rc.in ${RC_FILE} @ONLY)

    list(APPEND ${SOURCE_FILES} ${RC_FILE})
  elseif(APPLE)
    set(ICNS_FILE ${CMAKE_SOURCE_DIR}/cmake/packaging/icons/${ICON}.icns)
    set_source_files_properties(${ICNS_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    list(APPEND ${SOURCE_FILES} ${ICNS_FILE})
  elseif(UNIX)
    # Add the desktop file
    set(DESKTOP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.desktop)
    configure_file(${CMAKE_SOURCE_DIR}/cmake/packaging/desktop.in ${DESKTOP_FILE} @ONLY)
    install(FILES ${DESKTOP_FILE} DESTINATION share/applications)
  endif()
  
  set(${SOURCE_FILES} "${${SOURCE_FILES}}" PARENT_SCOPE)
endfunction()
