include(AddFlag)

include_directories(build/workarounds/msvc)

##---------------------------------------------------
## Set MSVC specific compiler flags
##---------------------------------------------------

function(add_cxx_flag FLAG)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${FLAG}>)
endfunction()

# TODO: Adapt this generator expression
function(add_cxx_debug_flag FLAG)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${FLAG}>)
endfunction()

function(add_c_flag FLAG)
  add_compile_options($<$<COMPILE_LANGUAGE:C>:${FLAG}>)
endfunction()

function(add_c_debug_flag FLAG)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${FLAG}>)
endfunction()

add_cxx_flag(/wd4267)      # Disable size_t to smaller type conversion warnings.
add_cxx_flag(/wd4244)      # Disable signed integer to smaller type conversion warnings.
add_cxx_flag(/EHs)         # From MSDN:  Although Windows and Visual C++ support SEH, we strongly recommend 
                           #     that you use ISO-standard C++ exception handling (/EHs or /EHsc) because 
                           #     it makes code more portable and flexible.
add_cxx_flag(/bigobj)      # Increases object addressable sections capacity to 4,294,967,296 (2^32), possibly required for template heavy code.
add_cxx_flag(/std:c++17)   # Enable for conformance checking in Visual Studio 2017 for the C++17 standard.
add_cxx_flag(/std:c17)
add_cxx_flag(/MP)          # Enable multicore compilation.
add_cxx_flag(/permissive-) # This option disables permissive behaviors, and sets the /Zc compiler options for strict conformance.
add_cxx_debug_flag(/W3)    # Increase the warning level.

add_c_flag(/wd4267)
add_c_flag(/wd4244)
add_c_flag(/EHs)
add_c_flag(/bigobj)
add_c_flag(/std:c++17)
add_c_flag(/std:c17)
add_c_flag(/MP)
add_c_flag(/permissive-)
add_c_debug_flag(/W3)
  
add_compile_definitions(NOMINMAX)                 # Don't let <windows.h> (re)define min and max
add_compile_definitions(_USE_MATH_DEFINES)        # Make <cmath> define M_PI, M_PI_2 etc.
add_compile_definitions(_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES) # Enables template overloads of standard CRT functions that call the more secure variants automatically.
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)                 # Prevents many CRT deprecation warnings, especially in dparser.
add_compile_definitions(BOOST_ALL_NO_LIB=1) # Tells the config system not to automatically select which libraries to link against. Normally if a compiler supports #pragma lib, 
                                            # then the correct library build variant will be automatically selected and linked against, simply by the act of including one of 
                                            # that library's headers. This macro turns that feature off. 

if(MCRL2_ENABLE_MSVC_CCACHE)
  # This changes the behaviour of MSVC to include debugging symbols into the object file, instead of generating a pdb file.
  string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
  string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
  string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
  string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
endif()

if(MCRL2_ENABLE_ADDRESSSANITIZER)
  add_cxx_flag(/fsanitize=address)
endif()

# increase the stack size
add_link_options("/STACK:32000000000")
