if(WIN32)

  # Platform is Windows
  set(BUILD_SHARED_LIBS OFF)
  set(MCRL2_RUNTIME_PATH )
  set(MCRL2_LIBRARY_PATH )
  set(MCRL2_ARCHIVE_PATH lib)
  set(MCRL2_INCLUDE_PATH include)

elseif(UNIX)

  if(APPLE)

    # Platform is Mac OSX
    set(CMAKE_EXE_LINKER_FLAGS "-framework Carbon ${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "-framework Carbon -single_module ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS "-framework Carbon ${CMAKE_SHARED_LINKER_FLAGS}")

    set(MCRL2_LIBRARY_PATH Frameworks)
    set(MCRL2_ARCHIVE_PATH lib)
    set(MCRL2_INCLUDE_PATH include)
    set(MCRL2_BUNDLE_PATH Applications)
    set(MCRL2_RUNTIME_PATH ${MCRL2_BUNDLE_PATH}/mcrl2-gui.app/Content/MacOS)

    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_INSTALL_RPATH "@loader_path/../../../../Frameworks")
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

  else()

    # Platform is any other *nix flavour
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/build/packaging/icons/hicolor DESTINATION share/icons)

    set(MCRL2_RUNTIME_PATH bin)
    set(MCRL2_LIBRARY_PATH lib)
    set(MCRL2_ARCHIVE_PATH share/mcrl2/lib)
    set(MCRL2_INCLUDE_PATH share/mcrl2/include)

    set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

  endif()

  # Platform is Mac OSX or *nix
  option(BUILD_SHARED_LIBS "Enable/disable creation of shared libraries" ON)
  option(MCRL2_MAN_PAGES "Enable/disable creation of UNIX man pages" ON)

else()
  message(FATAL_ERROR "Could not recognize platform (not Windows, Apple or Unix)!")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/stage/${MCRL2_RUNTIME_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/stage/${MCRL2_LIBRARY_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/stage/${MCRL2_ARCHIVE_PATH})
