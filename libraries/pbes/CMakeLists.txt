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

# boost::process's build_cmd_shell (pulled in via mcrl2/pbes/tools/pbescegps.h)
# calls the out-of-line std::string::insert(const_iterator, char) that libc++
# defines. MCRL2_ENABLE_STD_CHECKS defines _LIBCPP_ABI_BOUNDED_ITERATORS_IN_STRING,
# which changes that symbol to take __bounded_iter; the system libc++ only
# provides the unbounded variant. Undefine the ABI-changing iterator macros for
# the pbes test objects that include boost::process.
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
