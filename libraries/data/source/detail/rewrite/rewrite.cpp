// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "boost.hpp" // precompiled headers

#include <cstdlib>
#include <cassert>
#include <stdexcept>
#include <string>
#include <cstring>
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/aterm_ext.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/print.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/rewrite/jitty.h"
#ifdef MCRL2_JITTYC_AVAILABLE
#include "mcrl2/data/detail/rewrite/jittyc.h"
#endif

#include "mcrl2/data/detail/rewrite/with_prover.h"

#include "mcrl2/data/fresh_variable_generator.h"
#include "mcrl2/data/detail/enum/standard.h"

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/detail/rewrite/nfs_array.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace std;
using namespace mcrl2::log;

namespace mcrl2
{
namespace data
{
namespace detail
{

Rewriter::Rewriter()
{
}

Rewriter::~Rewriter()
{
}

data_expression_list Rewriter::rewrite_list(
     const data_expression_list terms,
     mutable_map_substitution<> &sigma)
{
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(term_array,data_expression, terms.size());
  size_t position=0;
  for(data_expression_list::const_iterator i=terms.begin(); i!=terms.end(); ++i,++position)
  {
    term_array[position]=rewrite(*i,sigma);
  }
  ++position;
  data_expression_list l;
  for( ; position>0 ; )
  {
    --position;
    l = push_front(l,term_array[position]);
  }
  return l;
}

atermpp::aterm_appl Rewriter::toRewriteFormat(const data_expression Term)
{
  assert(0);
  return atermpp::aterm_appl();
}

data_expression Rewriter::fromRewriteFormat(const atermpp::aterm_appl Term)
{
  assert(0);
  return data_expression();
}

atermpp::aterm_appl Rewriter::rewrite_internal(
     const atermpp::aterm_appl Term, 
     mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma)
{
  assert(0);
}

atermpp::term_list<atermpp::aterm_appl> Rewriter::rewrite_internal_list(
    const atermpp::term_list<atermpp::aterm_appl> terms, 
    mutable_map_substitution<atermpp::map < variable, atermpp::aterm_appl> > &sigma)
{
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(term_array, atermpp::aterm_appl, terms.size());
  size_t position=0;
  for(atermpp::term_list<atermpp::aterm_appl>::const_iterator i=terms.begin(); i!=terms.end(); ++i,++position)
  {
    term_array[position]=rewrite_internal(*i,sigma);
  }
  atermpp::term_list<atermpp::aterm_appl> l;
  for( ; position>0 ; )
  {
    --position;
    l = push_front(l,term_array[position]);
  }
  return l;
} 

bool Rewriter::addRewriteRule(const data_equation /*Rule*/)
{
  return false;
}

bool Rewriter::removeRewriteRule(const data_equation /*Rule*/)
{
  return false;
}

atermpp::aterm_appl Rewriter::internal_existential_quantifier_enumeration(
     const atermpp::aterm_appl ATermInInnerFormat,
     mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma) 
{
  /* Get Body of Exists */
  ATerm t1 = ATgetArgument(ATermInInnerFormat,1);
  data_expression d(fromRewriteFormat(t1));

  /* Get Sort for enumeration from Body*/
  sort_expression_list fsdomain = function_sort(d.sort()).domain();

  static data::fresh_variable_generator<> generator;
  generator.add_identifiers(find_identifiers(d));
  generator.set_hint("var");

  /* Create for each of the sorts for enumeration a new variable*/
  variable_vector vv;
  for(sort_expression_list::iterator i = fsdomain.begin(); i != fsdomain.end(); ++i)
  {
    variable v(generator(*i));
    vv.push_back(v);
  }

  /* Create Enumerator */
  EnumeratorStandard ES(m_data_specification_for_enumeration, this);

  /* Find A solution*/
  const variable_list vl=atermpp::convert< variable_list >(vv);

  EnumeratorSolutionsStandard sol(vl,
      toRewriteFormat(application (d, atermpp::convert< data_expression_list >(vv))),
      sigma,true,&ES,100);

  /* Create ATermList to store solutions */
  atermpp::term_list<atermpp::aterm_appl> x;
  bool has_exact_solution = false;
  bool has_no_solution =true;
  bool solution_possible=true;

  size_t loop_upperbound=5;
  while (loop_upperbound>0 && sol.next(has_exact_solution,x,solution_possible) && !has_exact_solution)
  {
    has_no_solution = false;
    loop_upperbound--;
  }

  if (solution_possible)
  {
    if (has_exact_solution)
    {
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
      std::cerr << "  return true (exist)\n";
#endif
      return toRewriteFormat(mcrl2::data::sort_bool::true_());
    }
    else if (has_no_solution)
    {
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
      std::cerr << "  return false (exist)\n";
#endif
      return toRewriteFormat(mcrl2::data::sort_bool::false_());
    }
  }
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
  std::cerr << "An existential quantifier could not be eliminated and remains unchanged.\n";
#endif

 return ATermInInnerFormat;
}

atermpp::aterm_appl Rewriter::internal_universal_quantifier_enumeration(
     const atermpp::aterm_appl ATermInInnerFormat,
     mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma)
{
  /* Get Body of forall */
  ATerm t1 = ATgetArgument(ATermInInnerFormat,1);
  data_expression d(fromRewriteFormat(t1));
  
  /* Get Sort for enumeration from Body*/
  sort_expression_list fsdomain = function_sort(d.sort()).domain();
  
  data::fresh_variable_generator<> generator;
  generator.add_identifiers(find_identifiers(d));
  generator.set_hint("var");
  
  /* Create for each of the sorts for enumeration a new variable*/
  variable_vector vv;
  for(sort_expression_list::iterator i = fsdomain.begin(); i != fsdomain.end(); ++i)
  {
    variable v(generator(*i));
    vv.push_back(v);
  }
  
  /* Create Enumerator */
  EnumeratorStandard ES(m_data_specification_for_enumeration, this);
  
  /* Find A solution*/
  const variable_list vl=atermpp::convert< variable_list >(vv);
  EnumeratorSolutionsStandard sol(vl,
      toRewriteFormat(application (d, atermpp::convert< data_expression_list >(vv))),
      sigma,false,&ES,100);
  
  /* Create ATermList to store solutions */
  atermpp::term_list<atermpp::aterm_appl> x;
  bool has_exact_solution = false;
  bool has_no_solution =true;
  bool solution_possible=true;
  
  size_t loop_upperbound=5;
  
  while (loop_upperbound>0 && sol.next(has_exact_solution,x,solution_possible) && !has_exact_solution)
  {
    has_no_solution = false;
    loop_upperbound--;
  }

  if (solution_possible)
  {
    if (has_exact_solution)
    {
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
      std::cerr << "  return false (forall)\n";
#endif
      return toRewriteFormat(mcrl2::data::sort_bool::false_());
    }
    else if (has_no_solution)
    {
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
      std::cerr << "  return true (forall)\n";
#endif
      return toRewriteFormat(mcrl2::data::sort_bool::true_());
    }
  }
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
  std::cerr << "  A universal quantifier could not be eliminated and remains unchanged.\n";
#endif
  return ATermInInnerFormat;  // We were unable to remove the universal quantifier.
}

atermpp::aterm_appl Rewriter::internal_quantifier_enumeration(
     const atermpp::aterm_appl ATermInInnerFormat,
     mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma)
{
  atermpp::aterm_appl result=ATermInInnerFormat;
#ifndef MCRL2_DISABLE_QUANTIFIER_ENUMERATION
  if (ATisAppl((ATerm)(ATermAppl)ATermInInnerFormat))
  {
    ATerm arg = ATgetArgument(ATermInInnerFormat,0);

    /* Make sure that we have indeed a rewrite rule */
    if (ATisInt(arg))
    {
      /* Convert internal rewrite number to ATerm representation*/
      ATermAppl a = get_int2term(ATgetInt((ATermInt) arg));

      if (is_function_symbol(a))
      {
        /* Check for universal quantifier */
        if (function_symbol(a).name() == forall_function_symbol())
        {
          result = internal_universal_quantifier_enumeration(ATermInInnerFormat,sigma);
        }
        /* Check for existential quantifier */
        else if (function_symbol(a).name() == exists_function_symbol())
        {
          result = internal_existential_quantifier_enumeration(ATermInInnerFormat,sigma);
        }
      }
    }
  }
#endif
  return result;
}


Rewriter* createRewriter(const data_specification& DataSpec, const RewriteStrategy Strategy, const bool add_rewrite_rules)
{
  switch (Strategy)
  {
    case GS_REWR_JITTY:
      return new RewriterJitty(DataSpec,add_rewrite_rules);
#ifdef MCRL2_JITTYC_AVAILABLE
    case GS_REWR_JITTYC:
      return new RewriterCompilingJitty(DataSpec,add_rewrite_rules);
#endif
    case GS_REWR_JITTY_P:
      return new RewriterProver(DataSpec,mcrl2::data::rewriter::jitty,add_rewrite_rules);
#ifdef MCRL2_JITTYC_AVAILABLE
    case GS_REWR_JITTYC_P:
      return new RewriterProver(DataSpec,data::rewriter::jitty_compiling,add_rewrite_rules);
#endif
    default:
      return NULL;
  }
}

//Prototype
static void check_vars(const data_expression expr, const std::set <variable> &vars, std::set <variable> &used_vars);

static void check_vars(const data_expression_list exprs, const std::set <variable> &vars, std::set <variable> &used_vars)
{
  for (data_expression_list::const_iterator i=exprs.begin(); i!=exprs.end(); ++i) 
  {
    check_vars(*i,vars,used_vars);
  }
}

static void check_vars(const data_expression expr, const std::set <variable> &vars, std::set <variable> &used_vars)
{
  if (is_application(expr))
  {
    check_vars(application(expr).head(),vars,used_vars);
    check_vars(application(expr).arguments(),vars,used_vars);
  }
  else if (is_variable(expr))
  {
    used_vars.insert(variable(expr));

    if (vars.count(variable(expr))==0)
    {
      throw variable(expr);
    }
  }
}

//Prototype
static void checkPattern(ATermAppl p);

static void checkPattern(ATermList l)
{
  for (; !ATisEmpty(l); l = ATgetNext(l))
  {
    checkPattern(ATAgetFirst(l));
  }
}

static void checkPattern(ATermAppl p)
{
  if (gsIsDataAppl(p))
  {
    if (gsIsDataVarId(ATAgetArgument(p,0)))
    {
      throw string("variable "+PrintPart_CXX(ATgetArgument(p,0),ppDefault)+" is used as head symbol in an application, which is not supported");
    }
    checkPattern(ATAgetArgument(p,0));
    checkPattern(ATLgetArgument(p,1));
  }
}

void CheckRewriteRule(const data_equation DataEqn)
{
  const variable_list rule_var_list = DataEqn.variables();
  const std::set <variable> rule_vars(rule_var_list.begin(),rule_var_list.end());

  // collect variables from lhs and check that they are in rule_vars
  std::set <variable> lhs_vars;
  try
  {
    check_vars(DataEqn.lhs(),rule_vars,lhs_vars);
  }
  catch (variable var)
  {
    // This should never occur if DataEqn is a valid data equation
    mCRL2log(log::error) << "Data Equation: " << atermpp::aterm_appl(DataEqn) << std::endl;
    assert(0);
    throw runtime_error("variable " + pp(var) + " occurs in left-hand side of equation but is not defined (in equation: " + pp(DataEqn) + ")");
  }

  // check that variables from the condition occur in the lhs
  try
  {
    std::set <variable> dummy;
    check_vars(DataEqn.condition(),lhs_vars,dummy);
  }
  catch (variable var)
  {
    throw runtime_error("variable " + pp(var) + " occurs in condition of equation but not in left-hand side (in equation: " + 
                    pp(DataEqn) + "); equation cannot be used as rewrite rule");
  }

  // check that variables from the rhs are occur in the lhs
  try
  {
    std::set <variable> dummy;
    check_vars(DataEqn.rhs(),lhs_vars,dummy);
  }
  catch (variable var)
  {
    throw runtime_error("variable " + pp(var) + " occurs in right-hand side of equation but not in left-hand side (in equation: " + 
                pp(DataEqn) + "); equation cannot be used as rewrite rule");
  }

  // check that the lhs is a supported pattern
  if (is_variable(DataEqn.lhs()))
  {
    throw runtime_error("left-hand side of equation is a variable; this is not allowed for rewriting");
  }
  try
  {
    checkPattern(DataEqn.lhs());
  }
  catch (string& s)
  {
    throw runtime_error(s+" (in equation: " + pp(DataEqn) + "); equation cannot be used as rewrite rule");
  }
}

bool isValidRewriteRule(const data_equation data_eqn)
{
  try
  {
    CheckRewriteRule(data_eqn);
    return true;
  }
  catch (runtime_error&)
  {
    return false;
  }
}

void PrintRewriteStrategy(FILE* stream, RewriteStrategy strat)
{
  if (strat == GS_REWR_JITTY)
  {
    fprintf(stream, "jitty");
#ifdef MCRL2_JITTYC_AVAILABLE
  }
  else if (strat == GS_REWR_JITTYC)
  {
    fprintf(stream, "jittyc");
#endif
  }
  else if (strat == GS_REWR_JITTY_P)
  {
    fprintf(stream, "jittyp");
#ifdef MCRL2_JITTYC_AVAILABLE
  }
  else if (strat == GS_REWR_JITTYC_P)
  {
    fprintf(stream, "jittycp");
#endif
  }
  else
  {
    fprintf(stream, "invalid");
  }
}

RewriteStrategy RewriteStrategyFromString(char const* s)
{
  static RewriteStrategy strategies[9] = { GS_REWR_INVALID,
#ifdef MCRL2_JITTYC_AVAILABLE
                                         GS_REWR_JITTY, GS_REWR_JITTYC, GS_REWR_JITTY_P, GS_REWR_JITTYC_P
                                         };
#else
                                         GS_REWR_JITTY, GS_REWR_INVALID, GS_REWR_JITTY_P, GS_REWR_INVALID
                                         };
#endif

