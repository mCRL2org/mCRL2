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
  return data_expression();
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
  assert(0);
  return false;
}

bool Rewriter::removeRewriteRule(const data_equation /*Rule*/)
{
  assert(0);
  return false;
}

atermpp::aterm_appl Rewriter::internal_existential_quantifier_enumeration(
     const atermpp::aterm_appl t,
     mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma) 
{
  /* Get Body of Exists */
  const atermpp::aterm_appl t1 = t(1);
  data_expression d(fromInner(t1)); // Do not apply reconstruct, which can generate an explicit lambda.

  /* Get Sort for enumeration from Body*/
  sort_expression_list fsdomain = function_sort(d.sort()).domain();

  /* static data::fresh_variable_generator<> generator;
  generator.add_identifiers(find_identifiers(d));
  generator.set_hint("var"); */

  /* Create for each of the sorts for enumeration a new variable*/
  size_t arity=fsdomain.size(); 
  if (ATisInt((ATerm)(ATermAppl)t1))
  { 
    arity=arity+1;
  }
  else 
  {
    arity=arity+t1.size();
  }

  MCRL2_SYSTEM_SPECIFIC_ALLOCA(terms,atermpp::aterm,arity);
  size_t count=1;
  if (ATisInt((ATerm)(ATermAppl)t1))
  {  
    terms[0]=t1;
  }
  else
  {
    for(count=0 ; count<t1.size(); ++count)
    terms[count]=t1(count);
  }

  const size_t oldcount=count;
  for(sort_expression_list::iterator i = fsdomain.begin(); i != fsdomain.end(); ++i,++count)
  {
    variable v(generator(*i));
    terms[count]=v;
  }
  // Put the variables in the right order in vl.
  variable_list vl;
  for(count-- ; count>=oldcount; count--)
  {
    vl=push_front(vl, variable(terms[count]));
  }

  /* Create Enumerator */
  EnumeratorStandard ES(m_data_specification_for_enumeration, this);

  /* Find A solution*/
  EnumeratorSolutionsStandard sol(vl, ApplyArray(arity,terms), sigma,true,&ES,100);

  /* Create a list to store solutions */
  atermpp::term_list<atermpp::aterm_appl> x;
  atermpp::aterm_appl evaluated_condition=internal_false;
  atermpp::aterm_appl partial_result=internal_false;
  bool solution_possible=true;

  size_t loop_upperbound=5;
  while (loop_upperbound>0 && 
         partial_result!=internal_true &&
         sol.next(evaluated_condition,x,solution_possible))
  {
    if (partial_result==internal_false)
    {
      partial_result=evaluated_condition;
    }
    else if (partial_result==internal_true)
    {
      partial_result=internal_true;
    }
    else
    { 
      partial_result=Apply2(internal_or, partial_result,evaluated_condition);
    }
    loop_upperbound--;
  }

  if (solution_possible && (loop_upperbound>0 || partial_result==internal_true))
  {
    return partial_result;
  }

  return Apply1(t(0),rewrite_internal(t1,sigma));
}

