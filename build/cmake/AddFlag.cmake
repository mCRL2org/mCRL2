function(add_cxx_flag FLAG)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${FLAG}>)
endfunction()

function(add_cxx_debug_flag FLAG)
  add_compile_options($<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Debug>>:${FLAG}>)
endfunction()

function(add_c_flag FLAG)
  add_compile_options($<$<COMPILE_LANGUAGE:C>:${FLAG}>)
endfunction()

function(add_c_debug_flag FLAG)
  add_compile_options($<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:Debug>>:${FLAG}>)
endfunction()

function(add_debug_link_options FLAGS)
  add_compile_options($<$<CONFIG:Debug>:${FLAG}>)
endfunction()