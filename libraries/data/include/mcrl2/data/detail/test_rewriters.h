// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/test_rewriters.h
/// \brief Functions for testing a rewriter

#ifndef MCRL2_DATA_DETAIL_TEST_REWRITERS_H
#define MCRL2_DATA_DETAIL_TEST_REWRITERS_H

#include "mcrl2/data/builder.h"
#include "mcrl2/data/join.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/detail/print_utility.h"
#include "mcrl2/utilities/optimized_boolean_operators.h"
#include "mcrl2/utilities/detail/test_operation.h"

namespace mcrl2 {

namespace data {

namespace detail {

// Normalizes conjunctions and disjunctions.
template <typename Derived>
struct normalize_and_or_builder: public data_expression_builder<Derived>
{
  typedef data_expression_builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  /// \brief Splits a disjunction into a sequence of operands
  /// Given a data expression of the form p1 || p2 || .... || pn, this will yield a
  /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a || as main
  /// function symbol.
  /// \param expr A data expression
  /// \return A sequence of operands
  std::multiset<data_expression> split_or(const data_expression& expr)
  {
    std::multiset<data_expression> result;
    utilities::detail::split(expr, std::insert_iterator<std::multiset<data_expression> >(result, result.begin()), sort_bool::is_or_application, data::binary_left1, data::binary_right1);
    return result;
  }

  /// \brief Splits a conjunction into a sequence of operands
  /// Given a data expression of the form p1 && p2 && .... && pn, this will yield a
  /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a && as main
  /// function symbol.
  /// \param expr A data expression
  /// \return A sequence of operands
  std::multiset<data_expression> split_and(const data_expression& expr)
  {
    std::multiset<data_expression> result;
    utilities::detail::split(expr, std::insert_iterator<std::multiset<data_expression> >(result, result.begin()), sort_bool::is_and_application, data::binary_left1, data::binary_right1);
    return result;
  }

  data_expression operator()(const application& x)
  {
    data_expression y = super::operator()(x);
    if (sort_bool::is_and_application(y))
    {
      std::multiset<data_expression> s = split_and(y);
      return data::join_and(s.begin(), s.end());
    }
    else if (sort_bool::is_or_application(y))
    {
      std::multiset<data_expression> s = split_or(y);
      return data::join_or(s.begin(), s.end());
    }
    return y;
  }
};

template <typename T>
T normalize_and_or(const T& x,
                   typename boost::enable_if<typename boost::is_base_of< atermpp::aterm, T>::type>::type* = 0
                  )
{
  return core::make_apply_builder<normalize_and_or_builder>()(x);
}

template <typename T>
void normalize_and_or(T& x,
                      typename boost::disable_if<typename boost::is_base_of< atermpp::aterm, T>::type>::type* = 0
                     )
{
  core::make_apply_builder<normalize_and_or_builder>()(x);
}

// Normalizes equalities.
template <typename Derived>
struct normalize_equality_builder: public data_expression_builder<Derived>
{
  typedef data_expression_builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  data_expression operator()(const application& x)
  {
    data_expression y = super::operator()(x);
    if (data::is_equal_to_application(y))
    {
      data_expression left = data::binary_left1(y);
      data_expression right = data::binary_right1(y);
      if (left < right)
      {
        return data::equal_to(left, right);
      }
      else
      {
        return data::equal_to(right, left);
      }
    }
    else if (data::is_not_equal_to_application(y))
    {
      data_expression left = data::binary_left1(y);
      data_expression right = data::binary_right1(y);
      if (left < right)
      {
        return data::not_equal_to(left, right);
      }
      else
      {
        return data::not_equal_to(right, left);
      }
    }
    return y;
  }
};

template <typename T>
T normalize_equality(const T& x,
                     typename boost::enable_if<typename boost::is_base_of< atermpp::aterm, T>::type>::type* = 0
                    )
{
  return core::make_apply_builder<normalize_equality_builder>()(x);
}

template <typename T>
void normalize_equality(T& x,
                        typename boost::disable_if<typename boost::is_base_of< atermpp::aterm, T>::type>::type* = 0
                       )
{
  core::make_apply_builder<normalize_equality_builder>()(x);
}

// normalize operator
template <typename Function>
struct normalizer
{
  const Function& f;

  normalizer(const Function& f0)
    : f(f0)
  {}

  data_expression operator()(const data_expression& t) const
  {
    return detail::normalize_and_or(f(t));
  }
};

// utility function for creating a normalizer
template <typename Function>
normalizer<Function> N(const Function& f)
{
  return normalizer<Function>(f);
}

inline
std::string VARIABLE_SPECIFICATION()
{
  return
    "  b:  Bool;     \n"
    "  b1: Bool;     \n"
    "  b2: Bool;     \n"
    "  b3: Bool;     \n"
    "                \n"
    "  n:  Nat;      \n"
    "  n1: Nat;      \n"
    "  n2: Nat;      \n"
    "  n3: Nat;      \n"
    "                \n"
    "  p:  Pos;      \n"
    "  p1: Pos;      \n"
    "  p2: Pos;      \n"
    "  p3: Pos;      \n"
    ;
}

class parser
{
  protected:
    std::string m_var_decl;
    std::string m_data_spec;

  public:

    parser(const std::string& var_decl = VARIABLE_SPECIFICATION(), const std::string& data_spec = "")
      : m_var_decl(var_decl),
        m_data_spec(data_spec)
    {}

    data_expression operator()(const std::string& expr)
    {
      return parse_data_expression(expr, m_var_decl, m_data_spec);
    }
};

template <typename Rewriter1, typename Rewriter2>
void test_rewriters(Rewriter1 R1, Rewriter2 R2, std::string expr1, std::string expr2, const std::string& var_decl = VARIABLE_SPECIFICATION(), const std::string& data_spec = "")
{
  utilities::detail::test_operation(
    expr1,
    expr2,
    parser(var_decl, data_spec),
    std::equal_to<data_expression>(),
    R1,
    "R1",
    R2,
    "R2"
  );
}

inline
data_expression I(const data_expression& x)
{
  return x;
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_TEST_REWRITERS_H
