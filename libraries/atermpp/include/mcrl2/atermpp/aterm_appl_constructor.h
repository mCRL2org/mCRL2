// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_appl_constructor.h
/// \brief Use the boost preprocessor library to generate constructors.

#ifndef BOOST_PP_IS_ITERATING

#  ifndef ATERM_APPL_CONSTRUCTOR_INCLUDED
#    define ATERM_APPL_CONSTRUCTOR_INCLUDED

#    include <boost/preprocessor/repetition.hpp>
#    include <boost/preprocessor/arithmetic/sub.hpp>
#    include <boost/preprocessor/punctuation/comma_if.hpp>
#    include <boost/preprocessor/iteration/iterate.hpp>

#    ifndef ATERM_MAX_SIZE
#      define ATERM_MAX_SIZE 7  // default maximum size is 7
#    endif

// generate specializations
#    define BOOST_PP_ITERATION_LIMITS (1, ATERM_MAX_SIZE - 1)
#    define BOOST_PP_FILENAME_1       "mcrl2/atermpp/aterm_appl_constructor.h" // this file
#    include BOOST_PP_ITERATE()

#  endif // ATERM_APPL_CONSTRUCTOR_INCLUDED

#else // BOOST_PP_IS_ITERATING

#  define n BOOST_PP_ITERATION()

#  define TEXT1(z, n, _) aterm a ## n
#  define TEXT2(z, n, _) a ## n
#  define ATMAKEAPPL(i) BOOST_PP_CAT(ATmakeAppl,i)

    /// Construct a term with a fixed number of arguments.
    term_appl(function_symbol sym, BOOST_PP_ENUM(n, TEXT1, nil))
      : aterm_base(ATMAKEAPPL(n)(sym, BOOST_PP_ENUM(n, TEXT2, nil)))
    {
    }

#  undef TEXT1
#  undef TEXT2
#  undef n

#endif // BOOST_PP_IS_ITERATING