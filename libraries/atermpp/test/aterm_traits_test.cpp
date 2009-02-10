// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file aterm_traits_test.cpp
/// \brief Test for aterm_traits.

#include <iostream>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/indexed_set.h"

class B : public atermpp::aterm_appl
{
};

template <typename Term>
void test_protection_traits(Term& t)
{
  atermpp::aterm_traits<Term>::protect(t);
  atermpp::aterm_traits<Term>::unprotect(t);
  atermpp::aterm_traits<Term>::mark(t);
}

template <typename Term>
void test_term_traits(Term& t)
{
  atermpp::aterm_traits<Term>::term(t);
  atermpp::aterm_traits<Term>::ptr(t);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  int i;
  test_term_traits(i);
  // test_protection_traits(i); // This is not supposed to work

  atermpp::aterm_appl a;
  test_term_traits(a);
  test_protection_traits(a);

  B b;
  test_term_traits(b);
  test_protection_traits(b);

  atermpp::indexed_set s;
  test_term_traits(s);
  // test_protection_traits(s); // No idea how to protect an indexed set

  return 0;
}
