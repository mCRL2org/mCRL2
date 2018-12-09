// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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
#include <boost/config.hpp>

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


typedef atermpp::detail::_aterm* unprotected_variable;           // Variable that is not protected (so a copy should exist at some other place)
typedef atermpp::detail::_aterm* unprotected_data_expression;    // Idem, but now a data expression.

struct jitty_variable_assignment_for_a_rewrite_rule
{
  unprotected_variable var;
  unprotected_data_expression term;
  bool variable_is_a_normal_form;
};

struct jitty_assignments_for_a_rewrite_rule
{
  std::size_t size;
  jitty_variable_assignment_for_a_rewrite_rule* assignment;

  jitty_assignments_for_a_rewrite_rule(jitty_variable_assignment_for_a_rewrite_rule* a)
   : size(0),
     assignment(a)
  {}

};


// The function symbol below is used to administrate that a term is in normal form. It is put around a term.
// Terms with this auxiliary function symbol cannot be printed using the pretty printer for data expressions.


static const function_symbol& this_term_is_in_normal_form()
{
  static const function_symbol this_term_is_in_normal_form(
                         std::string("Rewritten@@term"),
                         function_sort({ untyped_sort() },untyped_sort()));
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

  if (is_machine_number(t))
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

    return application(remove_normal_form_function(ta.head()),ta.begin(),ta.end(),remove_normal_form_function);
  }


  if (is_where_clause(t))
  {
    const where_clause& t1=atermpp::down_cast<where_clause>(t);
    const assignment_expression_list& assignments=t1.declarations();
    const data_expression& body=t1.body();

    assignment_vector new_assignments;
    for(const assignment_expression& ae: assignments)
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
  const data_expression& body=t1.body();

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
    std::set<std::size_t> m_dependencies;
    data_equation m_equation;

  public:
    dependencies_rewrite_rule_pair(std::set<std::size_t>& dependencies, const data_equation& eq)
     : m_dependencies(dependencies), m_equation(eq)
    {}

    const std::set<std::size_t>& dependencies() const
    {
      return m_dependencies;
    }

    const data_equation equation() const
    {
      return m_equation;
    }
};





