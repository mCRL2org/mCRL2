// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rewriter_test.cpp
/// \brief Test for PBES rewriters.

//#define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG
//#define PBES_REWRITE_TEST_DEBUG

#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/detail/data_expression_with_variables.h"
#include "mcrl2/data/detail/parse_substitutions.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/pbes/detail/normalize_and_or.h"
#include "mcrl2/pbes/detail/data2pbes_rewriter.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/pbes/rewriters/simplify_quantifiers_rewriter.h"
#include "mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/utilities/detail/test_operation.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

const std::string VARIABLE_SPECIFICATION =
  "datavar         \n"
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
  "                \n"
  "predvar         \n"
  "  X;            \n"
  "  Y: Nat;       \n"
  "  W: Bool;      \n"
  "  Z: Bool, Nat; \n"
  "  X0;           \n"
  "  X1: Bool;     \n"
  "  X2: Nat, Nat; \n"
  "  X3: Bool, Nat;\n"
  "  X4: Nat, Bool;\n"
  ;

// normalize operator
template <typename Function>
struct normalizer
{
  Function& f;

  normalizer(Function& f0)
    : f(f0)
  {}

  pbes_expression operator()(const pbes_expression& t) const
  {
    return pbes_system::detail::normalize_and_or(f(t));
  }
};

// utility function for creating a normalizer
template <typename Function>
normalizer<Function> N(Function& f)
{
  return normalizer<Function>(f);
}

/// \brief A rewriter that simplifies boolean expressions.
template <typename Rewriter>
class rewriter_with_substitution
{
  protected:
    Rewriter& R;
    data::mutable_map_substitution<> sigma;

  public:
    /// \brief The term type
    typedef typename Rewriter::term_type term_type;

    /// \brief The variable type
    typedef typename Rewriter::variable_type variable_type;

    rewriter_with_substitution(Rewriter& R_, const std::string& substitutions)
      : R(R_)
    {
      data::detail::parse_substitutions(substitutions, sigma);
    }

    /// \brief Rewrites a boolean expression.
    /// \param x A term
    /// \return The rewrite result.
    term_type operator()(const term_type& x)
    {
      return R(x, sigma);
    }
};

template <typename Rewriter>
rewriter_with_substitution<Rewriter> make_rewriter_with_substitution(Rewriter& R, const std::string& sigma)
{
  return rewriter_with_substitution<Rewriter>(R, sigma);
}

// default PBES expression parser
class default_parser
{
  public:
    pbes_expression operator()(const std::string& expr)
    {
      return pbes_system::parse_pbes_expression(expr);
    }
};

// PBES expression parser
class parser
{
  protected:
    std::string m_var_decl;
    std::string m_data_spec;

  public:

    parser(const std::string& var_decl = VARIABLE_SPECIFICATION, const std::string& data_spec = "")
      : m_var_decl(var_decl),
        m_data_spec(data_spec)
    {}

    pbes_expression operator()(const std::string& expr)
    {
      return pbes_system::parse_pbes_expression(expr, m_var_decl, m_data_spec);
    }
};

struct equal_to: public std::binary_function<pbes_expression, pbes_expression, bool>
{
  bool operator()(const pbes_expression& x, const pbes_expression& y) const
  {
    if (x == y)
    {
      return true;
    }
    if (is_true(x) && is_true(y))
    {
      return true;
    }
    if (is_false(x) && is_false(y))
    {
      return true;
    }
    return false;
  }
};

template <typename Rewriter1, typename Rewriter2>
void test_simplify(Rewriter1 R1, Rewriter2 R2, std::string expr1, std::string expr2)
{
  BOOST_CHECK(utilities::detail::test_operation(
    expr1,
    expr2,
    parser(),
    equal_to(),
    R1,
    "simplify",
    R2,
    "datarewr"
  ));
}

template <typename Rewriter1, typename Rewriter2>
void test_rewriters(Rewriter1 R1, Rewriter2 R2, std::string expr1, std::string expr2, const std::string& var_decl = VARIABLE_SPECIFICATION, const std::string& data_spec = "")
{
  BOOST_CHECK(utilities::detail::test_operation(
    expr1,
    expr2,
    parser(var_decl, data_spec),
    equal_to(),
    R1,
    "R1",
    R2,
    "R2"
  ));
}

