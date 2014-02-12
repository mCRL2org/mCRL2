include(AddFlag)

include_directories( build/workarounds/msvc )

##---------------------------------------------------
## Set MSVC specific compiler flags
##---------------------------------------------------

try_add_cxx_flag(/MD)                   # Creates multithreaded DLLs using MSVCRT.lib
try_add_cxx_flag(/EHs)                  # Synchronous exception handling (TODO: check why)
try_add_cxx_flag(/bigobj)               # Allow big object files
try_add_cxx_flag(/MP)                   # Use multicore compilation where possible
try_add_cxx_flag(/W3        MAINTAINER) # More warnings in Maintainer builds

try_add_c_flag(/MD)
try_add_c_flag(/EHs)
try_add_c_flag(/bigobj)
try_add_c_flag(/MP)
try_add_c_flag(/W3          MAINTAINER)

add_definitions(-DNOMINMAX)                 # Don't let <windows.h> (re)define min and max
add_definitions(-D_USE_MATH_DEFINES)        # Make <cmath> define M_PI, M_PI_2 etc.

# TODO: check if _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES works instead of _CRT_SECURE_NO_WARNINGS
# TODO: check if the _NO_DEPRECATE defines do anything (no documentation on MSDN)
# TODO: check if _SCL_SECURE_NO_WARNINGS is still needed
add_definitions(-D_CRT_SECURE_NO_DEPRECATE) 
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_SCL_SECURE_NO_DEPRECATE)
add_definitions(-D_SCL_SECURE_NO_WARNINGS)

# TODO: check if the following are still needed.
add_definitions(-DBOOST_ALL_NO_LIB=1)
add_definitions(-DWIN32)

# The following flag prevents compiling rewriter code from being compiled.
# TODO: rename NO_DYNLOAD to MCRL2_NO_DYNLOAD
add_definitions(-DNO_DYNLOAD)
