// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rewriter_test.cpp
/// \brief Test for the pbes rewriters.

//#define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include <iostream>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/pbes/pbes_parse.h"
#include "mcrl2/data/parser.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/data_expression_with_variables.h"
#include "mcrl2/pbes/pbes_parse.h"
#include "mcrl2/pbes/pbes_expression_with_variables.h"
#include "mcrl2/pbes/rewriter.h"

using namespace mcrl2;

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
  "  Z: Bool, Pos; \n"
  ;

inline
pbes_expression expr(const std::string& text)
{
  return pbes_system::parse_pbes_expression(text, VARIABLE_SPECIFICATION);
}

template <typename Rewriter>
void test_expressions(Rewriter R, std::string expr1, std::string expr2)
{
  if (R(expr(expr1)) != R(expr(expr2)))
  {
    BOOST_CHECK(R(expr(expr1)) == R(expr(expr2)));
    std::cout << "--- failed test --- " << expr1 << " -> " << expr2 << std::endl;
    std::cout << "expr1    " << core::pp(expr(expr1)) << std::endl;
    std::cout << "expr2    " << core::pp(expr(expr2)) << std::endl;
    std::cout << "R(expr1) " << core::pp(R(expr(expr1))) << std::endl;
    std::cout << "R(expr2) " << core::pp(R(expr(expr2))) << std::endl;
    std::cout << "R(expr1) " << R(expr(expr1)) << std::endl;
    std::cout << "R(expr2) " << R(expr(expr2)) << std::endl;
  }
}

void test_simplifying_rewriter()
{
  std::cout << "<test_simplifying_rewriter>" << std::endl;
  data::rewriter datar = data::default_data_rewriter();
  pbes_system::simplifying_rewriter<pbes_system::pbes_expression, data::rewriter> R(datar);

  test_expressions(R, "false"                                                           , "val(false)");
  test_expressions(R, "true"                                                            , "val(true)");
  test_expressions(R, "true && true"                                                    , "val(true)");
  test_expressions(R, "(true && true) && true"                                          , "val(true)");
  test_expressions(R, "true && false"                                                   , "val(false)");
  test_expressions(R, "true => val(b)"                                                  , "val(b)");
  test_expressions(R, "X && true"                                                       , "X");
  test_expressions(R, "true && X"                                                       , "X");
  test_expressions(R, "X && false"                                                      , "val(false)");
  test_expressions(R, "X && val(false)"                                                 , "val(false)");
  test_expressions(R, "false && X"                                                      , "val(false)");
  test_expressions(R, "X && (false && X)"                                               , "val(false)");
  test_expressions(R, "Y(1+2)"                                                          , "Y(3)");
  test_expressions(R, "true || true"                                                    , "true");
  test_expressions(R, "(true || true) || true"                                          , "true");
  test_expressions(R, "true || false"                                                   , "true");
  test_expressions(R, "false => X"                                                      , "true");
  test_expressions(R, "Y(n+n)"                                                          , "Y(n+n)");
  test_expressions(R, "Y(n+p)"                                                          , "Y(n+p)");
  test_expressions(R, "forall m:Nat. false"                                             , "false");
  test_expressions(R, "X && X"                                                          , "X");
  test_expressions(R, "val(true)"                                                       , "true");  
  test_expressions(R, "false => (exists m:Nat. exists k:Nat. val(m*m == k && k > 20))"  , "true");
  test_expressions(R, "exists m:Nat.true"                                               , "true");

  // test_expressions(R, "Y(n+p) && Y(p+n)"                                                , "Y(n+p)");
  // test_expressions(R, "exists m:Nat. val( m== p) && Y(m)"                               , "Y(p)");
  // test_expressions(R, "X && (Y(p) || X)"                                                , "X");
  // test_expressions(R, "X || (Y(p) && X)"                                                , "X");
  // test_expressions(R, "val(b || !b)"                                                    , "val(true)");
  // test_expressions(R, "Y(n1 + n2)"                                                      , "Y(n2 + n1)");
}

template <typename variable_type, typename data_term_type>
void test_enumerate_quantifiers_sequence_assign(variable_type v, data_term_type t)
{
  typedef data::rewriter_map<std::map<variable_type, data_term_type> > substitution_map;
  substitution_map sigma;
  pbes_system::detail::enumerate_quantifiers_sequence_assign<substitution_map> assign(sigma);
  assign(v, t);
}

template <typename PbesTerm>
void test_enumerate_quantifiers_sequence_action(PbesTerm phi)
{
  typedef typename core::term_traits<PbesTerm>::variable_type variable_type;
  typedef typename core::term_traits<PbesTerm>::data_term_type data_term_type;
  typedef data::rewriter_map<std::map<variable_type, data_term_type> > substitution_map;
  data::rewriter datar = data::default_data_specification();
  std::set<PbesTerm> A;
  pbes_system::simplifying_rewriter<PbesTerm, data::rewriter> r(datar);
  substitution_map sigma;
  bool is_constant;
  data::data_variable_list v;
  pbes_system::detail::make_enumerate_quantifiers_sequence_action(A, r, phi, sigma, v, is_constant, core::term_traits<PbesTerm>::is_false)();
}