void test_simplifying_rewriter()
{
  std::cout << "<test_simplifying_rewriter>" << std::endl;

  data::data_specification data_spec = data::data_specification();
  data_spec.add_context_sort(data::sort_nat::nat());
  data::rewriter datar(data_spec);
  pbes_system::simplify_quantifiers_data_rewriter<data::rewriter> R(datar);
  pbes_system::data_rewriter<data::rewriter> r(datar);

  test_simplify(R, r, "val(n >= 0) || Y(n)"                                             , "true");
  test_simplify(R, r, "false"                                                           , "false");
  test_simplify(R, r, "true"                                                            , "true");
  test_simplify(R, r, "true && true"                                                    , "true");
  test_simplify(R, r, "(true && true) && true"                                          , "true");
  test_simplify(R, r, "true && false"                                                   , "false");
  test_simplify(R, r, "true => val(b)"                                                  , "val(b)");
  test_simplify(R, r, "X && true"                                                       , "X");
  test_simplify(R, r, "true && X"                                                       , "X");
  test_simplify(R, r, "X && false"                                                      , "false");
  test_simplify(R, r, "X && val(false)"                                                 , "false");
  test_simplify(R, r, "false && X"                                                      , "false");
  test_simplify(R, r, "X && (false && X)"                                               , "false");
  test_simplify(R, r, "X && (X0 && X)"                                                  , "X && (X0 && X)");
  test_simplify(R, r, "X && (X && X0)"                                                  , "X && (X && X0)");
  test_simplify(R, r, "Y(1+2)"                                                          , "Y(3)");
  test_simplify(R, r, "true || true"                                                    , "true");
  test_simplify(R, r, "(true || true) || true"                                          , "true");
  test_simplify(R, r, "true || false"                                                   , "true");
  test_simplify(R, r, "false => X"                                                      , "true");
  test_simplify(R, r, "Y(n+n)"                                                          , "Y(n+n)");
  test_simplify(R, r, "Y(n+p)"                                                          , "Y(n+p)");
  test_simplify(R, r, "forall m:Nat. false"                                             , "false");
  test_simplify(R, r, "X && X"                                                          , "X");
  test_simplify(R, r, "val(true)"                                                       , "true");
  test_simplify(R, r, "false => (exists m:Nat. exists k:Nat. val(m*m == k && k > 20))"  , "true");
  test_simplify(R, r, "exists m:Nat.true"                                               , "true");
  test_simplify(R, r, "forall m:Nat. val(m < 0 && m > 3)"                               , "false");
  test_simplify(R, r, "forall m:Nat. val(m < 0 && m > 3) => Y(n)"                       , "true");
  test_simplify(R, r, "forall m:Nat. Y(n)"                                              , "Y(n)");
  test_simplify(R, r, "forall m:Nat. val(m < 0 && m > 3) || Y(n)"                       , "Y(n)");
  test_simplify(R, r, "!!X"                                                             , "X");
  test_simplify(R, r, "forall m:Nat. X"                                                 , "X");
  test_simplify(R, r, "forall m,n:Nat. Y(n)"                                            , "forall n:Nat. Y(n)");
  test_simplify(R, r, "forall m,n:Nat. Y(m)"                                            , "forall m:Nat. Y(m)");
  test_simplify(R, r, "forall b: Bool. forall n: Nat. val(n > 3) || Y(n)"               , "forall n: Nat. val(n > 3) || Y(n)");
  test_simplify(R, r, "forall n: Nat. forall b: Bool. val(n > 3) || Y(n)"               , "forall n: Nat. val(n > 3) || Y(n)");
  test_simplify(R, r, "forall n: Nat. val(b) && Y(n)"                                   , "val(b) && forall n: Nat. Y(n)");
  test_simplify(R, r, "forall n: Nat. val(b)"                                           , "val(b)");
  test_simplify(R, r, "forall m: Nat. val(m > 2) && X"                                  , "(forall m: Nat. val(m > 2)) && X");

  // ticket 1273
  test_simplify(R, r, "exists m: Nat. val(n == 0)"                                      , "val(n == 0)");

  // test_expressions(R, "Y(n+p) && Y(p+n)"                                                , "Y(n+p)");
  // test_expressions(R, "exists m:Nat. val( m== p) && Y(m)"                               , "Y(p)");
  // test_expressions(R, "X && (Y(p) || X)"                                                , "X");
  // test_expressions(R, "X || (Y(p) && X)"                                                , "X");
  // test_expressions(R, "val(b || !b)"                                                    , "val(true)");
  // test_expressions(R, "Y(n1 + n2)"                                                      , "Y(n2 + n1)");
}

