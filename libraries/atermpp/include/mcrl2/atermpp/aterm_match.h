// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_match.h
/// \brief Generate match functions using boost preprocessor library.

#ifndef BOOST_PP_IS_ITERATING

#  ifndef ATERM_MATCH_INCLUDED
#    define ATERM_MATCH_INCLUDED

#    include <boost/preprocessor/repetition.hpp>
#    include <boost/preprocessor/arithmetic/sub.hpp>
#    include <boost/preprocessor/punctuation/comma_if.hpp>
#    include <boost/preprocessor/iteration/iterate.hpp>

/// The default maximum for the number of parameters of match.
#    ifndef ATERM_MATCH_MAX_SIZE
#      define ATERM_MATCH_MAX_SIZE 10  // default maximum size is 10
#    endif

// generate specializations
#    define BOOST_PP_ITERATION_LIMITS (1, ATERM_MATCH_MAX_SIZE - 1)
#    define BOOST_PP_FILENAME_1       "mcrl2/atermpp/aterm_match.h" // this file
#    include BOOST_PP_ITERATE()

#  endif // ATERM_MATCH_INCLUDED

#else // BOOST_PP_IS_ITERATING

#  define n BOOST_PP_ITERATION()
#  define TEXT1(z, n, _) T ## n& t ## n
#  define TEXT2(z, n, _) aterm_traits<T ## n>::ptr(t ## n)

/// \brief Match terms based on patterns.
/// \param pattern A string
template <BOOST_PP_ENUM_PARAMS(n, class T)>
bool match(const aterm& t, const std::string& pattern, BOOST_PP_ENUM(n, TEXT1, nil))
{
  return ATmatch(t, const_cast<char*>(pattern.c_str()), BOOST_PP_ENUM(n, TEXT2, nil)) == ATtrue;
}

#  undef TEXT1
#  undef TEXT2
#  undef n

#endif // BOOST_PP_IS_ITERATING