template <typename PbesTerm, typename DataEnumerator>
void test_enumerator_quantifiers(PbesTerm phi, DataEnumerator datae)
{
  typedef typename core::term_traits<PbesTerm>::variable_type variable_type;
  typedef typename core::term_traits<PbesTerm>::variable_sequence_type variable_sequence_type;
  typedef typename core::term_traits<PbesTerm>::data_term_type data_term_type;
  typedef data::rewriter_map<std::map<variable_type, data_term_type> > substitution_map;
  data::rewriter datar = data::default_data_specification();
  pbes_system::simplifying_rewriter<PbesTerm, data::rewriter> r(datar);
  substitution_map sigma;

  variable_sequence_type variables;

  PbesTerm result =
  pbes_system::detail::enumerate_quantifiers(variables,
                                             phi,
                                             sigma,
                                             datae,
                                             r,
                                             core::term_traits<PbesTerm>::is_true,
                                             core::term_traits<PbesTerm>::true_(),
                                             pbes_system::detail::enumerate_quantifiers_join_or<PbesTerm>()
                                          );
}

void test_enumerate_quantifiers_rewriter()
{
  std::cout << "<test_enumerate_quantifiers_rewriter>" << std::endl;

  data::data_specification data_spec = default_data_specification();
  data::rewriter datar(data_spec);
  data::number_postfix_generator generator("UNIQUE_PREFIX");
  data::data_enumerator<data::rewriter, data::number_postfix_generator> datae(data_spec, datar, generator);

  data::data_variable   v = data::parse_data_expression("n", "n: Pos;\n");
  data::data_expression d = data::parse_data_expression("n < 10", "n: Pos;\n");
  data::data_expression_with_variables dv(d, data::data_variable_list());
  test_enumerate_quantifiers_sequence_assign(v, d);
  test_enumerate_quantifiers_sequence_assign(v, dv);

  pbes_system::pbes_expression y = expr("Y(n)");
  pbes_system::pbes_expression_with_variables yv(y, data::data_variable_list()); 
  // test_enumerate_quantifiers_sequence_action(y);
  test_enumerate_quantifiers_sequence_action(yv);

  // test_enumerator_quantifiers(y, datae); This doesn't work because of a mismatch between y and datae
  test_enumerator_quantifiers(yv, datae);

  pbes_system::enumerate_quantifiers_rewriter<pbes_system::pbes_expression, data::rewriter, data::data_enumerator<> > R(datar, datae);

  test_expressions(R, "false"                                                           , "val(false)");
  test_expressions(R, "true"                                                            , "val(true)");
  test_expressions(R, "true && true"                                                    , "val(true)");
  test_expressions(R, "(true && true) && true"                                          , "val(true)");
  test_expressions(R, "true && false"                                                   , "val(false)");
  test_expressions(R, "true => val(b)"                                                  , "val(b)");
  test_expressions(R, "X && true"                                                       , "X");
  test_expressions(R, "true && X"                                                       , "X");
  test_expressions(R, "X && false"                                                      , "val(false)");
  test_expressions(R, "X && val(false)"                                                 , "val(false)");
  test_expressions(R, "false && X"                                                      , "val(false)");
  test_expressions(R, "X && (false && X)"                                               , "val(false)");
  test_expressions(R, "Y(1+2)"                                                          , "Y(3)");
  test_expressions(R, "true || true"                                                    , "true");
  test_expressions(R, "(true || true) || true"                                          , "true");
  test_expressions(R, "true || false"                                                   , "true");
  test_expressions(R, "false => X"                                                      , "true");
  test_expressions(R, "Y(n+n)"                                                          , "Y(n+n)");
  test_expressions(R, "Y(n+p)"                                                          , "Y(n+p)");
  test_expressions(R, "forall m:Nat. false"                                             , "false");
  test_expressions(R, "X && X"                                                          , "X");
  test_expressions(R, "val(true)"                                                       , "true");  
  test_expressions(R, "false => (exists m:Nat. exists k:Nat. val(m*m == k && k > 20))"  , "true");
  test_expressions(R, "exists m:Nat.true"                                               , "true");
  test_expressions(R, "forall m:Nat.val(m < 3)"                                         , "false");
  test_expressions(R, "exists m:Nat.val(m > 3)"                                         , "true");
}

void test_substitutions()
{
  std::cout << "<test_substitutions>" << std::endl;

  data::rewriter datar = data::default_data_rewriter();
  pbes_system::simplifying_rewriter<pbes_system::pbes_expression, data::rewriter> r(datar);

  data::rewriter_map<atermpp::map<data::data_variable, pbes_system::pbes_expression> > sigma; 
  sigma[data::parse_data_expression("m", "m: Pos;")] = r(data::parse_data_expression("3"));
  sigma[data::parse_data_expression("n", "n: Pos;")] = r(data::parse_data_expression("4"));

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

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_simplifying_rewriter();
  test_enumerate_quantifiers_rewriter();
  test_substitutions();

  return 0;
}
