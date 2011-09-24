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
#include "mcrl2/atermpp/algorithm.h"
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

// function object to test if it is an aterm_appl with function symbol "f"
struct is_a_variable
{
  bool operator()(atermpp::aterm t) const
  {
    return core::detail::gsIsDataVarId(t);
  }
};

bool occur_check(const variable v, const atermpp::aterm_appl e)
{
  if (v==e)
  { 
    // The variable is reset. This is ok.
    return true;
  }
  std::set<variable> s;
  find_all_if(e,is_a_variable(),std::inserter(s,s.begin()));
  if (s.count(v)>0)
  {
    // ATfprintf(stderr,"Occur check %t <-- %t\n",v,fromInner(e));
    return false; // Occur check failed.
  }
  return true;
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

atermpp::aterm_appl Rewriter::rewrite_where(
                      const atermpp::aterm_appl term,
                      mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma)
{
  using namespace atermpp;
  const term_list < atermpp::aterm_appl > assignment_list=term(1);
  const aterm_appl body=term(0);
  atermpp::vector < aterm_appl > saved_substitutions(assignment_list.size());
  size_t count=0;
  // First rewrite and save substitutions of where clauses. Do not yet put them in 
  // the substitution, as according to the semantics different where clauses cannot 
  // be used in each other.
  for( term_list < atermpp::aterm_appl > :: const_iterator it=assignment_list.begin(); it!=assignment_list.end(); ++it, ++count)
  { 
    assert(count<assignment_list.size());
    saved_substitutions[count]=rewrite_internal((*it)(1),sigma);
  }
  // Now save old values for variables in sigma and carry out the substitutions in sigma.
  count=0;
  for( term_list < atermpp::aterm_appl > :: const_iterator it=assignment_list.begin(); it!=assignment_list.end(); ++it, ++count)
  { 
    assert(count<assignment_list.size());
    const variable v=variable((*it)(0));
    aterm_appl t=sigma(v); // temporary store used for swapping values.
    assert(occur_check(v,saved_substitutions[count]));
    sigma[v]=saved_substitutions[count];
    saved_substitutions[count]=t;
  }
  // Rewrite the body.
  const aterm_appl result=rewrite_internal(body,sigma);

  // Restore the old values of sigma.
  count=0;
  for( term_list < atermpp::aterm_appl > :: const_iterator it=assignment_list.begin(); it!=assignment_list.end(); ++it, ++count)
  { 
    assert(count<assignment_list.size());
    const variable v=variable((*it)(0));
    assert(occur_check(v,saved_substitutions[count]));
    sigma[v]=saved_substitutions[count];
  }

  return result;
}

atermpp::aterm_appl Rewriter::rewrite_single_lambda(
                      const variable_list vl,
                      const atermpp::aterm_appl body,
                      mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma)
{
  assert(vl.size()>0);
  // A lambda term without arguments; Take care that the bound variable is made unique with respect to 
  // the variables occurring in sigma. But in case vl is empty, just rewrite...

  // First filter the variables in vl by those occuring as left/right hand sides in sigma.

  size_t number_of_renamed_variables=0;
  size_t count=0;
  atermpp::vector <variable> new_variables(vl.size());
  {
    // Restrict the scope of variables_in_sigma.
    std::set < variable > variables_in_sigma;
    for(mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> >::const_iterator it=sigma.begin();
                  it!=sigma.end(); ++it)
    {
      variables_in_sigma.insert(it->first);
      find_all_if(it->second,is_a_variable(),std::inserter(variables_in_sigma,variables_in_sigma.begin()));
    }
  
    // Create new unique variables to replace the old and create storage for
    // storing old values for variables in vl.
    for(variable_list::const_iterator it=vl.begin(); it!=vl.end(); ++it,count++)
    {
      const variable v= *it;
      if ((variables_in_sigma.count(v)>0) || true)
      {
        number_of_renamed_variables++;
        new_variables[count]=generator(v.sort());
      }
      new_variables[count]=v;
    }
  }

  if (number_of_renamed_variables==0)
  {
    return gsMakeBinder(gsMakeLambda(),vl,rewrite_internal(body,sigma));
  }
  
  atermpp::vector <atermpp::aterm_appl> saved_substitutions;
  count=0;
  for(variable_list ::const_iterator it=vl.begin(); it!=vl.end(); ++it,++count)
  { 
    assert(count<number_of_renamed_variables);
    const variable v= *it;
    if (v!=new_variables[count])
    {
      saved_substitutions.push_back(sigma(v));
      assert(occur_check(v,atermpp::aterm_appl(new_variables[count])));
      sigma[v]=atermpp::aterm_appl(new_variables[count]);
    }
  }
  const atermpp::aterm_appl result= rewrite_internal(body,sigma);

  // restore saved substitutions;
  
  count=0;
  size_t new_variable_count=0;
  for(variable_list ::const_iterator it=vl.begin(); it!=vl.end(); ++it,++count)
  {
    assert(count<vl.size());
    const variable v= *it;
    if (v!=new_variables[count])
    {
      assert(new_variable_count<saved_substitutions.size());
      assert(occur_check(v,saved_substitutions[new_variable_count]));
      sigma[v]=saved_substitutions[new_variable_count];
      new_variable_count++;
    }
  }

  variable_list new_variable_list;

  for(atermpp::vector <variable>::const_reverse_iterator it=new_variables.rbegin(); it!=new_variables.rend(); ++it)
  {
    new_variable_list=push_front(new_variable_list,*it);
  }
  return gsMakeBinder(gsMakeLambda(),new_variable_list,result);
}


// The function rewrite_lambda_application rewrites a lambda term to a set of
// arguments which are the arguments 1,...,n of t. If t has the shape
// @REWR@(t0,t1,....,tn) and the lambda term is L, we calculate the normal form
// in internal format for L(t1,...,tn). Note that the term t0 is ignored.
// Note that we assume that neither L, nor t is in normal form.

atermpp::aterm_appl Rewriter::rewrite_lambda_application(
                      const atermpp::aterm_appl lambda_term,
                      const atermpp::aterm_appl t,
                      mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma)
{
  using namespace atermpp;
  assert(lambda_term(0)==gsMakeLambda());  // The function symbol in this position cannot be anything else than a lambda term.
  const variable_list vl=lambda_term(1);
  const atermpp::aterm_appl lambda_body=lambda_term(2);
  size_t arity=t.size();
  assert(vl.size()<=arity);

  mutable_map_substitution<atermpp::map < atermpp::aterm_appl,atermpp::aterm_appl> > variable_renaming;
  size_t count=1;
  for(variable_list::const_iterator i=vl.begin(); i!=vl.end(); ++i, ++count)
  {
    const variable v= (*i);
    const variable v_fresh(generator(v.sort()));
    variable_renaming[v]=atermpp::aterm_appl(v_fresh);
    sigma[v_fresh]=rewrite_internal(t(count),sigma);
  }
  const atermpp::aterm_appl result=rewrite_internal(atermpp::replace(lambda_body,variable_renaming),sigma);
  
  // Reset variables in sigma
  for(mutable_map_substitution<atermpp::map < atermpp::aterm_appl,atermpp::aterm_appl> >::const_iterator it=variable_renaming.begin();
                 it!=variable_renaming.end(); ++it)
  {
    sigma[it->second]=it->second;
  }
  if (vl.size()==arity)
  {
    return result;
  }

  // There are more arguments than bound variables.
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,atermpp::aterm, arity-vl.size());
  args[0]=result;
  for(size_t i=1; i<arity-vl.size(); ++i)
  {
    assert(vl.size()+i<arity);
    args[i]=t(vl.size()+i);
  }
  // We do not employ the knowledge that the first argument is in normal form... TODO.
  return rewrite_internal(ApplyArray(arity-vl.size(),args),sigma); 
}

