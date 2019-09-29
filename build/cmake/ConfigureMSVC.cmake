include(AddFlag)

include_directories( build/workarounds/msvc )

##---------------------------------------------------
## Set MSVC specific compiler flags
##---------------------------------------------------

try_add_cxx_flag(/EHs)                # From MSDN:  Although Windows and Visual C++ support SEH, we strongly recommend 
                                      #     that you use ISO-standard C++ exception handling (/EHs or /EHsc) because 
                                      #     it makes code more portable and flexible.
try_add_cxx_flag(/bigobj)             # Increases object addressable sections capacity to 4,294,967,296 (2^32), possibly required for template heavy code.
try_add_cxx_flag(/std:c++17)          # Enable for conformance checking in Visual Studio 2017 for the C++17 standard.
try_add_cxx_flag(/W3      DEBUG)      # Increase the warning level.
try_add_cxx_flag(/MP)                 # Enable multicore compilation.
try_add_cxx_flag(/permissive-)		  # This option disables permissive behaviors, and sets the /Zc compiler options for strict conformance.

try_add_c_flag(/EHs)
try_add_c_flag(/bigobj)
try_add_c_flag(/std:c++17)
try_add_c_flag(/W3         DEBUG)
try_add_c_flag(/MP)
try_add_c_flag(/permissive-)
  
add_definitions(-DNOMINMAX)                 # Don't let <windows.h> (re)define min and max
add_definitions(-D_USE_MATH_DEFINES)        # Make <cmath> define M_PI, M_PI_2 etc.
add_definitions(-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES) # Enables template overloads of standard CRT functions that call the more secure variants automatically.
add_definitions(-D_CRT_SECURE_NO_WARNINGS)                 # Prevents many CRT deprecation warnings, especially in dparser.
add_definitions(-DBOOST_ALL_NO_LIB=1)       # Tells the config system not to automatically select which libraries to link against. Normally if a compiler supports #pragma lib, 
                                            # then the correct library build variant will be automatically selected and linked against, simply by the act of including one of 
                                            # that library's headers. This macro turns that feature off. 

# increase the stack size
set(CMAKE_EXE_LINKER_FLAGS "/STACK:32000000000 ${CMAKE_EXE_LINKER_FLAGS}")
