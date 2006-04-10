// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/aterm_make.h
// date          : 25-10-2005
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#ifndef BOOST_PP_IS_ITERATING

#  ifndef ATERM_MAKE_INCLUDED
#    define ATERM_MAKE_INCLUDED

#    include <boost/preprocessor/repetition.hpp>
#    include <boost/preprocessor/arithmetic/sub.hpp>
#    include <boost/preprocessor/punctuation/comma_if.hpp>
#    include <boost/preprocessor/iteration/iterate.hpp>

#    ifndef ATERM_MAKE_MAX_SIZE
#      define ATERM_MAKE_MAX_SIZE 10  // default maximum size is 10
#    endif

// generate specializations
#    define BOOST_PP_ITERATION_LIMITS (1, ATERM_MAKE_MAX_SIZE - 1)
#    define BOOST_PP_FILENAME_1       "atermpp/aterm_make.h" // this file
#    include BOOST_PP_ITERATE()

#  endif // ATERM_MAKE_INCLUDED

#else // BOOST_PP_IS_ITERATING

#  define n BOOST_PP_ITERATION()

#  define TEXT1(z, n, _) const T ## n& t ## n
#  define TEXT2(z, n, _) aterm_ptr_ref(t ## n)
#  define TEXT3(z, n, _) aterm_ptr(t ## n)

template <BOOST_PP_ENUM_PARAMS(n, class T)>
aterm make_term(const std::string& format, BOOST_PP_ENUM(n, TEXT1, nil))
{
  return aterm(ATmake(const_cast<char*>(format.c_str()), BOOST_PP_ENUM(n, TEXT3, nil)));
}

#  undef TEXT1
#  undef TEXT2
#  undef TEXT3
#  undef n

#endif // BOOST_PP_IS_ITERATING

