include(CMakeParseArguments)

function(_add_tests TARGET_NAME)
  if(MCRL2_ENABLE_TEST_LIBRARIES)
    file(GLOB test "test/*.cpp")
  endif()
  if(MCRL2_ENABLE_TEST_EXAMPLES)
    file(GLOB example "example/*.cpp")
  endif()
  foreach(category "test" "example")
    foreach(test IN ITEMS ${${category}})
      get_filename_component(base ${test} NAME_WE)
      set(testname ${category}_${TARGET_NAME}_${base})
      add_executable(${testname} EXCLUDE_FROM_ALL ${test})
      set_target_properties(${testname} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${category})
      target_link_libraries(${testname} ${TARGET_NAME})
      # TODO: remove the following once we get rid of the PBES/BES dependency issue
      if(TARGET_NAME STREQUAL "mcrl2_bes")
        target_link_libraries(${testname} mcrl2_bes)
      endif()
      add_test(NAME ${testname} COMMAND ${CMAKE_CTEST_COMMAND}
         --build-and-test
         "${CMAKE_CURRENT_SOURCE_DIR}"
         "${CMAKE_CURRENT_BINARY_DIR}"
         --build-noclean
         --build-nocmake
         --build-generator "${CMAKE_GENERATOR}"
         --build-target "${testname}"
         --build-makeprogram "${CMAKE_MAKE_PROGRAM}"
         --test-command "${CMAKE_CURRENT_BINARY_DIR}/${category}/${testname}"
        )
    endforeach()
  endforeach()
endfunction()

macro(_install_qt_libs TARGET_NAME COMPONENT QT_LIBS)
  if(APPLE AND QT_LIBS)
    set(INSTALL_NAME_TOOL "install_name_tool")
    foreach(LIB ${QT_LIBS})
      get_target_property(UNRESOLVED_PATH Qt5::${LIB} LOCATION)
      get_filename_component(ABS_PATH ${UNRESOLVED_PATH} REALPATH)
      set(INSTALL_NAME_TOOL "${INSTALL_NAME_TOOL} -change ${ABS_PATH} @loader_path/../lib/Qt${LIB}")
      install(FILES ${ABS_PATH} DESTINATION ${MCRL2_BUNDLE_PATH}/${TARGET_NAME}.app/Contents/lib)
    endforeach()
    set(ABS_PATH "\${CMAKE_INSTALL_PREFIX}/Applications/${TARGET_NAME}.app/Contents/MacOS/${TARGET_NAME}")
    set(INSTALL_NAME_TOOL "${INSTALL_NAME_TOOL} ${ABS_PATH}")
    install(CODE 
      "
      message(STATUS \"Running ${INSTALL_NAME_TOOL}\")
      execute_process(COMMAND ${INSTALL_NAME_TOOL})
      "
      COMPONENT ${COMPONENT})
  elseif(WIN32 AND QT_LIBS)
    get_target_property(QT_PATH Qt5::Core LOCATION)
    get_filename_component(DIRS ${QT_PATH} PATH)
    get_target_property(TARGET_LOCATION ${TARGET_NAME} LOCATION)
    set(POSTBUILDSCRIPT ${CMAKE_CURRENT_BINARY_DIR}/post_build.cmake)
    configure_file(${CMAKE_SOURCE_DIR}/build/cmake/post_build.cmake.in ${POSTBUILDSCRIPT} @ONLY)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD 
      COMMAND ${CMAKE_COMMAND} -DCONFIGURATION=$<CONFIGURATION> -P ${POSTBUILDSCRIPT})
  endif()
endmacro()

function(_add_resource_file TARGET_NAME TOOLNAME DESCRIPTION ICON SOURCE_FILES)
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
    math(EXPR VERSION_LOHI "${REVISION} / 65536")
    math(EXPR VERSION_LOLO "${REVISION} % 65536")
    if(NOT MCRL2_PACKAGE_RELEASE)
      set(FILEFLAGS "VS_FF_PRERELEASE")
    elseif()
      set(FILEFLAGS "VER_DBG")
    endif()
    set(ICOFILE ${CMAKE_SOURCE_DIR}/build/packaging/icons/${ICON}.ico)
    get_filename_component(ORIGFILENAME ${ORIGFILENAME} NAME)
    configure_file(${CMAKE_SOURCE_DIR}/build/packaging/icon.rc.in ${RC_FILE} @ONLY)
    set(${SOURCE_FILES} ${${SOURCE_FILES}} ${RC_FILE})
    set(${SOURCE_FILES} "${${SOURCE_FILES}}" PARENT_SCOPE)
  endif()
endfunction()