void test_enumerate_quantifiers_rewriter()
{
  std::cout << "<test_enumerate_quantifiers_rewriter>" << std::endl;

  data::data_specification dataspec = data::data_specification();
  dataspec.add_context_sort(data::sort_nat::nat());
  data::rewriter datar(dataspec);
  pbes_system::data_rewriter<data::rewriter> r(datar);
  enumerate_quantifiers_rewriter R(datar, dataspec);

  // test_rewriters(N(R), N(r),  "(Y(0) && Y(1)) => (Y(1) && Y(0))"                                , "true");
  test_rewriters(N(R), N(r),  "forall n, m: Nat. val(n > 3) || val(b)"                          , "val(b)");
  test_rewriters(N(R), N(r),  "forall b: Bool. forall n: Nat. val(n > 3) || Y(n)"               , "Y(2) && Y(1) && Y(3) && Y(0)");
  test_rewriters(N(R), N(r),  "(Y(0) && Y(1)) => (Y(0) && Y(1))"                                , "true");
  test_rewriters(N(R), N(r),  "exists b: Bool. val(if(b, false, b))"                            , "false");
  test_rewriters(N(R), N(r),  "exists b: Bool. W(b)"                                            , "W(true) || W(false)");
  test_rewriters(N(R), N(r),  "forall n: Nat.val(!(n < 1)) || Y(n)"                             , "Y(0)");
  test_rewriters(N(R), N(r),  "false"                                                           , "false");
  test_rewriters(N(R), N(r),  "true"                                                            , "true");
  test_rewriters(N(R), N(r),  "true && true"                                                    , "true");
  test_rewriters(N(R), N(r),  "(true && true) && true"                                          , "true");
  test_rewriters(N(R), N(r),  "true && false"                                                   , "false");
  test_rewriters(N(R), N(r),  "true => val(b)"                                                  , "val(b)");
  test_rewriters(N(R), N(r),  "X && true"                                                       , "X");
  test_rewriters(N(R), N(r),  "true && X"                                                       , "X");
  test_rewriters(N(R), N(r),  "X && false"                                                      , "false");
  test_rewriters(N(R), N(r),  "X && val(false)"                                                 , "false");
  test_rewriters(N(R), N(r),  "false && X"                                                      , "false");
  test_rewriters(N(R), N(r),  "X && (false && X)"                                               , "false");
  test_rewriters(N(R), N(r),  "Y(1+2)"                                                          , "Y(3)");
  test_rewriters(N(R), N(r),  "true || true"                                                    , "true");
  test_rewriters(N(R), N(r),  "(true || true) || true"                                          , "true");
  test_rewriters(N(R), N(r),  "true || false"                                                   , "true");
  test_rewriters(N(R), N(r),  "false => X"                                                      , "true");
  test_rewriters(N(R), N(r),  "Y(n+n)"                                                          , "Y(n+n)");
  test_rewriters(N(R), N(r),  "Y(n+p)"                                                          , "Y(n+p)");
  test_rewriters(N(R), N(r),  "forall m:Nat. false"                                             , "false");
  test_rewriters(N(R), N(r),  "X && X"                                                          , "X");
  test_rewriters(N(R), N(r),  "val(true)"                                                       , "true");
  // test_rewriters(N(R), N(r),  "false => (exists m:Nat. exists k:Nat. val(m*m == k && k > 20))"  , "true");
  test_rewriters(N(R), N(r),  "exists m:Nat.true"                                               , "true");
  test_rewriters(N(R), N(r),  "forall m:Nat.val(m < 3)"                                         , "false");
  test_rewriters(N(R), N(r),  "exists m:Nat.val(m > 3)"                                         , "true");
  test_rewriters(N(R), N(r),  "forall m:Nat. X"                                                 , "X");
  test_rewriters(N(R), N(r),  "exists d: Nat. X && val(d == 0)"                                 , "X");
  test_rewriters(N(R), N(r),  "forall m: Nat. (val(!(m < 3)) || Y(m + 1))"                      , "Y(1) && Y(2) && Y(3)");
  test_rewriters(N(R), N(r),  "val(!true) || (((forall m: Nat. val(!(m < 3)) && X3(false, m + 1)) && X2(1, 1) && val(!false) || val(false)) || (exists m: Nat. val(m < 3) || (forall k: Nat. val(k < 3) && val(k < 2)))) && X1(false)", "X1(false)");
  test_rewriters(N(R), N(r),  "forall n: Nat. (val(n < 3) && (exists n: Nat. val(n < 3)))", "false");
  test_rewriters(N(R), N(r),  "Y(n + 1) || forall n: Nat. val(n < 3)", "Y(n + 1)");
  test_rewriters(N(R), N(r),  "forall b: Bool . val(b) || X1(b)", "X1(false)");
}

