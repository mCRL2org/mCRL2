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
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/aterm_ext.h"
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

static variable_list get_vars(const atermpp::aterm_appl a)
{
  if (is_variable(a))
  {
    return push_front(variable_list(),variable(a));
  }
  else
  {
    variable_list l;
    for(atermpp::aterm_appl::const_iterator arg=a.begin(); arg!=a.end(); ++arg)
    {
      l= get_vars(*arg)+l;
    }
    return l;
  }
}

// Assume that the expression t is an application, and return its leading function symbol.
static function_symbol get_function_symbol_of_head(const data_expression t)
{
  if (is_function_symbol(t))
  {
    return t;
  }
  assert(is_application(t));

  return get_function_symbol_of_head(application(t).head());
}



static ATermList create_strategy(data_equation_list rules1, RewriterJitty *rewriter)
{

  size_t max_arity = 0;
  for (data_equation_list::const_iterator l=rules1.begin(); l!=rules1.end(); ++l)
  {
    const size_t current_arity=rewriter->toRewriteFormat(l->lhs()).size();
    if (current_arity > max_arity + 1)
    {
      max_arity = current_arity-1;
    }
  }


  ATermList rules=ATempty;
  for(data_equation_list::const_iterator j=rules1.begin(); j!=rules1.end(); ++j)
  {
    rules = ATinsert(rules,(ATerm) ATmakeList4((ATerm) static_cast<ATermList>(j->variables()),
                                         (ATerm)(ATermAppl)rewriter->toRewriteFormat(j->condition()),
                                         (ATerm) (ATermAppl)rewriter->toRewriteFormat(j->lhs()),
                                         (ATerm)(ATermAppl)rewriter->toRewriteFormat(j->rhs())));
  }
  rules = ATreverse(rules);

  ATermList strat = ATmakeList0();
  size_t arity;

  MCRL2_SYSTEM_SPECIFIC_ALLOCA(used,bool, max_arity);
  for (size_t i=0; i<max_arity; ++i)
  {
    used[i]=false;
  }

  arity = 0;
  while (!ATisEmpty(rules))
  {
    ATermList l = ATmakeList0();
    ATermList m = ATmakeList0();

    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,int, arity);
    for (size_t i=0; i<arity; ++i)
    {
      args[i]=-1;
    }

    for (; !ATisEmpty(rules); rules=ATgetNext(rules))
    {
      if (ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(rules),2))) == arity + 1)
      {
        atermpp::aterm_appl cond = ATAelementAt(ATLgetFirst(rules),1);
        atermpp::term_list <variable_list> vars = push_front(atermpp::term_list <variable_list>(),get_vars(cond));
        atermpp::aterm_appl pars = ATAelementAt(ATLgetFirst(rules),2);

        MCRL2_SYSTEM_SPECIFIC_ALLOCA(bs, bool, arity);
        for (size_t i = 0; i < arity; i++)
        {
          bs[i]=false;
        }

        for (size_t i = 0; i < arity; i++)
        {
          if (!is_variable(pars(i+1)))
          {
            bs[i] = true;
            const variable_list evars = get_vars(pars(i+1));
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
            vars = push_back(vars,get_vars(pars(i+1)));
          }
          else
          {
            int j = -1;
            bool b = false;
            for (atermpp::term_list <variable_list>::const_iterator o=vars.begin(); o!=vars.end(); ++o)
            {
              if (std::find(o->begin(),o->end(),pars(i+1)) != o->end())
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
            vars = push_back(vars,get_vars(pars(i+1)));
          }
        }

        ATermList deps = ATmakeList0();
        for (size_t i = 0; i < arity; i++)
        {
          if (bs[i] && !used[i])
          {
            assert(i<((size_t)1)<<(8*sizeof(int)-1));  // Check whether i can safely be translated into an int.
            deps = ATinsert(deps,(ATerm) ATmakeInt((int)i));
            args[i] += 1;
          }
        }
        deps = ATreverse(deps);

        m = ATinsert(m,(ATerm) ATmakeList2((ATerm) deps,ATgetFirst(rules)));
      }
      else
      {
        l = ATinsert(l,ATgetFirst(rules));
      }
    }

    while (!ATisEmpty(m))
    {
      ATermList m2 = ATmakeList0();
      for (; !ATisEmpty(m); m=ATgetNext(m))
      {
        if (ATisEmpty(ATLgetFirst(ATLgetFirst(m))))
        {
          strat = ATinsert(strat, ATgetFirst(ATgetNext(ATLgetFirst(m))));
        }
        else
        {
          m2 = ATinsert(m2,ATgetFirst(m));
        }
      }
      m = ATreverse(m2);

      if (ATisEmpty(m))
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

        ATermInt k = ATmakeInt(maxidx);
        strat = ATinsert(strat,(ATerm) k);
        m2 = ATmakeList0();
        for (; !ATisEmpty(m); m=ATgetNext(m))
        {
          m2 = ATinsert(m2,(ATerm) ATinsert(ATgetNext(ATLgetFirst(m)),(ATerm) ATremoveElement(ATLgetFirst(ATLgetFirst(m)),(ATerm) k)));
        }
        m = ATreverse(m2);
      }
    }

    rules = ATreverse(l);
    arity++;
  }

  return ATreverse(strat);
}

