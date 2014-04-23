// Author(s): Muck van Weerdenburg
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

using namespace mcrl2::log;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;

namespace mcrl2
{
namespace data
{
namespace detail
{

atermpp::aterm_list RewriterJitty::create_strategy(const data_equation_list& rules1)
{
  atermpp::aterm_list rules;
  for(data_equation_list::const_iterator j=rules1.begin(); j!=rules1.end(); ++j)
  {
    const atermpp::aterm list_representing_rewrite_rule=atermpp::make_list<atermpp::aterm>(
                                         atermpp::aterm_cast<const atermpp::aterm_list>(j->variables()),
                                         j->condition(),
                                         j->lhs(),
                                         j->rhs());
    rules.push_front(list_representing_rewrite_rule);
  }
  rules = reverse(rules);

  atermpp::aterm_list strat;

  std::vector <bool> used;

  size_t arity = 0;
  while (!rules.empty())
  {
    atermpp::aterm_list l;
    atermpp::aterm_list m;

    std::vector<int> args(arity,-1);

    for (; !rules.empty(); rules.pop_front())
    {
      const atermpp::aterm_list& this_rule = core::down_cast<atermpp::aterm_list>(rules.front());
      const data_expression& this_rule_lhs = core::down_cast<data_expression>(element_at(this_rule,2));
      if ((is_function_symbol(this_rule_lhs)?1:detail::recursive_number_of_args(this_rule_lhs)+1) == arity + 1)
      {
        const data_expression& cond = core::down_cast<data_expression>(element_at(this_rule,1));
        atermpp::term_list <variable_list> vars = atermpp::make_list<variable_list>(get_vars(cond));

        std::vector < bool> bs(arity,false);

        for (size_t i = 0; i < arity; i++)
        {
          const data_expression this_rule_lhs_iplus1_arg=detail::get_argument_of_higher_order_term(this_rule_lhs,i);
          if (!is_variable(this_rule_lhs_iplus1_arg))
          {
            bs[i] = true;
            const variable_list evars = get_vars(this_rule_lhs_iplus1_arg);
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
            vars=push_back(vars,get_vars(this_rule_lhs_iplus1_arg));
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
            vars=push_back(vars,get_vars(this_rule_lhs_iplus1_arg));
          }
        }

        atermpp::aterm_list deps;
        for (size_t i = 0; i < arity; i++)
        {
          if (i>=used.size())
          {
            used.resize(i+1,false);
          }
          if (bs[i] && !used[i])
          {
            deps.push_front(atermpp::aterm_int(i));
            args[i] += 1;
          }
        }
        deps = reverse(deps);

        m.push_front(atermpp::make_list<atermpp::aterm>(deps,rules.front()));
      }
      else
      {
        l.push_front(rules.front());
      }
    }

    while (!m.empty())
    {
      atermpp::aterm_list m2;
      for (; !m.empty(); m.pop_front())
      {
        if (atermpp::aterm_cast<const atermpp::aterm_list>((atermpp::aterm_cast<const atermpp::aterm_list>(m.front())).front()).empty())
        {
          atermpp::aterm rule = atermpp::aterm_cast<const atermpp::aterm_list>(m.front()).tail().front();
          strat.push_front(rule);
          size_t len = atermpp::aterm_cast<const atermpp::aterm_list>(atermpp::aterm_cast<const atermpp::aterm_list>(rule).front()).size();
          if (len>MAX_LEN)
          {
            MAX_LEN=len;
          }
        }
        else
        {
          m2.push_front(m.front());
        }
      }
      m = reverse(m2);

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

        atermpp::aterm_int k(maxidx-1);
        strat.push_front(k);
        m2 = atermpp::aterm_list();
        for (; !m.empty(); m.pop_front())
        {
          atermpp::aterm_list temp=atermpp::aterm_cast<const atermpp::aterm_list>(m.front()).tail();
          temp.push_front(atermpp::remove_one_element<atermpp::aterm>(atermpp::aterm_cast<const atermpp::aterm_list>((atermpp::aterm_cast<const atermpp::aterm_list>(m.front())).front()), k));
          m2.push_front(temp);
        }
        m = reverse(m2);
      }
    }

    rules = reverse(l);
    arity++;
  }