template <typename Rewriter1, typename Rewriter2>
void test_expressions(Rewriter1& R1, std::string expr1, Rewriter2& R2, std::string expr2, std::string var_decl = VARIABLE_SPECIFICATION, std::string substitutions = "", std::string data_spec = "")
{
  if (substitutions == "")
  {
    test_rewriters(N(R1), N(R2), expr1, expr2, var_decl, data_spec);
  }
  else
  {
    rewriter_with_substitution<Rewriter1> r1(R1, substitutions);
    test_rewriters(N(r1), N(R2), expr1, expr2, var_decl, data_spec);
  }
}

void test_enumerate_quantifiers_rewriter(std::string expr1, std::string expr2, std::string var_decl, std::string sigma, std::string data_spec)
{
  std::cout << "<enumerate_quantifiers_rewriter>" << std::endl;

  data::data_specification dspec = data::parse_data_specification(data_spec);
  data::rewriter datar(dspec);
  pbes_system::enumerate_quantifiers_rewriter R(datar, dspec);
  test_expressions(R, expr1, R, expr2, var_decl, sigma, data_spec);
}

void test_enumerate_quantifiers_rewriter2()
{
  std::string var_decl;
  std::string data_spec;
  std::string sigma;
  std::string expr1;
  std::string expr2;

  //------------------------//
  data_spec = "sort Enum = struct e1 | e2;\n";
  var_decl =
    "datavar         \n"
    "predvar         \n"
    "  X: Enum;      \n"
    ;
  expr1 = "exists m:Enum.(X(m))";
  expr2 = "X(e1) || X(e2)";
  sigma = "";
  test_enumerate_quantifiers_rewriter(expr1, expr2, var_decl, sigma, data_spec);

  data_spec = "sort Enum = struct e1 | e2;\n";
  var_decl =
    "datavar         \n"
    "predvar         \n"
    "  X: Enum;      \n"
    ;
  expr1 = "exists m1,m2:Enum.(X(m1) || X(m2))";
  expr2 = "X(e1) || X(e2)";
  sigma = "";
  //test_enumerate_quantifiers_rewriter(expr1, expr2, var_decl, sigma, data_spec);
  // This test fails with output
  // x = exists m1,m2: Enum. X(m1) || X(m2)
  // y = X(e1) || X(e2)
  // R1(x) = ((X(e1) || X(e1)) || X(e2)) || X(e2)
  // R2(y) = X(e1) || X(e2)
}