atermpp::aterm_appl Rewriter::new_internal_existential_quantifier_enumeration(
     const atermpp::aterm_appl t,
     mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma) 
{
  // This is a quantifier elimination that works on the existential quantifier as specified
  // in data types, i.e. without applying the implement function anymore. This function is
  // to replace internal_existential_quantifier_enumeration and should then not be called new anymore.
  
  /* Get Body of Exists */
  const atermpp::aterm_appl t1 = t(2);

  // Put the variables in the right order in vl.
  variable_list vl=t(1);

  /* Create Enumerator */
  EnumeratorStandard ES(m_data_specification_for_enumeration, this);

  /* Find A solution*/
  EnumeratorSolutionsStandard sol(vl, t1, sigma,true,&ES,100);

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

  return gsMakeBinder(gsMakeExists(),vl,rewrite_internal(t1,sigma));
}


atermpp::aterm_appl Rewriter::internal_existential_quantifier_enumeration(
     const atermpp::aterm_appl t,
     mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma) 
{
  /* Get Body of Exists */
  const atermpp::aterm_appl t1 = t(1);

  /* Get Sort for enumeration from t */
  const sort_expression sort_of_exists = get_int2term(ATgetInt((ATermInt)(ATerm)t(0))).sort();
  const sort_expression_list fsdomain = function_sort(function_sort(sort_of_exists).domain().front()).domain();

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

atermpp::aterm_appl Rewriter::new_internal_universal_quantifier_enumeration(
     const atermpp::aterm_appl t,
     mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma)
{
  /* Get Body of forall */
  const atermpp::aterm_appl t1 = t(2);
  const variable_list vl=t(1);

  /* Create Enumerator */
  EnumeratorStandard ES(m_data_specification_for_enumeration, this);
  
  /* Find A solution*/
  EnumeratorSolutionsStandard sol(vl, t1, sigma,false,&ES,100);
  
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

  return gsMakeBinder(gsMakeForall(),vl,rewrite_internal(t1,sigma));
}

atermpp::aterm_appl Rewriter::internal_universal_quantifier_enumeration(
     const atermpp::aterm_appl t,
     mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma)
{
  /* Get Body of forall */
  const atermpp::aterm_appl t1 = t(1);

  /* Get Sort for enumeration from t */
  const sort_expression sort_of_exists = get_int2term(ATgetInt((ATermInt)(ATerm)t(0))).sort();
  const sort_expression_list fsdomain = function_sort(function_sort(sort_of_exists).domain().front()).domain();
  
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
    if (is_variable(arg0) || gsIsBinder(arg0) || gsIsWhr(arg0))
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
    const data_expression_list args= application(term).arguments();
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
  else if (is_where_clause(term))
  {
    const where_clause t=term;
    atermpp::term_list<atermpp::aterm> l;
    const atermpp::vector < assignment_expression > lv=atermpp::convert < atermpp::vector < assignment_expression > >(t.declarations());
    for(atermpp::vector < assignment_expression > :: const_reverse_iterator it=lv.rbegin() ; it!=lv.rend(); ++it)
    {
      l=atermpp::push_front(l,atermpp::aterm(core::detail::gsMakeDataVarIdInit(it->lhs(),toInner(it->rhs(),add_opids))));
    }
    return gsMakeWhr(toInner(t.body(),add_opids),l);
  }
  else if (is_abstraction(term))
  {
    const abstraction t=term;
    
    return gsMakeBinder(t.binding_operator(),t.variables(),toInner(t.body(),add_opids));
  }
  assert(0); // term has unexpected format.
  return Apply0((ATerm)(ATermAppl)term);
}

data_expression fromInner(atermpp::aterm_appl term)
{
  if (gsIsDataVarId((ATermAppl)term))
  {
    return variable(term);
  }

  if (gsIsWhr((ATermAppl)term))
  {
    const data_expression body=fromInner(term(0));
    const atermpp::term_list<atermpp::aterm_appl> l=term(1);
     
    atermpp::vector < assignment_expression > lv;
    for(atermpp::term_list<atermpp::aterm_appl> :: const_iterator it=l.begin() ; it!=l.end(); ++it)
    {
      const atermpp::aterm_appl ass_expr= *it;
      lv.push_back(assignment_expression(variable(ass_expr(0)),fromInner(ass_expr(1))));
    }
    return where_clause(body,lv);
  }

  if (gsIsBinder((ATermAppl)term))
  {
    return abstraction(binder_type(term(0)),variable_list(term(1)),fromInner(term(2)));
  }

  size_t arity = ATgetArity(ATgetAFun(term));
  atermpp::aterm_appl t = term(0);
  data_expression a;

  if (ATisInt((ATerm)(ATermAppl)t))
  {
    a = get_int2term(ATgetInt((ATermInt)(ATerm)(ATermAppl) t));
  }
  else
  {
    a = fromInner(t);
  }

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
  return a;
} 


}
}
}
