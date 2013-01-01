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
#include <limits>
#include <algorithm>
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
  bool operator()(const atermpp::aterm &t) const
  {
    return is_variable(t);
  }
};

#ifndef NDEBUG
static
bool occur_check(const variable &v, const atermpp::aterm_appl &e)
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
    return false; // Occur check failed.
  }
  return true;
}
#endif

template <class Rewriter>
struct rewrite_list_rewriter
{
  typename Rewriter::substitution_type &m_sigma;
  Rewriter &m_rewr;

  rewrite_list_rewriter(typename Rewriter::substitution_type &sigma, Rewriter &rewr):m_sigma(sigma),m_rewr(rewr)
  {}

  const data_expression operator() (const data_expression &t) const
  {
    return m_rewr.rewrite(t,m_sigma);
  }
};

data_expression_list Rewriter::rewrite_list(
     const data_expression_list &terms,
     substitution_type &sigma)
{
  rewrite_list_rewriter<Rewriter> r(sigma,*this);
  return data_expression_list(terms.begin(),terms.end(),r);
}

atermpp::aterm_appl Rewriter::toRewriteFormat(const data_expression &/*Term*/)
{
  assert(0);
  return atermpp::aterm_appl();
}

data_expression Rewriter::fromRewriteFormat(const atermpp::aterm_appl &t)
{
  return fromInner(t);
}

atermpp::aterm_appl Rewriter::rewrite_internal(
     const atermpp::aterm_appl &/*Term*/,
     internal_substitution_type & /*sigma*/)
{
  assert(0);
  return data_expression();
}

template <class Rewriter>
struct rewrite_list_rewriter_internal
{
  typename Rewriter::internal_substitution_type &m_sigma;
  Rewriter &m_rewr;

  rewrite_list_rewriter_internal(typename Rewriter::internal_substitution_type &sigma, Rewriter &rewr):m_sigma(sigma),m_rewr(rewr)
  {}

  const aterm_appl operator() (const aterm_appl &t) const
  {
    return m_rewr.rewrite_internal(t,m_sigma);
  }
};



atermpp::term_list<atermpp::aterm_appl> Rewriter::rewrite_internal_list(
    const atermpp::term_list<atermpp::aterm_appl> &terms,
    internal_substitution_type &sigma)
{
  rewrite_list_rewriter_internal<Rewriter> r(sigma,*this);
  return atermpp::term_list<atermpp::aterm_appl>(terms.begin(),terms.end(),r);
}

bool Rewriter::addRewriteRule(const data_equation &/*Rule*/)
{
  assert(0);
  return false;
}

bool Rewriter::removeRewriteRule(const data_equation &/*Rule*/)
{
  assert(0);
  return false;
}

atermpp::aterm_appl Rewriter::rewrite_where(
                      const atermpp::aterm_appl &term,
                      internal_substitution_type &sigma)
{
  const atermpp::term_list < atermpp::aterm_appl > assignment_list(term(1));
  const atermpp::aterm_appl body(term(0));

  mutable_map_substitution<std::map < atermpp::aterm_appl,atermpp::aterm_appl> > variable_renaming;
  for(atermpp::term_list < atermpp::aterm_appl >::const_iterator i=assignment_list.begin(); i!=assignment_list.end(); ++i)
  {
    const variable v=variable((*i)(0));
    const variable v_fresh(generator("whr_"), v.sort());
    variable_renaming[v]=atermpp::aterm_appl(v_fresh);
    sigma[v_fresh]=rewrite_internal(atermpp::aterm_appl((*i)(1)),sigma);
  }
  const atermpp::aterm_appl result=rewrite_internal(atermpp::replace(body,variable_renaming),sigma);

  // Reset variables in sigma
  for(mutable_map_substitution<std::map < atermpp::aterm_appl,atermpp::aterm_appl> >::const_iterator it=variable_renaming.begin();
      it!=variable_renaming.end(); ++it)
  {
    sigma[it->second]=it->second;
  }
  return result;
}