void test_enumerate_quantifiers_rewriter_finite()
{
  std::cout << "<test_enumerate_quantifiers_rewriter_finite>" << std::endl;

  data::data_specification data_spec = data::data_specification();
  data_spec.add_context_sort(data::sort_list::list(data::sort_nat::nat()));
  data::rewriter datar(data_spec);
  pbes_system::enumerate_quantifiers_rewriter R(datar, data_spec, false);
  pbes_system::simplify_data_rewriter<data::rewriter> S(datar);

  test_expressions(R, "forall n:Nat, b:Bool.Z(b,n)", S, "forall n:Nat.Z(false,n) && Z(true,n)");
  test_expressions(R, "forall n:Nat. Y(n)", S, "forall n:Nat. Y(n)");

  std::string expr1;
  std::string expr2;
  std::string sigma;
  std::string var_decl;

  //------------------------//
  var_decl =
    "datavar          \n"
    "  m: Nat;        \n"
    "  q: List(Nat);  \n"
    "                 \n"
    "predvar          \n"
    "  X: Nat, List(Nat), Nat;\n"
    ;
  expr1 = "forall k: Nat. val(!(k < m)) || X(m, q, q . (k mod 4))";
  expr2 = "forall k: Nat. val(!(k < m)) || X(m, q, q . (k mod 4))";
  sigma = "";
  test_expressions(R, expr1, S, expr2, var_decl, sigma);
}

void test_substitutions1()
{
  std::cout << "<test_substitutions1>" << std::endl;

  data::data_specification specification;
  specification.add_context_sort(data::sort_pos::pos());
  data::rewriter  datar(specification);
  pbes_system::simplify_data_rewriter<data::rewriter> r(datar);

  data::mutable_map_substitution<> sigma;
  sigma[data::parse_variable("m: Pos")] = datar(data::parse_data_expression("3"));
  sigma[data::parse_variable("n: Pos")] = datar(data::parse_data_expression("4"));

  std::cout << "<test_substitutions1aaa>" << std::endl;
  std::string var_decl =
    "datavar         \n"
    "  m, n:  Pos;   \n"
    "                \n"
    "predvar         \n"
    "  X: Pos;       \n"
    ;
  pbes_system::pbes_expression d1 = pbes_system::parse_pbes_expression("X(m+n)", var_decl);
  pbes_system::pbes_expression d2 = pbes_system::parse_pbes_expression("X(7)", var_decl);
  BOOST_CHECK(r(d1, sigma) == r(d2));
}

void test_substitutions2()
{
  std::cout << "<test_substitutions2>" << std::endl;
  data::data_specification data_spec;
  data_spec.add_context_sort(data::sort_nat::nat());
  data::rewriter datar(data_spec);
  pbes_system::enumerate_quantifiers_rewriter R(datar, data_spec);

  std::string var_decl;
  std::string sigma;
  std::string expr1;
  std::string expr2;
  std::string expr3;

  //------------------------//
  var_decl =
    "datavar         \n"
    "  m, n:  Pos;   \n"
    "                \n"
    "predvar         \n"
    "  X: Pos;       \n"
    ;
  expr1 = "X(m+n)";
  expr2 = "X(7)";
  sigma = "m: Pos := 3; n: Pos := 4";
  test_expressions(R, expr1, R, expr2, var_decl, sigma);

  //------------------------//
  var_decl =
    "datavar         \n"
    "  n: Nat;       \n"
    "                \n"
    "predvar         \n"
    "  X: Bool, Nat; \n"
    ;
  expr1 = "forall c: Bool. X(c, n)";
  expr2 = "X(false, 0) && X(true, 0)";
  sigma = "b: Bool := false; n: Nat := 0";
  test_expressions(R, expr1, R, expr2, var_decl, sigma);

  //------------------------//
  var_decl =
    "datavar         \n"
    "predvar         \n"
    "  X: Nat;       \n"
    ;
  expr1 = "exists b: Bool, c: Bool. val(b && c)";
  expr2 = "val(true)";
  sigma = "";
  test_expressions(R, expr1, R, expr2, var_decl, sigma);

  //------------------------//
  var_decl =
    "datavar         \n"
    "predvar         \n"
    "  X: Nat;       \n"
    ;
  expr1 = "exists b: Bool.exists c:Bool. val(b && c)";
  expr2 = "val(true)";
  sigma = "";
  test_expressions(R, expr1, R, expr2, var_decl, sigma);

  //------------------------//
  var_decl =
    "datavar         \n"
    "  b: Bool;      \n"
    "predvar         \n"
    "  X: Nat;       \n"
    ;
  expr1 = "!!val(!!b)";
  expr2 = "val(true)";
  sigma = "b:Bool := true";
  test_expressions(R, expr1, R, expr2, var_decl, sigma);
}

