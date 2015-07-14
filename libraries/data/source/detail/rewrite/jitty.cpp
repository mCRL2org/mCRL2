// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file jitty.cpp

#include "mcrl2/data/detail/rewrite/jitty.h"
#include "mcrl2/data/detail/rewrite/jitty_jittyc.h"

#define NAME std::string("rewr_jitty")

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include "boost/config.hpp"

#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/data/replace.h"

#ifdef MCRL2_DISPLAY_REWRITE_STATISTICS
#include "mcrl2/data/detail/rewrite_statistics.h"
#endif

using namespace mcrl2::log;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;

namespace mcrl2
{
namespace data
{
namespace detail
{


typedef const atermpp::detail::_aterm* unprotected_variable;           // Variable that is not protected (so a copy should exist at some other place)
typedef const atermpp::detail::_aterm* unprotected_data_expression;    // Idem, but now a data expression.

// The function symbol below is used to administrate that a term is in normal form. It is put around a term.
// Terms with this auxiliary function symbol cannot be printed using the pretty printer for data expressions.


const function_symbol this_term_is_in_normal_form()
{
  static const function_symbol this_term_is_in_normal_form(std::string("Rewritten@@term"),function_sort({ untyped_sort() },untyped_sort()));
  return this_term_is_in_normal_form;
}

// The function below is intended to remove the auxiliary function this_term_is_in_normal_form from a term
// such that it can for instance be pretty printed.

data_expression remove_normal_form_function(const data_expression& t)
{
  if (is_variable(t))
  {
    return t;
  }

  if (is_function_symbol(t))
  {
    assert(t!=this_term_is_in_normal_form());
    return t;
  }

  if (is_application(t))
  {
    const application& ta=atermpp::down_cast<application>(t);
    if (ta.head()==this_term_is_in_normal_form())
    {
      assert(ta.size()==1);
      return ta[0];
    }

    std::vector<data_expression> args;
    for(const data_expression& arg:ta)
    {
      args.push_back(remove_normal_form_function(arg));
    }
    return application(remove_normal_form_function(ta.head()),args.begin(),args.end());
  }


  if (is_where_clause(t))
  {
    const where_clause& t1=atermpp::down_cast<where_clause>(t);
    const assignment_expression_list& assignments=t1.declarations();
    const data_expression& body=t1.body();

    assignment_vector new_assignments;
    for(const assignment_expression& ae:assignments)
    {
      const assignment& assignment_expr = atermpp::down_cast<assignment>(ae);
      new_assignments.push_back(assignment(assignment_expr.lhs(), remove_normal_form_function(assignment_expr.rhs())));
    }
    return where_clause(remove_normal_form_function(body),assignment_list(new_assignments.begin(),new_assignments.end()));
  }

  assert(is_abstraction(t));

  const abstraction& t1=atermpp::down_cast<abstraction>(t);
  const binder_type& binder=t1.binding_operator();
  const variable_list& bound_variables=t1.variables();

  variable_vector new_variables;
  mutable_map_substitution<> sigma;
  bool sigma_trivial=true;
  data_expression body=t1.body();
  if (!sigma_trivial)
  {
    body=replace_variables(body,sigma);
  }

  return abstraction(binder, bound_variables, remove_normal_form_function(body));
}

class jitty_argument_rewriter
{
  protected:
    mutable_indexed_substitution<>& m_sigma;
    RewriterJitty& m_r;
  public:
    jitty_argument_rewriter(mutable_indexed_substitution<>& sigma, RewriterJitty& r)
     : m_sigma(sigma), m_r(r)
    {}

  data_expression operator()(const data_expression& t)
  {
    return m_r.rewrite(t, m_sigma);
  }
};

class dependencies_rewrite_rule_pair
{
  protected:
    std::set<size_t> m_dependencies;
    data_equation m_equation;

  public:
    dependencies_rewrite_rule_pair(std::set<size_t>& dependencies, const data_equation& eq)
     : m_dependencies(dependencies), m_equation(eq)
    {}

    const std::set<size_t>& dependencies() const
    {
      return m_dependencies;
    }

