mcrl2_add_library(mcrl2_pbes
  SOURCES
    source/algorithms.cpp
    source/pbes.cpp
    source/io.cpp
    source/pbes_explorer.cpp
    source/pgsolver.cpp
  DPARSER_SOURCES
    source/pg_syntax.g
  DEPENDS
    mcrl2_core
    mcrl2_data
    mcrl2_lps
    mcrl2_modal_formula
    mcrl2_lts
    mcrl2_symbolic 
  EXCLUDE_HEADERTEST
    # This test fails due to circular dependencies between the bes and
    # pbes libraries on a mac. Therefore it is disabled.
    mcrl2/pbes/pbes_solver_test.h
    # The headertest below fails as the sylvan library is required for it. 
    mcrl2/pbes/pbesbddsolve1.h   
)

add_subdirectory(example)

# boost::process (used via mcrl2/pbes/tools/pbescegps.h) is header-only code that
# calls the out-of-line std::string::insert(const_iterator, char). Under
# MCRL2_ENABLE_STD_CHECKS, ConfigureUNIX.cmake defines
# _LIBCPP_ABI_BOUNDED_ITERATORS_IN_STRING, which changes the mangled signature of
# that function to use __bounded_iter; the system libc++ does not provide that
# symbol. Undefine the ABI-changing iterator macros for the tests that reach it;
# only iterator typedefs change, so this stays ABI-compatible with the library.
if(APPLE AND MCRL2_ENABLE_TESTS)
  foreach(base pbesfindabs_test pbescegps_test pbescegps_ruling_test)
    set(test_target "librarytest_mcrl2_pbes_${base}")
    if(TARGET ${test_target})
      target_compile_options(${test_target} PRIVATE
        -U_LIBCPP_ABI_BOUNDED_ITERATORS
        -U_LIBCPP_ABI_BOUNDED_ITERATORS_IN_VECTOR
        -U_LIBCPP_ABI_BOUNDED_ITERATORS_IN_STRING
        -U_LIBCPP_ABI_BOUNDED_ITERATORS_IN_STD_ARRAY)
    endif()
  endforeach()
endif()
