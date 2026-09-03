mcrl2_add_tool(pbescegps
  SOURCES
    pbescegps.cpp
  DEPENDS
    mcrl2_pbes
)

if(APPLE)
  # boost::process's header-only build_cmd_shell calls the out-of-line
  # std::string::insert(const_iterator, char). Under MCRL2_ENABLE_STD_CHECKS,
  # ConfigureUNIX.cmake defines _LIBCPP_ABI_BOUNDED_ITERATORS_IN_STRING, which
  # changes the mangled signature of that function to use __bounded_iter; the
  # system libc++ does not provide that symbol. Undefine the ABI-changing
  # iterator macros for this target only; string/vector/array object layout is
  # unaffected, so this remains ABI-compatible with the mCRL2 libraries.
  target_compile_options(pbescegps PRIVATE
    -U_LIBCPP_ABI_BOUNDED_ITERATORS
    -U_LIBCPP_ABI_BOUNDED_ITERATORS_IN_VECTOR
    -U_LIBCPP_ABI_BOUNDED_ITERATORS_IN_STRING
    -U_LIBCPP_ABI_BOUNDED_ITERATORS_IN_STD_ARRAY)
endif()
