
  option(MCRL2_ENABLE_GUI_TOOLS "Enable/disable creation of GUI tools" ON)
  
  if(MCRL2_ENABLE_GUI_TOOLS)
  
    option(MCRL2_WITH_WXSTC "Enable/disable compilation of mcrl2xi with wxStyledTextControl support" OFF)
    message(STATUS "MCRL2_WITH_WXSTC: ${MCRL2_WITH_WXSTC}")
  
    if(MCRL2_WITH_WXSTC)			
      find_package(wxWidgets COMPONENTS base core adv gl xml html aui stc REQUIRED)
    else(MCRL2_WITH_WXSTC)			
      find_package(wxWidgets COMPONENTS base core adv gl xml html aui REQUIRED)
    endif(MCRL2_WITH_WXSTC)			

    if(NOT wxWidgets_FOUND)
      message( STATUS "Warning: GUI tools are not built" )
      message( STATUS "The mCRL2 toolset requires the installation of wxWidgets 2.8.0 or higher")
      message( FATAL_ERROR "Set MCRL2_ENABLE_GUI_TOOLS to FALSE to build without GUI-tools")
    endif(NOT wxWidgets_FOUND)

    INCLUDE(${wxWidgets_USE_FILE})
  
    IF( NOT ${wxWidgets_CONFIG_EXECUTABLE} MATCHES wxWidgets_wxrc_EXECUTABLE-NOTFOUND )
      #Find wxWidgets version
      execute_process(COMMAND ${wxWidgets_CONFIG_EXECUTABLE} --version
        OUTPUT_VARIABLE wxWidgets_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
      )
  
      if( wxWidgets_VERSION LESS 2.8.0 )
  
        message( STATUS "Warning: GUI tools are not built" )
        message( STATUS "The mCRL2 toolset requires wxWidgets version v2.8.0 or higher" )
        message( STATUS "Current wxWidgets version: ${wxWidgets_VERSION}")
        message( FATAL_ERROR "Set MCRL2_ENABLE_GUI_TOOLS to FALSE to build without GUI-tools")
  
      endif( wxWidgets_VERSION LESS 2.8.0 )
  
      message(STATUS "wxWidgets version: ${wxWidgets_VERSION}")
  
    else( NOT ${wxWidgets_CONFIG_EXECUTABLE} MATCHES wxWidgets_wxrc_EXECUTABLE-NOTFOUND )
  
      message( STATUS "Make sure that wxWidgets (version 2.8.0 or higher) is installed and compiled" )
  
    endif( NOT ${wxWidgets_CONFIG_EXECUTABLE} MATCHES wxWidgets_wxrc_EXECUTABLE-NOTFOUND )
  
    # wxWidgets include directory
    include_directories(${wxWidgets_INCLUDE_DIRS})

  endif(MCRL2_ENABLE_GUI_TOOLS)

