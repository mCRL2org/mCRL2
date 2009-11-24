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
//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG

#include <iostream>
#include <set>
#include <sstream>
#include <boost/test/minimal.hpp>
#include "mcrl2/core/text_utility.h"
#include "mcrl2/pbes/pbes_parse.h"
#include "mcrl2/data/parser.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/map_substitution.h"
#include "mcrl2/data/detail/data_expression_with_variables.h"
#include "mcrl2/data/detail/parse_substitutions.h"
#include "mcrl2/pbes/pbes_parse.h"
#include "mcrl2/pbes/pbes_expression_with_variables.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

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
  ;

/// print a term (pretty print + ascii representation)
template <typename Term>
std::string ppp(Term t)
{
  std::ostringstream out;
  out << core::pp(t) << " " << t;
  return out.str();
}

template <typename Rewriter1, typename Rewriter2>
void test_expressions(Rewriter1 R1, std::string expr1, Rewriter2 R2, std::string expr2, std::string var_decl = VARIABLE_SPECIFICATION, std::string substitutions = "", std::string data_spec = "")
{
  std::cout << "--- test case --- " << expr1 << " -> " << expr2 << " with substitution " << substitutions << std::endl;
  pbes_expression d1 = pbes_system::parse_pbes_expression(expr1, var_decl, data_spec);
  pbes_expression d2 = pbes_system::parse_pbes_expression(expr2, var_decl, data_spec);

  if (substitutions == "")
  {   
    if (R1(d1) != R2(d2))
    {
      BOOST_CHECK(R1(d1) == R2(d2));
      std::cout << "--- TEST FAILED --- " << std::endl;
    }
    else
    {
      std::cout << "--- TEST SUCCEEDED --- " << std::endl;
    }
    std::cout << "expr1    " << core::pp(d1) << std::endl;
    std::cout << "expr2    " << core::pp(d2) << std::endl;
    std::cout << "R(expr1) " << ppp(R1(d1)) << std::endl;
    std::cout << "R(expr2) " << ppp(R2(d2)) << std::endl << std::endl;
  }
  else
  {
    data::mutable_map_substitution< atermpp::map< data::variable, data::data_expression_with_variables > > sigma;
    data::detail::parse_substitutions(substitutions, sigma);
    if (R1(d1, sigma) != R2(d2))
    {
      BOOST_CHECK(R1(d1, sigma) == R2(d2));
      std::cout << "--- failed test --- " << expr1 << " -> " << expr2 << std::endl;
    }
    else
    {
      std::cout << "--- succeeded test --- " << expr1 << " -> " << expr2 << std::endl;
    }
    std::cout << "d1            " << core::pp(d1) << std::endl;
    std::cout << "d2            " << core::pp(d2) << std::endl;
    std::cout << "sigma         " << substitutions << std::endl;
    std::cout << "R1(d1, sigma) " << ppp(R1(d1, sigma)) << std::endl;
    std::cout << "R2(d2)        " << ppp(R2(d2)) << std::endl << std::endl;
  }
  core::garbage_collect();
}

template <typename Rewriter>
void test_expressions(Rewriter R, std::string expr1, std::string expr2, std::string var_decl = VARIABLE_SPECIFICATION, std::string substitutions = "", std::string data_spec = "")
{
  test_expressions(R, expr1, R, expr2, var_decl, substitutions, data_spec);
}

void test_simplifying_rewriter()
{
  std::cout << "<test_simplifying_rewriter>" << std::endl;

  data::data_specification data_spec = data::data_specification();
  data_spec.make_complete(data::sort_nat::nat());
  data::rewriter datar(data_spec);
  pbes_system::simplifying_quantifier_rewriter<pbes_system::pbes_expression, data::rewriter> R(datar);

  test_expressions(R, "val(n >= 0) || Y(n)"                                             , "val(true)");
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
  test_expressions(R, "forall m:Nat. val(m < 0 && m > 3)"                               , "false");
  test_expressions(R, "forall m:Nat. val(m < 0 && m > 3) => Y(n)"                       , "true");
  test_expressions(R, "forall m:Nat. Y(n)"                                              , "Y(n)");
  test_expressions(R, "forall m:Nat. val(m < 0 && m > 3) || Y(n)"                       , "Y(n)");
  test_expressions(R, "!!X"                                                             , "X");
  test_expressions(R, "forall m:Nat. X"                                                 , "X");
  test_expressions(R, "forall m,n:Nat. Y(n)"                                            , "forall n:Nat. Y(n)");
  test_expressions(R, "forall m,n:Nat. Y(m)"                                            , "forall m:Nat. Y(m)");
  test_expressions(R, "forall b: Bool. forall n: Nat. val(n > 3) || Y(n)"               , "forall n: Nat. val(n > 3) || Y(n)");
  test_expressions(R, "forall n: Nat. forall b: Bool. val(n > 3) || Y(n)"               , "forall n: Nat. val(n > 3) || Y(n)");
  test_expressions(R, "forall n: Nat. val(b) && Y(n)"                                   , "val(b) && forall n: Nat. Y(n)");
  test_expressions(R, "forall n: Nat. val(b)"                                           , "val(b)");

  // test_expressions(R, "Y(n+p) && Y(p+n)"                                                , "Y(n+p)");
  // test_expressions(R, "exists m:Nat. val( m== p) && Y(m)"                               , "Y(p)");
  // test_expressions(R, "X && (Y(p) || X)"                                                , "X");
  // test_expressions(R, "X || (Y(p) && X)"                                                , "X");
  // test_expressions(R, "val(b || !b)"                                                    , "val(true)");
  // test_expressions(R, "Y(n1 + n2)"                                                      , "Y(n2 + n1)");

  // pbes_expression p = R(expr("Y(n)"));
  // BOOST_CHECK(!core::term_traits<pbes_expression>::is_constant(p));
}