void RewriterJitty::make_jitty_strat_sufficiently_larger(const size_t i)
{
  if (i>=jitty_strat.size())
  {
    size_t oldsize=jitty_strat.size();
    jitty_strat.resize(i+1);
    for( ; oldsize<jitty_strat.size(); ++oldsize)
    {
      jitty_strat[oldsize]=NULL;
    }
  }
}

RewriterJitty::RewriterJitty(
           const data_specification& data_spec,
           const mcrl2::data::used_data_equation_selector& equation_selector):
        Rewriter()
{
  m_data_specification_for_enumeration = data_spec;

  max_vars = 0;
  need_rebuild = false;

  const atermpp::vector< data_equation > &l = data_spec.equations();
  for (atermpp::vector< data_equation >::const_iterator j=l.begin(); j!=l.end(); ++j)
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
      atermpp::map< atermpp::aterm_int, data_equation_list >::iterator it = jitty_eqns.find(lhs_head_index);
      if (it != jitty_eqns.end())
      {
        n = it->second;
      }
      if (j->variables().size() > max_vars)
      {
        max_vars = j->variables().size();
      }
      n = push_front(n,*j);
      jitty_eqns[lhs_head_index] = n;
    }
  }

  for(atermpp::map< function_symbol, atermpp::aterm_int >::const_iterator l1 = term2int_begin()
      ; l1 != term2int_end()
      ; ++l1)
  {

    atermpp::aterm_int i = l1->second;
    atermpp::map< atermpp::aterm_int, data_equation_list >::iterator it = jitty_eqns.find( i );

    make_jitty_strat_sufficiently_larger(i.value());
    if (it == jitty_eqns.end() )
    {
      jitty_strat[i.value()] = NULL;
    }
    else
    {
      jitty_strat[i.value()] = create_strategy(reverse(it->second), this);
    }
  }
}

RewriterJitty::~RewriterJitty()
{
}

bool RewriterJitty::addRewriteRule(const data_equation rule)
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
  atermpp::map< atermpp::aterm_int, data_equation_list >::iterator it = jitty_eqns.find(lhs_head_index);
  if (it != jitty_eqns.end())
  {
    n = it->second;
  }

  if (rule.variables().size() > max_vars)
  {
    max_vars = rule.variables().size();
  }
  n = push_front(n,rule);
  jitty_eqns[lhs_head_index] = n;
  make_jitty_strat_sufficiently_larger(lhs_head_index.value());
  jitty_strat[lhs_head_index.value()] = NULL; //create_strategy(n);
  need_rebuild = true;

  return true;
}

bool RewriterJitty::removeRewriteRule(const data_equation rule)
{
  atermpp::aterm_int lhs_head_index=OpId2Int(get_function_symbol_of_head(rule.lhs()));

  data_equation_list n;
  const atermpp::map< atermpp::aterm_int, data_equation_list >::iterator it = jitty_eqns.find(lhs_head_index);
  if (it != jitty_eqns.end())
  {
    n = it->second;
  }

  n = remove_one_element(n,rule);

  make_jitty_strat_sufficiently_larger(lhs_head_index.value());
  if (n.empty())
  {
    jitty_eqns.erase( it );
    jitty_strat[lhs_head_index.value()] = NULL;
  }
  else
  {
    jitty_eqns[lhs_head_index] = n;
    jitty_strat[lhs_head_index.value()] = NULL;//create_strategy(n);
    need_rebuild = true;
  }

  return true;
}