    const data_equation equation() const
    {
      return m_equation;
    }
};

strategy RewriterJitty::create_strategy(const data_equation_list& rules1)
{
  data_equation_list rules=rules1;
  std::vector<strategy_rule> strat;

  std::vector <bool> used;

  size_t arity = 0;
  while (!rules.empty())
  {
    data_equation_list l;
    std::vector<dependencies_rewrite_rule_pair> m;

    std::vector<int> args(arity,-1);

    for (data_equation_list::const_iterator i=rules.begin(); i!=rules.end(); ++i)
    {
      const data_equation& this_rule = *i;
      const data_expression& this_rule_lhs = this_rule.lhs();
      if ((is_function_symbol(this_rule_lhs)?1:detail::recursive_number_of_args(this_rule_lhs)+1) == arity + 1)
      {
        const data_expression& cond = this_rule.condition();
        atermpp::term_list<variable_list> vars = { get_free_vars(cond) };

        std::vector < bool> bs(arity,false);

        for (size_t i = 0; i < arity; i++)
        {
          const data_expression this_rule_lhs_iplus1_arg=detail::get_argument_of_higher_order_term(atermpp::down_cast<application>(this_rule_lhs),i);
          if (!is_variable(this_rule_lhs_iplus1_arg))
          {
            bs[i] = true;
            const variable_list evars = get_free_vars(this_rule_lhs_iplus1_arg);
            for (variable_list::const_iterator v=evars.begin(); v!=evars.end(); ++v)
            {
              int j=0;
              const atermpp::term_list <variable_list> next_vars=vars.tail();
              for (atermpp::term_list <variable_list>::const_iterator o=next_vars.begin(); o!=next_vars.end(); ++o)
              {
                if (std::find(o->begin(),o->end(),*v) != o->end())
                {
                  bs[j] = true;
                }
                j++;
              }
            }
            vars=push_back(vars,get_free_vars(this_rule_lhs_iplus1_arg));
          }
          else
          {
            int j = -1;
            bool b = false;
            for (atermpp::term_list <variable_list>::const_iterator o=vars.begin(); o!=vars.end(); ++o)
            {
              if (std::find(o->begin(),o->end(),variable(this_rule_lhs_iplus1_arg)) != o->end())
              {
                if (j >= 0)
                {
                  bs[j] = true;
                }
                b = true;
              }
              j++;
            }
            if (b)
            {
              bs[i] = true;
            }
            vars=push_back(vars,get_free_vars(this_rule_lhs_iplus1_arg));
          }
        }

        double_variable_traverser<data::variable_traverser> lhs_doubles;
        double_variable_traverser<data::variable_traverser> rhs_doubles;
        std::set<variable> condition_vars = find_free_variables(this_rule.condition());
        lhs_doubles.apply(this_rule.lhs());
        rhs_doubles.apply(this_rule.rhs());

        std::set<size_t> deps;
        for (size_t i = 0; i < arity; i++)
        {
          if (i>=used.size())
          {
            used.resize(i+1,false);
          }
          // Check whether argument i is a variable that occurs more than once in
          // the left or right hand side, or occurs in the condition. It is not clear whether it is
          // useful to check that it occurs in the condition, but this is what the jittyc rewriter also does.
          const data_expression& arg_i = get_argument_of_higher_order_term(atermpp::down_cast<application>(this_rule.lhs()), i);
          if ((bs[i] ||
               (is_variable(arg_i) && (lhs_doubles.result().count(atermpp::down_cast<variable>(arg_i)) > 0 ||
                                       condition_vars.count(atermpp::down_cast<variable>(arg_i)) > 0 ||
                                       rhs_doubles.result().count(atermpp::down_cast<variable>(arg_i)) > 0))
              ) && !used[i])
          {
            deps.insert(i);
            args[i] += 1;
          }
        }

        m.push_back(dependencies_rewrite_rule_pair(deps,this_rule));
      }
      else
      {
        l.push_front(this_rule);
      }
    }

    while (!m.empty())
    {
      std::vector<dependencies_rewrite_rule_pair> m2;
      for (std::vector<dependencies_rewrite_rule_pair>::const_iterator i=m.begin(); i!=m.end(); ++i)
      {
        if (i->dependencies().empty())
        {
          const data_equation rule = i->equation();
          strat.push_back(strategy_rule(rule));
          size_t len = rule.variables().size();
          if (len>MAX_LEN)
          {
            MAX_LEN=len;
          }
        }
        else
        {
          m2.push_back(*i);
        }
      }
      m = m2;

      if (m.empty())
      {
        break;
      }

      int max = -1;
      size_t maxidx = 0;

      for (size_t i = 0; i < arity; i++)
      {
        assert(i<((size_t)1)<<(8*sizeof(int)-1));
        if (args[i] > max)
        {
          maxidx = i+1;
          max = args[i];
        }
      }

      if (maxidx > 0)
      {
        args[maxidx-1] = -1;
        if (maxidx>used.size())
        {
          used.resize(maxidx,false);
        }
        used[maxidx-1] = true;

        const size_t k(maxidx-1);
        strat.push_back(strategy_rule(k));
        m2.clear();
        for (std::vector<dependencies_rewrite_rule_pair>::const_iterator i=m.begin(); i!=m.end(); ++i)
        {
          const data_equation eq=i->equation();
          std::set<size_t> dependencies=i->dependencies();
          dependencies.erase(k);
          m2.push_back(dependencies_rewrite_rule_pair(dependencies,eq));
        }
        m = m2;
      }
    }

    rules = reverse(l);
    arity++;
  }
  return strategy(strat.begin(),strat.end());
}

void RewriterJitty::make_jitty_strat_sufficiently_larger(const size_t i)
{
  if (i>=jitty_strat.size())
  {
    jitty_strat.resize(i+1);
  }
}

void RewriterJitty::rebuild_strategy()
{
  jitty_strat.clear();
  for(std::map< function_symbol, data_equation_list >::const_iterator l=jitty_eqns.begin(); l!=jitty_eqns.end(); ++l)
  {
    const size_t i=core::index_traits<data::function_symbol, function_symbol_key_type, 2>::index(l->first);
    make_jitty_strat_sufficiently_larger(i);
    jitty_strat[i] = create_strategy(reverse(l->second));
  }

}

RewriterJitty::RewriterJitty(
           const data_specification& data_spec,
           const mcrl2::data::used_data_equation_selector& equation_selector):
        Rewriter(data_spec,equation_selector)
{
  MAX_LEN=0;
  max_vars = 0;

  const std::vector< data_equation >& l = data_spec.equations();
  for (std::vector< data_equation >::const_iterator j=l.begin(); j!=l.end(); ++j)
  {
    if (equation_selector(*j))
    {
      try
      {
        CheckRewriteRule(*j);
      }
      catch (std::runtime_error& e)
      {
        mCRL2log(warning) << e.what() << std::endl;
        continue;
      }

      const function_symbol lhs_head_index=get_function_symbol_of_head(j->lhs());

      data_equation_list n;
      std::map< function_symbol, data_equation_list >::iterator it = jitty_eqns.find(lhs_head_index);
      if (it != jitty_eqns.end())
      {
        n = it->second;
      }
      if (j->variables().size() > max_vars)
      {
        max_vars = j->variables().size();
      }
      n.push_front(*j);
      jitty_eqns[lhs_head_index] = n;
    }
  }

  rebuild_strategy();
}

RewriterJitty::~RewriterJitty()
{
}

static data_expression subst_values(
            const unprotected_variable* vars,
            const unprotected_data_expression* terms,
            const bool* variable_is_a_normal_form,
            const size_t assignment_size,
            const data_expression& t,
            data::set_identifier_generator& generator); // prototype;

class subst_values_argument
{
  private:
    const unprotected_variable* m_vars;
    const unprotected_data_expression* m_terms;
    const bool* m_variable_is_a_normal_form;
    const size_t m_assignment_size;
    data::set_identifier_generator& m_generator;

