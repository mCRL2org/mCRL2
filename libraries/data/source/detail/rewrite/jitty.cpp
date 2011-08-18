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
#include "mcrl2/exception.h"
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/aterm_ext.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/print.h"

using namespace mcrl2::log;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;

namespace mcrl2
{
namespace data
{
namespace detail
{

static AFun opidAFun;
static unsigned int is_initialised = 0;

#define gsIsOpId(x) (ATgetAFun(x) == opidAFun)


static void initialise_common()
{
  if (is_initialised == 0)
  {
    opidAFun = ATgetAFun(static_cast<ATermAppl>(sort_bool::true_()));
    ATprotectAFun(opidAFun);
  }

  is_initialised++;
}

static void finalise_common()
{
  assert(is_initialised > 0);
  is_initialised--;

  if (is_initialised == 0)
  {
    ATunprotectAFun(opidAFun);
  }
}

static ATermList get_vars(ATerm a)
{
  if (gsIsDataVarId((ATermAppl) a))
  {
    return ATmakeList1(a);
  }
  else
  {
    ATermList l = ATmakeList0();
    ATermList m = ATgetArguments((ATermAppl) a);
    for (; !ATisEmpty(m); m=ATgetNext(m))
    {
      l = ATconcat(l,get_vars(ATgetFirst(m)));
    }
    return l;
  }
}

static ATermList create_strategy(ATermList rules, ATermAppl jitty_true)
{
  ATermList strat = ATmakeList0();
  size_t arity;

  size_t max_arity = 0;
  for (ATermList l=rules; !ATisEmpty(l); l=ATgetNext(l))
  {
    if (ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(l),2))) > max_arity + 1)
    {
      max_arity = ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(l),2)))-1;
    }
  }
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
        ATermAppl cond = ATAelementAt(ATLgetFirst(rules),1);
        ATermList vars = (ATisEqual(cond,jitty_true))?ATmakeList1((ATerm) ATmakeList0()):ATmakeList1((ATerm) get_vars((ATerm) cond));
        ATermAppl pars = ATAelementAt(ATLgetFirst(rules),2);

        MCRL2_SYSTEM_SPECIFIC_ALLOCA(bs,bool, arity);
        for (size_t i = 0; i < arity; i++)
        {
          bs[i]=false;
        }

        for (size_t i = 0; i < arity; i++)
        {
          if (!gsIsDataVarId(ATAgetArgument(pars,i+1)))
          {
            bs[i] = true;
            ATermList evars = get_vars(ATgetArgument(pars,i+1));
            for (; !ATisEmpty(evars); evars=ATgetNext(evars))
            {
              int j=0;
              for (ATermList o=ATgetNext(vars); !ATisEmpty(o); o=ATgetNext(o))
              {
                if (ATindexOf(ATLgetFirst(o),ATgetFirst(evars),0) != ATERM_NON_EXISTING_POSITION)
                {
                  bs[j] = true;
                }
                j++;
              }
            }
            vars = ATappend(vars,(ATerm) get_vars(ATgetArgument(pars,i+1)));
          }
          else
          {
            int j = -1;
            bool b = false;
            for (ATermList o=vars; !ATisEmpty(o); o=ATgetNext(o))
            {
              if (ATindexOf(ATLgetFirst(o),ATgetArgument(pars,i+1),0) != ATERM_NON_EXISTING_POSITION)
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
            vars = ATappend(vars,(ATerm) get_vars(ATgetArgument(pars,i+1)));
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

RewriterJitty::RewriterJitty(const data_specification& DataSpec, const mcrl2::data::used_data_equation_selector& equation_selector)
{
  ATermList n;
  ATermInt i;

  m_data_specification_for_enumeration = DataSpec;

  initialise_common();

  max_vars = 0;
  need_rebuild = false;

  jitty_true=NULL;
  ATprotectAppl(&jitty_true);
  jitty_true = (ATermAppl)toRewriteFormat(sort_bool::true_());

  const atermpp::vector< data_equation > l = DataSpec.equations();
  for (atermpp::vector< data_equation >::const_iterator j=l.begin();
         j!=l.end(); ++j)
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
  
      ATermAppl u = (ATermAppl)toRewriteFormat(j->lhs());
  
      atermpp::map< ATermInt, ATermList >::iterator it = jitty_eqns.find( (ATermInt) ATgetArgument(u,0));
      if (it == jitty_eqns.end())
      {
        n = ATmakeList0();
      } else {
        n = it->second;
      }
      if (j->variables().size() > max_vars)
      {
        max_vars = j->variables().size();
      }
      n = ATinsert(n,(ATerm) ATmakeList4((ATerm) static_cast<ATermList>(j->variables()),
                                         (ATerm)(ATermAppl)toRewriteFormat(j->condition()),
                                         (ATerm) u,
                                         (ATerm)(ATermAppl)toRewriteFormat(j->rhs())));
      jitty_eqns[ (ATermInt) ATgetArgument(u,0)] = n;
    }
  }

  for(atermpp::map< function_symbol, atermpp::aterm_int >::const_iterator l1 = term2int_begin()
      ; l1 != term2int_end()
      ; ++l1)
  {

    i = l1->second;

     atermpp::map< ATermInt, ATermList >::iterator it = jitty_eqns.find( i );

    if (ATgetInt(i)>=jitty_strat.size())
    { 
      int oldsize=jitty_strat.size();
      jitty_strat.resize(ATgetInt(i)+1);
      for( ; oldsize<jitty_strat.size(); ++oldsize)
      {
        jitty_strat[oldsize]=NULL;
      }
    }
    if (it == jitty_eqns.end() )
    {
      jitty_strat[ATgetInt(i)] = NULL;
    }
    else
    {
      jitty_strat[ATgetInt(i)] = create_strategy(ATreverse( it->second ), jitty_true);
    }
  }
}

RewriterJitty::~RewriterJitty()
{
  ATunprotectAppl(&jitty_true);
  finalise_common();
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

  ATermAppl u = (ATermAppl) toRewriteFormat(rule.lhs());
  ATermList n = (ATermList) jitty_eqns[ (ATermInt) ATgetArgument(u,0)];
  if (n == NULL)
  {
    n = ATmakeList0();
  }
  if (ATgetLength(ATLgetArgument(rule,0)) > max_vars)
  {
    max_vars = ATgetLength(ATLgetArgument(rule,0));
  }
  n = ATinsert(n,(ATerm) ATmakeList4(
                            (ATerm)(ATermList)rule.variables(),
                            (ATerm)(ATermAppl)toRewriteFormat(rule.condition()),
                            (ATerm) u,
                            (ATerm)(ATermAppl)toRewriteFormat(rule.rhs())));
  jitty_eqns[ (ATermInt) ATgetArgument(u,0)] = n;

  jitty_strat[ATgetInt((ATermInt) ATgetArgument(u,0))] = NULL; //create_strategy(n);
  need_rebuild = true;

  return true;
}

bool RewriterJitty::removeRewriteRule(const data_equation rule)
{
  ATermAppl u = (ATermAppl) toRewriteFormat(rule.lhs());
  ATermList n = jitty_eqns[ (ATermInt) ATgetArgument(u,0)];
  if (n == NULL)
  {
    return true;
  }
  ATermList e = ATmakeList4(
                    (ATerm)(ATermList)rule.variables(),
                    (ATerm)(ATermAppl)toRewriteFormat(rule.condition()),
                    (ATerm) u,
                    (ATerm)(ATermAppl)toRewriteFormat(rule.rhs()));

  ATermList l = n;
  for (n = ATmakeList0(); !ATisEmpty(l); l=ATgetNext(l))
  {
    if (!ATisEqual(e,ATgetFirst(l)))
    {
      ATinsert(n,ATgetFirst(l));
    }
  }
  ATermInt i = (ATermInt) ATgetArgument(u,0);
  if (ATisEmpty(n))
  {
    atermpp::map< ATermInt, ATermList >::iterator it = jitty_eqns.find( i );
    jitty_eqns.erase( it );
    jitty_strat[ATgetInt(i)] = NULL;
  }
  else
  {
    jitty_eqns[i] = n;
    jitty_strat[ATgetInt(i)] = NULL;//create_strategy(n);
    need_rebuild = true;
  }

  return true;
}

static atermpp::aterm subst_values(ATermAppl* vars, ATerm* vals, size_t len, const atermpp::aterm t)
{
  if (ATisInt((ATerm)t))
  {
    return t;
  }
  else if (gsIsDataVarId((ATerm)t))
  {
    for (size_t i=0; i<len; i++)
    {
      if (ATisEqual((ATerm)t,vars[i]))
      {
        return vals[i];
      }
    }
    throw mcrl2::runtime_error(NAME + ": variable not assigned.");
  }
  else
  {
    size_t arity = ATgetArity(ATgetAFun((ATerm)t));
    size_t new_arity = arity;
    atermpp::aterm arg0 = subst_values(vars,vals,len,ATgetArgument((ATerm) t,0));
    if (!(ATisInt((ATerm)arg0) || gsIsDataVarId((ATerm) arg0)))
    {
      new_arity += ATgetArity(ATgetAFun((ATerm) arg0))-1;
    }

    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args, atermpp::aterm, new_arity);
    size_t i;
    if (ATisInt((ATerm)arg0) || gsIsDataVarId((ATerm) arg0))
    {
      args[0] = arg0;
      i = 1;
    }
    else
    {
      i = 0;
      size_t arg0_arity = ATgetArity(ATgetAFun((ATermAppl)(ATerm) arg0));
      while (i < arg0_arity)
      {
        args[i] = atermpp::aterm_appl(arg0)(i);
        i++;
      }
    }

    for (size_t j=1; j<arity; j++)
    {
      args[i] = subst_values(vars,vals,len,ATgetArgument((ATermAppl)(ATerm) t,j));
      i++;
    }

    if (arity == new_arity)
    {
      return atermpp::aterm(ATmakeApplArray(ATgetAFun((ATerm) t),(ATerm*)args));
    }
    else
    {
      return atermpp::aterm_appl(ApplyArray(new_arity,args));
    }
  }
}