void test_enumerate_quantifiers_rewriter()
{
  std::cout << "<test_enumerate_quantifiers_rewriter>" << std::endl;

  data::data_specification data_spec = data::data_specification();
  data_spec.make_complete(data::sort_nat::nat());
  data::rewriter datar(data_spec);
  data::number_postfix_generator generator("UNIQUE_PREFIX");
  data::data_enumerator<data::number_postfix_generator> datae(data_spec, datar, generator);
  data::rewriter_with_variables datarv(data_spec);

  data::variable   v = data::parse_data_expression("n", "n: Pos;\n");
  data::data_expression d = data::parse_data_expression("n < 10", "n: Pos;\n");
  data::data_expression_with_variables dv(d);

  pbes_system::pbes_expression y = pbes_system::parse_pbes_expression("Y(n)", VARIABLE_SPECIFICATION);
  pbes_system::pbes_expression_with_variables yv(y, data::variable_list());

  pbes_system::enumerate_quantifiers_rewriter<pbes_system::pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > R(datarv, datae);

  test_expressions(R, "(Y(0) && Y(1)) => (Y(1) && Y(0))"                                , "true");
  test_expressions(R, "forall b: Bool. forall n: Nat. val(n > 3) || Y(n)"               , "Y(2) && Y(1) && Y(3) && Y(0)");
  test_expressions(R, "(Y(0) && Y(1)) => (Y(0) && Y(1))"                                , "true");
  test_expressions(R, "exists b: Bool. val(if(b, false, b))"                            , "val(false)");
  test_expressions(R, "exists b: Bool. W(b)"                                            , "W(true) || W(false)");
  test_expressions(R, "forall n: Nat.val(!(n < 1)) || Y(n)"                             , "Y(0)");
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
  test_expressions(R, "forall m:Nat. X"                                                 , "X");
}

void test_enumerate_quantifiers_rewriter(std::string expr1, std::string expr2, std::string var_decl, std::string sigma, std::string data_spec)
{
  std::cout << "<enumerate_quantifiers_rewriter>" << std::endl;

  data::data_specification dspec = data::parse_data_specification(data_spec);
  data::rewriter datar(dspec);
  data::number_postfix_generator generator("UNIQUE_PREFIX");
  data::data_enumerator<data::number_postfix_generator> datae(dspec, datar, generator);
  data::rewriter_with_variables datarv(dspec);
  pbes_system::enumerate_quantifiers_rewriter<pbes_system::pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > R(datarv, datae);
  test_expressions(R, expr1, expr2, var_decl, sigma, data_spec);
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

  //------------------------//
  data_spec = "sort Enum = struct e1 | e2;\n";
  var_decl =
    "datavar         \n"
    "predvar         \n"
    "  X: Enum;      \n"
    ;
  expr1 = "exists m1,m2:Enum.(X(m1) || X(m2))";
  expr2 = "X(e1) || X(e2)";
  sigma = "";
  test_enumerate_quantifiers_rewriter(expr1, expr2, var_decl, sigma, data_spec);
}

void test_enumerate_quantifiers_rewriter_finite()
{
  std::cout << "<test_enumerate_quantifiers_rewriter_finite>" << std::endl;

  data::data_specification data_spec = data::data_specification();
  data_spec.make_complete(data::sort_list::list(data::sort_nat::nat()));
  data::rewriter datar(data_spec);
  data::number_postfix_generator generator("UNIQUE_PREFIX");
  data::data_enumerator<data::number_postfix_generator> datae(data_spec, datar, generator);
  data::rewriter_with_variables datarv(data_spec);
  pbes_system::enumerate_quantifiers_rewriter<pbes_system::pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > R(datarv, datae, false);
  pbes_system::simplifying_rewriter<pbes_system::pbes_expression, data::rewriter> S(datar);

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
  specification.make_complete(data::sort_pos::pos());
  data::rewriter  datar(specification);
  pbes_system::simplifying_rewriter<pbes_system::pbes_expression, data::rewriter> r(datar);

  data::mutable_map_substitution< atermpp::map< data::variable, data::data_expression_with_variables > > sigma;
  sigma[data::parse_variable("m: Pos")] = r(data::parse_data_expression("3"));
  sigma[data::parse_variable("n: Pos")] = r(data::parse_data_expression("4"));

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
  core::garbage_collect();
}

