function(add_header_target)
  file(GLOB_RECURSE EXTRAFILES include)
  add_custom_target(${PROJECT_NAME}_headers SOURCES ${HEADERS})
endfunction()