atermpp::aterm_appl Rewriter::rewrite_single_lambda(
                      const variable_list &vl,
                      const atermpp::aterm_appl &body,
                      const bool body_in_normal_form,
                      internal_substitution_type &sigma)
{
  assert(vl.size()>0);
  // A lambda term without arguments; Take care that the bound variable is made unique with respect to
  // the variables occurring in sigma. But in case vl is empty, just rewrite...

  // First filter the variables in vl by those occuring as left/right hand sides in sigma.

  size_t number_of_renamed_variables=0;
  size_t count=0;
  std::vector <variable> new_variables(vl.size());
  {
    std::set < variable > variables_in_sigma(get_free_variables(sigma));
    // Create new unique variables to replace the old and create storage for
    // storing old values for variables in vl.
    for(variable_list::const_iterator it=vl.begin(); it!=vl.end(); ++it,count++)
    {
      const variable v= *it;
      if (variables_in_sigma.find(v) != variables_in_sigma.end())
      {
        number_of_renamed_variables++;
        new_variables[count]=data::variable(generator("y_"), v.sort());
      }
      else new_variables[count]=v;
    }
  }

  if (number_of_renamed_variables==0)
  {
    atermpp::aterm_appl a=gsMakeBinder(gsMakeLambda(),vl,(body_in_normal_form?body:rewrite_internal(body,sigma)));
    return a;
  }

  std::vector <atermpp::aterm_appl> saved_substitutions;
  count=0;
  for(variable_list ::const_iterator it=vl.begin(); it!=vl.end(); ++it,++count)
  {
    assert(count<new_variables.size());
    const variable v= *it;
    if (v!=new_variables[count])
    {
      saved_substitutions.push_back(sigma(v));
      assert(occur_check(v,atermpp::aterm_appl(new_variables[count])));
      sigma[v]=atermpp::aterm_appl(new_variables[count]);
    }
  }
  const atermpp::aterm_appl result=(body_in_normal_form?body:rewrite_internal(body,sigma));

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

  for(std::vector <variable>::reverse_iterator it=new_variables.rbegin(); it!=new_variables.rend(); ++it)
  {
    new_variable_list.push_front(*it);
  }
  const atermpp::aterm_appl a=gsMakeBinder(gsMakeLambda(),new_variable_list,result);
  return a;
}


// The function rewrite_lambda_application rewrites a lambda term to a set of
// arguments which are the arguments 1,...,n of t. If t has the shape
// #REWR#(t0,t1,....,tn) and the lambda term is L, we calculate the normal form
// in internal format for L(t1,...,tn). Note that the term t0 is ignored.
// Note that we assume that neither L, nor t is in normal form.

atermpp::aterm_appl Rewriter::rewrite_lambda_application(
                      const atermpp::aterm_appl &lambda_term,
                      const atermpp::aterm_appl &t,
                      internal_substitution_type &sigma)
{
  using namespace atermpp;
  assert(lambda_term(0)==gsMakeLambda());  // The function symbol in this position cannot be anything else than a lambda term.
  const variable_list vl=static_cast<variable_list>(lambda_term(1));
  const atermpp::aterm_appl lambda_body=rewrite_internal(atermpp::aterm_appl(lambda_term(2)),sigma);
  size_t arity=t.size();
  assert(arity>0);
  if (arity==1) // The term has shape #REWR(lambda d..:D...t), i.e. without arguments.
  {
    return rewrite_single_lambda(vl, lambda_body, true, sigma);
  }
  assert(vl.size()<arity);

  mutable_map_substitution<std::map < atermpp::aterm_appl,atermpp::aterm_appl> > variable_renaming;
  size_t count=1;
  for(variable_list::const_iterator i=vl.begin(); i!=vl.end(); ++i, ++count)
  {
    const variable v= (*i);
    const variable v_fresh(generator("x_"), v.sort());
    variable_renaming[v]=atermpp::aterm_appl(v_fresh);
    sigma[v_fresh]=rewrite_internal(atermpp::aterm_appl(t(count)),sigma);
  }

  const atermpp::aterm_appl result=rewrite_internal(atermpp::replace(lambda_body,variable_renaming),sigma);

  // Reset variables in sigma
  for(mutable_map_substitution<std::map < atermpp::aterm_appl,atermpp::aterm_appl> >::const_iterator it=variable_renaming.begin();
                 it!=variable_renaming.end(); ++it)
  {
    sigma[it->second]=it->second;
  }
  if (vl.size()+1==arity)
  {
    return result;
  }

  // There are more arguments than bound variables.
  std::vector < atermpp::aterm > args; 
  args.push_back(result);
  for(size_t i=1; i<arity-vl.size(); ++i)
  {
    assert(vl.size()+i<arity);
    args.push_back(t(vl.size()+i));
  }
  // We do not employ the knowledge that the first argument is in normal form... TODO.
  return rewrite_internal(ApplyArray(arity-vl.size(),args.begin(),args.end()),sigma);
}