  public:
    subst_values_argument(const unprotected_variable* vars,
                          const unprotected_data_expression* terms,
                          const bool* variable_is_a_normal_form,
                          const size_t assignment_size,
                          data::set_identifier_generator& generator)
      : m_vars(vars),
        m_terms(terms),
        m_variable_is_a_normal_form(variable_is_a_normal_form),
        m_assignment_size(assignment_size),
        m_generator(generator)
    {}

    data_expression operator()(const data_expression& t) const
    {
      return subst_values(m_vars,m_terms,m_variable_is_a_normal_form,m_assignment_size,t,m_generator);
    }
};

static data_expression subst_values(
            const unprotected_variable* vars,
            const unprotected_data_expression* terms,
            const bool* variable_is_a_normal_form,
            const size_t assignment_size,
            const data_expression& t,
            data::set_identifier_generator& generator) // This generator is used for the generation of fresh variable names.
{
  if (is_function_symbol(t))
  {
    return t;
  }
  else if (is_variable(t))
  {
    for (size_t i=0; i<assignment_size; i++)
    {
      if (t==vars[i])
      {
        if (variable_is_a_normal_form[i])
        {
          return application(this_term_is_in_normal_form(),data_expression(terms[i]));  // Variables that are in normal form get a tag that they are in normal form.
        }
        return data_expression(terms[i]);
      }
    }
    return data_expression(t);
  }
  else if (is_abstraction(t))
  {
    const abstraction& t1=atermpp::down_cast<abstraction>(t);
    const binder_type& binder=t1.binding_operator();
    const variable_list& bound_variables=t1.variables();
    // Check that variables in the left and right hand sides of equations do not clash with bound variables.
    std::set<variable> variables_in_substitution;
    for(size_t i=0; i<assignment_size; ++i)
    {
      std::set<variable> s=find_free_variables(data_expression(terms[i]));
      variables_in_substitution.insert(s.begin(),s.end());
      variables_in_substitution.insert(variable(vars[i]));
    }

    variable_vector new_variables;
    mutable_map_substitution<> sigma;
    bool sigma_trivial=true;
    for(variable_list::const_iterator it=bound_variables.begin(); it!=bound_variables.end(); ++it)
    {
      if (variables_in_substitution.count(*it)>0)
      {
        // Replace *it in the list and in the body by a new variable name.
        const variable fresh_variable(generator(it->name()),it->sort());
        new_variables.push_back(fresh_variable);
        sigma[*it]=fresh_variable;
        sigma_trivial=false;
      }
      else
      {
        new_variables.push_back(*it);
      }
    }
    data_expression body=t1.body();
    if (!sigma_trivial)
    {
      body=replace_variables(body,sigma);
    }

    return abstraction(binder,
                       variable_list(new_variables.begin(),new_variables.end()),
                       subst_values(vars,terms,variable_is_a_normal_form,assignment_size,body,generator));

  }
  else if (is_where_clause(t))
  {
    const where_clause& t1=atermpp::down_cast<where_clause>(t);
    const assignment_expression_list& assignments=t1.declarations();
    const data_expression& body=t1.body();

#ifndef NDEBUG
    // Check that variables in right hand sides of equations do not clash with bound variables.
    for(size_t i=0; i<assignment_size; ++i)
    {
      for(assignment_expression_list::const_iterator it=assignments.begin(); it!=assignments.end(); ++it)
      {
        assert((*it)[0]!= vars[i]);
      }
    }
#endif

    assignment_vector new_assignments;

    for(assignment_expression_list::const_iterator it=assignments.begin(); it!=assignments.end(); ++it)
    {
      const assignment& assignment_expr = atermpp::down_cast<assignment>(*it);
      new_assignments.push_back(assignment(assignment_expr.lhs(), subst_values(vars,terms,variable_is_a_normal_form,assignment_size,assignment_expr.rhs(),generator)));
    }
    return where_clause(subst_values(vars,terms,variable_is_a_normal_form,assignment_size,body,generator),assignment_list(new_assignments.begin(),new_assignments.end()));
  }
  else
  {
    const application& t1 = atermpp::down_cast<application>(t);
    const subst_values_argument substitute_values_in_arguments(vars,terms,variable_is_a_normal_form,assignment_size,generator);
    return application(subst_values(vars,terms,variable_is_a_normal_form,assignment_size,t1.head(),generator),t1.begin(),t1.end(),substitute_values_in_arguments);
  }
}

// Match term t with the lhs p of an equation.

static bool match_jitty(
                    const data_expression& t,
                    const data_expression& p,
                    unprotected_variable* vars,
                    unprotected_data_expression* terms,
                    bool* variable_is_in_normal_form,
                    size_t& assignment_size,
                    const bool term_context_guarantees_normal_form)
{
  if (is_function_symbol(p))
  {
    return p==t;
  }
  else if (is_variable(p))
  {

    for (size_t i=0; i<assignment_size; i++)
    {
      if (p==vars[i])
      {
        if (t==terms[i])
        {
          return true;
        }
        else
        {
          return false;
        }
      }
    }

    /* new (&vars[assignment_size]) variable(atermpp::down_cast<variable>(p));
    new (&terms[assignment_size]) data_expression(t); */
    vars[assignment_size]=atermpp::detail::address(p);
    terms[assignment_size]=atermpp::detail::address(t);
    variable_is_in_normal_form[assignment_size]=term_context_guarantees_normal_form;
    assignment_size++;
    return true;
  }
  else
  {
    if (is_function_symbol(t) || is_variable(t) || is_abstraction(t) || is_where_clause(t))
    {
      return false;
    }
    // p and t must be applications.
    assert(term_context_guarantees_normal_form); // If the argument must match an expression it must be a normal form.

    const application& pa=atermpp::down_cast<application>(p);
    const application& ta=atermpp::down_cast<application>(t);
    if (pa.size()!=ta.size()) // are p and t applications of the same arity?
    {
      return false;
    }


    if (!match_jitty(ta.head(),
                     pa.head(),vars,terms,variable_is_in_normal_form,assignment_size,true))
    {
      return false;
    }

    size_t arity = pa.size();
    for (size_t i=0; i<arity; i++)
    {
      if (!match_jitty(ta[i], pa[i],vars,terms,variable_is_in_normal_form,assignment_size,true))
      {
        return false;
      }
    }

    return true;
  }
}

data_expression RewriterJitty::rewrite_aux(
                      const data_expression& term,
                      substitution_type& sigma)
{
  if (is_application(term))
  {
    const application terma=atermpp::down_cast<application>(term);
    if (terma.head()==this_term_is_in_normal_form())
    {
      assert(terma.size()==1);
      return terma[0];
    }
  }
  if (is_function_symbol(term))
  {
    assert(term!=this_term_is_in_normal_form());
    return rewrite_aux_function_symbol(atermpp::down_cast<const function_symbol>(term),term,sigma,false);
  }
  if (is_variable(term))
  {
    return sigma(atermpp::down_cast<variable>(term));
  }
  if (is_where_clause(term))
  {
    const where_clause& w = atermpp::down_cast<where_clause>(term);
    return rewrite_where(w,sigma);
  }
  if (is_abstraction(term))
  {
    const abstraction& ta(term);
    if (is_exists(ta))
    {
      return existential_quantifier_enumeration(ta,sigma);
    }
    if (is_forall(ta))
    {
      return universal_quantifier_enumeration(ta,sigma);
    }
    assert(is_lambda(ta));
    return rewrite_single_lambda(ta.variables(),ta.body(),false,sigma);
  }

  // The variable term has the shape appl(t,t1,...,tn);

  // First check whether t has the shape appl(appl...appl(f,u1,...,un)(...)(...) where f is a function symbol.
  // In this case rewrite that function symbol. This is an optimisation. If this does not apply t is rewritten,
  // including all its subterms. But this is costly, as not all subterms will be rewritten again
  // in rewrite_aux_function_symbol.

  function_symbol head;

  if (detail::head_is_function_symbol(term,head) && head!=this_term_is_in_normal_form())
  {
    return rewrite_aux_function_symbol(head,term,sigma,false);
  }

  const application& tapp=atermpp::down_cast<application>(term);
  data_expression t=tapp.head();
  t = rewrite_aux(t,sigma);
  // Here t has the shape f(u1,....,un)(u1',...,um')....: f applied several times to arguments,
  // x(u1,....,un)(u1',...,um')....: x applied several times to arguments, or
  // binder x1,...,xn.t' where the binder is a lambda, exists or forall.

  if (head_is_function_symbol(t,head))
  {
    // In this case t has the shape f(u1...un)(u1'...um')....  where all u1,...,un,u1',...,um' are normal formas.
    // In the invocation of rewrite_aux_function_symbol these terms are rewritten to normalform again.
    const data_expression result=application(t,tapp.begin(), tapp.end());
    return rewrite_aux_function_symbol(head,result,sigma,true);
  }
  else if (head_is_variable(t))
  {
    // return appl(t,t1,...,tn) where t1,...,tn still need to be rewritten.
    jitty_argument_rewriter r(sigma,*this);
    return application(t,tapp.begin(),tapp.end(),r);
  }
  assert(is_abstraction(t));
  const abstraction& ta(t);
  const binder_type& binder(ta.binding_operator());
  if (is_lambda_binder(binder))
  {
    return rewrite_lambda_application(t,term,sigma);
  }
  if (is_exists_binder(binder))
  {
    assert(term.size()==1);
    return existential_quantifier_enumeration(t,sigma);
  }
  assert(is_forall_binder(binder));
  assert(term.size()==1);
  return universal_quantifier_enumeration(head,sigma);
}

data_expression RewriterJitty::rewrite_aux_function_symbol(
                      const function_symbol& op,
                      const data_expression& term,
                      substitution_type& sigma,
                      const bool first_term_is_a_normal_form)
{
  // The first term is function symbol; apply the necessary rewrite rules using a jitty strategy.

  const size_t arity=(is_function_symbol(term)?0:detail::recursive_number_of_args(term));

  MCRL2_SYSTEM_SPECIFIC_ALLOCA(rewritten,data_expression, arity);
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(rewritten_defined,bool, arity);

  for(size_t i=0; i<arity; ++i)
  {
    rewritten_defined[i]=false;
  }
  if (first_term_is_a_normal_form)
  {
    assert(arity>0);
    new (&rewritten[0]) data_expression(detail::get_argument_of_higher_order_term(atermpp::down_cast<application>(term),0));
    rewritten_defined[0]=true;
  }

  const size_t op_value=core::index_traits<data::function_symbol,function_symbol_key_type, 2>::index(op);
  if (op_value>=jitty_strat.size())
  {
    make_jitty_strat_sufficiently_larger(op_value);
  }

  const strategy strat=jitty_strat[op_value];
  if (!strat.empty())
  {
    MCRL2_SYSTEM_SPECIFIC_ALLOCA(vars,unprotected_variable,max_vars);
    MCRL2_SYSTEM_SPECIFIC_ALLOCA(terms,unprotected_data_expression,max_vars);
    MCRL2_SYSTEM_SPECIFIC_ALLOCA(variable_is_in_normal_form,bool,max_vars);
    size_t no_assignments=0;
    for (const strategy_rule& rule: strat)
    {
      if (rule.is_rewrite_index())
      {
        const size_t i = rule.rewrite_index();
        if (i < arity)
        {
          assert(!rewritten_defined[i]||i==0);
          if (!rewritten_defined[i])
          {
            new (&rewritten[i]) data_expression(rewrite_aux(detail::get_argument_of_higher_order_term(atermpp::down_cast<application>(term),i),sigma));
            rewritten_defined[i]=true;
          }
          assert(rewritten[i].defined());
        }
        else
        {
          break;
        }
      }
      else
      {
        const data_equation rule1=rule.equation();
        const data_expression lhs=rule1.lhs();
        size_t rule_arity = (is_function_symbol(lhs)?0:detail::recursive_number_of_args(lhs));

        if (rule_arity > arity)
        {
          break;
        }

        assert(no_assignments==0);

        bool matches = true;
        for (size_t i=0; i<rule_arity; i++)
        {
          assert(i<arity);
          if (!match_jitty(rewritten_defined[i]?rewritten[i]:detail::get_argument_of_higher_order_term(atermpp::down_cast<application>(term),i),
                           detail::get_argument_of_higher_order_term(atermpp::down_cast<application>(lhs),i),
                           vars,terms,variable_is_in_normal_form,no_assignments,rewritten_defined[i]))
          {
            matches = false;
            break;
          }
        }
        if (matches)
        {
          if (rule1.condition()==sort_bool::true_() || rewrite_aux(
                   subst_values(vars,terms,variable_is_in_normal_form,no_assignments,rule1.condition(),generator),sigma)==sort_bool::true_())
          {
            const data_expression& rhs=rule1.rhs();

            if (arity == rule_arity)
            {
              const data_expression result=rewrite_aux(subst_values(vars,terms,variable_is_in_normal_form,no_assignments,rhs,generator),sigma);
              for (size_t i=0; i<arity; i++)
              {
                if (rewritten_defined[i])
                {
                  rewritten[i].~data_expression();
                }
              }
              return result;
            }
            else
            {

              assert(arity>rule_arity);
              // There are more arguments than those that have been rewritten.
              // Get those, put them in rewritten.

              data_expression result=subst_values(vars,terms,variable_is_in_normal_form,no_assignments,rhs,generator);

              for(size_t i=rule_arity; i<arity; ++i)
              {
                if (rewritten_defined[i])
                {
                  rewritten[i]=detail::get_argument_of_higher_order_term(atermpp::down_cast<application>(term),i);
                }
                else
                {
                  new (&rewritten[i]) data_expression(detail::get_argument_of_higher_order_term(atermpp::down_cast<application>(term),i));
                  rewritten_defined[i]=true;
                }
              }
              size_t i = rule_arity;
              sort_expression sort = detail::residual_sort(op.sort(),i);
              while (is_function_sort(sort) && (i < arity))
              {
                const function_sort& fsort =  atermpp::down_cast<function_sort>(sort);
                const size_t end=i+fsort.domain().size();
                assert(end-1<arity);
                result = application(result,&rewritten[0]+i,&rewritten[0]+end);
                i=end;
                sort = fsort.codomain();
              }

              for (size_t i=0; i<arity; ++i)
              {
                if (rewritten_defined[i])
                {
                  rewritten[i].~data_expression();
                }
              }
              return rewrite_aux(result,sigma);
            }
          }
        }
        no_assignments=0;
      }
    }
  }

  // No rewrite rule is applicable. Rewrite the not yet rewritten arguments.
  // As we rewrite all, we do not record anymore whether terms are rewritten.

  for (size_t i=0; i<arity; i++)
  {
    if (!rewritten_defined[i])
    {
      new (&rewritten[i]) data_expression(rewrite_aux(detail::get_argument_of_higher_order_term(atermpp::down_cast<application>(term),i),sigma));
    }
  }

  //Construct this potential higher order term.
  data_expression result=data_expression(op);
  size_t i = 0;
  sort_expression sort = op.sort();
  while (is_function_sort(sort) && (i < arity))
  {
    const function_sort& fsort=atermpp::down_cast<function_sort>(sort);
    const size_t end=i+fsort.domain().size();
    assert(end-1<arity);
    result = application(result,&rewritten[0]+i,&rewritten[0]+end);
    i=end;
    sort = fsort.codomain();
  }

  for (size_t i=0; i<arity; i++)
  {
    rewritten[i].~data_expression();
  }
  return result;
}

data_expression RewriterJitty::rewrite(
     const data_expression& term,
     substitution_type& sigma)
{
#ifdef MCRL2_DISPLAY_REWRITE_STATISTICS
  data::detail::increment_rewrite_count();
#endif
  const data_expression t=rewrite_aux(term, sigma);
  assert(remove_normal_form_function(t)==t);
  return t;
}

rewrite_strategy RewriterJitty::getStrategy()
{
  return jitty;
}
}
}
}