// Create a strategy for the rewrite rules belonging to one particular symbol.
// It is a prerequisite for this function to that all rewrite rules in rules1 have
// the same main function symbol in the lhs. 
strategy RewriterJitty::create_a_rewriting_based_strategy(const function_symbol& f, const data_equation_list& rules1)
{
  static_cast<void>(f); // Avoid an unused variable warning. 
  data_equation_list rules=rules1;
  std::vector<strategy_rule> strat;

  std::vector <bool> used;

  std::size_t arity = 0;
  std::size_t max_number_of_variables = 0;
  while (!rules.empty())
  {
    data_equation_list l;
    std::vector<dependencies_rewrite_rule_pair> m;

    std::vector<int> args(arity,-1);

    for (const data_equation& this_rule: rules)
    {
      max_number_of_variables=std::max(this_rule.variables().size(),max_number_of_variables);
      const data_expression& this_rule_lhs = this_rule.lhs();
      if ((is_function_symbol(this_rule_lhs)?1:detail::recursive_number_of_args(this_rule_lhs)+1) == arity + 1)
      {
        const data_expression& cond = this_rule.condition();
        atermpp::term_list<variable_list> vars = { get_free_vars(cond) };

        std::vector < bool> bs(arity,false);

        for (std::size_t i = 0; i < arity; i++)
        {
          const data_expression this_rule_lhs_iplus1_arg=detail::get_argument_of_higher_order_term(atermpp::down_cast<application>(this_rule_lhs),i);
          if (!is_variable(this_rule_lhs_iplus1_arg))
          {
            bs[i] = true;
            const variable_list evars = get_free_vars(this_rule_lhs_iplus1_arg);
            for (variable_list::const_iterator v=evars.begin(); v!=evars.end(); ++v)
            {
              int j=0;
              const atermpp::term_list <variable_list>& next_vars=vars.tail();
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

        std::set<std::size_t> deps;
        for (std::size_t i = 0; i < arity; i++)
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
      for (const dependencies_rewrite_rule_pair& p: m)
      {
        if (p.dependencies().empty())
        {
          const data_equation rule = p.equation();
          strat.push_back(strategy_rule(rule));
          std::size_t len = rule.variables().size();
          if (len>MAX_LEN)
          {
            MAX_LEN=len;
          }
        }
        else
        {
          m2.push_back(p);
        }
      }
      m = m2;

      if (m.empty())
      {
        break;
      }

      int max = -1;
      std::size_t maxidx = 0;

      for (std::size_t i = 0; i < arity; i++)
      {
        assert(i<((std::size_t)1)<<(8*sizeof(int)-1));
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

        const std::size_t k(maxidx-1);
        strat.push_back(strategy_rule(k));
        m2.clear();
        for (const dependencies_rewrite_rule_pair& p: m)
        {
          const data_equation eq=p.equation();
          std::set<std::size_t> dependencies=p.dependencies();
          dependencies.erase(k);
          m2.push_back(dependencies_rewrite_rule_pair(dependencies,eq));
        }
        m = m2;
      }
    }

    rules = reverse(l);
    arity++;
  }
  
  return strategy(max_number_of_variables,strat);
}

// Create an explicit rewrite strategy when rewriting using an explicitly given 
// C++ function. First rewrite all the arguments, then apply the function. 
strategy RewriterJitty::create_a_cpp_function_based_strategy(const function_symbol& f, const data_specification& data_spec)
{
  size_t number_of_arguments=0;
  if (is_function_sort(f.sort()))
  {
    number_of_arguments=atermpp::down_cast<function_sort>(f.sort()).domain().size();
  }
  // Indicate that all arguments must be rewritten first. 
  std::vector<strategy_rule> result;
  for(size_t i=0; i<number_of_arguments; ++i)
  {
    result.push_back(strategy_rule(i));
  }
  result.push_back(strategy_rule(data_spec.cpp_implemented_functions().find(f)->second.first));
  
  return strategy(0,result);
}

// Create a strategy to rewrite terms. This can either be a strategy that is based on rewrite
// rules or it can be a strategy based on an explicitly given c++ function for this function symbol. 
strategy RewriterJitty::create_strategy(const function_symbol& f, const data_equation_list& rules1, const data_specification& data_spec)
{
  if (data_spec.cpp_implemented_functions().count(f)==0)    // There is no explicit implementation.
  {
    return create_a_rewriting_based_strategy(f, rules1);
  } 
  else 
  {
    assert(rules1.size()==0);  // There should be no explicit rewrite rules, as this function is implemented by 
                               // an explicit C++ function. 
    return create_a_cpp_function_based_strategy(f, data_spec);
  }
}

void RewriterJitty::make_jitty_strat_sufficiently_larger(const std::size_t i)
{
  if (i>=jitty_strat.size())
  {
    jitty_strat.resize(i+1);
  }
}

void RewriterJitty::rebuild_strategy(const data_specification& data_spec, const mcrl2::data::used_data_equation_selector& equation_selector)
{
  jitty_strat.clear();
  function_symbol_vector function_symbols=data_spec.constructors();
  function_symbols.insert(function_symbols.end(), data_spec.mappings().begin(), data_spec.mappings().end());
  for(const function_symbol& f: function_symbols)
  // for(std::map< function_symbol, data_equation_list >::const_iterator l=jitty_eqns.begin(); l!=jitty_eqns.end(); ++l)
  {
    if (equation_selector(f)) 
    {
      const std::size_t i=core::index_traits<data::function_symbol, function_symbol_key_type, 2>::index(f);
      make_jitty_strat_sufficiently_larger(i);
      std::map< function_symbol, data_equation_list >::const_iterator j=jitty_eqns.find(f);
      jitty_strat[i] = 
            (j==jitty_eqns.end()
                 ?create_strategy(f,data_equation_list(), data_spec)
                 :create_strategy(f,reverse(j->second), data_spec));
    }
  }

}

RewriterJitty::RewriterJitty(
           const data_specification& data_spec,
           const mcrl2::data::used_data_equation_selector& equation_selector):
        Rewriter(data_spec,equation_selector)
{
  MAX_LEN=0;

  for (const data_equation& eq: data_spec.equations())
  {
    if (equation_selector(eq))
    {
      try
      {
        CheckRewriteRule(eq);
      }
      catch (std::runtime_error& e)
      {
        mCRL2log(warning) << e.what() << std::endl;
        continue;
      }

      const function_symbol& lhs_head_index=atermpp::down_cast<function_symbol>(get_nested_head(eq.lhs()));

      data_equation_list n;
      std::map< function_symbol, data_equation_list >::iterator it = jitty_eqns.find(lhs_head_index);
      if (it != jitty_eqns.end())
      {
        n = it->second;
      }
      n.push_front(eq);
      jitty_eqns[lhs_head_index] = n;
    }
  }

  rebuild_strategy(data_spec, equation_selector);
}

RewriterJitty::~RewriterJitty()
{
}

static data_expression subst_values(
            const jitty_assignments_for_a_rewrite_rule& assignments,
            const data_expression& t,
            data::enumerator_identifier_generator& generator) // This generator is used for the generation of fresh variable names.
{
  if (is_machine_number(t))
  {
    return t;
  }
  if (is_function_symbol(t))
  {
    return t;
  }
  else if (is_variable(t))
  {
    for (std::size_t i=0; i<assignments.size; i++)
    {
      if (atermpp::detail::address(t)==assignments.assignment[i].var)
      {
        if (assignments.assignment[i].variable_is_a_normal_form)
        {
          // Variables that are in normal form get a tag that they are in normal form.
          return application(this_term_is_in_normal_form(),atermpp::down_cast<data_expression>(atermpp::aterm(assignments.assignment[i].term)));  
        }
        return atermpp::down_cast<data_expression>(atermpp::aterm(assignments.assignment[i].term));
      }
    }
    return t;
  }
  else if (is_abstraction(t))
  {
    const abstraction& t1=atermpp::down_cast<abstraction>(t);
    const binder_type& binder=t1.binding_operator();
    const variable_list& bound_variables=t1.variables();
    // Check that variables in the left and right hand sides of equations do not clash with bound variables.
    std::set<variable> variables_in_substitution;
    for(std::size_t i=0; i<assignments.size; ++i)
    {
      std::set<variable> s=find_free_variables(atermpp::down_cast<data_expression>(atermpp::aterm(assignments.assignment[i].term)));
      variables_in_substitution.insert(s.begin(),s.end());
      variables_in_substitution.insert(atermpp::down_cast<variable>(atermpp::aterm(assignments.assignment[i].var)));
    }

    variable_vector new_variables;
    mutable_map_substitution<> sigma;
    bool sigma_trivial=true;
    for(const variable& v: bound_variables)
    {
      if (variables_in_substitution.count(v)>0)
      {
        // Replace v in the list and in the body by a new variable name.
        const variable fresh_variable(generator(),v.sort());
        new_variables.push_back(fresh_variable);
        sigma[v]=fresh_variable;
        sigma_trivial=false;
      }
      else
      {
        new_variables.push_back(v);
      }
    }
    return abstraction(binder,
                       variable_list(new_variables.begin(),new_variables.end()),
                       subst_values(assignments,
                                    (sigma_trivial?t1.body():replace_variables(t1.body(),sigma)),
                                    generator));
  }
  else if (is_where_clause(t))
  {
    const where_clause& t1=atermpp::down_cast<where_clause>(t);
    const assignment_expression_list& local_assignments=t1.declarations();
    const data_expression& body=t1.body();

#ifndef NDEBUG
    // Check that variables in right hand sides of equations do not clash with bound variables.
    for(std::size_t i=0; i<assignments.size; ++i)
    {
      for(const assignment_expression& a: local_assignments)
      {
        assert(a[0]!= atermpp::aterm(assignments.assignment[i].var));
      }
    }
#endif

    assignment_vector new_assignments;

    for(const assignment_expression& a: local_assignments)
    {
      const assignment& assignment_expr = atermpp::down_cast<assignment>(a);
      new_assignments.push_back(assignment(assignment_expr.lhs(), subst_values(assignments,assignment_expr.rhs(),generator)));
    }
    return where_clause(subst_values(assignments,body,generator),assignment_list(new_assignments.begin(),new_assignments.end()));
  }
  else
  {
    const application& t1 = atermpp::down_cast<application>(t);
    return application(subst_values(assignments,
                                    t1.head(),
                                    generator),
                       t1.begin(),
                       t1.end(),
                       [&](const data_expression& t){ return subst_values(assignments,t,generator);});
  }
}

// Match term t with the lhs p of an equation.
static bool match_jitty(
                    const data_expression& t,
                    const data_expression& p,
                    jitty_assignments_for_a_rewrite_rule& assignments,
                    const bool term_context_guarantees_normal_form)
{
  if (is_function_symbol(p) || is_machine_number(p))
  {
    return p==t;
  }
  else if (is_variable(p))
  {

    for (std::size_t i=0; i<assignments.size; i++)
    {
      if (atermpp::detail::address(p)==assignments.assignment[i].var)
      {
        return atermpp::detail::address(t)==assignments.assignment[i].term;
      }
    }

    assignments.assignment[assignments.size].var=atermpp::detail::address(p);
    assignments.assignment[assignments.size].term=atermpp::detail::address(t);
    assignments.assignment[assignments.size].variable_is_a_normal_form=term_context_guarantees_normal_form;
    assignments.size++;
    return true;
  }
  else
  {
    if (is_machine_number(t) || is_function_symbol(t) || is_variable(t) || is_abstraction(t) || is_where_clause(t))
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
                     pa.head(),assignments,true))
    {
      return false;
    }

    for (std::size_t i=0; i<pa.size(); i++)
    {
      if (!match_jitty(ta[i], pa[i],assignments,true))
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
  if (is_machine_number(term))
  {
    return term;
  }
  if (is_application(term))
  {
    const application& terma=atermpp::down_cast<application>(term);
    if (terma.head()==this_term_is_in_normal_form())
    {
      assert(terma.size()==1);
      assert(remove_normal_form_function(terma[0])==terma[0]);
      return terma[0];
    }

    // The variable term has the shape appl(t,t1,...,tn);
  
    // First check whether t has the shape appl(appl...appl(f,u1,...,un)(...)(...) where f is a function symbol.
    // In this case rewrite that function symbol. This is an optimisation. If this does not apply t is rewritten,
    // including all its subterms. But this is costly, as not all subterms will be rewritten again
    // in rewrite_aux_function_symbol.
  
    const data_expression& head=get_nested_head(term);
  
    if (is_function_symbol(head) && head!=this_term_is_in_normal_form())
    {
      return rewrite_aux_function_symbol(atermpp::down_cast<function_symbol>(head),term,sigma);
    }
  
    const application& tapp=atermpp::down_cast<application>(term);
    
    const data_expression& t = rewrite_aux(tapp.head(),sigma);
    // Here t has the shape f(u1,....,un)(u1',...,um')....: f applied several times to arguments,
    // x(u1,....,un)(u1',...,um')....: x applied several times to arguments, or
    // binder x1,...,xn.t' where the binder is a lambda, exists or forall.
  
    const data_expression& head1 = get_nested_head(t);
    if (is_function_symbol(head1))
    {
      // In this case t has the shape f(u1...un)(u1'...um')....  where all u1,...,un,u1',...,um' are normal formas.
      // In the invocation of rewrite_aux_function_symbol these terms are rewritten to normalform again.
      const data_expression& result=application(t,tapp.begin(), tapp.end()); 
      return rewrite_aux_function_symbol(atermpp::down_cast<function_symbol>(head1),result,sigma);
    }
    else if (is_variable(head1))
    {
      // return appl(t,t1,...,tn) where t1,...,tn still need to be rewritten.
      jitty_argument_rewriter r(sigma,*this);
      return application(t,tapp.begin(),tapp.end(),r); // Replacing r by a lambda term requires 16 more bytes on the stack. 
    }
    assert(is_abstraction(t));
    const abstraction& ta=atermpp::down_cast<abstraction>(t);
    const binder_type& binder(ta.binding_operator());
    if (is_lambda_binder(binder))
    {
      return rewrite_lambda_application(t,tapp,sigma);
    }
    if (is_exists_binder(binder))
    {
      return existential_quantifier_enumeration(t,sigma);
    }
    assert(is_forall_binder(binder));
    return universal_quantifier_enumeration(head1,sigma);
  }
  // Here term does not have the shape appl(t1,...,tn)
  if (is_function_symbol(term))
  {
    assert(term!=this_term_is_in_normal_form());
    return rewrite_aux_const_function_symbol(atermpp::down_cast<const function_symbol>(term),sigma);
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

  { 
    assert(is_abstraction(term));
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
}

data_expression RewriterJitty::rewrite_aux_function_symbol(
                      const function_symbol& op,
                      const data_expression& term,
                      substitution_type& sigma)
{
  // The first term is function symbol; apply the necessary rewrite rules using a jitty strategy.

  const std::size_t arity=(is_function_symbol(term)?0:detail::recursive_number_of_args(term));

  data_expression* rewritten = MCRL2_SPECIFIC_STACK_ALLOCATOR(data_expression, arity);
  bool* rewritten_defined = MCRL2_SPECIFIC_STACK_ALLOCATOR(bool, arity);

  for(std::size_t i=0; i<arity; ++i)
  {
    rewritten_defined[i]=false;
  }

  const std::size_t op_value=core::index_traits<data::function_symbol,function_symbol_key_type, 2>::index(op);
  make_jitty_strat_sufficiently_larger(op_value);
  const strategy& strat=jitty_strat[op_value];

  if (!strat.rules.empty())
  {
    jitty_assignments_for_a_rewrite_rule assignments(MCRL2_SPECIFIC_STACK_ALLOCATOR(jitty_variable_assignment_for_a_rewrite_rule,strat.number_of_variables));

    for (const strategy_rule& rule: strat.rules)
    {
      if (rule.is_rewrite_index())
      {
        const std::size_t i = rule.rewrite_index();
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
      else if (rule.is_cpp_code())
      {
        // Here it is assumed that precompiled code only works on the exact right number of arguments and
        // precompiled functions are not used in a higher order fashion. Maybe this requires an explicit check. 
        if (arity==0)
        { 
          return rule.rewrite_cpp_code()(op);
        }
        else 
        {
          // application rewriteable_term(op,0,arity,[&rewritten, &rewritten_defined](size_t i){assert(rewritten_defined[i]); return rewritten[i];});
          application rewriteable_term(op, &rewritten[0], &rewritten[arity]);
          return rule.rewrite_cpp_code()(rewriteable_term);

        }
      }
      else
      {
        const data_equation& rule1=rule.equation();
        const data_expression& lhs=rule1.lhs();
        std::size_t rule_arity = (is_function_symbol(lhs)?0:detail::recursive_number_of_args(lhs));

        if (rule_arity > arity)
        {
          break;
        }

        assert(assignments.size==0);

        bool matches = true;
        for (std::size_t i=0; i<rule_arity; i++)
        {
          assert(i<arity);
          if (!match_jitty(rewritten_defined[i]?rewritten[i]:detail::get_argument_of_higher_order_term(atermpp::down_cast<application>(term),i),
                           detail::get_argument_of_higher_order_term(atermpp::down_cast<application>(lhs),i),
                           assignments,rewritten_defined[i]))
          {
            matches = false;
            break;
          }
        }
        if (matches)
        {
          if (rule1.condition()==sort_bool::true_() || rewrite_aux(
                   subst_values(assignments,rule1.condition(),m_generator),sigma)==sort_bool::true_())
          {
            const data_expression& rhs=rule1.rhs();

            if (arity == rule_arity)
            {
              const data_expression& result=rewrite_aux(subst_values(assignments,rhs,m_generator),sigma);
              for (std::size_t i=0; i<arity; i++)
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

              data_expression result=subst_values(assignments,rhs,m_generator);

              for(std::size_t i=rule_arity; i<arity; ++i)
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
              std::size_t i = rule_arity;
              sort_expression sort = detail::residual_sort(op.sort(),i);
              while (is_function_sort(sort) && (i < arity))
              {
                const function_sort& fsort =  atermpp::down_cast<function_sort>(sort);
                const std::size_t end=i+fsort.domain().size();
                assert(end-1<arity);
                result = application(result,&rewritten[0]+i,&rewritten[0]+end);
                i=end;
                sort = fsort.codomain();
              }

              for (std::size_t i=0; i<arity; ++i)
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
        assignments.size=0;
      }
    }
  }

  // No rewrite rule is applicable. Rewrite the not yet rewritten arguments.
  // As we rewrite all, we do not record anymore whether terms are rewritten.

  for (std::size_t i=0; i<arity; i++)
  {
    if (!rewritten_defined[i])
    {
      new (&rewritten[i]) data_expression(rewrite_aux(detail::get_argument_of_higher_order_term(atermpp::down_cast<application>(term),i),sigma));
    }
  }

  //Construct this potential higher order term.
  data_expression result=op;
  std::size_t i = 0;
  sort_expression sort = op.sort();
  while (is_function_sort(sort) && (i < arity))
  {
    const function_sort& fsort=atermpp::down_cast<function_sort>(sort);
    const std::size_t end=i+fsort.domain().size();
    assert(end-1<arity);
    result = application(result,&rewritten[0]+i,&rewritten[0]+end);
    i=end;
    sort = fsort.codomain();
  }

  for (std::size_t i=0; i<arity; i++)
  {
    rewritten[i].~data_expression();
  } 
  return result; 
}

data_expression RewriterJitty::rewrite_aux_const_function_symbol(
                      const function_symbol& op,
                      substitution_type& sigma)
{
  // This is special code to rewrite a function symbol. Note that the function symbol can be higher order,
  // e.g., it can be a function symbol f for which a rewrite rule f(n)=... exists. 

  const std::size_t op_value=core::index_traits<data::function_symbol,function_symbol_key_type, 2>::index(op);
  make_jitty_strat_sufficiently_larger(op_value);
  const strategy& strat=jitty_strat[op_value];

  for (const strategy_rule& rule: strat.rules)
  {
    if (rule.is_rewrite_index())
    {
      // In this case a standalone function symbol is rewritten, which could have arguments. 
      // It is not needed to rewrite the arguments. 
      break;
    }
    else if (rule.is_cpp_code())
    {
      return rule.rewrite_cpp_code()(op);
    }
    else
    {
      const data_equation& rule1=rule.equation();
      const data_expression& lhs=rule1.lhs();
      std::size_t rule_arity = (is_function_symbol(lhs)?0:detail::recursive_number_of_args(lhs));

      if (rule_arity > 0)
      {
        break;
      }

      if (rule1.condition()==sort_bool::true_() || rewrite_aux(rule1.condition(),sigma)==sort_bool::true_())
      {
        return rewrite_aux(rule1.rhs(),sigma);
      }
    }
  }

  return op; 
}


data_expression RewriterJitty::rewrite(
     const data_expression& term,
     substitution_type& sigma)
{
// std::cerr << "START REWRITE " << term << "\n"; // << atermpp::aterm(term) << "\n";
#ifdef MCRL2_DISPLAY_REWRITE_STATISTICS
  data::detail::increment_rewrite_count();
#endif
  const data_expression& t=rewrite_aux(term, sigma);
  assert(remove_normal_form_function(t)==t);
//  std::cerr << "END REWRITE " << term << "  ---> " << t << "\n"; // << atermpp::aterm(t) << "\n";
  return t;
}

rewrite_strategy RewriterJitty::getStrategy()
{
  return jitty;
}
}
}
}
