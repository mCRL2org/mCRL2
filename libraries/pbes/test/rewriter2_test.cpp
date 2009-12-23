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
#include "mcrl2/core/text_utility.h"
#include "mcrl2/core/detail/test_operation.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/map_substitution.h"
#include "mcrl2/data/detail/data_expression_with_variables.h"
#include "mcrl2/data/detail/parse_substitutions.h"
#include "mcrl2/pbes/detail/normalize_and_or.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/pbes_expression_with_variables.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

// normalize operator
template <typename Function>
struct normalizer
{
	const Function& f;

	normalizer(const Function& f0)
	  : f(f0)
	{}

	pbes_expression operator()(const pbes_expression& t) const
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

// PBES expression parser
class parser
{
protected:
	std::string m_data_spec;
	std::string m_var_decl;

  void init()
  {
    m_var_decl =
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
  }

public:
  parser()
  {
  	init();
  }
	
  parser(const std::string& data_spec)
  	: m_data_spec(data_spec)
  {
  	init();
  }
	
  pbes_expression operator()(const std::string& expr)
  {
    return pbes_system::parse_pbes_expression(expr, m_var_decl, m_data_spec);
  }
};

// PBES expression printer (pretty print + ascii representation)
template <typename Term>
std::string print(const Term& x)
{
  std::ostringstream out;
  out << core::pp(x);
#ifdef PBES_REWRITE_TEST_DEBUG
  out << " " << x;
#endif
  return out.str();
}

template <typename Rewriter1, typename Rewriter2>
void test_simplify(Rewriter1 R1, Rewriter2 R2, std::string expr1, std::string expr2)
{
  core::detail::test_operation(
    expr1,
    expr2,
    parser(),
    print<pbes_expression>,
	  R1,
	  R2,
    "simplify",
    "datarewr",
	  std::equal_to<pbes_expression>()
  );
}

void test_simplifying_rewriter()
{
  std::cout << "<test_simplifying_rewriter>" << std::endl;

  data::data_specification data_spec = data::data_specification();
  data_spec.add_context_sort(data::sort_nat::nat());
  data::rewriter datar(data_spec);
  pbes_system::simplifying_quantifier_rewriter<pbes_system::pbes_expression, data::rewriter> R(datar);
  pbes_system::data_rewriter<pbes_system::pbes_expression, data::rewriter> r(datar);

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

  // test_expressions(R, "Y(n+p) && Y(p+n)"                                                , "Y(n+p)");
  // test_expressions(R, "exists m:Nat. val( m== p) && Y(m)"                               , "Y(p)");
  // test_expressions(R, "X && (Y(p) || X)"                                                , "X");
  // test_expressions(R, "X || (Y(p) && X)"                                                , "X");
  // test_expressions(R, "val(b || !b)"                                                    , "val(true)");
  // test_expressions(R, "Y(n1 + n2)"                                                      , "Y(n2 + n1)");
}

template <typename Rewriter1, typename Rewriter2>
void test_enumerate_quantifiers(Rewriter1 R1, Rewriter2 R2, std::string expr1, std::string expr2)
{
  core::detail::test_operation(
    expr1,
    expr2,
    parser(),
    print<pbes_expression>,
	  R1,
	  R2,
    "quantify",
    "datarewr",
	  std::equal_to<pbes_expression>()
  );
}

void test_enumerate_quantifiers_rewriter()                                                                                                            
{                                                                                                                                                     
  std::cout << "<test_enumerate_quantifiers_rewriter>" << std::endl;                                                                                  
                                                                                                                                                      
  data::data_specification data_spec = data::data_specification();                                                                                    
  data_spec.add_context_sort(data::sort_nat::nat());                                                                                                     
  data::rewriter datar(data_spec);                                                                                                                    
  data::number_postfix_generator generator("UNIQUE_PREFIX");                                                                                          
  data::data_enumerator<data::number_postfix_generator> datae(data_spec, datar, generator);                                                           
  data::rewriter_with_variables datarv(data_spec);                                                                                                    
  pbes_system::enumerate_quantifiers_rewriter<pbes_system::pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > R(datarv, datae);
  pbes_system::data_rewriter<pbes_system::pbes_expression, data::rewriter> r(datar);

  // test_enumerate_quantifiers(N(R), N(r),  "(Y(0) && Y(1)) => (Y(1) && Y(0))"                                , "true");                                                    
  test_enumerate_quantifiers(N(R), N(r),  "forall b: Bool. forall n: Nat. val(n > 3) || Y(n)"               , "Y(2) && Y(1) && Y(3) && Y(0)");                            
  test_enumerate_quantifiers(N(R), N(r),  "(Y(0) && Y(1)) => (Y(0) && Y(1))"                                , "true");                                                    
  test_enumerate_quantifiers(N(R), N(r),  "exists b: Bool. val(if(b, false, b))"                            , "false");                                              
  test_enumerate_quantifiers(N(R), N(r),  "exists b: Bool. W(b)"                                            , "W(true) || W(false)");                                     
  test_enumerate_quantifiers(N(R), N(r),  "forall n: Nat.val(!(n < 1)) || Y(n)"                             , "Y(0)");                                                    
  test_enumerate_quantifiers(N(R), N(r),  "false"                                                           , "false");                                              
  test_enumerate_quantifiers(N(R), N(r),  "true"                                                            , "true");                                               
  test_enumerate_quantifiers(N(R), N(r),  "true && true"                                                    , "true");                                               
  test_enumerate_quantifiers(N(R), N(r),  "(true && true) && true"                                          , "true");                                               
  test_enumerate_quantifiers(N(R), N(r),  "true && false"                                                   , "false");                                              
  test_enumerate_quantifiers(N(R), N(r),  "true => val(b)"                                                  , "val(b)");                                                  
  test_enumerate_quantifiers(N(R), N(r),  "X && true"                                                       , "X");                                                       
  test_enumerate_quantifiers(N(R), N(r),  "true && X"                                                       , "X");                                                       
  test_enumerate_quantifiers(N(R), N(r),  "X && false"                                                      , "false");                                              
  test_enumerate_quantifiers(N(R), N(r),  "X && val(false)"                                                 , "false");                                              
  test_enumerate_quantifiers(N(R), N(r),  "false && X"                                                      , "false");                                              
  test_enumerate_quantifiers(N(R), N(r),  "X && (false && X)"                                               , "false");                                              
  test_enumerate_quantifiers(N(R), N(r),  "Y(1+2)"                                                          , "Y(3)");                                                    
  test_enumerate_quantifiers(N(R), N(r),  "true || true"                                                    , "true");                                                    
  test_enumerate_quantifiers(N(R), N(r),  "(true || true) || true"                                          , "true");                                                    
  test_enumerate_quantifiers(N(R), N(r),  "true || false"                                                   , "true");                                                    
  test_enumerate_quantifiers(N(R), N(r),  "false => X"                                                      , "true");                                                    
  test_enumerate_quantifiers(N(R), N(r),  "Y(n+n)"                                                          , "Y(n+n)");                                                  
  test_enumerate_quantifiers(N(R), N(r),  "Y(n+p)"                                                          , "Y(n+p)");                                                  
  test_enumerate_quantifiers(N(R), N(r),  "forall m:Nat. false"                                             , "false");                                                   
  test_enumerate_quantifiers(N(R), N(r),  "X && X"                                                          , "X");                                                       
  test_enumerate_quantifiers(N(R), N(r),  "val(true)"                                                       , "true");                                                    
  test_enumerate_quantifiers(N(R), N(r),  "false => (exists m:Nat. exists k:Nat. val(m*m == k && k > 20))"  , "true");                                                    
  test_enumerate_quantifiers(N(R), N(r),  "exists m:Nat.true"                                               , "true");                                                    
  test_enumerate_quantifiers(N(R), N(r),  "forall m:Nat.val(m < 3)"                                         , "false");                                                   
  test_enumerate_quantifiers(N(R), N(r),  "exists m:Nat.val(m > 3)"                                         , "true");                                                    
  test_enumerate_quantifiers(N(R), N(r),  "forall m:Nat. X"                                                 , "X");                                                       
  test_enumerate_quantifiers(N(R), N(r),  "exists d: Nat. X && val(d == 0)"                                 , "X");                         
}                                                                                                                                                     

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_simplifying_rewriter();
  test_enumerate_quantifiers_rewriter();

#if defined(MCRL2_PBES_EXPRESSION_BUILDER_DEBUG) || defined(MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG)
  BOOST_CHECK(false);
#endif

  return 0;
}