static atermpp::aterm subst_values(variable* vars, atermpp::aterm* vals, size_t len, const atermpp::aterm t)
{
  if (t.type()==AT_INT)
  {
    return t;
  }
  else if (is_variable(t))
  {
    for (size_t i=0; i<len; i++)
    {
      if (t==vars[i])
      {
        return vals[i];
      }
    }
    return t;
  }
  else if (is_abstraction(t))
  {
    const atermpp::aterm_appl t1=t;
    const atermpp::aterm_appl binder=t1(0);
    const variable_list bound_variables=t1(1);
    const atermpp::aterm_appl body=subst_values(vars,vals,len,t1(2));
#ifndef NDEBUG
    // Check that variables in right hand sides of equations do not clash with bound variables.
    for(size_t i=0; i<len; ++i)
    {
      for(variable_list::const_iterator it=bound_variables.begin(); it!=bound_variables.end(); ++it)
      {
        assert(*it!=vars[i]);
      }
    }
#endif
    return gsMakeBinder(binder,bound_variables,body);

  }
  else if (is_where_clause(t))
  {
    const atermpp::aterm_appl t1=t;
    const atermpp::term_list < atermpp::aterm_appl > assignment_list=t1(1);
    const atermpp::aterm_appl body=subst_values(vars,vals,len,t1(0));

#ifndef NDEBUG
    // Check that variables in right hand sides of equations do not clash with bound variables.
    for(size_t i=0; i<len; ++i)
    {
      for(atermpp::term_list < atermpp::aterm_appl >::const_iterator it=assignment_list.begin(); it!=assignment_list.end(); ++it)
      {
        assert(atermpp::aterm_appl(*it)(0)!=vars[i]);
      }
    }
#endif

    atermpp::vector < atermpp::aterm_appl > new_assignments;

    for(atermpp::term_list < atermpp::aterm_appl > :: const_iterator it=assignment_list.begin(); it!=assignment_list.end(); ++it)
    {
      const atermpp::aterm_appl assignment= *it;
      new_assignments.push_back(core::detail::gsMakeDataVarIdInit(variable(assignment(0)),
    		(atermpp::aterm_appl)  subst_values(vars,vals,len,(atermpp::aterm_appl)assignment(1))));
    }
    atermpp::term_list < atermpp::aterm_appl > new_assignment_list;
    for(atermpp::vector < atermpp::aterm_appl >::reverse_iterator it=new_assignments.rbegin(); it!=new_assignments.rend(); ++it)
    {
      new_assignment_list=push_front(new_assignment_list,*it);
    }
    return gsMakeWhr(body,new_assignment_list);
  }
  else
  {
    const atermpp::aterm_appl t1=t;
    const size_t arity = t1.size();
    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args, atermpp::aterm, arity);
    for (size_t i=0; i<arity; i++)
    {
      args[i] = subst_values(vars,vals,len,t1(i));
    }
    return ApplyArray(arity,args);
  }
}

// Match term t with the lhs p of an equation in internal format.
static bool match_jitty(
                    const atermpp::aterm t,
                    const atermpp::aterm p,
                    variable* vars,
                    atermpp::aterm* vals,
                    size_t* len,
                    const size_t maxlen)
{
  if (p.type()==AT_INT)
  {
    return p==t;
  }
  else if (is_variable(p))
  {
    for (size_t i=0; i<*len; i++)
    {
      assert(i<maxlen);
      if (p==vars[i])
      {
        if (t==vals[i])
        {
          return true;
        }
        else
        {
          return false;
        }
      }
    }
    assert(*len<maxlen);
    vars[*len] = variable(p);
    vals[*len] = t;
    (*len)++;
    return true;
  }
  else
  {
    if (t.type()==AT_INT || is_variable(t) || is_abstraction(t) || is_where_clause(t))
    {
      return false;
    }
    // p and t must be aterm_appl's.
    atermpp::aterm_appl pa=p;
    atermpp::aterm_appl ta=t;

    if (pa.function()!=ta.function())
    {
      return false;
    }

    size_t arity = pa.size();

    for (size_t i=0; i<arity; i++)
    {
      if (!match_jitty(ta(i),pa(i),vars,vals,len,maxlen))
      {
        return false;
      }
    }

    return true;
  }
}

