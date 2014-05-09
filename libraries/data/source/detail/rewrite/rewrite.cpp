// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <cassert>
#include <stdexcept>
#include <string>
#include <cstring>
#include <limits>
#include <algorithm>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/core/down_cast.h"
#include "mcrl2/core/print.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/rewrite/jitty.h"
#include "mcrl2/data/detail/rewrite/jitty_jittyc.h"
#ifdef MCRL2_JITTYC_AVAILABLE
#include "mcrl2/data/detail/rewrite/jittyc.h"
#endif

#include "mcrl2/data/detail/rewrite/with_prover.h"

#include "mcrl2/data/detail/enum/standard.h" // To be removed.
#include "mcrl2/data/detail/rewriter_wrapper.h" // To be removed.
#include "mcrl2/data/classic_enumerator.h"

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::log;

namespace mcrl2
{
namespace data
{
namespace detail
{

static size_t npos()
{
  return size_t(-1);
}

// function object to test if it is an aterm_appl with function symbol "f"
struct is_a_variable
{
  bool operator()(const atermpp::aterm& t) const
  {
    return is_variable(atermpp::aterm_cast<atermpp::aterm_appl>(t));
  }
};

#ifndef NDEBUG
static
bool occur_check(const variable& v, const atermpp::aterm_appl& e)
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
  typename Rewriter::substitution_type& m_sigma;
  Rewriter& m_rewr;

  rewrite_list_rewriter(typename Rewriter::substitution_type& sigma, Rewriter& rewr):m_sigma(sigma),m_rewr(rewr)
  {}

