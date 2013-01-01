// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file jitty.cpp

#include "boost.hpp" // precompiled headers

#include "mcrl2/data/detail/rewrite/jitty.h"

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
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/core/detail/struct_core.h"

using namespace mcrl2::log;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;

namespace mcrl2
{
namespace data
{
namespace detail
{

static variable_list get_vars(const atermpp::aterm_appl &a)
{
  if (is_variable(a))
  {
    return make_list(aterm_cast<variable>(a));
  }
  else
  {
    variable_list l;
    for(atermpp::aterm_appl::const_iterator arg=a.begin(); arg!=a.end(); ++arg)
    {
      if (!arg->type_is_int())
      {
        l= get_vars(aterm_cast<const atermpp::aterm_appl>(*arg))+l;
      }
    }
    return l;
  }
}


// Assume that the expression t is an application, and return its leading function symbol.
static function_symbol get_function_symbol_of_head(const data_expression &t)
{
  if (is_function_symbol(t))
  {
    return t;
  }
  assert(is_application(t));

  return get_function_symbol_of_head(application(t).head());
}

aterm_list RewriterJitty::create_strategy(const data_equation_list &rules1)
{
  size_t max_arity = 0;
  for (data_equation_list::const_iterator l=rules1.begin(); l!=rules1.end(); ++l)
  {
    const size_t current_arity=toRewriteFormat(l->lhs()).size();
    if (current_arity > max_arity + 1)
    {
      max_arity = current_arity-1;
    }
  }


  aterm_list rules=aterm_list();
  for(data_equation_list::const_iterator j=rules1.begin(); j!=rules1.end(); ++j)
  {
    const aterm list_representing_rewrite_rule=make_list<aterm>(
                                         aterm_cast<const aterm_list>(j->variables()),
                                         toRewriteFormat(j->condition()),
                                         toRewriteFormat(j->lhs()),
                                         toRewriteFormat(j->rhs()));
    rules.push_front(list_representing_rewrite_rule);
  }
  rules = reverse(rules);

  aterm_list strat;
  size_t arity;

  MCRL2_SYSTEM_SPECIFIC_ALLOCA(used,bool, max_arity);
  for (size_t i=0; i<max_arity; ++i)
  {
    used[i]=false;
  }

  arity = 0;
  while (!rules.empty())
  {
    aterm_list l;
    aterm_list m;

    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,int, arity);
    for (size_t i=0; i<arity; ++i)
    {
      args[i]=-1;
    }

    for (; !rules.empty(); rules=pop_front(rules))
    {
      if (aterm_cast<aterm_appl>(element_at(aterm_cast<const aterm_list>(rules.front()),2)).function().arity() == arity + 1)
      {
        const atermpp::aterm_appl &cond = aterm_cast<const aterm_appl>(element_at(aterm_cast<const aterm_list>(rules.front()),1));
        atermpp::term_list <variable_list> vars = make_list<variable_list>(get_vars(cond));
        const atermpp::aterm_appl &pars = aterm_cast<const aterm_appl>(element_at(aterm_cast<const aterm_list>(rules.front()),2));

        MCRL2_SYSTEM_SPECIFIC_ALLOCA(bs, bool, arity);
        for (size_t i = 0; i < arity; i++)
        {
          bs[i]=false;
        }

        for (size_t i = 0; i < arity; i++)
        {
          if (!is_variable(atermpp::aterm_cast<const atermpp::aterm_appl>(pars(i+1))))
          {
            bs[i] = true;
            const variable_list evars = get_vars(atermpp::aterm_cast<const atermpp::aterm_appl>(pars(i+1)));
            for (variable_list::const_iterator v=evars.begin(); v!=evars.end(); ++v)
            {
              int j=0;
              const atermpp::term_list <variable_list> next_vars=pop_front(vars);
              for (atermpp::term_list <variable_list>::const_iterator o=next_vars.begin(); o!=next_vars.end(); ++o)
              {
                if (std::find(o->begin(),o->end(),*v) != o->end())
                {
                  bs[j] = true;
                }
                j++;
              }
            }
            vars = push_back(vars,get_vars(atermpp::aterm_cast<const atermpp::aterm_appl>(pars(i+1))));
          }
          else
          {
            int j = -1;
            bool b = false;
            for (atermpp::term_list <variable_list>::const_iterator o=vars.begin(); o!=vars.end(); ++o)
            {
              if (std::find(o->begin(),o->end(),variable(atermpp::aterm_cast<const atermpp::aterm_appl>(pars(i+1)))) != o->end())
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
            vars = push_back(vars,get_vars(aterm_cast<const atermpp::aterm_appl>(pars(i+1))));
          }
        }

        aterm_list deps;
        for (size_t i = 0; i < arity; i++)
        {
          if (bs[i] && !used[i])
          {
            assert(i<((size_t)1)<<(8*sizeof(int)-1));  // Check whether i can safely be translated into an int.
            deps.push_front(aterm_int(i));
            args[i] += 1;
          }
        }
        deps = reverse(deps);

        m.push_front(make_list<aterm>(deps,rules.front())); 
      }
      else
      {
        l.push_front(rules.front());
      }
    }

    while (!m.empty())
    {
      aterm_list m2;
      for (; !m.empty(); m=pop_front(m))
      {
        if (aterm_cast<const aterm_list>((aterm_cast<const aterm_list>(m.front())).front()).empty())
        {
          aterm rule = pop_front(aterm_cast<const aterm_list>(m.front())).front();
          strat.push_front(rule);
          size_t len = aterm_cast<const aterm_list>(aterm_cast<const aterm_list>(rule).front()).size();
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
      int maxidx = -1;

      for (size_t i = 0; i < arity; i++)
      {
        assert(i<((size_t)1)<<(8*sizeof(int)-1));
        if (args[i] > max)
        {
          maxidx = (int)i;
          max = args[i];
        }
      }

      if (maxidx >= 0)
      {
        args[maxidx] = -1;
        used[maxidx] = true;

        aterm_int k(static_cast<size_t>(maxidx));
        strat.push_front(k);
        m2 = aterm_list();
        for (; !m.empty(); m=pop_front(m))
        {
          aterm_list temp=pop_front(aterm_cast<const aterm_list>(m.front()));
          temp.push_front(remove_one_element<aterm>(aterm_cast<const aterm_list>((aterm_cast<const aterm_list>(m.front())).front()), k));
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
    size_t oldsize=jitty_strat.size();
    jitty_strat.resize(i+1);
    for( ; oldsize<jitty_strat.size(); ++oldsize)
    {
      jitty_strat[oldsize]=aterm_list(aterm());
    }
  }
}

RewriterJitty::RewriterJitty(
           const data_specification& data_spec,
           const mcrl2::data::used_data_equation_selector& equation_selector):
        Rewriter()
{
  m_data_specification_for_enumeration = data_spec;

  MAX_LEN=0;
  max_vars = 0;
  need_rebuild = false;

  const std::vector< data_equation > &l = data_spec.equations();
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

      atermpp::aterm_int lhs_head_index=OpId2Int(get_function_symbol_of_head(j->lhs()));

      data_equation_list n;
      std::map< atermpp::aterm_int, data_equation_list >::iterator it = jitty_eqns.find(lhs_head_index);
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

  for(std::map< function_symbol, atermpp::aterm_int >::const_iterator l1 = term2int.begin()
      ; l1 != term2int.end()
      ; ++l1)
  {

    atermpp::aterm_int i = l1->second;
    std::map< atermpp::aterm_int, data_equation_list >::iterator it = jitty_eqns.find( i );

    make_jitty_strat_sufficiently_larger(i.value());
    if (it == jitty_eqns.end() )
    {
      jitty_strat[i.value()] = aterm_list(aterm());
    }
    else
    {
      jitty_strat[i.value()] = create_strategy(reverse(it->second));
    }
  }
}

RewriterJitty::~RewriterJitty()
{
}

bool RewriterJitty::addRewriteRule(const data_equation &rule)
{
  try
  {
    CheckRewriteRule(rule);
  }
  catch (std::runtime_error& e)
  {
    mCRL2log(warning) << e.what() << std::endl;
    return false;
  }

  atermpp::aterm_int lhs_head_index=OpId2Int(get_function_symbol_of_head(rule.lhs()));
  data_equation_list n;
  std::map< atermpp::aterm_int, data_equation_list >::iterator it = jitty_eqns.find(lhs_head_index);
  if (it != jitty_eqns.end())
  {
    n = it->second;
  }

  if (rule.variables().size() > max_vars)
  {
    max_vars = rule.variables().size();
  }
  n.push_front(rule);
  jitty_eqns[lhs_head_index] = n;
  make_jitty_strat_sufficiently_larger(lhs_head_index.value());
  jitty_strat[lhs_head_index.value()] = aterm_list(aterm()); //create_strategy(n);
  need_rebuild = true;

  return true;
}

bool RewriterJitty::removeRewriteRule(const data_equation &rule)
{
  atermpp::aterm_int lhs_head_index=OpId2Int(get_function_symbol_of_head(rule.lhs()));

  data_equation_list n;
  const std::map< atermpp::aterm_int, data_equation_list >::iterator it = jitty_eqns.find(lhs_head_index);
  if (it != jitty_eqns.end())
  {
    n = it->second;
  }

  n = remove_one_element(n,rule);

  make_jitty_strat_sufficiently_larger(lhs_head_index.value());
  if (n.empty())
  {
    jitty_eqns.erase( it );
    jitty_strat[lhs_head_index.value()] = aterm_list(aterm());
  }
  else
  {
    jitty_eqns[lhs_head_index] = n;
    jitty_strat[lhs_head_index.value()] = aterm_list(aterm());//create_strategy(n);
    need_rebuild = true;
  }

  return true;
}

static atermpp::aterm subst_values(
            const variable** vars,
            const atermpp::aterm** vals,
            const size_t number_of_vars,
            const atermpp::aterm &t); //Prototype.

class subst_values_argument
{
  private:
    const variable** m_vars;
    const atermpp::aterm** m_vals;
    const size_t m_number_of_vars;

  public:
    subst_values_argument(
              const variable** vars,
              const atermpp::aterm** vals,
              const size_t number_of_vars):
                 m_vars(vars),
                 m_vals(vals),
                 m_number_of_vars(number_of_vars)
    {}

    aterm operator()(const aterm &t) const
    {
      return subst_values(m_vars,m_vals,m_number_of_vars,t);
    }
};

static atermpp::aterm subst_values(
            const variable** vars,
            const atermpp::aterm** vals,
            const size_t number_of_vars,
            const atermpp::aterm &t)
{
  if (t.type_is_int())
  {
    return t;
  }
  else if (is_variable(t))
  {
    for (size_t i=0; i<number_of_vars; i++)
    {
      if (t==*vars[i])
      {
        return *vals[i];
      }
    }
    return t;
  }
  else if (is_abstraction(atermpp::aterm_cast<const atermpp::aterm_appl>(t)))
  {
    const atermpp::aterm_appl &t1=atermpp::aterm_cast<const atermpp::aterm_appl>(t);
    const atermpp::aterm_appl &binder=atermpp::aterm_cast<const atermpp::aterm_appl>(t1(0));
    const variable_list &bound_variables=atermpp::aterm_cast<const variable_list>(t1(1));
    const atermpp::aterm_appl body=atermpp::aterm_cast<const atermpp::aterm_appl>(subst_values(vars,vals,number_of_vars,atermpp::aterm_cast<const atermpp::aterm_appl>(t1(2))));
#ifndef NDEBUG
    // Check that variables in right hand sides of equations do not clash with bound variables.
    for(size_t i=0; i<number_of_vars; ++i)
    {
      for(variable_list::const_iterator it=bound_variables.begin(); it!=bound_variables.end(); ++it)
      {
        assert(*it!= *vars[i]);
      }
    }
#endif
    return gsMakeBinder(binder,bound_variables,body);

  }
  else if (is_where_clause(atermpp::aterm_cast<const atermpp::aterm_appl>(t)))
  {
    const atermpp::aterm_appl &t1=atermpp::aterm_cast<const atermpp::aterm_appl>(t);
    const atermpp::term_list < atermpp::aterm_appl > &assignment_list=atermpp::aterm_cast<const atermpp::term_list < atermpp::aterm_appl > >(t1(1));
    const atermpp::aterm_appl body=atermpp::aterm_cast<const atermpp::aterm_appl>(subst_values(vars,vals,number_of_vars,atermpp::aterm_cast<const atermpp::aterm_appl>(t1(0))));

#ifndef NDEBUG
    // Check that variables in right hand sides of equations do not clash with bound variables.
    for(size_t i=0; i<number_of_vars; ++i)
    {
      for(atermpp::term_list < atermpp::aterm_appl >::const_iterator it=assignment_list.begin(); it!=assignment_list.end(); ++it)
      {
        assert(atermpp::aterm_cast<const atermpp::aterm_appl>(*it)(0)!= *vars[i]);
      }
    }
#endif

    std::vector < atermpp::aterm_appl > new_assignments;

    for(atermpp::term_list < atermpp::aterm_appl > :: const_iterator it=assignment_list.begin(); it!=assignment_list.end(); ++it)
    {
      const atermpp::aterm_appl &assignment= *it;
      new_assignments.push_back(core::detail::gsMakeDataVarIdInit(variable(assignment(0)),
    		atermpp::aterm_cast<const atermpp::aterm_appl>(subst_values(vars,vals,number_of_vars,atermpp::aterm_cast<const atermpp::aterm_appl>(assignment(1))))));
    }
    atermpp::term_list < atermpp::aterm_appl > new_assignment_list;
    for(std::vector < atermpp::aterm_appl >::reverse_iterator it=new_assignments.rbegin(); it!=new_assignments.rend(); ++it)
    {
      new_assignment_list.push_front(*it);
    }
    return gsMakeWhr(body,new_assignment_list);
  }
  else
  {
    const atermpp::aterm_appl &t1=aterm_cast<const atermpp::aterm_appl>(t);
    const size_t arity = t1.size();
    const subst_values_argument substitute_values_in_arguments(vars,vals,number_of_vars);
    return ApplyArray(arity,t1.begin(),t1.end(),substitute_values_in_arguments);
  }
}

// Match term t with the lhs p of an equation in internal format.
static bool match_jitty(
                    const atermpp::aterm &t,
                    const atermpp::aterm &p,
                    // std::vector <variable> &vars,
                    // std::vector <atermpp::aterm> &vals,
                    variable const ** vars,
                    atermpp::aterm const** vals,
                    size_t &number_of_vars,
                    const size_t maxlen)
{
  if (p.type_is_int())
  {
    return p==t;
  }
  else if (is_variable(p))
  {
    for (size_t i=0; i<number_of_vars; i++)
    {
      assert(i<maxlen);
      if (p== *vars[i])
      {
        if (t== *vals[i])
        {
          return true;
        }
        else
        {
          return false;
        }
      }
    }
    assert(number_of_vars<maxlen);
    // vars.push_back(aterm_cast<const variable>(p));
    // vals.push_back(t);
    vars[number_of_vars]=reinterpret_cast<const variable*>(&p);
    vals[number_of_vars]=&t;
    ++number_of_vars;
    return true;
  }
  else
  {
    if (t.type_is_int() || is_variable(t) || is_abstraction(aterm_cast<const atermpp::aterm_appl>(t)) || is_where_clause(aterm_cast<const atermpp::aterm_appl>(t)))
    {
      return false;
    }
    // p and t must be aterm_appl's.
    const atermpp::aterm_appl &pa=aterm_cast<const atermpp::aterm_appl>(p);
    const atermpp::aterm_appl &ta=aterm_cast<const atermpp::aterm_appl>(t);

    if (pa.function()!=ta.function())
    {
      return false;
    }

    size_t arity = pa.size();

    for (size_t i=0; i<arity; i++)
    {
      if (!match_jitty(ta(i),pa(i),vars,vals,number_of_vars,maxlen))
      {
        return false;
      }
    }

    return true;
  }
}

atermpp::aterm_appl RewriterJitty::rewrite_aux(
                      const atermpp::aterm_appl &term,
                      internal_substitution_type &sigma)
{
  if (is_variable(term))
  {
    return sigma(term);
  }
  else if (is_where_clause(term))
  {
    return rewrite_where(term,sigma);
  }
  else if (is_abstraction(term))
  {
    const atermpp::aterm_appl &binder=atermpp::aterm_cast<const atermpp::aterm_appl>(term(0));
    if (binder==gsMakeExists())
    {
      return internal_existential_quantifier_enumeration(term,sigma);
    }
    if (binder==gsMakeForall())
    {
      return internal_universal_quantifier_enumeration(term,sigma);
    }
    if (binder==gsMakeLambda())
    {
      return rewrite_single_lambda(aterm_cast<const variable_list>(term(1)),atermpp::aterm_cast<const atermpp::aterm_appl>(term(2)),false,sigma);
    }
    assert(0);
    return term;
  }
  else // Term has the shape #REWR#(t1,...,tn);
  {
    const atermpp::aterm &op = term(0);
    const size_t arity=term.size();
    atermpp::aterm_appl head;

    if (op.type_is_int())
    {
      return rewrite_aux_function_symbol(aterm_cast<const atermpp::aterm_int>(op),term,sigma);
    }
    else if (is_variable(op))
    {
      head=sigma(atermpp::aterm_cast<const variable>(op));
    }
    else if (is_abstraction(atermpp::aterm_cast<const atermpp::aterm_appl>(op)))
    {
      head=aterm_cast<const atermpp::aterm_appl>(op);
    }
    else if (is_where_clause(atermpp::aterm_cast<const atermpp::aterm_appl>(op)))
    {
      head = rewrite_aux(atermpp::aterm_cast<const atermpp::aterm_appl>(op),sigma);
    }
    else
    {
      // op has the shape = #REWR#(u1,...,un).
      head=aterm_cast<const atermpp::aterm_appl>(op);
    }

    // Here head has the shape
    // u(u1,...,um), lambda y1,....,ym.u, forall y1,....,ym.u or exists y1,....,ym.u,
    if (is_abstraction(head))
    {
      const atermpp::aterm_appl &binder=atermpp::aterm_cast<const atermpp::aterm_appl>(head(0));
      if (binder==gsMakeLambda())
      {
        return rewrite_lambda_application(head,term,sigma);
      }
      if (binder==gsMakeExists())
      {
        return internal_existential_quantifier_enumeration(head,sigma);
      }
      if (binder==gsMakeForall())
      {
        return internal_universal_quantifier_enumeration(head,sigma);
      }
      assert(0); // One cannot end up here.
    }

    if (is_variable(head))
    {
      MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,atermpp::aterm, arity);
      // std::vector < atermpp::aterm > args(arity);
      // args[0]=head;
      new (&args[0]) aterm(head);
      for(size_t i=1; i<arity; ++i)
      {
        // args[i]=rewrite_aux(atermpp::aterm_cast<const atermpp::aterm_appl>(term(i)),sigma);
        new (&args[i]) aterm(rewrite_aux(atermpp::aterm_cast<const atermpp::aterm_appl>(term(i)),sigma));
      }
      const aterm_appl result=ApplyArray(arity,&args[0],&args[0]+arity);
      for(size_t i=0; i<arity; ++i)
      {
        args[i].~aterm();
      }
      return result;
    }
    else
    {
    // Here head has the shape #REWR#(u0,u1,...,un).

      const atermpp::aterm_appl &term_op=head;
      assert(term_op(0).type_is_int());
      const size_t arity_op=term_op.size();
      MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,atermpp::aterm, arity+arity_op-1);
      // std::vector < atermpp::aterm > args(arity+arity_op-1);
      for(size_t i=0; i<arity_op; ++i)
      {
        // args[i]=term_op(i);
        new (&args[i]) aterm(term_op(i));
      }
      for(size_t i=1; i<arity; ++i)
      {
        // args[i+arity_op-1]=term(i);
        new (&args[i+arity_op-1]) aterm(term(i));
      }
      const atermpp::aterm_appl result=rewrite_aux(ApplyArray(arity+arity_op-1,&args[0],&args[0]+arity+arity_op-1),sigma);
      for(size_t i=0; i<arity+arity_op-1; ++i)
      {
        args[i].~aterm();
      }
      return result;
    }
  }
}

aterm_appl RewriterJitty::rewrite_aux_function_symbol(
                      const aterm_int &op,
                      const aterm_appl &term,
                      internal_substitution_type &sigma)
{
  // The first term is function symbol; apply the necessary rewrite rules using a jitty strategy.
  const size_t arity=term.size();

  MCRL2_SYSTEM_SPECIFIC_ALLOCA(rewritten,aterm, arity);
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(rewritten_defined,bool, arity);

  for(size_t i=0; i<arity; ++i)
  {
    rewritten_defined[i]=false;
  }

  const size_t op_value=op.value();
  if (op_value>=jitty_strat.size())
  { 
    make_jitty_strat_sufficiently_larger(op_value);
  }

  const aterm_list &strat=jitty_strat[op_value];
  if (strat.defined())
  {
    for (aterm_list::const_iterator strategy_it=strat.begin(); strategy_it!=strat.end(); ++strategy_it)
    {
      const aterm_list &rule = aterm_cast<const aterm_list>(*strategy_it);
      if (rule.type_is_int())
      {
        const size_t i = (aterm_cast<const aterm_int>(rule)).value()+1;
        if (i < arity)
        {
          assert(!rewritten_defined[i]);
          rewritten_defined[i]=true;
          new (&rewritten[i]) aterm(rewrite_aux(aterm_cast<const aterm_appl>(term(i)),sigma));
          assert(rewritten[i].defined());
        }
        else
        {
          break;
        }
      }
      else
      {
        const aterm_appl &lhs = aterm_cast<const aterm_appl>(element_at(rule,2));
        size_t rule_arity = lhs.size();

        if (rule_arity > arity)
        {
          break;
        }

        size_t max_len = aterm_cast<const aterm_list>(rule.front()).size();
        assert(max_len<=MAX_LEN);
        size_t number_of_vars=0;

        MCRL2_SYSTEM_SPECIFIC_ALLOCA(vars,const variable*, max_len);
        MCRL2_SYSTEM_SPECIFIC_ALLOCA(vals,const aterm*, max_len);
        
        bool matches = true;

        for (size_t i=1; i<rule_arity; i++)
        {
          assert(i<arity);
          if (!match_jitty(rewritten_defined[i]?rewritten[i]:term(i),lhs(i),vars,vals,number_of_vars,max_len))
          {
            matches = false;
            break;
          }
        }
        assert(number_of_vars<=max_len);
        if (matches && (element_at(rule,1)==internal_true ||
                        rewrite_aux(aterm_cast<const aterm_appl>(subst_values(vars,vals,number_of_vars,element_at(rule,1))),sigma)==internal_true))
        {
          const aterm_appl &rhs = aterm_cast<const aterm_appl>(element_at(rule,3));

          if (arity == rule_arity)
          {
            const aterm_appl result=rewrite_aux(aterm_cast<const aterm_appl>(subst_values(vars,vals,number_of_vars,rhs)),sigma);
            for (size_t i=0; i<arity; i++)
            {
              if (rewritten_defined[i])
              {
                rewritten[i].~aterm();
              }
            }
            return result;
          }
          else
          {
            assert(arity>rule_arity);
            const size_t new_arity=1+arity-rule_arity;
            MCRL2_SYSTEM_SPECIFIC_ALLOCA(newargs, aterm, new_arity);
            // std::vector < aterm > newargs;
            // newargs.reserve(new_arity);
            // newargs.push_back(subst_values(vars,vals,number_of_vars,rhs));
            new (&newargs[0]) aterm(subst_values(vars,vals,number_of_vars,rhs));
            for(size_t i=1; i<new_arity; ++i)
            {
              assert(rule_arity+i-1<arity);
              // newargs.push_back(term(rule_arity+i-1)));
              new (&newargs[i]) aterm(term(rule_arity+i-1));
            }
            const aterm_appl result=rewrite_aux(ApplyArray(new_arity,&newargs[0],&newargs[0]+new_arity),sigma);
            for (size_t i=0; i<new_arity; ++i)
            {
              newargs[i].~aterm();
            }
            for (size_t i=0; i<arity; ++i)
            {
              if (rewritten_defined[i])
              {
                rewritten[i].~aterm();
              }
            }
            return result;
          }
        }
      }
    }
  }

  // No rewrite rule is applicable. Rewrite the not yet rewritten arguments.
  assert(!rewritten_defined[0]);
  rewritten_defined[0]=true;
  new (&rewritten[0]) aterm(op);
  // rewritten[0] = op;
  for (size_t i=1; i<arity; i++)
  {
    if (!rewritten_defined[i])
    {
      rewritten_defined[i]=true;
      // rewritten[i]=rewrite_aux(aterm_cast<const aterm_appl>(term(i)),sigma);
      new (&rewritten[i]) aterm(rewrite_aux(aterm_cast<const aterm_appl>(term(i)),sigma));
    }
  }

  const aterm_appl result=ApplyArray(arity,&rewritten[0],&rewritten[0]+arity);
  for (size_t i=0; i<arity; i++)
  {
    rewritten[i].~aterm();
  }
  return result;
}

atermpp::aterm_appl RewriterJitty::toRewriteFormat(const data_expression &term)
{
  return toInner(term,true);
}

data_expression RewriterJitty::rewrite(const data_expression &term, substitution_type &sigma)
{
  internal_substitution_type internal_sigma = apply(sigma, boost::bind(&RewriterJitty::toRewriteFormat, this, _1));
  data_expression result=fromRewriteFormat(rewrite_internal(toRewriteFormat(term), internal_sigma));
  return result;
}

atermpp::aterm_appl RewriterJitty::rewrite_internal(
     const atermpp::aterm_appl &term,
     internal_substitution_type &sigma)
{
  if (need_rebuild)
  {
    for( std::map< atermpp::aterm_int, data_equation_list >::iterator opids = jitty_eqns.begin()
        ; opids != jitty_eqns.end()
        ; ++opids )
    {
      const size_t j=opids->first.value();
      make_jitty_strat_sufficiently_larger(j);
      if (!jitty_strat[j].defined())
      {
        jitty_strat[j] = create_strategy(opids->second);
      }
    }
    need_rebuild = false;
  }

  return rewrite_aux(term, sigma);
}

rewrite_strategy RewriterJitty::getStrategy()
{
  return jitty;
}
}
}
}