void test_substitutions3()
{
  std::cout << "<test_substitutions3>" << std::endl;
  std::string DATA_SPEC =
    "sort D = struct d1 | d2;                                                                                                   \n"
    "     DBuf = List(D);                                                                                                       \n"
    "     BBuf = List(Bool);                                                                                                    \n"
    "                                                                                                                           \n"
    "map  n: Pos;                                                                                                               \n"
    "     empty: BBuf;                                                                                                          \n"
    "     insert: D # Nat # DBuf -> DBuf;                                                                                       \n"
    "     insert: Bool # Nat # BBuf -> BBuf;                                                                                    \n"
    "     nextempty_mod: Nat # BBuf # Nat # Pos -> Nat;                                                                         \n"
    "     q1,q2: DBuf;                                                                                                          \n"
    "                                                                                                                           \n"
    "var  d,d': D;                                                                                                              \n"
    "     i,j,m: Nat;                                                                                                           \n"
    "     q: DBuf;                                                                                                              \n"
    "     c,c': Bool;                                                                                                           \n"
    "     n': Pos;                                                                                                              \n"
    "     b: BBuf;                                                                                                              \n"
    "eqn  q1  =  [d1, d1];                                                                                                      \n"
    "     q2  =  [d1, d1];                                                                                                      \n"
    "     n  =  2;                                                                                                              \n"
    "     q1  =  [d1, d1];                                                                                                      \n"
    "     q2  =  [d1, d1];                                                                                                      \n"
    "     empty  =  [false, false];                                                                                             \n"
    "     i == 0  ->  insert(d, i, q)  =  d |> tail(q);                                                                         \n"
    "     i > 0  ->  insert(d, i, d' |> q)  =  d' |> insert(d, Int2Nat(i - 1), q);                                              \n"
    "     i == 0  ->  insert(c, i, b)  =  c |> tail(b);                                                                         \n"
    "     i > 0  ->  insert(c, i, c' |> b)  =  c' |> insert(c, Int2Nat(i - 1), b);                                              \n"
    "     b . (i mod n') && m > 0  ->  nextempty_mod(i, b, m, n')  =  nextempty_mod((i + 1) mod 2 * n', b, Int2Nat(m - 1), n'); \n"
    "     !(b . (i mod n') && m > 0)  ->  nextempty_mod(i, b, m, n')  =  i mod 2 * n';                                          \n"
    ;
  data::data_specification data_spec = data::parse_data_specification(DATA_SPEC);
  data::rewriter datar(data_spec);
  pbes_system::enumerate_quantifiers_rewriter r(datar, data_spec);

  data::mutable_map_substitution<> sigma;
  sigma[data::parse_variable("l_S:Nat")]             = data::parse_data_expression("0");
  sigma[data::parse_variable("m_S:Nat")]             = data::parse_data_expression("0");
  sigma[data::parse_variable("bst_K:Bool")]          = data::parse_data_expression("false");
  sigma[data::parse_variable("bst1_K:Bool")]         = data::parse_data_expression("false");
  sigma[data::parse_variable("k_K:Nat")]             = data::parse_data_expression("0");
  sigma[data::parse_variable("bst2_L:Bool")]         = data::parse_data_expression("false");
  sigma[data::parse_variable("bst3_L:Bool")]         = data::parse_data_expression("false");
  sigma[data::parse_variable("k_L:Nat")]             = data::parse_data_expression("0");
  sigma[data::parse_variable("l'_R:Nat")]            = data::parse_data_expression("0");
  sigma[data::parse_variable("b_R:BBuf", data_spec)] = data::normalize_sorts(data::parse_data_expression("[false, false]"),data_spec);

  std::string var_decl =
    "datavar                                                     \n"
    "  l_S:Nat    ;                                              \n"
    "  m_S:Nat    ;                                              \n"
    "  bst_K:Bool ;                                              \n"
    "  bst1_K:Bool;                                              \n"
    "  k_K:Nat    ;                                              \n"
    "  bst2_L:Bool;                                              \n"
    "  bst3_L:Bool;                                              \n"
    "  k_L:Nat    ;                                              \n"
    "  l'_R:Nat   ;                                              \n"
    "  b_R:BBuf   ;                                              \n"
    "                                                            \n"
    "predvar                                                     \n"
    "  X: Nat, Nat, Bool, Bool, Nat, Bool, Bool, Nat, Nat, BBuf; \n"
    ;

  pbes_system::pbes_expression phi = pbes_system::parse_pbes_expression("forall k_S2_00: Nat. val(!(k_S2_00 < m_S && !bst_K && !bst1_K)) || X(l_S, m_S, false, true, (l_S + k_S2_00) mod 4, bst2_L, bst3_L, k_L, l'_R, b_R)", var_decl, DATA_SPEC);
  pbes_system::pbes_expression x = r(phi, sigma);
}