  return reverse(strat);
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
            atermpp::detail::_aterm** vars,
            atermpp::detail::_aterm** terms,
            const size_t assignment_size,
            const data_expression& t,
            data::set_identifier_generator& generator); // prototype; */

class subst_values_argument
{
  private:
    atermpp::detail::_aterm** m_vars;
    atermpp::detail::_aterm** m_terms;
    const size_t m_assignment_size;
    data::set_identifier_generator& m_generator;

  public:
    subst_values_argument(atermpp::detail::_aterm** vars,
                          atermpp::detail::_aterm** terms,
                          const size_t assignment_size,
                          data::set_identifier_generator& generator)
      : m_vars(vars), m_terms(terms), m_assignment_size(assignment_size), m_generator(generator)
    {}

    data_expression operator()(const data_expression& t) const
    {
      return subst_values(m_vars,m_terms,m_assignment_size,t,m_generator);
    }
};

static data_expression subst_values(
            atermpp::detail::_aterm** vars,
            atermpp::detail::_aterm** terms,
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
      if (atermpp::detail::address(t)==vars[i])
      {
        return atermpp::aterm_cast<data_expression>(atermpp::aterm(terms[i]));
      }
    }
    return t;
  }
  else if (is_abstraction(t))
  {
    const abstraction& t1=core::down_cast<abstraction>(t);
    const binder_type& binder=t1.binding_operator();
    const variable_list& bound_variables=t1.variables();
    // Check that variables in the left and right hand sides of equations do not clash with bound variables.
    std::set<variable> variables_in_substitution;
    for(size_t i=0; i<assignment_size; ++i)
    {
      std::set<variable> s=find_free_variables(atermpp::aterm_cast<data_expression>(terms[i]));
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

    return abstraction(binder,variable_list(new_variables.begin(),new_variables.end()),subst_values(vars,terms,assignment_size,body,generator));

  }
  else if (is_where_clause(atermpp::aterm_cast<atermpp::aterm_appl>(t)))
  {
    const where_clause& t1=core::down_cast<where_clause>(t);
    const assignment_expression_list& assignments=t1.declarations();
    const data_expression& body=t1.body();

#ifndef NDEBUG
    // Check that variables in right hand sides of equations do not clash with bound variables.
    for(size_t i=0; i<assignment_size; ++i)
    {
      for(assignment_expression_list::const_iterator it=assignments.begin(); it!=assignments.end(); ++it)
      {
        assert(atermpp::aterm_cast<const data_expression>(*it)[0]!= vars[i]);
      }
    }
#endif

    assignment_vector new_assignments;

    for(assignment_expression_list::const_iterator it=assignments.begin(); it!=assignments.end(); ++it)
    {
      const assignment& assignment_expr = core::down_cast<assignment>(*it);
      new_assignments.push_back(assignment(assignment_expr.lhs(), subst_values(vars,terms,assignment_size,assignment_expr.rhs(),generator)));
    }
    return where_clause(subst_values(vars,terms,assignment_size,body,generator),assignment_list(new_assignments.begin(),new_assignments.end()));
  }
  else
  {
    const application& t1 = core::down_cast<application>(t);
    const subst_values_argument substitute_values_in_arguments(vars,terms,assignment_size,generator);
    return application(subst_values(vars,terms,assignment_size,t1.head(),generator),t1.begin(),t1.end(),substitute_values_in_arguments);
  }
}

// Match term t with the lhs p of an equation.

static bool match_jitty(
                    const data_expression& t,
                    const data_expression& p,
                    atermpp::detail::_aterm** vars,
                    atermpp::detail::_aterm** terms,
                    size_t& assignment_size)
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
    // subst.push_back(std::pair<variable,data_expression>(atermpp::aterm_cast<const variable>(p),t));
    vars[assignment_size]=const_cast<atermpp::detail::_aterm*>(atermpp::detail::address(p));
    terms[assignment_size]=const_cast<atermpp::detail::_aterm*>(atermpp::detail::address(t));
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
    assert(is_application(p));
    assert(is_application(t));
    if (p.function()!=t.function()) // are p and t applications of the same arity?
    {
      return false;
    }


    size_t arity = p.size();