atermpp::aterm_appl RewriterJitty::rewrite_aux(
                      const atermpp::aterm_appl term,
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
    atermpp::aterm_appl binder=term(0);
    if (binder==gsMakeExists())
    {
      atermpp::aterm_appl a= internal_existential_quantifier_enumeration(term,sigma);
      return a;
    }
    if (binder==gsMakeForall())
    {
      atermpp::aterm_appl a=internal_universal_quantifier_enumeration(term,sigma);
      return a;
    }
    if (binder==gsMakeLambda())
    {
      atermpp::aterm_appl a=rewrite_single_lambda(term(1),term(2),false,sigma);
      return a;
    }
    assert(0);
    return term;
  }
  else // Term has the shape #REWR#(t1,...,tn);
  {
    const atermpp::aterm op = term(0);
    const size_t arity=term.size();
    atermpp::aterm_appl head;

    if (ATisInt(op))
    {
      atermpp::aterm_appl a= rewrite_aux_function_symbol(atermpp::aterm_int(op),term,sigma);
      return a;
    }
    else if (is_variable(op))
    {
      head=sigma(variable(op));
    }
    else if (is_abstraction(op))
    {
      head=op;
    }
    else if (is_where_clause(op))
    {
      head = rewrite_aux(atermpp::aterm_appl(op),sigma);
    }
    else
    {
      // op has the shape = #REWR#(u1,...,un).
      head=op;
    }

    // Here head has the shape
    // u(u1,...,um), lambda y1,....,ym.u, forall y1,....,ym.u or exists y1,....,ym.u,
    if (is_abstraction(head))
    {
      const atermpp::aterm_appl binder=head(0);
      if (binder==gsMakeLambda())
      {
        atermpp::aterm_appl a= rewrite_lambda_application(head,term,sigma);
        return a;
      }
      if (binder==gsMakeExists())
      {
        atermpp::aterm_appl a=  internal_existential_quantifier_enumeration(head,sigma);
        return a;
      }
      if (binder==gsMakeForall())
      {
        atermpp::aterm_appl a= internal_universal_quantifier_enumeration(head,sigma);
        return a;
      }
      assert(0); // One cannot end up here.
    }

    if (is_variable(head))
    {
      MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,atermpp::aterm, arity);
      args[0]=head;
      for(size_t i=1; i<arity; ++i)
      {
        args[i]=rewrite_aux(atermpp::aterm_appl(term(i)),sigma);
      }
      atermpp::aterm_appl a=ApplyArray(arity,args);
      return a;
    }
    else
    {
    // Here head has the shape #REWR#(u0,u1,...,un).

      atermpp::aterm_appl term_op=head;
      assert(ATisInt(term_op(0)));
      const size_t arity_op=term_op.size();
      MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,atermpp::aterm, arity+arity_op-1);
      for(size_t i=0; i<arity_op; ++i)
      {
        args[i]=term_op(i);
      }
      for(size_t i=1; i<arity; ++i)
      {
        args[i+arity_op-1]=term(i);
      }
      atermpp::aterm_appl a=rewrite_aux(ApplyArray(arity+arity_op-1,args),sigma);
      return a;
    }
  }
}

