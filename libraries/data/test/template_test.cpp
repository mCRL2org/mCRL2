// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file template_test.cpp
/// \brief Template tests.

#include <iostream>
#include <boost/test/minimal.hpp>

template <typename Derived>
class data_traverser
{};

template <typename Derived, template <typename> class Traverser = data_traverser>
class lps_traverser: public Traverser<Derived>
{};

template <template <typename> class Traverser = data_traverser>
struct find_helper: public Traverser<find_helper<Traverser> >
{
  void print() const
  {
    std::cout << "hello, world!" << std::endl;
  }
};

template <template <typename> class Traverser>
find_helper<Traverser> make_find_helper()
{
  return find_helper<Traverser>();
}

template <template <typename> class Traverser>
void f1()
{
  std::cout << "hello, world!" << std::endl;
}

template <template <typename, typename> class Traverser>
void f2()
{
  std::cout << "hello, world!" << std::endl;
}

int test_main(int argc, char* argv[])
{
  make_find_helper<data_traverser>().print();
  f1<data_traverser>();

  //--- This does not compile ---//
  // make_find_helper<lps_traverser>().print();
  //f1<lps_traverser>();
  //f2<lps_traverser>();

  // The question is: can we make a (make_)find_helper that accepts both
  // data_traverser and lps_traverser?

  return 0;
}