    for (size_t i=0; i<arity; i++)
    {
      if (!match_jitty(atermpp::aterm_cast<const data_expression>(t[i]),
                       atermpp::aterm_cast<const data_expression>(p[i]),vars,terms,assignment_size))
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
  if (is_function_symbol(term))
  {
    return rewrite_aux_function_symbol(atermpp::aterm_cast<const function_symbol>(term),term,sigma);
  }
  if (is_variable(term))
  {
    return sigma(core::down_cast<variable>(term));
  }
  if (is_where_clause(term))
  {
    const where_clause& w = core::down_cast<where_clause>(term);
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
  const application& tapp=core::down_cast<application>(term);
  data_expression t=tapp.head();
  if (detail::head_is_function_symbol(term,head))
  {
    // In this case t has the shape f(u1...un)(u1'...um')....  where all u1,...,un,u1',...,um' are normal formas.
    // In the invocation of rewrite_aux_function_symbol these terms are rewritten to normalform again.

/*    const size_t arity=term.size()-1;
    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,data_expression, arity);
    // new (&args[0]) data_expression(t);
    for(size_t i=0; i<arity; ++i)
    {
      new (&args[i]) data_expression(atermpp::aterm_cast<data_expression>(term[i+1]));
    }
    const data_expression result=application(t,&args[0],&args[0]+arity);
    for(size_t i=0; i<arity; ++i)
    {
      args[i].~data_expression();
    } */

    return rewrite_aux_function_symbol(head,term,sigma);
  }

  t = rewrite_aux(atermpp::aterm_cast<data_expression>(term[0]),sigma);
  // Here t has the shape f(u1,....,un)(u1',...,um')....: f applied several times to arguments,
  // x(u1,....,un)(u1',...,um')....: x applied several times to arguments, or
  // binder x1,...,xn.t' where the binder is a lambda, exists or forall.

  if (head_is_function_symbol(t,head))
  {
    // In this case t has the shape f(u1...un)(u1'...um')....  where all u1,...,un,u1',...,um' are normal formas.
    // In the invocation of rewrite_aux_function_symbol these terms are rewritten to normalform again.

    const size_t arity=term.size()-1;
    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,data_expression, arity);
    // new (&args[0]) data_expression(t);
    for(size_t i=0; i<arity; ++i)
    {
      new (&args[i]) data_expression(atermpp::aterm_cast<data_expression>(term[i+1]));
    }
    const data_expression result=application(t,&args[0],&args[0]+arity);
    for(size_t i=0; i<arity; ++i)
    {
      args[i].~data_expression();
    }

    return rewrite_aux_function_symbol(head,result,sigma);
  }
  else if (head_is_variable(t))
  {
    // return appl(t,t1,...,tn) where t1,...,tn still need to be rewritten.
    const size_t arity=term.size()-1;
    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,data_expression, arity);
    // new (&args[0]) data_expression(t);
    for(size_t i=0; i<arity; ++i)
    {
      new (&args[i]) data_expression(rewrite_aux(atermpp::aterm_cast<data_expression>(term[i+1]),sigma));
    }
    const data_expression result=application(t,&args[0],&args[0]+arity);
    for(size_t i=0; i<arity; ++i)
    {
      args[i].~data_expression();
    }
    return result;
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
                      substitution_type& sigma)
{
  // The first term is function symbol; apply the necessary rewrite rules using a jitty strategy.
  const size_t arity=(is_function_symbol(term)?1:detail::recursive_number_of_args(term)+1);

  MCRL2_SYSTEM_SPECIFIC_ALLOCA(rewritten,data_expression, arity);
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(rewritten_defined,bool, arity);

  for(size_t i=0; i<arity; ++i)
  {
    rewritten_defined[i]=false;
  }

  const size_t op_value=core::index_traits<data::function_symbol,function_symbol_key_type, 2>::index(op);
  if (op_value>=jitty_strat.size())
  {
    make_jitty_strat_sufficiently_larger(op_value);
  }

  const atermpp::aterm_list& strat=jitty_strat[op_value];
  if (!strat.empty())
  {
    MCRL2_SYSTEM_SPECIFIC_ALLOCA(vars,atermpp::detail::_aterm*,max_vars);
    MCRL2_SYSTEM_SPECIFIC_ALLOCA(terms,atermpp::detail::_aterm*,max_vars);
    size_t no_assignments=0;

    for (atermpp::aterm_list::const_iterator strategy_it=strat.begin(); strategy_it!=strat.end(); ++strategy_it)
    {
      const atermpp::aterm& rule = *strategy_it;
      if (rule.type_is_int())
      {
        const size_t i = (atermpp::aterm_cast<const atermpp::aterm_int>(rule)).value()+1;
        if (i < arity)
        {
          assert(!rewritten_defined[i]);
          rewritten_defined[i]=true;
          new (&rewritten[i]) data_expression(rewrite_aux(detail::get_argument_of_higher_order_term(term,i-1),sigma));
          assert(rewritten[i].defined());
        }
        else
        {
          break;
        }
      }
      else
      {
        const atermpp::aterm_list& rule1=atermpp::aterm_cast<const atermpp::aterm_list>(rule);
        const data_expression& lhs=atermpp::aterm_cast<const data_expression>(element_at(rule1,2));
        size_t rule_arity = (is_function_symbol(lhs)?1:detail::recursive_number_of_args(lhs)+1);

        if (rule_arity > arity)
        {
          break;
        }

        no_assignments=0; //subst.clear();

        bool matches = true;

        for (size_t i=1; i<rule_arity; i++)
        {
          assert(i<arity);
          if (!match_jitty(rewritten_defined[i]?rewritten[i]:detail::get_argument_of_higher_order_term(term,i-1),
                                                             detail::get_argument_of_higher_order_term(lhs,i-1),vars,terms,no_assignments))
          {
            matches = false;
            break;
          }
        }
        if (matches && (element_at(rule1,1)==sort_bool::true_() || rewrite_aux(
                   subst_values(vars,terms,no_assignments,atermpp::aterm_cast<data_expression>(element_at(rule1,1)),generator),sigma)==sort_bool::true_()))
        {
          const data_expression& rhs=atermpp::aterm_cast<const data_expression>(element_at(rule1,3));

          if (arity == rule_arity)
          {
            const data_expression result=rewrite_aux(subst_values(vars,terms,no_assignments,rhs,generator),sigma);
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

            if (rewritten_defined[rule_arity-1])
            {
              rewritten[rule_arity-1]=subst_values(vars,terms,no_assignments,rhs,generator);
            }
            else
            {
              new (&rewritten[rule_arity-1]) data_expression(subst_values(vars,terms,no_assignments,rhs,generator));
              rewritten_defined[rule_arity-1]=true;
            }

            for(size_t i=rule_arity; i<arity; ++i)
            {
              if (rewritten_defined[i])
              {
                rewritten[i]=detail::get_argument_of_higher_order_term(term,i-1);
              }
              else
              {
                new (&rewritten[i]) data_expression(detail::get_argument_of_higher_order_term(term,i-1));
                rewritten_defined[i]=true;
              }
            }
            size_t i = rule_arity-1;
            sort_expression sort = detail::residual_sort(op.sort(),i);
            while (is_function_sort(sort) && (i < arity))
            {
              const sort_expression_list& sort_dom = core::down_cast<function_sort>(sort).domain();
              size_t a=sort_dom.size()+1;
              const size_t end=i+a;
              assert(end-1<arity);
              rewritten[end-1] = application(rewritten[i],&rewritten[0]+i+1,&rewritten[0]+end);
              i=end-1;
              sort = core::down_cast<function_sort>(sort).codomain();
            }
            const data_expression result=rewrite_aux(rewritten[i],sigma);

            for (size_t i=0; i<arity; ++i)
            {
              if (rewritten_defined[i])
              {
                rewritten[i].~data_expression();
              }
            }
            return result;
          }
        }
      }
    }
  }

  // No rewrite rule is applicable. Rewrite the not yet rewritten arguments.
  // As we rewrite all, we do not record anymore whether terms are rewritten.
  assert(!rewritten_defined[0]);
  new (&rewritten[0]) data_expression(op);
  for (size_t i=1; i<arity; i++)
  {
    if (!rewritten_defined[i])
    {
      new (&rewritten[i]) data_expression(rewrite_aux(detail::get_argument_of_higher_order_term(term,i-1),sigma));
    }
  }

  //Construct this potential higher order term.
  data_expression result;
  if (is_function_symbol(term))
  {
    result=rewritten[0];
  }
  else
  {
    size_t i = 0;
    sort_expression sort = op.sort();
    while (is_function_sort(sort) && (i+1 < arity))
    {
      const sort_expression_list& sort_dom = core::down_cast<function_sort>(sort).domain();
      const size_t a=sort_dom.size()+1;
      const size_t end=i+a;
      assert(end-1<arity);
      rewritten[end-1] = application(rewritten[i],&rewritten[0]+i+1,&rewritten[0]+end);
      i=end-1;
      sort = core::down_cast<function_sort>(sort).codomain();
    }
    result=rewritten[i];
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
  return rewrite_aux(term, sigma);
}

rewrite_strategy RewriterJitty::getStrategy()
{
  return jitty;
}
}
}
}
