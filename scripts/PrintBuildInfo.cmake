message(STATUS "CMAKE VERSION: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE} " )
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")

include(GetArchitectureInformation)

message(STATUS "SVN data: ${Project_WC_LAST_CHANGED_DATE}")

include(GetCompilerVersion)
message(STATUS "mCRL2 version: ${MCRL2_VERSION}")
message(STATUS "Found CVC3 version ${cvc3_VERSION}")