  const data_expression operator() (const data_expression& t) const
  {
    return m_rewr.rewrite(t,m_sigma);
  }
};

data_expression_list Rewriter::rewrite_list(
     const data_expression_list& terms,
     substitution_type& sigma)
{
  rewrite_list_rewriter<Rewriter> r(sigma,*this);
  return data_expression_list(terms.begin(),terms.end(),r);
}

data_expression Rewriter::rewrite_where(
                      const where_clause& term,
                      substitution_type& sigma)
{
  const assignment_list& assignments = term.assignments();
  const data_expression& body=term.body();

  mutable_map_substitution<std::map < variable,data_expression> > variable_renaming;
  for(assignment_list::const_iterator i=assignments.begin(); i!=assignments.end(); ++i)
  {
    const variable& v=i->lhs();
    const variable v_fresh(generator("whr_"), v.sort());
    variable_renaming[v]=v_fresh;
    sigma[v_fresh]=rewrite(i->rhs(),sigma);
  }
  const data_expression result=rewrite(replace_variables(body,variable_renaming),sigma);

  // Reset variables in sigma
  for(mutable_map_substitution<std::map < variable,data_expression> >::const_iterator it=variable_renaming.begin();
      it!=variable_renaming.end(); ++it)
  {
    sigma[core::down_cast<variable>(it->second)]=it->second;
  }
  return result;
}

abstraction Rewriter::rewrite_single_lambda(
                      const variable_list& vl,
                      const data_expression& body,
                      const bool body_in_normal_form,
                      substitution_type& sigma)
{
  assert(vl.size()>0);
  // A lambda term without arguments; Take care that the bound variable is made unique with respect to
  // the variables occurring in sigma. But in case vl is empty, just rewrite...

  // First filter the variables in vl by those occuring as left/right hand sides in sigma.

  size_t number_of_renamed_variables=0;
  size_t count=0;
  std::vector<variable> new_variables(vl.size());
  {
    const std::set<variable>& variables_in_sigma(sigma.variables_in_rhs());
    // Create new unique variables to replace the old and create storage for
    // storing old values for variables in vl.
    for(variable_list::const_iterator it=vl.begin(); it!=vl.end(); ++it,count++)
    {
      const variable v= *it;
      if (variables_in_sigma.find(v) != variables_in_sigma.end() || sigma(v) != v)
      {
        number_of_renamed_variables++;
        new_variables[count]=data::variable(generator("y_"), v.sort());
        assert(occur_check(v, new_variables[count]));
      }
      else new_variables[count]=v;
    }
  }

  if (number_of_renamed_variables==0)
  {
    return abstraction(lambda_binder(),vl,(body_in_normal_form?body:rewrite(body,sigma)));
  }

  data_expression result;
  variable_list::const_iterator v;
  if (body_in_normal_form)
  {
    // If the body is already in normal form, a simple replacement of the old variables
    // by the new ones will do
    mutable_map_substitution<std::map<variable,data_expression> > variable_renaming;
    for(v = vl.begin(), count = 0; v != vl.end(); ++v, ++count)
    {
      if (*v != new_variables[count])
      {
        variable_renaming[*v] = new_variables[count];
      }
    }
    result = replace_variables(body, variable_renaming);
  }
  else
  {
    // If the body is not in normal form, then we have to rewrite with an updated sigma.
    // We first change sigma and save the values in sigma we overwrote...
    std::vector<data_expression> saved_substitutions;
    for(v = vl.begin(), count = 0; v != vl.end(); ++v, ++count)
    {
      if (*v != new_variables[count])
      {
        saved_substitutions.push_back(sigma(*v));
        sigma[*v] = new_variables[count];
      }
    }
    // ... then we rewrite with the new sigma ...
    result = rewrite(body,sigma);
    // ... and then we restore sigma to its old state.
    size_t new_variable_count = 0;
    for(v = vl.begin(), count = 0; v != vl.end(); ++v, ++count)
    {
      if (*v != new_variables[count])
      {
        sigma[*v] = saved_substitutions[new_variable_count++];
      }
    }
  }
  variable_list new_variable_list(new_variables.rbegin(), new_variables.rend());
  return abstraction(lambda_binder(),new_variable_list,result);
}


// The function rewrite_lambda_application assumes that t has the shape
// application(...application(lambda x:D...., arg1,...,argn),argn+1,...,argN).
// It applies the lambda term to its arguments, and rewrites the result to
// normal form.

data_expression Rewriter::rewrite_lambda_application(
                      const data_expression& t,
                      substitution_type& sigma)
{
  if (is_lambda(t))
  {
    const abstraction& ta(t);
    return rewrite_single_lambda(ta.variables(),ta.body(),false,sigma);
  }

  const application ta(t);
  if (is_lambda(ta.head()))
  {
    return rewrite_lambda_application(ta.head(),ta,sigma);
  }

  return rewrite(application(rewrite_lambda_application(ta.head(),sigma),ta.begin(),ta.end()),sigma);
}



// The function rewrite_lambda_application rewrites a lambda term to a set of
// arguments which are the arguments 1,...,n of t. If t has the shape
// #REWR#(t0,t1,....,tn) and the lambda term is L, we calculate the normal form
// in internal format for L(t1,...,tn). Note that the term t0 is ignored.
// Note that we assume that neither L, nor t is in normal form.

data_expression Rewriter::rewrite_lambda_application(
                      const abstraction& lambda_term,
                      const data_expression& t,
                      substitution_type& sigma)
{
  // using namespace atermpp;
  assert(is_lambda(lambda_term));  // The function symbol in this position cannot be anything else than a lambda term.
  const variable_list& vl=lambda_term.variables();
  const data_expression lambda_body=rewrite(lambda_term.body(),sigma);
  size_t arity=t.size();
  assert(arity>0);
  if (arity==1) // The term has shape application(lambda d..:D...t), i.e. without arguments.
  {
    return rewrite_single_lambda(vl, lambda_body, true, sigma);
  }
  assert(vl.size()<arity);

  mutable_map_substitution<std::map < variable,data_expression> > variable_renaming;
  size_t count=1;
  for(variable_list::const_iterator i=vl.begin(); i!=vl.end(); ++i, ++count)
  {
    const variable v= (*i);
    const variable v_fresh(generator("x_"), v.sort());
    variable_renaming[v]=v_fresh;
    sigma[v_fresh]=rewrite(data_expression(t[count]),sigma);
  }

  const data_expression result=rewrite(replace_variables(lambda_body,variable_renaming),sigma);

  // Reset variables in sigma
  for(mutable_map_substitution<std::map < variable,data_expression> >::const_iterator it=variable_renaming.begin();
                 it!=variable_renaming.end(); ++it)
  {
    sigma[core::down_cast<variable>(it->second)]=it->second;
  }
  if (vl.size()+1==arity)
  {
    return result;
  }

  // There are more arguments than bound variables.
  std::vector < data_expression > args;
  for(size_t i=1; i<arity-vl.size(); ++i)
  {
    assert(vl.size()+i<arity);
    args.push_back(atermpp::aterm_cast<data_expression>(t[vl.size()+i]));
  }
  // We do not employ the knowledge that the first argument is in normal form... TODO.
  return rewrite(application(result, args.begin(), args.end()),sigma);
}

data_expression Rewriter::existential_quantifier_enumeration(
     const abstraction& t,
     substitution_type& sigma)
{
  // This is a quantifier elimination that works on the existential quantifier as specified
  // in data types, i.e. without applying the implement function anymore.

  assert(is_exists(t));
  return existential_quantifier_enumeration(t.variables(),t.body(),false,sigma);
}

// Generate a term equivalent to exists vl.t1.
// The variable t1_is_normal_form indicates whether t1 is in normal
// form, but this information is not used as it stands.
data_expression Rewriter::existential_quantifier_enumeration(
      const variable_list& vl,
      const data_expression& t1,
      const bool t1_is_normal_form,
      substitution_type& sigma)
{


  // Rename the bound variables to unique
  // variables, to avoid naming conflicts.

  mutable_map_substitution<std::map < variable,data_expression> > variable_renaming;
  variable_vector vl_new_v;
  for(variable_list::const_iterator i=vl.begin(); i!=vl.end(); ++i)
  {
    const variable v= *i;
    if (sigma(v)!=v)
    {
      const variable v_fresh(generator("ex_"), v.sort());
      variable_renaming[v]=v_fresh;
      vl_new_v.push_back(v_fresh);
    }
    else
    {
      vl_new_v.push_back(v);
    }
  }

  const data_expression t2=replace_variables(t1,variable_renaming);
  const data_expression t3=(t1_is_normal_form?t2:rewrite(t2,sigma));

  // Check whether the bound variables occur free in the rewritten body
  std::set < variable > free_variables=find_free_variables(t3);
  variable_list vl_new_l;

  bool sorts_are_finite=true;
  for(variable_vector::const_reverse_iterator i=vl_new_v.rbegin(); i!=vl_new_v.rend(); ++i)
  {
    const variable v= *i;
    if (free_variables.count(v)>0)
    {
      vl_new_l.push_front(v);
      sorts_are_finite=sorts_are_finite && m_data_specification_for_enumeration.is_certainly_finite(v.sort());
    }
  }

  if (vl_new_l.empty())
  {
    return t3; // No quantified variables are bound.
  }

  /* Find A solution*/
  /* EnumeratorSolutionsStandard<data_expression,rewriter>
                              sol(vl_new_l, t3, sigma,true,
                                  m_data_specification_for_enumeration, this,
                                  (sorts_are_finite?npos():data::detail::get_enumerator_variable_limit()),true); */

  typedef classic_enumerator<rewriter_wrapper> enumerator_type;
  rewriter_wrapper wrapped_rewriter(this);
  enumerator_type enumerator(m_data_specification_for_enumeration, wrapped_rewriter);
    
   /*                            sol(vl_new_l, t3, sigma,true,
                                  m_data_specification_for_enumeration, this,
                                  (sorts_are_finite?npos():data::detail::get_enumerator_variable_limit()),true); */

  /* Create a list to store solutions */
  data_expression partial_result=sort_bool::false_();
  bool solution_possible=true;

  size_t loop_upperbound=(sorts_are_finite?npos():10);
  for(enumerator_type::iterator sol=enumerator.begin(vl_new_l, t3, sigma, true, (sorts_are_finite?npos():data::detail::get_enumerator_variable_limit()),true);
         loop_upperbound>0 && 
         partial_result!=sort_bool::true_() &&
         sol!=enumerator.end(); 
         ++sol)
  {
    if (partial_result==sort_bool::false_())
    {
      partial_result=sol.resulting_condition();
    }
    else if (partial_result!=sort_bool::true_())
    {
      partial_result=application(sort_bool::or_(), partial_result,sol.resulting_condition());
    }
    loop_upperbound--;
  }

  if (solution_possible && (loop_upperbound>0 || partial_result==sort_bool::true_()))
  {
    return partial_result;
  }

  // One can consider to replace the variables by their original, in order to not show
  // internally generated variables in the output.
  return abstraction(exists_binder(),vl_new_l,rewrite(t3,sigma));
}


data_expression Rewriter::universal_quantifier_enumeration(
     const abstraction& t,
     substitution_type& sigma)
{
  assert(is_forall(t));
  return universal_quantifier_enumeration(t.variables(),t.body(),false,sigma);
}

// Generate a term equivalent to forall vl.t1.
// The variable t1_is_normal_form indicates whether t1 is in normal
// form, but this information is not used as it stands.
data_expression Rewriter::universal_quantifier_enumeration(
      const variable_list& vl,
      const data_expression& t1,
      const bool t1_is_normal_form,
      substitution_type& sigma)
{
  // Rename the bound variables to unique
  // variables, to avoid naming conflicts.

  mutable_map_substitution<std::map < variable,data_expression> > variable_renaming;
  variable_vector vl_new_v;
  for(variable_list::const_iterator i=vl.begin(); i!=vl.end(); ++i)
  {
    const variable v= *i;
    if (sigma(v)!=v)  // Check whether sigma is defined on v. If not, renaming is not necessary.
    {
      const variable v_fresh(generator("all_"), v.sort());
      variable_renaming[v]=v_fresh;
      vl_new_v.push_back(v_fresh);
    }
    else
    {
      vl_new_v.push_back(v);
    }
  }

  const data_expression t2=replace_variables(t1,variable_renaming);
  const data_expression t3=(t1_is_normal_form?t2:rewrite(t2,sigma));

  // Check whether the bound variables occur free in the rewritten body.
  std::set < variable > free_variables=find_free_variables(t3);
  variable_list vl_new_l;

  bool sorts_are_finite=true;
  for(variable_vector::const_reverse_iterator i=vl_new_v.rbegin(); i!=vl_new_v.rend(); ++i)
  {
    const variable v= *i;
    if (free_variables.count(v)>0)
    {
      vl_new_l.push_front(v);
      sorts_are_finite=sorts_are_finite && m_data_specification_for_enumeration.is_certainly_finite(v.sort());
    }
  }

  if (vl_new_l.empty())
  {
    return t3; // No quantified variables occur in the rewritten body.
  }

  /* Find A solution*/

 /*  EnumeratorSolutionsStandard<data_expression,rewriter>
                              sol(vl_new_l, t3, sigma,false,
                                  m_data_specification_for_enumeration, this,
                                  (sorts_are_finite?npos():data::detail::get_enumerator_variable_limit()),true); */
  typedef classic_enumerator<rewriter_wrapper> enumerator_type;
  rewriter_wrapper wrapped_rewriter(this);
  enumerator_type enumerator(m_data_specification_for_enumeration, wrapped_rewriter);


  /* Create lists to store solutions */
  data_expression partial_result=sort_bool::true_();
  bool solution_possible=true;

  size_t loop_upperbound=(sorts_are_finite?npos():10);
  for(enumerator_type::iterator sol=enumerator.begin(vl_new_l, t3, sigma, false, (sorts_are_finite?npos():data::detail::get_enumerator_variable_limit()),true);
         loop_upperbound>0 &&
         partial_result!=sort_bool::false_() &&
         sol!=enumerator.end();
         ++sol)
  {
    if (partial_result==sort_bool::true_())
    {
      partial_result=sol.resulting_condition();
    }
    else if (partial_result!=sort_bool::false_())
    {
      partial_result=application(sort_bool::and_(), partial_result, sol.resulting_condition());
    }
    loop_upperbound--;
  }

  if (solution_possible && (loop_upperbound>0 || partial_result==sort_bool::false_()))
  {
    return partial_result;
  }

  // One can consider to replace the variables by their original, in order to not show
  // internally generated variables in the output.
  return abstraction(forall_binder(),vl_new_l,rewrite(t3,sigma));
}


Rewriter* createRewriter(
            const data_specification& DataSpec,
            const used_data_equation_selector& equations_selector,
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
static void check_vars(const data_expression& expr, const std::set <variable>& vars, std::set <variable>& used_vars);

static void check_vars(application::const_iterator begin,
                       const application::const_iterator& end,
                       const std::set <variable>& vars,
                       std::set <variable>& used_vars)
{
  while (begin != end)
  {
    check_vars(*begin++, vars, used_vars);
  }
}

static void check_vars(const data_expression& expr, const std::set <variable>& vars, std::set <variable>& used_vars)
{
  if (is_application(expr))
  {
    const application& a=core::down_cast<const application>(expr);
    check_vars(a.head(),vars,used_vars);
    check_vars(a.begin(),a.end(),vars,used_vars);
  }
  else if (is_variable(expr))
  {
    const variable& v=core::down_cast<const variable>(expr);
    used_vars.insert(v);

    if (vars.count(v)==0)
    {
      throw v;
    }
  }
}

//Prototype
static void checkPattern(const data_expression& p);

static void checkPattern(application::const_iterator begin,
                         const application::const_iterator& end)
{
  while (begin != end)
  {
    checkPattern(*begin++);
  }
}

static void checkPattern(const data_expression& p)
{
  if (is_application(p))
  {
    if (is_variable(core::down_cast<application>(p).head()))
    {
      throw mcrl2::runtime_error(std::string("variable ") + data::pp(application(p).head()) +
               " is used as head symbol in an application, which is not supported");
    }
    const application& a=core::down_cast<const application>(p);
    checkPattern(a.head());
    checkPattern(a.begin(),a.end());
  }
}

void CheckRewriteRule(const data_equation& data_eqn)
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
    mCRL2log(log::error) << "Data Equation: " << data_expression(data_eqn) << std::endl;
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
  catch (mcrl2::runtime_error& s)
  {
    throw runtime_error(std::string(s.what()) + " (in equation: " + pp(data_eqn) + "); equation cannot be used as rewrite rule");
  }
}

bool isValidRewriteRule(const data_equation& data_eqn)
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

}
}
}
