// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_make.h
/// \brief Generate make_term functions using boost preprocessor library.

#ifndef BOOST_PP_IS_ITERATING

#  ifndef ATERM_MAKE_INCLUDED
#    define ATERM_MAKE_INCLUDED

#    include <string>
#    include <boost/preprocessor/repetition.hpp>
#    include <boost/preprocessor/arithmetic/sub.hpp>
#    include <boost/preprocessor/punctuation/comma_if.hpp>
#    include <boost/preprocessor/iteration/iterate.hpp>
#    include "mcrl2/atermpp/aterm.h"
#    include "mcrl2/atermpp/aterm_traits.h"

/// The default maximum for the number of parameters of make_term.
#    ifndef ATERM_MAKE_MAX_SIZE
#      define ATERM_MAKE_MAX_SIZE 10  // default maximum size is 10
#    endif

// generate specializations
#    define BOOST_PP_ITERATION_LIMITS (1, ATERM_MAKE_MAX_SIZE - 1)
#    define BOOST_PP_FILENAME_1       "mcrl2/atermpp/aterm_make.h" // this file
#    include BOOST_PP_ITERATE()

#  endif // ATERM_MAKE_INCLUDED

#else // BOOST_PP_IS_ITERATING

namespace atermpp {
  #  define n BOOST_PP_ITERATION()
  #  define TEXT1(z, n, _) const T ## n& t ## n
  #  define TEXT2(z, n, _) atermpp::aterm_traits<T ## n>::ptr(t ## n)
  #  define TEXT3(z, n, _) atermpp::aterm_traits<T ## n>::term(t ## n)
  
  /// \brief Make a term with a fixed number of arguments.
  /// \param format A string
  template <BOOST_PP_ENUM_PARAMS(n, class T)>
  aterm make_term(const std::string& format, BOOST_PP_ENUM(n, TEXT1, nil))
  {
    return aterm(ATmake(const_cast<char*>(format.c_str()), BOOST_PP_ENUM(n, TEXT3, nil)));
  }
  
  #  undef TEXT1
  #  undef TEXT2
  #  undef TEXT3
  #  undef n
}

#endif // BOOST_PP_IS_ITERATING