static bool match_jitty(
                    ATerm t, 
                    ATerm p, 
                    ATermAppl* vars, 
                    ATerm* vals, 
                    size_t* len,
                    const size_t maxlen)
{
  if (ATisInt(p))
  {
    return ATisEqual(p,t);
  }
  else if (gsIsDataVarId((ATermAppl) p))
  {
    for (size_t i=0; i<*len; i++)
    {
      assert(i<maxlen);
      if (ATisEqual(p,vars[i]))
      {
        if (ATisEqual(t,vals[i]))
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
    vars[*len] = (ATermAppl) p;
    vals[*len] = t;
    (*len)++;
    return true;
  }
  else
  {
    if (ATisInt(t) || gsIsDataVarId((ATermAppl) t))
    {
      return false;
    }
    if (!ATisEqualAFun(ATgetAFun((ATermAppl) p),ATgetAFun((ATermAppl) t)))
    {
      return false;
    }

    size_t arity = ATgetArity(ATgetAFun((ATermAppl) p));

    for (size_t i=0; i<arity; i++)
    {
      if (!match_jitty(ATgetArgument((ATermAppl) t,i),ATgetArgument((ATermAppl) p,i),vars,vals,len,maxlen))
      {
        return false;
      }
    }

    return true;
  }
}

atermpp::aterm_appl RewriterJitty::rewrite_aux(
                      const atermpp::aterm_appl term, 
                      mutable_map_substitution<atermpp::map < variable,atermpp::aterm_appl> > &sigma)
{
  if (gsIsDataVarId(term))
  {
    return sigma(term);
  }
  else
  {
    atermpp::aterm op = term(0);
    atermpp::aterm_appl head;
    ATermList strat;
    size_t head_arity = 0;
    size_t arity = ATgetArity(ATgetAFun(term));

    if (!ATisInt((ATerm)op))
    {
      head = sigma(atermpp::aterm_appl(op));
      if (ATisInt((ATerm)(ATermAppl)head))
      {
        op = head;
      }
      else if (!gsIsDataVarId((ATermAppl) head))
      {
        op = head(0);
        head_arity = ATgetArity(ATgetAFun(head));
        arity = arity-1 + head_arity;
      }
    }

    MCRL2_SYSTEM_SPECIFIC_ALLOCA(rewritten,atermpp::aterm, arity);
    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,atermpp::aterm, arity);

    if (head_arity > 0)
    {
      head_arity--;
    }
    for (size_t i=1; i<arity; i++)
    {
      rewritten[i] = atermpp::aterm();
      if (i < head_arity+1)
      {
        args[i] = head(i);
      }
      else
      {
        args[i] = ATAgetArgument(term,i-head_arity);
      }
    }
    if (ATisInt((ATerm)op) && ((strat = jitty_strat[ATgetInt((ATermInt)(ATerm)op)]) != NULL))
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

          MCRL2_SYSTEM_SPECIFIC_ALLOCA(vars,ATermAppl, max_len);
          MCRL2_SYSTEM_SPECIFIC_ALLOCA(vals,ATerm, max_len);
          size_t len = 0;
          bool matches = true;

          for (size_t i=1; i<rule_arity; i++)
          {
            assert(i<arity);
            if (!match_jitty((rewritten[i]==atermpp::aterm())?((ATerm) args[i]):(ATerm)rewritten[i],ATgetArgument(lhs,i),vars,vals,&len,max_len))
            {
              matches = false;
              break;
            }
          }
          assert(len<=max_len);
          if (matches && (ATisEqual(ATAelementAt(rule,1),jitty_true) || 
                          ATisEqual((ATermAppl)rewrite_aux(subst_values(vars,vals,len,ATelementAt(rule,1)),sigma),jitty_true)))
          {
            atermpp::aterm_appl rhs = ATAelementAt(rule,3);

            if (arity == rule_arity)
            {
              return rewrite_aux(subst_values(vars,vals,len, rhs),sigma);
            }

            size_t rhs_arity;
            size_t new_arity;
            atermpp::aterm arg0;

            if (gsIsDataVarId(rhs))
            {
              arg0 = subst_values(vars,vals,len,rhs);
              if (gsIsDataVarId((ATerm) arg0))
              {
                rhs_arity = 0;
                new_arity = 1+arity-rule_arity;
              }
              else
              {
                rhs_arity = ATgetArity(ATgetAFun((ATerm) arg0));
                new_arity = rhs_arity+arity-rule_arity;
              }
            }
            else
            {
              rhs_arity = ATgetArity(ATgetAFun(rhs));
              new_arity = rhs_arity+arity-rule_arity;
              arg0 = subst_values(vars,vals,len,rhs(0));
              if (!(ATisInt((ATerm)arg0) || gsIsDataVarId((ATerm) arg0)))
              {
                new_arity += ATgetArity(ATgetAFun((ATermAppl)(ATerm) arg0))-1;
              }
            }

            MCRL2_SYSTEM_SPECIFIC_ALLOCA(newargs, atermpp::aterm, new_arity);
            size_t i;
            if (gsIsDataVarId(rhs))
            {
              if (gsIsDataVarId((ATerm) arg0))
              {
                newargs[0] = arg0;
                i = 1;
              }
              else
              {
                i = 0;
                while (i < rhs_arity)
                {
                  newargs[i] = atermpp::aterm_appl(arg0)(i);
                  i++;
                }
              }
            }
            else
            {
              if (ATisInt((ATerm)arg0) || gsIsDataVarId((ATerm) arg0))
              {
                newargs[0] = arg0;
                i = 1;
              }
              else
              {
                i = 0;
                size_t arg0_arity = ATgetArity(ATgetAFun((ATerm) arg0));
                while (i < arg0_arity)
                {
                  newargs[i] = atermpp::aterm_appl(arg0)(i);
                  i++;
                }
              }
              for (size_t j=1; j<rhs_arity; j++)
              {
                newargs[i] = subst_values(vars,vals,len,ATgetArgument(rhs,j));
                i++;
              }
            }

            for (size_t j=0; j<arity-rule_arity; j++)
            {
              newargs[i] = args[rule_arity+j];
              i++;
            }

            const atermpp::aterm_appl a = ApplyArray(new_arity,newargs);

            return rewrite_aux(a,sigma);
          }

        }
      }
    }

    rewritten[0] = op;
    for (size_t i=1; i<arity; i++)
    {
      if (rewritten[i] == atermpp::aterm_appl())
      {
        rewritten[i] = rewrite_aux(args[i],sigma);
      }
    }

    const atermpp::aterm_appl a = ApplyArray(arity,rewritten);

    return internal_quantifier_enumeration(a,sigma);
  }
}

