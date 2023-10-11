include(CMakeParseArguments)

find_package(Threads)

macro(append_unique LIST_VAR VALUE)
  list(FIND ${LIST_VAR} ${VALUE} _index)
  if(_index EQUAL -1)
    list(APPEND ${LIST_VAR} "${VALUE}")
  endif()
endmacro()

function(_add_library_tests TARGET_NAME)
  file(GLOB librarytest "test/*.cpp")

  if(MCRL2_SKIP_LONG_TESTS)
    add_definitions(-DMCRL2_SKIP_LONG_TESTS)
  endif(MCRL2_SKIP_LONG_TESTS)

  foreach(category "librarytest")
    foreach(test IN LISTS ${category})

      get_filename_component(base ${test} NAME_WE)
      set(testname ${category}_${TARGET_NAME}_${base})

      add_executable(${testname} ${test})
      set_target_properties(${testname} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${category})
      target_link_libraries(${testname} ${TARGET_NAME})

      if(MCRL2_TEST_JITTYC)
        target_compile_definitions(${testname} PUBLIC -DMCRL2_TEST_JITTYC)
      endif()

      if(CMAKE_CONFIGURATION_TYPES)
        add_test(NAME ${testname} COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${category}/$<CONFIG>/${testname}")
      else()
        add_test(NAME ${testname} COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${category}/${testname}")
      endif()

      target_link_libraries(${testname} Threads::Threads)

      set_tests_properties(${testname} PROPERTIES
        LABELS ${category}
        ENVIRONMENT "MCRL2_COMPILEREWRITER=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mcrl2compilerewriter")
    endforeach()
  endforeach()
endfunction()

function(_add_header_tests TARGET_NAME EXCLUDE_FILES)
  file(GLOB_RECURSE headers "include/*.h")
  foreach(header ${headers})
    file(RELATIVE_PATH cppname ${CMAKE_CURRENT_SOURCE_DIR}/include ${header})
    file(TO_CMAKE_PATH ${cppname} cppname)
    list(FIND EXCLUDE_FILES ${cppname} found)
    if(${found} LESS 0)
      string(REGEX REPLACE "\\.|/" "_" testname "${cppname}")
      set(testname headertest_${testname})
      if(NOT TARGET ${testname})
        add_executable(${testname} EXCLUDE_FROM_ALL "${CMAKE_SOURCE_DIR}/build/cmake/test.cpp")
        target_link_libraries(${testname} ${TARGET_NAME})
        set_target_properties(${testname} PROPERTIES COMPILE_DEFINITIONS "BOOST_UNITS_HEADER_NAME=${cppname}")
        add_test(NAME ${testname} COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${testname})
        set_tests_properties(${testname} PROPERTIES LABELS headertest)
      endif()
    endif()
  endforeach()
endfunction()

function(_add_resource_files TARGET_NAME TOOLNAME DESCRIPTION ICON SOURCE_FILES)
  if(MSVC)
    if(NOT ICON)
      set(ICON "mcrl2-blue")
    endif()
    if(NOT TOOLNAME)
      set(TOOLNAME ${TARGET_NAME})
      set(DESCRIPTION ${TARGET_NAME})
    endif()
    set(ORIGFILENAME ${TARGET_NAME}.exe)
    set(RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}_icon.rc)

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
    set(ICOFILE ${CMAKE_SOURCE_DIR}/build/packaging/icons/${ICON}.ico)
    get_filename_component(ORIGFILENAME ${ORIGFILENAME} NAME)
    configure_file(${CMAKE_SOURCE_DIR}/build/packaging/icon.rc.in ${RC_FILE} @ONLY)
    set(${SOURCE_FILES} ${${SOURCE_FILES}} ${RC_FILE})
  elseif(APPLE)
    set(ICNS_FILE ${CMAKE_SOURCE_DIR}/build/packaging/icons/${ICON}.icns)
    set_source_files_properties(${ICNS_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(${SOURCE_FILES} ${${SOURCE_FILES}} ${ICNS_FILE})
  endif()
  set(${SOURCE_FILES} "${${SOURCE_FILES}}" PARENT_SCOPE)
endfunction()

function(_prepare_desktop_application TARGET_NAME TOOLNAME DESCRIPTION ICON)
  if(MSVC)
    set_target_properties(${TARGET_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
  elseif(UNIX AND NOT APPLE)
    set(DESKTOP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.desktop)
    set(COMMANDLINE ${TARGET_NAME})
    configure_file(${CMAKE_SOURCE_DIR}/build/packaging/desktop.in ${DESKTOP_FILE} @ONLY)
    install(FILES ${DESKTOP_FILE} DESTINATION share/applications)
  endif()
endfunction()

macro(_add_man_page TARGET_NAME)
  if(MCRL2_MAN_PAGES)
    add_custom_command(
      TARGET ${TARGET_NAME} POST_BUILD
      COMMAND ${TARGET_NAME} --generate-man-page | gzip --best > ${TARGET_NAME}.1.gz
      COMMENT "Generating man page for ${TARGET_NAME}"
      USES_TERMINAL
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.1.gz DESTINATION share/man/man1 COMPONENT Documentation)
  endif(MCRL2_MAN_PAGES)
endmacro()

function(_install_header_files)
  foreach(HEADER ${ARGN})
    get_filename_component(HEADER_ABS "${HEADER}" ABSOLUTE)
    file(RELATIVE_PATH RELPATH "${CMAKE_CURRENT_SOURCE_DIR}/include" "${HEADER_ABS}")
    get_filename_component(RELPATH ${RELPATH} PATH)
    install(FILES ${HEADER} DESTINATION ${MCRL2_INCLUDE_PATH}/${RELPATH} COMPONENT Headers)
  endforeach()
endfunction()

function(_add_mcrl2_binary TARGET_NAME TARGET_TYPE)
  set(OPTION_KW "NOTEST")
  set(VALUE_KW "SOURCEDIR" "INCLUDEDIR" "COMPONENT" "DESCRIPTION" "MENUNAME" "ICON")
  set(LIST_KW "SOURCES" "DEPENDS" "INCLUDE" "RESOURCES" "NOHEADERTEST")
  cmake_parse_arguments("ARG" "${OPTION_KW}" "${VALUE_KW}" "${LIST_KW}" ${ARGN})
  if(NOT ARG_COMPONENT)
    if("${TARGET_TYPE}" STREQUAL "LIBRARY")
      set(ARG_COMPONENT "Libraries")
    else()
      set(ARG_COMPONENT "Stable")
    endif()
  endif()

  foreach(DEP ${ARG_DEPENDS})
    if("${DEP}" MATCHES "Qt.*")
      # This variable is true iff one dependency starts with Qt.*.
      set(HAS_QT_DEPENDENCY TRUE)
    endif()

    if("${DEP}" STREQUAL "Qt6::Widgets")
      # This mCRL2 binary depends on Qt6::Widgets, so it is a gui binary.
      set(IS_GUI_BINARY TRUE)
    endif()
  endforeach()
  set(DEPENDS ${ARG_DEPENDS})

  string(TOLOWER ${ARG_COMPONENT} COMPONENT_LC)
  if(("${COMPONENT_LC}" STREQUAL "experimental" AND NOT MCRL2_ENABLE_EXPERIMENTAL) OR
     ("${COMPONENT_LC}" STREQUAL "deprecated" AND NOT MCRL2_ENABLE_DEPRECATED) OR
     ("${COMPONENT_LC}" STREQUAL "developer" AND NOT MCRL2_ENABLE_DEVELOPER) OR
     ("${COMPONENT_LC}" STREQUAL "stable" AND NOT MCRL2_ENABLE_STABLE) OR
     (IS_GUI_BINARY AND NOT MCRL2_ENABLE_GUI_TOOLS))
    return()
  endif()
 
  get_filename_component(SOURCEDIR_ABS ${ARG_SOURCEDIR} ABSOLUTE)
  get_filename_component(INCLUDEDIR_ABS ${ARG_INCLUDEDIR} ABSOLUTE)

  foreach(_SRC ${ARG_SOURCES})
    get_filename_component(_SRC_ABS ${_SRC} ABSOLUTE)

    # TODO: Is this necessary/wanted?
    if(NOT EXISTS ${_SRC_ABS})
      file(GLOB_RECURSE _SRC_ABS "${SOURCEDIR_ABS}/${_SRC}")
      if(NOT _SRC_ABS)
        message(WARNING " - ${TARGET_NAME} - No SRC_ABS with GLOB_RECURSE or get_filename_component for file: ${_SRC}")
      endif()
    endif()
    list(APPEND ARG_SOURCES_ABS ${_SRC_ABS})
  endforeach()

  foreach(_SRC_ABS ${ARG_SOURCES_ABS})
    # Check if file extension requires extra work
    get_filename_component(SRC_EXT ${_SRC_ABS} EXT)    
    if("${SRC_EXT}" STREQUAL ".g")
      # This is a grammar file. Generate a .c file using DParser
      get_filename_component(SRC_BASE ${_SRC_ABS} NAME_WE)
      file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/source)
      set(PARSER_CODE ${CMAKE_CURRENT_BINARY_DIR}/source/${SRC_BASE}.c)
      string(REGEX REPLACE "(.*)_syntax" "\\1" GRAMMAR_IDENT ${SRC_BASE})
      add_custom_command(
          OUTPUT ${PARSER_CODE}
          COMMAND make_dparser -A -H0 -i${GRAMMAR_IDENT} -o${PARSER_CODE} ${_SRC_ABS}
          DEPENDS make_dparser ${_SRC_ABS}
          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
          COMMENT "Generating ${GRAMMAR_IDENT} parser"
        )
      add_custom_target(${LIBNAME}_${SRC_BASE} SOURCES ${_SRC_ABS})
      set(_SRC_ABS ${PARSER_CODE})
      
      append_unique(DEPENDS dparser)
      append_unique(ARG_INCLUDE ${CMAKE_SOURCE_DIR}/3rd-party/dparser)
    endif()

    append_unique(ARG_INCLUDE ${CMAKE_SOURCE_DIR}/3rd-party/sylvan/src)
    append_unique(SOURCES ${_SRC_ABS})
  endforeach()

  # TODO: Fix finding header files to not use GLOB_RECURSE.
  #       Potential fixes: - HEADERS list parameter
  #                        - INCLUDE_DIR list parameter -> multiple include directories
  file(GLOB_RECURSE TARGET_INCLUDE_FILES ${INCLUDEDIR_ABS}/*.h)
  
  if("${TARGET_TYPE}" STREQUAL "LIBRARY")
    if(NOT SOURCES)
      # This is a header-only library. We're still going to make a static library
      # (exporting nothing) out of it, so we can use CMake's dependency handling
      # mechanisms.
      # We are adding an empty file here so CMake does not complain that it does
      # not know what linker to use. We could have used the LINKER_LANGUAGE CXX
      # property, but then that breaks the RPATH handling at install time on *nix
      # systems...
      if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/empty.cpp)
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/empty.cpp "")
      endif()
      add_library(${TARGET_NAME} ${CMAKE_CURRENT_BINARY_DIR}/empty.cpp ${TARGET_INCLUDE_FILES})
    else()
      add_library(${TARGET_NAME} ${SOURCES} ${TARGET_INCLUDE_FILES})
    endif()
        
    install(TARGETS ${TARGET_NAME}
            COMPONENT ${ARG_COMPONENT}
            LIBRARY DESTINATION ${MCRL2_LIBRARY_PATH}
            ARCHIVE DESTINATION ${MCRL2_ARCHIVE_PATH}
            FRAMEWORK DESTINATION ${MCRL2_LIBRARY_PATH})
    _install_header_files(${TARGET_INCLUDE_FILES})	
    
    if (NOT ${ARG_NOTEST} AND ${MCRL2_ENABLE_HEADER_TESTS})
      _add_header_tests(${TARGET_NAME} "${ARG_NOHEADERTEST}")	
    endif()
    
    if (NOT ${ARG_NOTEST} AND ${MCRL2_ENABLE_TESTS})
      _add_library_tests(${TARGET_NAME})
    endif()
  elseif("${TARGET_TYPE}" STREQUAL "EXECUTABLE")
    if(IS_GUI_BINARY)
      _add_resource_files(${TARGET_NAME} "${ARG_MENUNAME}" "${ARG_DESCRIPTION}" "${ARG_ICON}" SOURCES)
    endif()
    
    
    add_executable(${TARGET_NAME} ${SOURCES} ${TARGET_INCLUDE_FILES})
    
    if(IS_GUI_BINARY)
      _prepare_desktop_application(${TARGET_NAME} "${ARG_MENUNAME}" "${ARG_DESCRIPTION}" "${ARG_ICON}")
    endif()
    
    _add_man_page(${TARGET_NAME})
    install(TARGETS ${TARGET_NAME}
            COMPONENT ${ARG_COMPONENT}
            RUNTIME DESTINATION ${MCRL2_RUNTIME_PATH}
            BUNDLE DESTINATION ${MCRL2_BUNDLE_PATH})
    get_target_property(IS_BUNDLE ${TARGET_NAME} MACOSX_BUNDLE)
    
    get_property(MCRL2_TOOLS GLOBAL PROPERTY MCRL2_TOOLS)
    set_property(GLOBAL PROPERTY MCRL2_TOOLS "${MCRL2_TOOLS},${TARGET_NAME}")
  endif()
  
  target_include_directories(${TARGET_NAME} PUBLIC ${ARG_INCLUDEDIR} ${ARG_INCLUDE} )

  if(DEPENDS)
    target_link_libraries(${TARGET_NAME} ${DEPENDS})
  endif()

  if(HAS_QT_DEPENDENCY)
    if("${TARGET_TYPE}" STREQUAL "EXECUTABLE")
      # The variable ${MCRL2_QT_APPS} contains a list of mCRL2 executables that use Qt.
      if(MCRL2_QT_APPS)
        set(MCRL2_QT_APPS "${MCRL2_QT_APPS};${TARGET_NAME}" CACHE INTERNAL "")
      else()
        set(MCRL2_QT_APPS "${TARGET_NAME}" CACHE INTERNAL "")
      endif()
    endif()

    # Enable the CMake build system to automatically run MOC on source files that need it.
    set_target_properties(${TARGET_NAME} PROPERTIES AUTOMOC TRUE)
    set_target_properties(${TARGET_NAME} PROPERTIES AUTOUIC TRUE)
    set_target_properties(${TARGET_NAME} PROPERTIES AUTORCC TRUE)
  endif()
endfunction()

function(add_mcrl2_library LIBNAME)
  _add_mcrl2_binary(mcrl2_${LIBNAME} LIBRARY
    SOURCEDIR "source"
    INCLUDEDIR "include"
    ${ARGN}
    COMPONENT "Libraries")
endfunction()

function(add_mcrl2_tool TOOLNAME)
  _add_mcrl2_binary(${TOOLNAME} EXECUTABLE
    SOURCEDIR "."
    INCLUDEDIR "."
    ${ARGN})
endfunction()
