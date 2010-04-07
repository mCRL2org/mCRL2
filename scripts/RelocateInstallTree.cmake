if( APPLE )
install(CODE "
    include(BundleUtilities) 
    fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}\" \"\" \"${CMAKE_INSTALL_PREFIX}/lib\")
    " COMPONENT Runtime)
endif( APPLE )