atermpp::aterm_appl RewriterJitty::toRewriteFormat(const data_expression term)
{
  size_t old_opids = get_num_opids();
  atermpp::aterm_appl a = toInner(m_conversion_helper.implement(term),true);
  if (old_opids < get_num_opids())
  {
    int oldsize=jitty_strat.size();
    jitty_strat.resize(get_num_opids());
    for( ; oldsize<jitty_strat.size(); ++oldsize)
    {
      jitty_strat[oldsize]=NULL;
    }
  }

  return a;
}

data_expression RewriterJitty::fromRewriteFormat(const atermpp::aterm_appl term)
{
  return m_conversion_helper.lazy_reconstruct(fromInner(term));
}

data_expression RewriterJitty::rewrite(const data_expression term, mutable_map_substitution<> &sigma)
{
  mutable_map_substitution<atermpp::map < variable, atermpp::aterm_appl> > internal_sigma;
  for(mutable_map_substitution<>::const_iterator i=sigma.begin(); i!=sigma.end(); ++i)
  {
    internal_sigma[i->first]=toRewriteFormat(i->second);
  }
  return fromRewriteFormat(rewrite_internal(toRewriteFormat(term), internal_sigma));
}

atermpp::aterm_appl RewriterJitty::rewrite_internal(
     const atermpp::aterm_appl term,
     mutable_map_substitution<atermpp::map < variable, atermpp::aterm_appl> > &sigma)
{
  if (need_rebuild)
  {
    for( atermpp::map< ATermInt, ATermList >::iterator opids = jitty_eqns.begin()
        ; opids != jitty_eqns.end()
        ; ++opids )
    {
      ATermInt i = opids->first;
      if (jitty_strat[ATgetInt(i)] == NULL)
      {
        jitty_strat[ATgetInt(i)] = create_strategy( opids->second , jitty_true);
      }
    }
    need_rebuild = false;
  }

  const atermpp::aterm_appl a=rewrite_aux(term, sigma);
  return a;
}

RewriteStrategy RewriterJitty::getStrategy()
{
  return GS_REWR_JITTY;
}
}
}
}