atermpp::aterm_appl Rewriter::internal_universal_quantifier_enumeration(
     const atermpp::aterm_appl t,
     mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma)
{
  /* Get Body of forall */
  atermpp::aterm_appl t1 = t(1);
  data_expression d(fromInner(t1));  // Do not apply fromRewriteFormat, as this generates an explicit lambda function,
                                     // which when translated back to rewrite format can cause recompilation of the
                                     // compiling rewriter, which causes havoc, as eliminating quantifiers is done while
                                     // in the midst of a rewriting session.
  
  /* Get Sort for enumeration from Body*/
  sort_expression_list fsdomain = function_sort(d.sort()).domain();
  
  /* static data::fresh_variable_generator<> generator;
  generator.add_identifiers(find_identifiers(d));
  generator.set_hint("var"); */
  
  /* Create for each of the sorts for enumeration a new variable*/
  size_t arity=fsdomain.size(); 
  if (ATisInt((ATerm)(ATermAppl)t1))
  { 
    arity=arity+1;
  }
  else 
  {
    arity=arity+t1.size();
  }

  MCRL2_SYSTEM_SPECIFIC_ALLOCA(terms,atermpp::aterm,arity);
  size_t count=1;
  if (ATisInt((ATerm)(ATermAppl)t1))
  {  
    terms[0]=t1;
  }
  else
  {
    for(count=0 ; count<t1.size(); ++count)
    terms[count]=t1(count);
  }

  const size_t oldcount=count;
  for(sort_expression_list::iterator i = fsdomain.begin(); i != fsdomain.end(); ++i,++count)
  {
    variable v(generator(*i));
    terms[count]=v;
  }
  // Put the variables in the right order in vl.
  variable_list vl;
  for(count-- ; count>=oldcount; count--)
  {
    vl=push_front(vl, variable(terms[count]));
  }

  /* Create Enumerator */
  EnumeratorStandard ES(m_data_specification_for_enumeration, this);
  
  /* Find A solution*/
  EnumeratorSolutionsStandard sol(vl, ApplyArray(arity,terms), sigma,false,&ES,100);
  
  /* Create ATermList to store solutions */
  atermpp::term_list<atermpp::aterm_appl> x;
  atermpp::aterm_appl evaluated_condition=internal_true;
  atermpp::aterm_appl partial_result=internal_true;
  bool solution_possible=true;

  size_t loop_upperbound=5;
  while (loop_upperbound>0 && 
         partial_result!=internal_false &&
         sol.next(evaluated_condition,x,solution_possible))
  {
    // The returned evaluated condition is the negation of the entered condition,
    // as is not_equal_to_true_or_false is set to false in sol. So, we must first 
    // negate it.

    if (evaluated_condition == internal_true)
    {
      evaluated_condition=internal_false;
    }
    else if (evaluated_condition == internal_false)
    {
      evaluated_condition=internal_true;
    }
    else if (!ATisInt((ATerm)(ATermAppl)evaluated_condition) && evaluated_condition(0) == internal_not)
    {
      evaluated_condition=evaluated_condition(1);
    }
    else 
    {
      evaluated_condition=Apply1(internal_not, evaluated_condition);
    }

    
    if (partial_result==internal_true)
    {
      partial_result=evaluated_condition;
    }
    else if (partial_result==internal_false)
    {
      partial_result=internal_false;
    }
    else
    { 
      partial_result=Apply2(internal_and, partial_result, evaluated_condition);
    }
    loop_upperbound--;
  }

  if (solution_possible && (loop_upperbound>0 || partial_result==internal_false))
  {
    return partial_result;
  }

  return Apply1(t(0),rewrite_internal(t1,sigma));
}

atermpp::aterm_appl Rewriter::internal_quantifier_enumeration(
     const atermpp::aterm_appl t,
     mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma)
{
  atermpp::aterm_appl result=t;
#ifndef MCRL2_DISABLE_QUANTIFIER_ENUMERATION
  if (ATisAppl((ATerm)(ATermAppl)t))
  {
    ATerm arg = ATgetArgument(t,0);

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
          result = internal_universal_quantifier_enumeration(t,sigma);
        }
        /* Check for existential quantifier */
        else if (function_symbol(a).name() == exists_function_symbol())
        {
          result = internal_existential_quantifier_enumeration(t,sigma);
        }
      }
    }
  }
#endif
  return result;
}


Rewriter* createRewriter(
            const data_specification& DataSpec, 
            const used_data_equation_selector &equations_selector, 
            const RewriteStrategy Strategy)
{
  switch (Strategy)
  {
    case GS_REWR_JITTY:
      return new RewriterJitty(DataSpec,equations_selector);
#ifdef MCRL2_JITTYC_AVAILABLE
    case GS_REWR_JITTYC:
      return new RewriterCompilingJitty(DataSpec,equations_selector);
#endif
    case GS_REWR_JITTY_P:
      return new RewriterProver(DataSpec,mcrl2::data::rewriter::jitty,equations_selector);
#ifdef MCRL2_JITTYC_AVAILABLE
    case GS_REWR_JITTYC_P:
      return new RewriterProver(DataSpec,data::rewriter::jitty_compiling,equations_selector);
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
atermpp::map< function_symbol, atermpp::aterm_int > &term2int()
{ 
  static atermpp::map< function_symbol, atermpp::aterm_int > term2int;
  return term2int;
}

atermpp::map< data::function_symbol, atermpp::aterm_int >::const_iterator term2int_begin()
{ 
  return term2int().begin();
}

atermpp::map< data::function_symbol, atermpp::aterm_int >::const_iterator term2int_end()
{ 
  return term2int().end();
}

static
atermpp::vector < data::function_symbol > &int2term()
{ 
  static atermpp::vector < data::function_symbol > int2term;
  return int2term;
}

size_t get_num_opids()
{
  return int2term().size();
}

function_symbol get_int2term(const size_t n)
{
  assert(n<int2term().size());
  return int2term()[n];
}

atermpp::aterm_int OpId2Int(const function_symbol term)
{
  atermpp::map< function_symbol, atermpp::aterm_int >::iterator f = term2int().find(term);
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
        int2term().push_back(function_symbol()); 
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
    a = variable(t);
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
