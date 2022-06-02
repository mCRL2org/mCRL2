// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file template_test.cpp
/// \brief Template tests.

#define BOOST_TEST_MODULE template_test
#include <boost/test/included/unit_test.hpp>
#include <iostream>

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

//-------------------------------------------------------------//
template <typename Derived>
class lps_traverser2: public data_traverser<Derived>
{};

BOOST_AUTO_TEST_CASE(test_main)
{
  make_find_helper<data_traverser>().print();
  f1<data_traverser>();

  //--- The intended usage pattern does not compile ---//
  // make_find_helper<lps_traverser>().print();
  //f1<lps_traverser>();

  //--- This also doesn't compile ---//
  //f2<lps_traverser>();

  //--- But this does compile ---//
  make_find_helper<lps_traverser2>().print();
  f1<lps_traverser2>();
}
