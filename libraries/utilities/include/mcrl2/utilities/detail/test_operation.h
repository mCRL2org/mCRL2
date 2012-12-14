// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/detail/test_operation.h
/// \brief Function for testing operations.

#ifndef MCRL2_UTILITIES_DETAIL_TEST_OPERATION_H
#define MCRL2_UTILITIES_DETAIL_TEST_OPERATION_H

#include <functional>
#include <string>
#include <boost/test/minimal.hpp>

namespace mcrl2
{

namespace utilities
{

namespace detail
{

template <typename T>
T identity(T t)
{
  return t;
}

/// \brief Generic function that applies two operations to two objects, and compares the results.
template <typename Parser, typename Printer, typename Operation1, typename Operation2, typename Compare>
void test_operation(
  const std::string& expr1,
  const std::string& expr2,
  Parser parse,
  Printer print,
  Compare comp,
  Operation1 op1,
  const std::string& opname1,
  Operation2 op2,
  const std::string& opname2
)
{
  typedef typename Compare::first_argument_type T;
  T x1 = parse(expr1);
  T x2 = parse(expr2);
  T f1 = op1(x1);
  T f2 = op2(x2);
  bool success = comp(f1, f2);
  BOOST_CHECK(success);
  if (success)
  {
    std::cout << "--- TEST SUCCEEDED --- " << std::endl;
  }
  else
  {
    std::cout << "--- TEST FAILED --- " << std::endl;
  }
  std::cout << "x"               << " = " << print(x1) << std::endl;
  std::cout << "y"               << " = " << print(x2) << std::endl;
  if (!opname1.empty())
  {
    std::cout << (opname1 + "(x)") << " = " << print(f1) << std::endl;
  }
  if (!opname2.empty())
  {
    std::cout << (opname2 + "(y)") << " = " << print(f2) << std::endl;
  }
  assert(success);
}

/// \brief Generic function that applies an operation to an object, and compares it
/// with another object.
template <typename Parser, typename Printer, typename Operation, typename Compare>
void test_operation(
  const std::string& expr1,
  const std::string& expr2,
  Parser parse,
  Printer print,
  Compare comp,
  Operation op,
  const std::string& opname
)
{
  typedef typename Compare::first_argument_type T;
  return test_operation(expr1, expr2, parse, print, comp, op, opname, identity<T>, "");
}

} // namespace detail

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_DETAIL_TEST_OPERATION_H