  size_t main_strategy = 0; // default invalid

  if (std::strncmp(&s[0], "jitty", 5) == 0)   // jitty{,c,cp}
  {
    main_strategy = 1;
  
    if (s[5] == '\0')   // interpreting
    {
      return strategies[main_strategy];
    }
    else if (s[6] == '\0')
    {
      if (s[5] == 'c')   // compiling
      {
        return strategies[main_strategy + 1];
      }
      else if (s[5] == 'p')   // with prover
      {
        return strategies[main_strategy + 2];
      }
    }
    else if (s[5] == 'c' && s[6] == 'p' && s[7] == '\0')   // compiling with prover
    {
      return strategies[main_strategy + 3];
    }
  }

  return GS_REWR_INVALID;
}

std::vector <AFun> apples;

/*************  Below the functions toInner and fromInner are being defined *********************/

static
atermpp::map< data_expression, atermpp::aterm_int > &term2int()
{ 
  static atermpp::map< data_expression, atermpp::aterm_int > term2int;
  return term2int;
}

atermpp::map< data_expression, atermpp::aterm_int >::const_iterator term2int_begin()
{ 
  return term2int().begin();
}

atermpp::map< data_expression, atermpp::aterm_int >::const_iterator term2int_end()
{ 
  return term2int().end();
}

static
atermpp::vector < data_expression > &int2term()
{ 
  static atermpp::vector < data_expression > int2term;
  return int2term;
}

size_t get_num_opids()
{
  return int2term().size();
}

data_expression get_int2term(const size_t n)
{
  assert(n<int2term().size());
  return int2term()[n];
}

void set_int2term(const size_t n, const data_expression t)
{
  if (n>=int2term().size())
  {
    int2term().resize(n+1);
  }
  int2term()[n]=t;
}

void initialize_internal_translation_table_rewriter()
{
}

atermpp::aterm_int OpId2Int(const function_symbol term)
{
  atermpp::map< data_expression, atermpp::aterm_int >::iterator f = term2int().find(term);
  if (f == term2int().end())
  {
    const size_t num_opids=get_num_opids();
    atermpp::aterm_int i(num_opids);
    term2int()[term] =  i;
    int arity = getArity(term);
    if (arity > NF_MAX_ARITY)
    {
      arity = NF_MAX_ARITY;
    }
    assert(int2term().size()==num_opids);
    int2term().push_back(term);
    size_t num_aux = (1 << (arity+1)) - arity - 2; // 2^(arity+1) - arity - 2; Reserve extra space
                                                   // to accomodate function symbols that represent
                                                   // terms with partially normalized arguments.
    if (arity <= NF_MAX_ARITY)
    {
      for(size_t k=0; k<num_aux; ++k)
      {
        int2term().push_back(data_expression()); 
      }
    }
    return i;
  }

  atermpp::aterm_int j = f->second;
  return j;
} 

atermpp::aterm_appl toInner(const data_expression term, const bool add_opids)
{

  assert(!gsIsNil((ATermAppl)term)); // Originally Nil was returned unchanged, but is hopefully not used anymore. JFG

  if (is_variable(term))
  {
    return term;
  }
  else if (is_application(term))
  {
    atermpp::term_list <atermpp::aterm_appl> l;
    atermpp::aterm_appl arg0 = toInner(application(term).head(), add_opids);
    // Reflect the way of encoding the other arguments!
    // if (gsIsNil(arg0) || gsIsDataVarId(arg0))
    if (is_variable(arg0))
    {
      l = push_front(l, arg0);
    }
    else
    {
      size_t arity = arg0.size(); //ATgetArity(ATgetAFun(arg0));
      for (size_t i = 0; i < arity; ++i)
      {
        l = push_front(l, atermpp::aterm_appl(arg0(i)));
      }
    }
    const data_expression_list args=application(term).arguments();
    for (data_expression_list::const_iterator i=args.begin(); i!=args.end(); ++i) 
    {
      l = push_front(l, toInner(*i,add_opids));
    }
    l = reverse(l);
    return Apply(l);
  }
  else if (is_function_symbol(term))
  {
    return Apply0(OpId2Int(term));
  }
  else
  {
    return Apply0((ATerm)(ATermAppl)term);
  }
}

data_expression fromInner(atermpp::aterm_appl term)
{
  data_expression a;

  if (gsIsDataVarId((ATermAppl)term))
  {
    return variable(term);
  }

  size_t arity = ATgetArity(ATgetAFun(term));
  atermpp::aterm_appl t = term(0);
  if (ATisInt((ATerm)(ATermAppl)t))
  {
    a = get_int2term(ATgetInt((ATermInt)(ATerm)(ATermAppl) t));
  }
  else
  {
    a = data_expression(t);
  }

  if (gsIsOpId(a) || gsIsDataVarId(a))
  {
    size_t i = 1;
    sort_expression sort = a.sort();
    while (is_function_sort(sort) && (i < arity))
    {
      sort_expression_list sort_dom = function_sort(sort).domain();
      data_expression_list list;
      while (!ATisEmpty(sort_dom))
      {
        assert(i < arity);
        list = push_front(list, fromInner(ATgetArgument(term,i)));
        sort_dom = pop_front(sort_dom);
        ++i;
      }
      list = reverse(list);
      a = application(a, list);
      sort = function_sort(sort).codomain();
    }
  }
  return a;
} 


}
}
}