function(_prepare_desktop_application TARGET_NAME TOOLNAME DESCRIPTION ICON)
  if(UNIX AND NOT APPLE)
    set(DESKTOP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.desktop)
    set(COMMANDLINE ${TARGET_NAME})
    configure_file(${CMAKE_SOURCE_DIR}/build/packaging/desktop.in ${DESKTOP_FILE} @ONLY)
    install(FILES ${DESKTOP_FILE} DESTINATION share/applications)
  elseif(APPLE)
    set(ICNS_FILE ${CMAKE_SOURCE_DIR}/build/packaging/icons/${ICON}.icns)
    set_source_files_properties(${ICNS_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(${SOURCE_FILES} ${${SOURCE_FILES}} ${ICNS_FILE})
    set_target_properties(${TARGET_NAME} PROPERTIES
      MACOSX_BUNDLE TRUE
      MACOSX_BUNDLE_BUNDLE_NAME ${TOOLNAME}
      MACOSX_BUNDLE_ICON_FILE ${ICON}.icns
      MACOSX_BUNDLE_LONG_VERSION_STRING ${MCRL2_VERSION}
      MACOSX_BUNDLE_SHORT_VERSION_STRING ${MCRL2_VERSION}
      MACOSX_BUNDLE_BUNDLE_VERSION ${MCRL2_VERSION}
      MACOSX_BUNDLE_GUI_IDENTIFIER org.mcrl2.${TARGET_NAME})
  endif()
endfunction()

macro(_add_man_page TARGET_NAME)
  if(MCRL2_MAN_PAGES)
    add_custom_command(
      TARGET ${TARGET_NAME} POST_BUILD
      COMMAND ${TARGET_NAME} --generate-man-page | gzip --best > ${TARGET_NAME}.1.gz
      COMMENT "Generating man page for ${TARGET_NAME}"
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.1.gz DESTINATION share/man/man1 COMPONENT Documentation)
  endif(MCRL2_MAN_PAGES)
endmacro()

function(_install_header_files TARGET_INCLUDE_FILES)
  foreach(HEADER ${TARGET_INCLUDE_FILES})
    get_filename_component(HEADER_ABS "${HEADER}" ABSOLUTE)
    file(RELATIVE_PATH RELPATH "${CMAKE_CURRENT_SOURCE_DIR}/include" "${HEADER_ABS}")
    get_filename_component(RELPATH ${RELPATH} PATH)
    if(APPLE)
      set_property(SOURCE ${HEADER} PROPERTY MACOSX_PACKAGE_LOCATION Headers/${RELPATH})
    else()
      install(FILES ${HEADER} DESTINATION ${MCRL2_INCLUDE_PATH}/${RELPATH} COMPONENT Headers)
    endif()
  endforeach()
endfunction()

function(_add_mcrl2_binary TARGET_NAME TARGET_TYPE)
  set(VALUE_KW "SOURCEDIR" "INCLUDEDIR" "COMPONENT" "DESCRIPTION" "MENUNAME" "ICON")
  set(LIST_KW "SOURCES" "DEPENDS" "INCLUDE" "RESOURCES")
  cmake_parse_arguments("ARG" "" "${VALUE_KW}" "${LIST_KW}" ${ARGN})

  set(INCLUDE ${ARG_INCLUDEDIR} ${ARG_INCLUDE})
  file(GLOB_RECURSE TARGET_INCLUDE_FILES ${ARG_INCLUDEDIR}/*.h)
  if(TARGET_INCLUDE_FILES)
    add_custom_target(${TARGET_NAME}_headers SOURCES ${TARGET_INCLUDE_FILES})
  endif()

  if(ARG_COMPONENT)
    string(TOLOWER ${ARG_COMPONENT} COMPONENT_LC)
    if((${COMPONENT_LC} STREQUAL "experimental" AND NOT MCRL2_ENABLE_EXPERIMENTAL) OR
       (${COMPONENT_LC} STREQUAL "deprecated" AND NOT MCRL2_ENABLE_DEPRECATED) OR
       (${COMPONENT_LC} STREQUAL "developer"))
      unset(ARG_COMPONENT)
    endif()
  else()
    set(ARG_COMPONENT ${CMAKE_INSTALL_DEFAULT_COMPONENT_NAME})
  endif()

  foreach(DEP ${ARG_DEPENDS})
    string(REGEX REPLACE "Qt(.*)" "\\1" QT_LIB ${DEP})
    if(NOT ${QT_LIB} STREQUAL ${DEP})
      set(QT_LIBS ${QT_LIBS} ${QT_LIB})
      if(${QT_LIB} STREQUAL "Widgets")
        set(IS_GUI_BINARY TRUE)
        set(WIN32 "WIN32")
      endif()
    else()
      list(APPEND DEPENDS ${DEP})
    endif()
  endforeach()

  foreach(SRC ${ARG_SOURCES})
    get_filename_component(SRC_ABS ${SRC} ABSOLUTE)
    if(NOT ${SRC_ABS} STREQUAL ${SRC})
      get_filename_component(SRC_ABS ${ARG_SOURCEDIR}/${SRC} ABSOLUTE)
    endif()
    get_filename_component(SRC_EXT ${SRC_ABS} EXT)
    if("${SRC_EXT}" STREQUAL ".g")
      # This is a grammar file. Generate a .c file using DParser
      get_filename_component(SRC_BASE ${SRC_ABS} NAME_WE)
      file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/source)
      set(PARSER_CODE ${CMAKE_CURRENT_BINARY_DIR}/source/${SRC_BASE}.c)
      string(REGEX REPLACE "(.*)_syntax" "\\1" GRAMMAR_IDENT ${SRC_BASE})
      add_custom_command(
          OUTPUT ${PARSER_CODE}
          COMMAND make_dparser -A -H0 -i${GRAMMAR_IDENT} -o${PARSER_CODE} ${SRC_ABS}
          DEPENDS make_dparser ${SRC_ABS}
          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
          COMMENT "Generating ${GRAMMAR_IDENT} parser"
        )
      add_custom_target(${LIBNAME}_${SRC_BASE} SOURCES ${SRC_ABS})
      set(SRC_ABS ${PARSER_CODE})
      set(DEPENDS ${DEPENDS} dparser)
      set(INCLUDE ${INCLUDE} ${CMAKE_SOURCE_DIR}/3rd-party/dparser)
    elseif("${SRC_EXT}" STREQUAL ".ui")
      qt5_wrap_ui(SRC_ABS ${SRC_ABS})
    elseif("${SRC_EXT}" STREQUAL ".qrc")
      qt5_add_resources(SRC_ABS ${SRC_ABS})
    endif()
    set(SOURCES ${SRC_ABS} ${SOURCES})
  endforeach()

  if((NOT IS_GUI_BINARY OR MCRL2_ENABLE_GUI_TOOLS) AND ARG_COMPONENT)

    include_directories(${INCLUDE})
    if(${TARGET_TYPE} STREQUAL "LIBRARY")
      if(NOT SOURCES)
        # This is a header-only library. We're still going to make a static library
        # (exporting nothing) out of it, so we can use CMake's dependency handling
        # mechanisms.
        # We are adding an empty file here so CMake does not complain that it does 
        # not know what linker to use. We could have used the LINKER_LANGUAGE CXX 
        # property, but then that breaks the RPATH handling at install time on *nix
        # systems...
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/empty.cpp "")
        add_library(${TARGET_NAME} STATIC ${CMAKE_CURRENT_BINARY_DIR}/empty.cpp ${TARGET_INCLUDE_FILES})
      else()
        add_library(${TARGET_NAME} ${SOURCES} ${TARGET_INCLUDE_FILES})
      endif()
      target_include_directories(${TARGET_NAME} INTERFACE ${ARG_INCLUDEDIR} ${ARG_INCLUDE})
      set_target_properties(${TARGET_NAME} PROPERTIES 
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER org.mcrl2.${TARGET_NAME}
        MACOSX_FRAMEWORK_BUNDLE_VERSION ${MCRL2_VERSION}
        MACOSX_FRAMEWORK_ICON_FILE mcrl2-orange.icns
      )
      install(TARGETS ${TARGET_NAME}
              LIBRARY DESTINATION ${MCRL2_LIBRARY_PATH}
              ARCHIVE DESTINATION ${MCRL2_ARCHIVE_PATH}
              FRAMEWORK DESTINATION ${MCRL2_LIBRARY_PATH}
              COMPONENT ${ARG_COMPONENT})
      _install_header_files(${TARGET_INCLUDE_FILES})
    elseif(${TARGET_TYPE} STREQUAL "EXECUTABLE")
      _add_resource_file(${TARGET_NAME} "${ARG_MENUNAME}" "${ARG_DESCRIPTION}" "${ARG_ICON}" SOURCES)
      add_executable(${TARGET_NAME} ${WIN32} ${SOURCES} ${TARGET_INCLUDE_FILES})
      _add_man_page(${TARGET_NAME})
      install(TARGETS ${TARGET_NAME}
              RUNTIME DESTINATION ${MCRL2_RUNTIME_PATH}
              BUNDLE DESTINATION ${MCRL2_BUNDLE_PATH}
              COMPONENT ${INSTALL_COMPONENT})
      _install_qt_libs(${TARGET_NAME} ${ARG_COMPONENT} "${QT_LIBS}")
      if(IS_GUI_BINARY)
        _prepare_desktop_application(${TARGET_NAME} "${ARG_MENUNAME}" "${ARG_DESCRIPTION}" "${ARG_ICON}")
      endif()
    endif()
    if(DEPENDS)
      target_link_libraries(${TARGET_NAME} ${DEPENDS})
    endif()
    if(QT_LIBS)
      qt5_use_modules(${TARGET_NAME} ${QT_LIBS})
      set_target_properties(${TARGET_NAME} PROPERTIES AUTOMOC TRUE)
    endif()
    _add_tests(${TARGET_NAME})

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
