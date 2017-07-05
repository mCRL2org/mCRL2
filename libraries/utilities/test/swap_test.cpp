// Author(s): Jeroen Keiren, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file swap_test.cpp
/// \brief Test for std::swap usage

#include <boost/test/included/unit_test_framework.hpp>

#include <algorithm>
#include <utility>
#include <vector>

namespace nsp
{
class swappable
{
protected:
  int val_;

public:
  swappable(int x) : val_(x)
  {}

  void swap(swappable& other)
  {
    std::cerr << "swap method called" << std::endl;
    int tmp = other.val_;
    other.val_ = val_;
    val_ = tmp;
  }

  int val() const
  {
    return val_;
  }
};

  inline bool operator<(const swappable& x, const swappable& y)
  {
    return x.val() < y.val();
  }
} // namespace nsp

// The following approach does not work on recent versions of Clang using C++11,
// whereas according to the standard and existing documentation it should.
/*
namespace std
{
  template<>
  void swap(nsp::swappable& x, nsp::swappable& y)
  {
    std::cerr << "std::swap specialisation" << std::endl;
    x.swap(y);
  }
}
 */
namespace nsp
{
  void swap(swappable& x, swappable& y)
  {
    std::cerr << "nsp::swap overload" << std::endl;
    x.swap(y);
  }
} // namespace nsp

// This test case check whether, if both std::swap and
// nsp::swap are visible, the nsp::swap is called.
BOOST_AUTO_TEST_CASE(swapping_element)
{
  using std::swap;
  using namespace nsp;
  swappable x(1);
  swappable y(2);
  swap(x,y);
  BOOST_CHECK(x.val() == 2);
  BOOST_CHECK(y.val() == 1);
}

// This test case checks whether, if a vector is sorted and
// swap is called internally, then nsp::swap is called through
// argument dependent lookup (ADL) instead of std::swap.
// if nsp::swap is called, a message to this effect is printed.
BOOST_AUTO_TEST_CASE(swapping_vector)
{
  std::vector<nsp::swappable> v;
  nsp::swappable x(1);
  nsp::swappable y(2);
  nsp::swappable z(3);
  v.push_back(y);
  v.push_back(x);
  std::sort(v.begin(),v.end());
}

namespace nA {
  template <typename T>
  class A
  {
    protected:
      T val_;

    public:
      A(T x) : val_(x)
      {}

      void swap(A<T>& other)
      {
        std::cerr << "swap method called" << std::endl;
        T tmp = other.val_;
        other.val_ = val_;
        val_ = tmp;
      }

      T val() const
      {
        return val_;
      }
  };

  template <typename T>
  inline bool operator<(const A<T>& x, const A<T>& y)
  {
    return x.val() < y.val();
  }

  template <typename T>
  void swap(A<T>& x, A<T>& y)
  {
    std::cerr << "nA::swap overload" << std::endl;
    x.swap(y);
  }
} // namespace nA

// This test case checks whether ADL works if std::swap is applied to a template class.
BOOST_AUTO_TEST_CASE(template_class_swap)
{
  using std::swap;
  using namespace nA;
  A<int> x(1);
  A<int> y(2);
  swap(x, y);
  BOOST_CHECK(x.val() == 2);
  BOOST_CHECK(y.val() == 1);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return nullptr;
}