atermpp::aterm_appl RewriterJitty::rewrite_aux_function_symbol(
                      const atermpp::aterm_int op,
                      const atermpp::aterm_appl term,
                      internal_substitution_type &sigma)
{
  // The first term is function symbol; apply the necessary rewrite rules using a jitty strategy.

  ATermList strat;
  const size_t arity=term.size();

  MCRL2_SYSTEM_SPECIFIC_ALLOCA(rewritten,atermpp::aterm, arity);
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,atermpp::aterm, arity);

  for(size_t i=1; i<arity; ++i)
  {
    rewritten[i]=atermpp::aterm_appl();
    args[i]=term(i);
  }

  make_jitty_strat_sufficiently_larger(op.value());
  if ((strat = jitty_strat[op.value()]) != NULL)
  {
    for (; !ATisEmpty(strat); strat=ATgetNext(strat))
    {
      if (ATisInt(ATgetFirst(strat)))
      {
        size_t i = ATgetInt((ATermInt) ATgetFirst(strat))+1;
        if (i < arity)
        {
          rewritten[i] = rewrite_aux(args[i],sigma);
        }
        else
        {
          break;
        }
      }
      else
      {
        ATermList rule = ATLgetFirst(strat);
        ATermAppl lhs = ATAelementAt(rule,2);
        size_t rule_arity = ATgetArity(ATgetAFun(lhs));

        if (rule_arity > arity)
        {
          break;
        }

        size_t max_len = ATgetLength(ATLgetFirst(rule));

        MCRL2_SYSTEM_SPECIFIC_ALLOCA(vars,variable, max_len);
        MCRL2_SYSTEM_SPECIFIC_ALLOCA(vals,atermpp::aterm, max_len);

        size_t len = 0;
        bool matches = true;

        for (size_t i=1; i<rule_arity; i++)
        {
          assert(i<arity);
          if (!match_jitty((rewritten[i]==atermpp::aterm())?(args[i]):rewritten[i],ATgetArgument(lhs,i),vars,vals,&len,max_len))
          {
            matches = false;
            break;
          }
        }
        assert(len<=max_len);
        if (matches && (ATAelementAt(rule,1)==internal_true ||
                        rewrite_aux(subst_values(vars,vals,len,ATelementAt(rule,1)),sigma)==internal_true))
        {
          atermpp::aterm_appl rhs = ATAelementAt(rule,3);

          if (arity == rule_arity)
          {
            return rewrite_aux(subst_values(vars,vals,len, rhs),sigma);
          }
          else
          {
            assert(arity>rule_arity);
            const size_t new_arity=1+arity-rule_arity;
            MCRL2_SYSTEM_SPECIFIC_ALLOCA(newargs, atermpp::aterm, new_arity);
            newargs[0]=subst_values(vars,vals,len,rhs);
            for(size_t i=1; i<new_arity; ++i)
            {
              assert(rule_arity+i-1<arity);
              newargs[i]=args[rule_arity+i-1];
            }
            const atermpp::aterm_appl a = ApplyArray(new_arity,newargs);
            return rewrite_aux(a,sigma);
          }
        }
      }
    }
  }

  // No rewrite rule is applicable. Rewrite the not yet rewritten arguments.
  rewritten[0] = op;
  for (size_t i=1; i<arity; i++)
  {
    if (rewritten[i] == atermpp::aterm_appl())
    {
      rewritten[i] = rewrite_aux(args[i],sigma);
    }
  }

  return ApplyArray(arity,rewritten);
}

atermpp::aterm_appl RewriterJitty::toRewriteFormat(const data_expression term)
{
  return toInner(term,true);
}

data_expression RewriterJitty::rewrite(const data_expression term, substitution_type &sigma)
{
  internal_substitution_type internal_sigma = apply(sigma, boost::bind(&RewriterJitty::toRewriteFormat, this, _1));
  return fromRewriteFormat(rewrite_internal(toRewriteFormat(term), internal_sigma));
}

atermpp::aterm_appl RewriterJitty::rewrite_internal(
     const atermpp::aterm_appl term,
     internal_substitution_type &sigma)
{
  if (need_rebuild)
  {
    for( atermpp::map< atermpp::aterm_int, data_equation_list >::iterator opids = jitty_eqns.begin()
        ; opids != jitty_eqns.end()
        ; ++opids )
    {
      const size_t j=opids->first.value();
      make_jitty_strat_sufficiently_larger(j);
      if (jitty_strat[j] == NULL)
      {
        jitty_strat[j] = create_strategy(opids->second, this);
      }
    }
    need_rebuild = false;
  }

  const atermpp::aterm_appl a=rewrite_aux(term, sigma);
  return a;
}

rewrite_strategy RewriterJitty::getStrategy()
{
  return jitty;
}
}
}
}