atermpp::aterm_appl Rewriter::internal_existential_quantifier_enumeration(
     const atermpp::aterm_appl &t,
     internal_substitution_type &sigma)
{
  // This is a quantifier elimination that works on the existential quantifier as specified
  // in data types, i.e. without applying the implement function anymore.

  assert(is_abstraction(t) && t(0)==gsMakeExists());
  /* Get Body of Exists */
  const atermpp::aterm_appl t1 = atermpp::aterm_appl(t(2));

  // Put the variables in the right order in vl.
  variable_list vl=static_cast<variable_list>(t(1));
  return internal_existential_quantifier_enumeration(vl,t1,false,sigma);
}

// Generate a term equivalent to exists vl.t1.
// The variable t1_is_normal_form indicates whether t1 is in normal
// form, but this information is not used as it stands.
atermpp::aterm_appl Rewriter::internal_existential_quantifier_enumeration(
      const variable_list &vl,
      const atermpp::aterm_appl &t1,
      const bool t1_is_normal_form,
      internal_substitution_type &sigma)
{
  mutable_map_substitution<std::map < atermpp::aterm_appl,atermpp::aterm_appl> > variable_renaming;

  variable_list vl_new;

  const atermpp::aterm_appl t2=(t1_is_normal_form?t1:rewrite_internal(t1,sigma));
  std::set < variable > free_variables;
  // find_all_if(t2,is_a_variable(),std::inserter(free_variables,free_variables.begin()));
  get_free_variables(t2,free_variables);

  // Rename the bound variables to unique
  // variables, to avoid naming conflicts.

  for(variable_list::const_iterator i=vl.begin(); i!=vl.end(); ++i)
  {
    const variable v= *i;
    if (free_variables.count(v)>0)
    {
      const variable v_fresh(generator("ex_"), v.sort());
      variable_renaming[v]=atermpp::aterm_appl(v_fresh);
      vl_new.push_front(v_fresh);
    }
  }

  if (vl_new.empty())
  {
    return t2; // No quantified variables are bound.
  }

  const atermpp::aterm_appl t3=atermpp::replace(t2,variable_renaming);

  /* Create Enumerator */
  EnumeratorStandard ES(m_data_specification_for_enumeration, this);

  /* Find A solution*/
  EnumeratorSolutionsStandard sol(vl_new, t3, sigma,true,&ES,data::detail::get_enumerator_variable_limit(),true);

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


atermpp::aterm_appl Rewriter::internal_universal_quantifier_enumeration(
     const atermpp::aterm_appl &t,
     internal_substitution_type &sigma)
{
  assert(is_abstraction(t) && t(0)==gsMakeForall());
  /* Get Body of forall */
  const atermpp::aterm_appl t1 = atermpp::aterm_appl(t(2));
  const variable_list vl=static_cast<variable_list>(t(1));
  return internal_universal_quantifier_enumeration(vl,t1,false,sigma);
}

// Generate a term equivalent to forall vl.t1.
// The variable t1_is_normal_form indicates whether t1 is in normal
// form, but this information is not used as it stands.
atermpp::aterm_appl Rewriter::internal_universal_quantifier_enumeration(
      const variable_list &vl,
      const atermpp::aterm_appl &t1,
      const bool t1_is_normal_form,
      internal_substitution_type &sigma)
{
  mutable_map_substitution<std::map < atermpp::aterm_appl,atermpp::aterm_appl> > variable_renaming;

  variable_list vl_new;

  const atermpp::aterm_appl t2=(t1_is_normal_form?t1:rewrite_internal(t1,sigma));
  std::set < variable > free_variables;
  // find_all_if(t2,is_a_variable(),std::inserter(free_variables,free_variables.begin()));
  get_free_variables(t2,free_variables);

  // Rename the bound variables to unique
  // variables, to avoid naming conflicts.

  for(variable_list::const_iterator i=vl.begin(); i!=vl.end(); ++i)
  {
    const variable v= *i;
    if (free_variables.count(v)>0)
    {
      const variable v_fresh(generator("all_"), v.sort());
      variable_renaming[v]=atermpp::aterm_appl(v_fresh);
      vl_new.push_front(v_fresh);
    }
  }

  if (vl_new.empty())
  {
    return t2; // No quantified variables occur in the body.
  }

  const atermpp::aterm_appl t3=atermpp::replace(t2,variable_renaming);

  /* Create Enumerator */
  EnumeratorStandard ES(m_data_specification_for_enumeration, this);

  /* Find A solution*/
  EnumeratorSolutionsStandard sol(vl_new, t3, sigma,false,&ES,data::detail::get_enumerator_variable_limit(),true);

  /* Create lists to store solutions */
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
    else if (evaluated_condition(0) == internal_not)
    {
      evaluated_condition=static_cast<atermpp::aterm_appl>(evaluated_condition(1));
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


Rewriter* createRewriter(
            const data_specification& DataSpec,
            const used_data_equation_selector &equations_selector,
            const rewrite_strategy Strategy)
{
  switch (Strategy)
  {
    case jitty:
      return new RewriterJitty(DataSpec,equations_selector);
#ifdef MCRL2_JITTYC_AVAILABLE
    case jitty_compiling:
      return new RewriterCompilingJitty(DataSpec,equations_selector);
#endif
    case jitty_prover:
      return new RewriterProver(DataSpec,jitty,equations_selector);
#ifdef MCRL2_JITTYC_AVAILABLE
    case jitty_compiling_prover:
      return new RewriterProver(DataSpec,jitty_compiling,equations_selector);
#endif
    default:
      return NULL;
  }
}

//Prototype
static void check_vars(const data_expression &expr, const std::set <variable> &vars, std::set <variable> &used_vars);

static void check_vars(const data_expression_list &exprs, const std::set <variable> &vars, std::set <variable> &used_vars)
{
  for (data_expression_list::const_iterator i=exprs.begin(); i!=exprs.end(); ++i)
  {
    check_vars(*i,vars,used_vars);
  }
}

static void check_vars(const data_expression &expr, const std::set <variable> &vars, std::set <variable> &used_vars)
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
static void checkPattern(const data_expression &p);

static void checkPattern(const data_expression_list &l)
{
  for (data_expression_list::const_iterator i=l.begin(); i!=l.end(); ++i)
  {
    checkPattern(*i);
  }
}

static void checkPattern(const data_expression &p)
{
  if (is_application(p))
  {
    if (is_variable(application(p).head()))
    {
      throw mcrl2::runtime_error(std::string("variable ") + data::pp(application(p).head()) +
               " is used as head symbol in an application, which is not supported");
    }
    checkPattern(application(p).head());
    checkPattern(application(p).arguments());
  }
}

void CheckRewriteRule(const data_equation &data_eqn)
{
  const variable_list rule_var_list = data_eqn.variables();
  const std::set <variable> rule_vars(rule_var_list.begin(),rule_var_list.end());

  // collect variables from lhs and check that they are in rule_vars
  std::set <variable> lhs_vars;
  try
  {
    check_vars(data_eqn.lhs(),rule_vars,lhs_vars);
  }
  catch (variable& var)
  {
    // This should never occur if data_eqn is a valid data equation
    mCRL2log(log::error) << "Data Equation: " << atermpp::aterm_appl(data_eqn) << std::endl;
    assert(0);
    throw runtime_error("variable " + pp(var) + " occurs in left-hand side of equation but is not defined (in equation: " + pp(data_eqn) + ")");
  }

  // check that variables from the condition occur in the lhs
  try
  {
    std::set <variable> dummy;
    check_vars(data_eqn.condition(),lhs_vars,dummy);
  }
  catch (variable& var)
  {
    throw runtime_error("variable " + pp(var) + " occurs in condition of equation but not in left-hand side (in equation: " +
                    pp(data_eqn) + "); equation cannot be used as rewrite rule");
  }

  // check that variables from the rhs are occur in the lhs
  try
  {
    std::set <variable> dummy;
    check_vars(data_eqn.rhs(),lhs_vars,dummy);
  }
  catch (variable& var)
  {
    throw runtime_error("variable " + pp(var) + " occurs in right-hand side of equation but not in left-hand side (in equation: " +
                pp(data_eqn) + "); equation cannot be used as rewrite rule");
  }

  // check that the lhs is a supported pattern
  if (is_variable(data_eqn.lhs()))
  {
    throw runtime_error("left-hand side of equation is a variable; this is not allowed for rewriting");
  }
  try
  {
    checkPattern(data_eqn.lhs());
  }
  catch (mcrl2::runtime_error &s)
  {
    throw runtime_error(std::string(s.what()) + " (in equation: " + pp(data_eqn) + "); equation cannot be used as rewrite rule");
  }
}

bool isValidRewriteRule(const data_equation &data_eqn)
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
  return false; // compiler warning
}

std::vector <atermpp::function_symbol> apples;

/*************  Below the functions toInner and fromInner are being defined *********************/


std::map< function_symbol, atermpp::aterm_int > term2int;
std::vector < data::function_symbol > int2term;

atermpp::aterm_appl toInner(const data_expression &term, const bool add_opids)
{
  if (is_variable(term))
  {
    return term;
  }
  else if (is_application(term))
  {
    atermpp::term_list <atermpp::aterm> l;
    atermpp::aterm_appl arg0 = toInner(application(term).head(), add_opids);
    // Reflect the way of encoding the other arguments!
    if (is_variable(arg0) || is_abstraction(arg0) || is_where_clause(arg0))
    {
      l.push_front(arg0);
    }
    else
    {
      size_t arity = arg0.size(); 
      for (size_t i = 0; i < arity; ++i)
      {
        l.push_front(arg0(i));
      }
    }
    const data_expression_list &args= application(term).arguments();
    for (data_expression_list::const_iterator i=args.begin(); i!=args.end(); ++i)
    {
      l.push_front(toInner(*i,add_opids));
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
    const where_clause &t=term;
    atermpp::term_list<atermpp::aterm> l;
    const std::vector < assignment_expression > lv=atermpp::convert < std::vector < assignment_expression > >(t.declarations());
    for(std::vector < assignment_expression > :: const_reverse_iterator it=lv.rbegin() ; it!=lv.rend(); ++it)
    {
      l.push_front(core::detail::gsMakeDataVarIdInit(it->lhs(),toInner(it->rhs(),add_opids)));
    }
    return gsMakeWhr(toInner(t.body(),add_opids),l);
  }
  else if (is_abstraction(term))
  {
    const abstraction &t=term;

    return gsMakeBinder(t.binding_operator(),t.variables(),toInner(t.body(),add_opids));
  }
  assert(0); // term has unexpected format.
  return atermpp::aterm_appl();
}

data_expression fromInner(const atermpp::aterm_appl &term)
{
  using namespace atermpp;
  if (is_variable(term))
  {
    return aterm_cast<data_expression>(term);
  }

  if (is_where_clause(term))
  {
    const data_expression body=fromInner(aterm_cast<const aterm_appl>(term(0)));
    const term_list<atermpp::aterm_appl> &l=aterm_cast<const term_list<aterm_appl> >(term(1));

    std::vector < assignment_expression > lv;
    for(term_list<aterm_appl> :: const_iterator it=l.begin() ; it!=l.end(); ++it)
    {
      const aterm_appl &ass_expr= *it;
      lv.push_back(assignment_expression(variable(ass_expr(0)),fromInner(aterm_cast<const aterm_appl>(ass_expr(1)))));
    }
    return where_clause(body,lv);
  }

  if (is_abstraction(term))
  {
    return abstraction(binder_type(aterm_cast<const aterm_appl>(term(0))),variable_list(term(1)),fromInner(aterm_cast<const aterm_appl>(term(2))));
  }

  size_t arity = term.size();
  const atermpp::aterm &t = term(0);
  data_expression a;

  if (t.type_is_int())
  {
    a = get_int2term(aterm_cast<const aterm_int>(t).value());
  }
  else
  {
    a = fromInner(aterm_cast<const aterm_appl>(t));
  }

  size_t i = 1;
  sort_expression sort = a.sort();
  while (is_function_sort(sort) && (i < arity))
  {
    sort_expression_list sort_dom = function_sort(sort).domain();
    data_expression_list list;
    while (!sort_dom.empty())
    {
      assert(i < arity);
      list.push_front(fromInner(aterm_cast<const atermpp::aterm_appl>(term(i))));
      sort_dom.pop_front();
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