void test_substitutions4()
{
  std::cout << "<test_substitutions4>" << std::endl;
  data::data_specification data_spec;
  data_spec.add_context_sort(data::sort_nat::nat());
  data::rewriter datar(data_spec);
  pbes_system::simplify_data_rewriter<data::rewriter> R(datar);

  std::string var_decl;
  std::string sigma;
  std::string expr1;
  std::string expr2;
  std::string expr3;

  //------------------------//
  var_decl =
    "datavar         \n"
    "  m, n:  Pos;   \n"
    "                \n"
    "predvar         \n"
    "  X: Pos;       \n"
    ;
  expr1 = "val(m < n) && X(m + n)";
  expr2 = "X(7)";
  sigma = "m: Pos := 3; n: Pos := 4";
  // test_expressions(R, expr1, R, expr2, var_decl, sigma);

  //------------------------//
  var_decl =
    "datavar         \n"
    "  n1, n2: Nat;  \n"
    "                \n"
    "predvar         \n"
    "  X: Pos;       \n"
    ;
  expr1 = "!(forall m: Nat. false) && !(exists m: Nat. false) && !(forall m: Nat. false) && !(exists m: Nat. val(!(n1 > n2))) && !(forall m: Nat. false) && !(exists m: Nat. val(!(n1 > n2)))";
  expr1 = "!(exists m: Nat. val(!(n1 > n2)))";
  expr2 = "true";
  sigma = "n2:Nat := 2; n1:Nat := 1";
  // test_expressions(R, expr1, R, expr2, var_decl, sigma);
}

void test_data2pbes()
{
  std::cout << "<test_data2pbes>" << std::endl;
  pbes_system::pbes_expression x;
  pbes_system::pbes_expression y;

  x = pbes_system::parse_pbes_expression("forall n: Nat. val(n != 3 && n == 5)");
  y = pbes_system::detail::data2pbes(x);
  std::clog << "x = " << pbes_system::pp(x) << std::endl;
  std::clog << "y = " << pbes_system::pp(y) << std::endl;
  BOOST_CHECK(pbes_system::pp(y) == "forall n: Nat. val(n != 3) && val(n == 5)");

  x = pbes_system::parse_pbes_expression("val(forall n: Nat. n != 3 && n == 5)");
  y = pbes_system::detail::data2pbes(x);
  std::clog << "x = " << pbes_system::pp(x) << std::endl;
  std::clog << "y = " << pbes_system::pp(y) << std::endl;
  BOOST_CHECK(pbes_system::pp(y) == "forall n: Nat. val(n != 3) && val(n == 5)");
}

int test_main(int argc, char* argv[])
{
  log::mcrl2_logger::set_reporting_level(log::debug);

  test_simplifying_rewriter();
  test_enumerate_quantifiers_rewriter();
  test_enumerate_quantifiers_rewriter2();
  test_enumerate_quantifiers_rewriter_finite();
  test_substitutions1();
  test_substitutions2();
  test_substitutions3();
  test_substitutions4();
  test_data2pbes();

  return 0;
}