template <typename PbesRewriter>
void test_map_substitution_adapter(PbesRewriter r)
{
  atermpp::map<data::variable, data::data_expression_with_variables> sigma;
  pbes_system::pbes_expression x = data::sort_bool::true_();
  pbes_system::pbes_expression y = r(x, data::make_map_substitution_adapter(sigma));
  core::garbage_collect();
}

void test_substitutions2()
{
  std::cout << "<test_substitutions2>" << std::endl;
  data::data_specification data_spec;
  data_spec.make_complete(data::sort_nat::nat());
  data::number_postfix_generator generator("UNIQUE_PREFIX");
  data::rewriter datar(data_spec);
  data::data_enumerator<data::number_postfix_generator> datae(data_spec, datar, generator);
  data::rewriter_with_variables datarv(data_spec);
  pbes_system::enumerate_quantifiers_rewriter<pbes_system::pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > R(datarv, datae);

  std::string var_decl;
  std::string sigma;
  std::string expr1;
  std::string expr2;

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
  test_expressions(R, expr1, expr2, var_decl, sigma);

  //------------------------//
  var_decl =
    "datavar         \n"
    "  n: Nat;       \n"
    "                \n"
    "predvar         \n"
    "  X: Bool, Nat; \n"
    ;
  expr1 = "forall c: Bool. X(c, n)";
  expr2 = "X(true, 0) && X(false, 0)";
  sigma = "b: Bool := true; n: Nat := 0";
  test_expressions(R, expr1, expr2, var_decl, sigma);

  //------------------------//
  var_decl =
    "datavar         \n"
    "predvar         \n"
    "  X: Nat;       \n"
    ;
  expr1 = "exists b: Bool, c: Bool. val(b && c)";
  expr2 = "val(true)";
  sigma = "";
  test_expressions(R, expr1, expr2, var_decl, sigma);

  //------------------------//
  var_decl =
    "datavar         \n"
    "predvar         \n"
    "  X: Nat;       \n"
    ;
  expr1 = "exists b: Bool.exists c:Bool. val(b && c)";
  expr2 = "val(true)";
  sigma = "";
  test_expressions(R, expr1, expr2, var_decl, sigma);

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
  test_expressions(R, expr1, expr2, var_decl, sigma);

  test_map_substitution_adapter(R);
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
  data::number_postfix_generator generator("UNIQUE_PREFIX");
  data::rewriter datar(data_spec);
  data::data_enumerator<data::number_postfix_generator> datae(data_spec, datar, generator);
  data::rewriter_with_variables datarv(data_spec);
  pbes_system::enumerate_quantifiers_rewriter<pbes_system::pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > r(datarv, datae);

  data::mutable_map_substitution< atermpp::map< data::variable, data::data_expression_with_variables > > sigma;
  sigma[data::parse_variable("l_S:Nat")]             = data::parse_data_expression("0");
  sigma[data::parse_variable("m_S:Nat")]             = data::parse_data_expression("0");
  sigma[data::parse_variable("bst_K:Bool")]          = data::parse_data_expression("false");
  sigma[data::parse_variable("bst1_K:Bool")]         = data::parse_data_expression("false");
  sigma[data::parse_variable("k_K:Nat")]             = data::parse_data_expression("0");
  sigma[data::parse_variable("bst2_L:Bool")]         = data::parse_data_expression("false");
  sigma[data::parse_variable("bst3_L:Bool")]         = data::parse_data_expression("false");
  sigma[data::parse_variable("k_L:Nat")]             = data::parse_data_expression("0");
  sigma[data::parse_variable("l'_R:Nat")]            = data::parse_data_expression("0");
  sigma[data::parse_variable("b_R:BBuf", data_spec)] = data::parse_data_expression("[false, false]");

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
  core::garbage_collect();
}

void test_pfnf_rewriter()
{
  using namespace pbes_system;

  pfnf_rewriter<pbes_expression> R;
  pbes_expression x = parse_pbes_expression("val(n1 > 3) && forall b: Bool. forall n: Nat. val(n > 3) || exists n:Nat. val(n > 5)", VARIABLE_SPECIFICATION);
  pbes_expression y = R(x);

  // TODO: add real test cases for PFNF rewriter
  core::garbage_collect();
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_simplifying_rewriter();
  test_enumerate_quantifiers_rewriter();
  test_enumerate_quantifiers_rewriter2();
  test_enumerate_quantifiers_rewriter_finite();
  test_substitutions1();
  test_substitutions2();
  test_substitutions3();
  test_pfnf_rewriter();

#if defined(MCRL2_PBES_EXPRESSION_BUILDER_DEBUG) || defined(MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG)
  BOOST_CHECK(false);
#endif

  return 0;
}
