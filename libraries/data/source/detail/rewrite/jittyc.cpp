// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file jittyc.cpp

#include "mcrl2/data/detail/rewrite.h" // Required fo MCRL2_JITTTYC_AVAILABLE.

#ifdef MCRL2_JITTYC_AVAILABLE

#define NAME "rewr_jittyc"

#include <utility>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <cassert>
#include <sstream>
#include <sys/stat.h>
#include "mcrl2/utilities/file_utility.h"
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/utilities/basename.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/substitute.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/detail/rewrite/jittyc.h"
#include "mcrl2/data/detail/rewrite/jitty_jittyc.h"
#include "mcrl2/data/traverser.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace std;
using namespace atermpp;
using namespace mcrl2::log;

namespace mcrl2
{
namespace data
{
namespace detail
{

static atermpp::function_symbol afunS, afunM, afunF, afunN, afunD, afunR, afunCR, afunC, afunX, afunRe, afunCRe, afunMe;
static aterm dummy;
static atermpp::function_symbol afunARtrue, afunARfalse, afunARand, afunARor, afunARvar;
static aterm_appl ar_true, ar_false;

#define isS(x) x.function()==afunS
#define isM(x) x.function()==afunM
#define isF(x) x.function()==afunF
#define isN(x) x.function()==afunN
#define isD(x) x.function()==afunD
#define isR(x) x.function()==afunR
#define isCR(x) x.function()==afunCR
#define isC(x) x.function()==afunC
#define isX(x) x.function()==afunX
#define isRe(x) x.function()==afunRe
#define isCRe(x) x.function()==afunCRe
#define isMe(x) x.function()==afunMe

static size_t is_initialised = 0;

static void initialise_common()
{
  if (is_initialised == 0)
  {
    afunS = atermpp::function_symbol("@@S",2); // Store term ( target_variable, result_tree )
    afunM = atermpp::function_symbol("@@M",3); // Match term ( match_variable, true_tree , false_tree )
    afunF = atermpp::function_symbol("@@F",3); // Match function ( match_function, true_tree, false_tree )
    afunN = atermpp::function_symbol("@@N",1); // Go to next parameter ( result_tree )
    afunD = atermpp::function_symbol("@@D",1); // Go down a level ( result_tree )
    afunR = atermpp::function_symbol("@@R",1); // End of tree ( matching_rule )
    afunCR = atermpp::function_symbol("@@CR",2); // End of tree ( condition, matching_rule )
    afunC = atermpp::function_symbol("@@C",3); // Check condition ( condition, true_tree, false_tree )
    afunX = atermpp::function_symbol("@@X",0); // End of tree
    afunRe = atermpp::function_symbol("@@Re",2); // End of tree ( matching_rule , vars_of_rule)
    afunCRe = atermpp::function_symbol("@@CRe",4); // End of tree ( condition, matching_rule, vars_of_condition, vars_of_rule )
    afunMe = atermpp::function_symbol("@@Me",2); // Match term ( match_variable, variable_index )

    dummy = gsMakeNil();

    afunARtrue = atermpp::function_symbol("@@true",0);
    afunARfalse = atermpp::function_symbol("@@false",0);
    afunARand = atermpp::function_symbol("@@and",2);
    afunARor = atermpp::function_symbol("@@or",2);
    afunARvar = atermpp::function_symbol("@@var",1);
    ar_true = atermpp::aterm_appl(afunARtrue);
    ar_false = atermpp::aterm_appl(afunARfalse);
  }

  is_initialised++;
}

static void finalise_common()
{
  is_initialised--;
}

#define is_ar_true(x) (x==ar_true)
#define is_ar_false(x) (x==ar_false)
#define is_ar_and(x) (x.function()==afunARand)
#define is_ar_or(x) (x.function()==afunARor)
#define is_ar_var(x) (x.function()==afunARvar)

// Prototype
// static aterm toInner_list_odd(const data_expression& t);

static aterm_appl make_ar_true()
{
  return ar_true;
}

static aterm_appl make_ar_false()
{
  return ar_false;
}

static aterm_appl make_ar_and(const aterm_appl& x, const aterm_appl& y)
{
  if (is_ar_true(x))
  {
    return y;
  }
  else if (is_ar_true(y))
  {
    return x;
  }
  else if (is_ar_false(x) || is_ar_false(y))
  {
    return make_ar_false();
  }

  return aterm_appl(afunARand,x,y);
}

static aterm_appl make_ar_or(aterm_appl x, aterm_appl y)
{
  if (is_ar_false(x))
  {
    return y;
  }
  else if (is_ar_false(y))
  {
    return x;
  }
  else if (is_ar_true(x) || is_ar_true(y))
  {
    return make_ar_true();
  }

  return aterm_appl(afunARor,x,y);
}

static aterm_appl make_ar_var(size_t var)
{
  return atermpp::aterm_appl(afunARvar,atermpp::aterm_int(var));
}

/* static size_t num_int2aterms = 0;
static const atermpp::detail::_aterm** int2aterms = NULL; // An array with prepared aterm_int's.
static const atermpp::detail::_aterm* get_int2aterm_value(size_t i)
{
  if (((size_t) i) >= num_int2aterms)
  {
    size_t old_num = num_int2aterms;
    num_int2aterms = i+1;

    int2aterms = (const atermpp::detail::_aterm**) realloc(int2aterms,num_int2aterms*sizeof(aterm));
    if (int2aterms == NULL)
    {
      throw mcrl2::runtime_error("Cannot allocate enough memory.");
    }
    for (size_t j=old_num; j < num_int2aterms; j++)
    {
      int2aterms[j] = NULL;
    }
    for (; old_num < num_int2aterms; old_num++)
    {
      int2aterms[old_num] = reinterpret_cast<const atermpp::detail::_aterm*>(atermpp::detail::address(atermpp::aterm_int(old_num)));
    }
  }
  return int2aterms[i];
}

static const atermpp::detail::_aterm* get_int2aterm_value(const function_symbol& i)
{
  return get_int2aterm_value(OpId2Int(i).value());
} */

/* std::vector <aterm_appl> rewr_appls;

static void set_rewrappl_value(const size_t i)
{
  while (rewr_appls.size()<i+1)
  {
    rewr_appls.push_back(Apply0(atermpp::aterm_int(rewr_appls.size())));
  }
}

static aterm_appl get_rewrappl_value(const size_t i)
{
  set_rewrappl_value(i);
  return rewr_appls[i];
} */

data_expression RewriterCompilingJitty::toRewriteFormat(const data_expression& t)
{
  size_t old_opids = get_num_opids();

  data_expression r = toInner(t,true);

  if (old_opids != get_num_opids())
  {
    need_rebuild = true;
  }

  return r;
} 

/* data_expression RewriterCompilingJitty::fromRewriteFormat(const data_expression t)
{
  return fromInner(t);
} */

static char* whitespace_str = NULL;
static size_t whitespace_len;
static size_t whitespace_pos;
static char* whitespace(size_t len)
{
  if (whitespace_str == NULL)
  {
    whitespace_str = (char*) malloc((2*len+1)*sizeof(char));
    for (size_t i=0; i<2*len; i++)
    {
      whitespace_str[i] = ' ';
    }
    whitespace_len = 2*len;
    whitespace_pos = len;
    whitespace_str[whitespace_pos] = 0;
  }
  else
  {
    if (len > whitespace_len)
    {
      whitespace_str = (char*) realloc(whitespace_str,(2*len+1)*sizeof(char));
      for (size_t i=whitespace_len; i<2*len; i++)
      {
        whitespace_str[i] = ' ';
      }
      whitespace_len = 2*len;
    }

    whitespace_str[whitespace_pos] = ' ';
    whitespace_pos = len;
    whitespace_str[whitespace_pos] = 0;
  }

  return whitespace_str;
}


static void term2seq(const aterm_appl& t, aterm_list* s, size_t *var_cnt)
{
  if (is_function_symbol(t))
  {
    s->push_front(atermpp::aterm_appl(afunF,t,dummy,dummy));
    s->push_front(atermpp::aterm_appl(afunD,dummy));

  }
  else if (t.type_is_appl())
  {
    if (is_variable((aterm_appl) t))
    {
      aterm store = atermpp::aterm_appl(afunS, t,dummy);

      if (std::find(s->begin(),s->end(),store) != s->end())
      {
        s->push_front(atermpp::aterm_appl(afunM,t,dummy,dummy));
      }
      else
      {
        (*var_cnt)++;
        s->push_front(store);
      }
    }
    else
    {
      size_t arity = aterm_cast<aterm_appl>(t).function().arity();

      s->push_front(atermpp::aterm_appl(afunF,atermpp::aterm_cast<const aterm_appl>(t)[0],dummy,dummy));

      for (size_t i=1; i<arity; ++i)
      {
        term2seq(atermpp::aterm_cast<const aterm_appl>(t[i]),s,var_cnt);
        if (i<arity-1)
        {
          s->push_front(atermpp::aterm_appl(afunN,dummy));
        }
      }
      s->push_front(atermpp::aterm_appl(afunD,dummy));
    }
  }
  else
  {
    assert(0);
  }

}

static void get_used_vars_aux(const aterm_appl& t, aterm_list& vars)
{
  using namespace atermpp;
  /* if (t.type_is_list())
  {
    const atermpp::aterm_list& l=atermpp::aterm_cast<const aterm_list>(t);
    for (aterm_list::const_iterator i=l.begin(); i!=l.end(); ++i)
    {
      get_used_vars_aux(*i,vars);
    }
  } */
  if (is_function_symbol(t))
  {
    return;
  }
  else if (t.type_is_appl())
  {
    if (is_variable(t))
    {
      if (find(vars.begin(),vars.end(),t) == vars.end())
      {
        vars.push_front(t);
      }
    }
    else
    {
      size_t a = aterm_cast<aterm_appl>(t).function().arity();
      for (size_t i=0; i<a; i++)
      {
        get_used_vars_aux(aterm_cast<const aterm_appl>(t[i]),vars);
      }
    }
  }
}

static aterm_list get_used_vars(const aterm_appl& t)
{
  aterm_list l;
  get_used_vars_aux(t,l);
  return l;
}

static aterm_list create_sequence(const data_equation& rule, size_t* var_cnt, const aterm_appl& true_inner)
{
  const aterm_appl lhs_inner = toInner(rule.lhs(),true);
  size_t lhs_arity = lhs_inner.size();
  // aterm cond = toInner_list_odd(rule.condition());
  // aterm rslt = toInner_list_odd(rule.rhs());
  const aterm_appl cond = toInner(rule.condition(),true);
  const aterm_appl rslt = toInner(rule.rhs(),true);
  aterm_list rseq;

  if (!is_function_symbol(lhs_inner))
  {
    for (size_t i=1; i<lhs_arity; ++i)
    {
      term2seq(aterm_cast<aterm_appl>(lhs_inner[i]),&rseq,var_cnt);
      if (i<lhs_arity-1)
      {
        rseq.push_front(atermpp::aterm_appl(afunN,dummy));
      }
    }
  }

  if (cond==true_inner)
  {
    rseq.push_front(atermpp::aterm_appl(afunRe,rslt,get_used_vars(rslt)));
  }
  else
  {
    rseq.push_front(atermpp::aterm_appl(afunCRe,cond,rslt, get_used_vars(cond), get_used_vars(rslt)));
  }

  return reverse(rseq);
}


// Structure for build_tree paramters
typedef struct
{
  aterm_list Flist;   // List of sequences of which the first action is an F
  aterm_list Slist;   // List of sequences of which the first action is an S
  aterm_list Mlist;   // List of sequences of which the first action is an M
  aterm_list stack;   // Stack to maintain the sequences that do not have to
  // do anything in the current term
  aterm_list upstack; // List of sequences that have done an F at the current
  // level
} build_pars;

static void initialise_build_pars(build_pars* p)
{
  p->Flist = aterm_list();
  p->Slist = aterm_list();
  p->Mlist = aterm_list();
  p->stack = make_list<aterm>(aterm_list());
  p->upstack = aterm_list();
}

static aterm_list add_to_stack(const aterm_list& stack, aterm_list seqs, aterm_appl* r, aterm_list* cr)
{
  if (stack.empty())
  {
    return stack;
  }

  aterm_list l;
  aterm_list h = aterm_cast<aterm_list>(stack.front());

  for (; !seqs.empty(); seqs=seqs.tail())
  {
    aterm_list e = aterm_cast<aterm_list>(seqs.front());

    if (isD(aterm_cast<aterm_appl>(e.front())))
    {
      l.push_front(e.tail());
    }
    else if (isN(aterm_cast<aterm_appl>(e.front())))
    {
      h.push_front(e.tail());
    }
    else if (isRe(aterm_cast<aterm_appl>(e.front())))
    {
      *r = aterm_cast<aterm_appl>(e.front());
    }
    else
    {
      cr->push_front(e.front());
    }
  }

  aterm_list result=add_to_stack(stack.tail(),l,r,cr);
  result.push_front(h);
  return result;
}

static void add_to_build_pars(build_pars* pars, aterm_list seqs, aterm_appl* r, aterm_list* cr)
{
  aterm_list l;

  for (; !seqs.empty(); seqs=seqs.tail())
  {
    aterm_list e = aterm_cast<aterm_list>(seqs.front());

    if (isD(aterm_cast<aterm_appl>(e.front())) || isN(aterm_cast<aterm_appl>(e.front())))
    {
      l.push_front(e);
    }
    else if (isS(aterm_cast<aterm_appl>(e.front())))
    {
      pars->Slist.push_front(e);
    }
    else if (isMe(aterm_cast<aterm_appl>(e.front())))     // M should not appear at the head of a seq
    {
      pars->Mlist.push_front(e);
    }
    else if (isF(aterm_cast<aterm_appl>(e.front())))
    {
      pars->Flist.push_front(e);
    }
    else if (isRe(aterm_cast<aterm_appl>(e.front())))
    {
      *r = aterm_cast<aterm_appl>(e.front());
    }
    else
    {
      cr->push_front(e.front());
    }
  }

  pars->stack = add_to_stack(pars->stack,l,r,cr);
}

static char tree_var_str[20];
static aterm_appl createFreshVar(const aterm_appl& sort, size_t* i)
{
  sprintf(tree_var_str,"@var_%lu",(*i)++);
  return data::variable(tree_var_str, atermpp::aterm_cast<const sort_expression>(sort));
}

// static aterm_list subst_var(aterm_list l, const aterm_appl& old, const aterm& new_val, const aterm& num, const aterm_list& substs)
static aterm_list subst_var(aterm_list l, const aterm_appl& old, const aterm& new_val, const aterm& num, const substitution& substs)
{
  if (l.empty())
  {
    return l;
  }

  aterm_appl head = (aterm_appl) l.front();
  l = l.tail();

  if (isM(head))
  {
    if (head[0]==old)
    {
      head = atermpp::aterm_appl(afunMe,new_val,num);
    }
  }
  else if (isCRe(head))
  {
    aterm_list l = (aterm_list) head[2];
    aterm_list m ;
    for (; !l.empty(); l=l.tail())
    {
      if (l.front()==old)
      {
        m.push_front(num);
      }
      else
      {
        m.push_front(l.front());
      }
    }
    l = (aterm_list) head[3];
    aterm_list n;
    for (; !l.empty(); l=l.tail())
    {
      if (l.front()==old)
      {
        n.push_front(num);
      }
      else
      {
        n.push_front(l.front());
      }
    }
    head = atermpp::aterm_appl(afunCRe,substs(head[0]),substs(head[1]),m, n);
  }
  else if (isRe(head))
  {
    aterm_list l = (aterm_list) head[1];
    aterm_list m ;
    for (; !l.empty(); l=l.tail())
    {
      if (l.front()==old)
      {
        m.push_front(num);
      }
      else
      {
        m.push_front(l.front());
      }
    }
    head = atermpp::aterm_appl(afunRe,substs(head[0]),m);
  }
  aterm_list result=subst_var(l,old,new_val,num,substs);
  result.push_front(head);
  return result;
}

static std::vector < size_t> treevars_usedcnt;

static void inc_usedcnt(aterm_list l)
{
  for (; !l.empty(); l=l.tail())
  {
    aterm first=l.front();
    if (first.type_is_int())
    {
      treevars_usedcnt[aterm_cast<aterm_int>(first).value()]++;
    }
  }
}

static aterm_appl build_tree(build_pars pars, size_t i)
{
  if (!pars.Slist.empty())
  {
    aterm_list l,m;

    size_t k = i;
    aterm_appl v = createFreshVar(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(aterm_cast<aterm_list>(pars.Slist.front()).front())[0])[1]),&i);
    treevars_usedcnt[k] = 0;

    l = aterm_list();
    m = aterm_list();
    for (; !pars.Slist.empty(); pars.Slist=pars.Slist.tail())
    {
      aterm_list e = aterm_cast<aterm_list>(pars.Slist.front());

      e = subst_var(e,aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(e.front())[0]), v,aterm_int(k),substitution(aterm_cast<aterm_appl>(e.front())[0],v));

      l.push_front(e.front());
      m.push_front(e.tail());
    }

    aterm_appl r;
    aterm_list readies;

    pars.stack = add_to_stack(pars.stack,m,&r,&readies);

    if (r==aterm_appl())
    {
      aterm_appl tree;

      tree = build_tree(pars,i);
      for (; !readies.empty(); readies=readies.tail())
      {
        inc_usedcnt((aterm_list) aterm_cast<aterm_appl>(readies.front())[2]);
        inc_usedcnt((aterm_list) aterm_cast<aterm_appl>(readies.front())[3]);
        tree = atermpp::aterm_appl(afunC,aterm_cast<aterm_appl>(readies.front())[0],atermpp::aterm_appl(afunR,aterm_cast<aterm_appl>(readies.front())[1]),tree);
      }
      r = tree;
    }
    else
    {
      inc_usedcnt((aterm_list) r[1]);
      r = atermpp::aterm_appl(afunR,r[0]);
    }

    if ((treevars_usedcnt[k] > 0) || ((k == 0) && isR(r)))
    {
       return aterm_appl(afunS,v,r);
    }
    else
    {
       return r;
    }
  }
  else if (!pars.Mlist.empty())
  {
    aterm M = aterm_cast<aterm_list>(pars.Mlist.front()).front();

    aterm_list l;
    aterm_list m;
    for (; !pars.Mlist.empty(); pars.Mlist=pars.Mlist.tail())
    {
      if (M==aterm_cast<aterm_list>(pars.Mlist.front()).front())
      {
        l.push_front(aterm_cast<aterm_list>(pars.Mlist.front()).tail());
      }
      else
      {
        m.push_front(pars.Mlist.front());
      }
    }
    pars.Mlist = m;

    aterm_appl true_tree,false_tree;
    aterm_appl r ;
    aterm_list readies;

    aterm_list newstack = add_to_stack(pars.stack,l,&r,&readies);

    false_tree = build_tree(pars,i);

    if (r==aterm_appl())
    {
      pars.stack = newstack;
      true_tree = build_tree(pars,i);
      for (; !readies.empty(); readies=readies.tail())
      {
        inc_usedcnt((aterm_list) aterm_cast<aterm_appl>(readies.front())[2]);
        inc_usedcnt((aterm_list) aterm_cast<aterm_appl>(readies.front())[3]);
        true_tree = atermpp::aterm_appl(afunC,aterm_cast<aterm_appl>(readies.front())[0],atermpp::aterm_appl(afunR,aterm_cast<aterm_appl>(readies.front())[1]),true_tree);
      }
    }
    else
    {
      inc_usedcnt((aterm_list) r[1]);
      true_tree = atermpp::aterm_appl(afunR,r[0]);
    }

    if (true_tree==false_tree)
    {
       return true_tree;
    }
    else
    {
      treevars_usedcnt[aterm_cast<aterm_int>(((aterm_appl) M)[1]).value()]++;
      return atermpp::aterm_appl(afunM,((aterm_appl) M)[0],true_tree,false_tree);
    }
  }
  else if (!pars.Flist.empty())
  {
    aterm_list F = aterm_cast<aterm_list>(pars.Flist.front());
    aterm_appl true_tree,false_tree;

    aterm_list newupstack = pars.upstack;
    aterm_list l;

    for (; !pars.Flist.empty(); pars.Flist=pars.Flist.tail())
    {
      if (aterm_cast<aterm_list>(pars.Flist.front()).front()==F.front())
      {
        newupstack.push_front(aterm_cast<aterm_list>(pars.Flist.front()).tail());
      }
      else
      {
        l.push_front(pars.Flist.front());
      }
    }

    pars.Flist = l;
    false_tree = build_tree(pars,i);
    pars.Flist = aterm_list();
    pars.upstack = newupstack;
    true_tree = build_tree(pars,i);

    if (true_tree==false_tree)
    {
      return true_tree;
    }
    else
    {
      return atermpp::aterm_appl(afunF,aterm_cast<aterm_appl>(F.front())[0],true_tree,false_tree);
    }
  }
  else if (!pars.upstack.empty())
  {
    aterm_list l;

    aterm_appl r;
    aterm_list readies;

    pars.stack.push_front(aterm_list());
    l = pars.upstack;
    pars.upstack = aterm_list();
    add_to_build_pars(&pars,l,&r,&readies);


    if (r==aterm_appl())
    {
      aterm_appl t = build_tree(pars,i);

      for (; !readies.empty(); readies=readies.tail())
      {
        inc_usedcnt((aterm_list) aterm_cast<aterm_appl>(readies.front())[2]);
        inc_usedcnt((aterm_list) aterm_cast<aterm_appl>(readies.front())[3]);
        t = atermpp::aterm_appl(afunC,aterm_cast<aterm_appl>(readies.front())[0],atermpp::aterm_appl(afunR,aterm_cast<aterm_appl>(readies.front())[1]),t);
      }

      return t;
    }
    else
    {
      inc_usedcnt((aterm_list) r[1]);
      return atermpp::aterm_appl(afunR,r[0]);
    }
  }
  else
  {
    if (aterm_cast<aterm_list>(pars.stack.front()).empty())
    {
      if (pars.stack.tail().empty())
      {
        return atermpp::aterm_appl(afunX);
      }
      else
      {
        pars.stack = pars.stack.tail();
        return atermpp::aterm_appl(afunD,build_tree(pars,i));
      }
    }
    else
    {
      aterm_list l = aterm_cast<aterm_list>(pars.stack.front());
      aterm_appl r ;
      aterm_list readies;

      pars.stack = pars.stack.tail();
      pars.stack.push_front(aterm_list());
      add_to_build_pars(&pars,l,&r,&readies);

      aterm_appl tree;
      if (r==aterm_appl())
      {
        tree = build_tree(pars,i);
        for (; !readies.empty(); readies=readies.tail())
        {
          inc_usedcnt((aterm_list) aterm_cast<aterm_appl>(readies.front())[2]);
          inc_usedcnt((aterm_list) aterm_cast<aterm_appl>(readies.front())[3]);
          tree = atermpp::aterm_appl(afunC,aterm_cast<aterm_appl>(readies.front())[0],atermpp::aterm_appl(afunR,aterm_cast<aterm_appl>(readies.front())[1]),tree);
        }
      }
      else
      {
        inc_usedcnt((aterm_list) r[1]);
        tree = atermpp::aterm_appl(afunR,r[0]);
      }

      return atermpp::aterm_appl(afunN,tree);
    }
  }
}

static aterm_appl create_tree(const data_equation_list& rules, size_t /*opid*/, size_t /*arity*/, const aterm_appl& true_inner)
// Create a match tree for OpId int2term[opid] and update the value of
// *max_vars accordingly.
//
// Pre:  rules is a list of rewrite rules for int2term[opid] in the
//       INNER internal format
//       opid is a valid entry in int2term
//       max_vars is a valid pointer to an integer
// Post: *max_vars is the maximum of the original *max_vars value and
//       the number of variables in the result tree
// Ret:  A match tree for int2term[opid]
{
  // Create sequences representing the trees for each rewrite rule and
  // store the total number of variables used in these sequences.
  // (The total number of variables in all sequences should be an upper
  // bound for the number of variable in the final tree.)
  aterm_list rule_seqs;
  size_t total_rule_vars = 0;
  for (data_equation_list::const_iterator it=rules.begin(); it!=rules.end(); ++it)
  {
    rule_seqs.push_front(create_sequence(*it,&total_rule_vars, true_inner));
  }

  // Generate initial parameters for built_tree
  build_pars init_pars;
  aterm_appl r;
  aterm_list readies;

  initialise_build_pars(&init_pars);
  add_to_build_pars(&init_pars,rule_seqs,&r,&readies);

  aterm_appl tree;
  if (!r.defined())
  {
    treevars_usedcnt=std::vector < size_t> (total_rule_vars);
    tree = build_tree(init_pars,0);
    for (; !readies.empty(); readies=readies.tail())
    {
      tree = atermpp::aterm_appl(afunC,aterm_cast<aterm_appl>(readies.front())[0],atermpp::aterm_appl(afunR,aterm_cast<aterm_appl>(readies.front())[1]), tree);
    }
  }
  else
  {
    tree = atermpp::aterm_appl(afunR,r[0]);
  }

  return tree;
}

template < template <class> class Traverser >
struct auxiliary_count_variables_class: public Traverser < auxiliary_count_variables_class < Traverser > >
{
  typedef Traverser< auxiliary_count_variables_class < Traverser > > super;
  using super::enter;
  using super::leave;
  using super::operator();

  std::map <variable,size_t> m_map;

  void operator ()(const variable& v)
  {
    if (m_map.count(v)==0)
    {
      m_map[v]=1;
    }
    else
    {
      m_map[v]=m_map[v]+1;
    }
  }

  std::map <variable,size_t> get_map()
  {
    return m_map;
  }
};

static variable_list get_doubles(const data_expression& t)
{
  typedef std::map <variable,size_t> t_variable_map;
  auxiliary_count_variables_class<data::variable_traverser> acvc;
  acvc(t);
  t_variable_map variable_map=acvc.get_map();
  variable_list result;
  for(t_variable_map::const_iterator i=variable_map.begin();
         i!=variable_map.end(); ++i)
  {
    if (i->second>1)
    {
      result.push_front(i->first);
    }
  }
  return result;
}


/* static variable_list get_vars(const data_expression& t)
{
  std::set < variable > s=find_free_variables(t);
  variable_list result;
  for(std::set < variable >::const_iterator i=s.begin(); i!=s.end(); ++i)
  {
    result.push_front(*i);
  }
  return result;
}


static variable_list get_vars(const aterm_appl& a)
{
  if (is_function_symbol(aterm_cast<aterm_appl>(a)))
  {
    return aterm_list();
  }
  else if (is_variable(a))
  {
    return make_list<variable>(a);
  }
  else if (a.type_is_list())
  {
    assert(0); // Ik verwacht dat dit niet kan bestaan. JFG
    aterm_list l;
    for (aterm_list m=(aterm_list) a; !m.empty(); m=m.tail())
    {
      l = get_vars(m.front())+l;
    }
    return l;
  }
  else     // a.type_is_appl()
  {
    aterm_list l;
    size_t arity = a.size();
    for (size_t i=0; i<arity; ++i)
    {
      l = get_vars(aterm_cast<aterm_appl>(a[i]))+l;
    }
    return l;
  }
} */

static variable_list dep_vars(const data_equation& eqn)
{
  size_t rule_arity = toInner(eqn.lhs(),true).function().arity()-1;
  std::vector < bool > bs(rule_arity);

  aterm_appl lhs_internal = toInner(eqn.lhs(),true); 
  aterm_list vars = make_list<aterm>( get_doubles(eqn.rhs())+ get_vars(eqn.condition())
                               ); // List of variables occurring in each argument of the lhs
                                   // (except the first element which contains variables from the
                                   // condition and variables which occur more than once in the result)

  // Indices of arguments that need to be rewritten
  for (size_t i = 0; i < rule_arity; i++)
  {
    bs[i] = false;
  }

  // Check all arguments
  for (size_t i = 0; i < rule_arity; i++)
  {
    if (!is_variable(aterm_cast<aterm_appl>(lhs_internal[i+1])))
    {
      // Argument is not a variable, so it needs to be rewritten
      bs[i] = true;
      aterm_list evars = get_vars(aterm_cast<data_expression>(lhs_internal[i+1]));
      for (; !evars.empty(); evars=evars.tail())
      {
        int j=i-1; // vars.tail().size()-1
        for (aterm_list o=vars.tail(); !o.empty(); o=o.tail())
        {
          const aterm_list l=aterm_cast<aterm_list>(o.front());
          if (std::find(l.begin(),l.end(),evars.front()) != l.end())
          {
            bs[j] = true;
          }
          --j;
        }
      }
    }
    else
    {
      // Argument is a variable; check whether it occurred before
      int j = i-1; // vars.size()-1-1
      bool b = false;
      for (aterm_list o=vars; !o.empty(); o=o.tail())
      {
        const aterm_list l=aterm_cast<aterm_list>(o.front());
        if (std::find(o.begin(),o.end(),lhs_internal[i+1]) != o.end())
        {
          // Same variable, mark it
          if (j >= 0)
          {
            bs[j] = true;
          }
          b = true;
        }
        --j;
      }
      if (b)
      {
        // Found same variable(s), so mark this one as well
        bs[i] = true;
      }
    }
    // Add vars used in expression
    vars.push_front(get_vars(aterm_cast<data_expression>(lhs_internal[i+1])));
  }

  variable_list deps;
  for (size_t i = 0; i < rule_arity; i++)
  {
    if (bs[i] && is_variable(aterm_cast<data_expression>(lhs_internal[i+1])))
    {
      deps.push_front(aterm_cast<variable>(lhs_internal[i+1]));
    }
  }

  return deps;
}

// This function assigns a unique index to variable v and stores
// v at this position in the vector rewriter_bound_variables. This is
// used in the compiling rewriter to obtain this variable again.
// Note that the static variable variable_indices is not cleared
// during several runs, as generally the variables bound in rewrite
// rules do not change.
size_t RewriterCompilingJitty::bound_variable_index(const variable &v)
{
  if (variable_indices0.count(v)>0)
  {
    return variable_indices0[v];
  }
  const size_t index_for_v=rewriter_bound_variables.size();
  variable_indices0[v]=index_for_v;
  rewriter_bound_variables.push_back(v);
  return index_for_v;
}

// This function assigns a unique index to variable list vl and stores
// vl at this position in the vector rewriter_binding_variable_lists. This is
// used in the compiling rewriter to obtain this variable again.
// Note that the static variable variable_indices is not cleared
// during several runs, as generally the variables bound in rewrite
// rules do not change.
size_t RewriterCompilingJitty::binding_variable_list_index(const variable_list &vl)
{
  if (variable_list_indices1.count(vl)>0)
  {
    return variable_list_indices1[vl];
  }
  const size_t index_for_vl=rewriter_binding_variable_lists.size();
  variable_list_indices1[vl]=index_for_vl;
  rewriter_binding_variable_lists.push_back(vl);
  return index_for_vl;
}

static aterm_list create_strategy(
        const data_equation_list &rules,
        const size_t opid,
        const size_t arity,
        nfs_array &nfs,
        const data_expression& true_inner)
{
  aterm_list strat;
  // Array to keep note of the used parameters
  std::vector <bool> used;
  for (size_t i = 0; i < arity; i++)
  {
    used.push_back(nfs.get(i));
  }

  // Maintain dependency count (i.e. the number of rules that depend on a given argument)
  std::vector<int> args(arity,-1);
  // Process all (applicable) rules
  std::vector<bool> bs(arity);
  aterm_list dep_list;
  for (data_equation_list::const_iterator it=rules.begin(); it!=rules.end(); ++it)
  {
    size_t rule_arity = (toInner(it->lhs(),true).function().arity())-1;
    if (rule_arity > arity)
    {
      continue;
    }

    data_expression lhs_internal = toInner(it->lhs(),true);  
    aterm_list vars = make_list<aterm>( get_doubles(it->rhs())+ get_vars(it->condition())
                                 ); // List of variables occurring in each argument of the lhs
                                     // (except the first element which contains variables from the
                                     // condition and variables which occur more than once in the result)

    // Indices of arguments that need to be rewritten
    for (size_t i = 0; i < rule_arity; i++)
    {
      bs[i] = false;
    }

    // Check all arguments
    for (size_t i = 0; i < rule_arity; i++)
    {
      if (!is_variable(aterm_cast<data_expression>(lhs_internal[i+1])))
      {
        // Argument is not a variable, so it needs to be rewritten
        bs[i] = true;
        aterm_list evars = get_vars(aterm_cast<data_expression>(lhs_internal[i+1]));
        for (; !evars.empty(); evars=evars.tail())
        {
          int j=i-1;
          for (aterm_list o=vars; !o.tail().empty(); o=o.tail())
          {
            const aterm_list l=aterm_cast<aterm_list>(o.front());
            if (std::find(l.begin(),l.end(),evars.front()) != l.end())
            {
              bs[j] = true;
            }
            --j;
          }
        }
      }
      else
      {
        // Argument is a variable; check whether it occurred before
        int j = i-1; // vars.size()-1-1
        bool b = false;
        for (aterm_list o=vars; !o.empty(); o=o.tail())
        {
          const aterm_list l=aterm_cast<aterm_list>(o.front());
          if (std::find(l.begin(),l.end(),lhs_internal[i+1]) != l.end())
          {
            // Same variable, mark it
            if (j >= 0)
            {
              bs[j] = true;
            }
            b = true;
          }
          --j;
        }
        if (b)
        {
          // Found same variable(s), so mark this one as well
          bs[i] = true;
        }
      }
      // Add vars used in expression
      vars.push_front(get_vars(aterm_cast<data_expression>(lhs_internal[i+1])));
    }

    // Create dependency list for this rule
    aterm_list deps;
    for (size_t i = 0; i < rule_arity; i++)
    {
      // Only if needed and not already rewritten
      if (bs[i] && !used[i])
      {
        deps.push_front(aterm_int(i));
        // Increase dependency count
        args[i] += 1;
        //fprintf(stderr,"dep of arg %i\n",i);
      }
    }
    deps = reverse(deps);

    // Add rule with its dependencies
    dep_list.push_front(make_list<aterm>( deps, (aterm_appl)*it));
  }

  // Process all rules with their dependencies
  while (1)
  {
    // First collect rules without dependencies to the strategy
    data_equation_list no_deps;
    aterm_list has_deps;
    for (; !dep_list.empty(); dep_list=dep_list.tail())
    {
      if (aterm_cast<aterm_list>(aterm_cast<aterm_list>(dep_list.front()).front()).empty())
      {
        no_deps.push_front(data_equation(aterm_cast<aterm_list>(dep_list.front()).tail().front()));
      }
      else
      {
        has_deps.push_front(dep_list.front());
      }
    }
    dep_list = reverse(has_deps);

    // Create and add tree of collected rules
    if (!no_deps.empty())
    {
      strat.push_front(create_tree(no_deps,opid,arity,true_inner));
    }

    // Stop if there are no more rules left
    if (dep_list.empty())
    {
      break;
    }

    // Otherwise, figure out which argument is most useful to rewrite
    int max = -1;
    int maxidx = -1;
    for (size_t i = 0; i < arity; i++)
    {
      if (args[i] > max)
      {
        maxidx = i;
        max = args[i];
      }
    }

    // If there is a maximum (which should always be the case), add it to the strategy and remove it from the dependency lists
    assert(maxidx >= 0);
    if (maxidx >= 0)
    {
      args[maxidx] = -1;
      used[maxidx] = true;
      aterm_int rewr_arg = aterm_int(maxidx);

      strat.push_front(rewr_arg);

      aterm_list l;
      for (; !dep_list.empty(); dep_list=dep_list.tail())
      {
        aterm_list temp= aterm_cast<aterm_list>(dep_list.front()).tail();
        temp.push_front(remove_one_element<aterm>(aterm_cast<aterm_list>(aterm_cast<aterm_list>(dep_list.front()).front()), rewr_arg));
        l.push_front(temp);
      }
      dep_list = reverse(l);
    }
  }

  return reverse(strat);
}

void RewriterCompilingJitty::add_base_nfs(nfs_array &nfs, const function_symbol& opid, size_t arity)
{
  for (size_t i=0; i<arity; i++)
  {
    if (always_rewrite_argument(opid,arity,i))
    {
      nfs.set(i);
    }
  }
}

void RewriterCompilingJitty::extend_nfs(nfs_array &nfs, const function_symbol& opid, size_t arity)
{
  data_equation_list eqns = (size_t(OpId2Int(opid).value())<jittyc_eqns.size()
                                ?jittyc_eqns[OpId2Int(opid).value()]:data_equation_list());
  if (eqns.empty())
  {
    nfs.fill(arity);
    return;
  }
  aterm_list strat = create_strategy(eqns,OpId2Int(opid).value(),arity,nfs,true_inner);
  while (!strat.empty() && strat.front().type_is_int())
  {
    nfs.set(aterm_cast<aterm_int>(strat.front()).value());
    strat = strat.tail();
  }
}

// Determine whether the opid is a normal form, with the given number of arguments.
bool RewriterCompilingJitty::opid_is_nf(const function_symbol& opid, size_t num_args)
{
  // First check whether the opid is a forall or an exists with one argument.
  // Then the routines for exists/forall quantifier enumeration must be applied.
  /* if (num_args==1 &&
        (get_int2term(opid.value()).name() == exists_function_symbol() ||
         get_int2term(opid.value()).name() == forall_function_symbol()))
  {
    return false;
  } */

  // Otherwise check whether there are applicable rewrite rules.
  data_equation_list l = (size_t(OpId2Int(opid).value())<jittyc_eqns.size()
                                        ?jittyc_eqns[OpId2Int(opid).value()]:data_equation_list());

  if (l.empty())
  {
    return true;
  }

  for (data_equation_list::const_iterator it=l.begin(); it!=l.end(); ++it)
  {
    if (toInner(it->lhs(),true).function().arity()-1 <= num_args)
    {
      return false;
    }
  }

  return true;
}

void RewriterCompilingJitty::calc_nfs_list(nfs_array &nfs, size_t arity, data_expression_list args, int startarg, aterm_list nnfvars)
{
  if (args.empty())
  {
    return;
  }

  nfs.set(arity-args.size(),calc_nfs(static_cast<data_expression>(args.front()),startarg,nnfvars));
  calc_nfs_list(nfs,arity,args.tail(),startarg+1,nnfvars);
}

bool RewriterCompilingJitty::calc_nfs(const data_expression& t, int startarg, aterm_list nnfvars)
{
  if (is_function_symbol(t))
  {
    return opid_is_nf(aterm_cast<function_symbol>(t),0);
  }
  else if (is_nil((aterm_appl) t))
  {
    assert(0);
    // assert(startarg>=0); This value may be negative.
    return (nnfvars==aterm_list(aterm())) || (std::find(nnfvars.begin(),nnfvars.end(), aterm_int(startarg)) == nnfvars.end());
  }
  else if (is_variable((aterm_appl) t))
  {
    return (nnfvars==aterm_list(aterm())) || (std::find(nnfvars.begin(),nnfvars.end(),t) == nnfvars.end());
  }
  else if (is_abstraction(atermpp::aterm_appl(t)))
  {
    return false; // I assume that lambda, forall and exists are not in normal form by default.
                  // This might be too weak, and may require to be reinvestigated later.
  }
  else if (is_where_clause(atermpp::aterm_appl(t)))
  {
    return false; // I assume that a where clause is not in normal form by default.
                  // This might be too weak, and may require to be reinvestigated later.
  }
  
  // t has the shape #REWR#(head,t1,...,tn)
  const application& ta(t);
  int arity = ta.size();
  const data_expression& head=ta.head();
  if (is_function_symbol(head))    // XXXXXX This function symbol can also be burried deeper in the term
                                   // for higher order functions.
  {
    if (opid_is_nf(aterm_cast<function_symbol>(head),arity) && arity != 0)
    {
      nfs_array args(arity);
      calc_nfs_list(args,arity,ta.arguments(),startarg,nnfvars);
      bool b = args.is_filled(arity);
      return b;
    }
    else
    {
      return false;
    }
  }
  else
  {
    if (arity == 0)
    {
      assert(false);
      return calc_nfs(head, startarg, nnfvars);
    }
    return false;
  }
}

string RewriterCompilingJitty::calc_inner_terms(nfs_array &nfs, size_t arity, data_expression_list args, int startarg, aterm_list nnfvars, nfs_array *rewr)
{
  if (args.empty())
  {
    return "";
  }

  pair<bool,string> head = calc_inner_term(args.front(),
                       startarg,nnfvars,rewr?(rewr->get(arity-args.size())):false,arity);
  nfs.set(arity-args.size(),head.first);
  string tail = calc_inner_terms(nfs,arity,args.tail(),startarg+1,nnfvars,rewr);
  return head.second+(args.tail().empty()?"":",")+tail;
}

static string calc_inner_appl_head(size_t arity)
{
  stringstream ss;
  if (arity == 1)
  {
    ss << "makeAppl1";  // This is to avoid confusion with atermpp::aterm_appl on a function symbol and two iterators.
  }
  else if (arity == 2)
  {
    ss << "makeAppl2";  // This is to avoid confusion with atermpp::aterm_appl on a function symbol and two iterators.
  }
  else if (arity <= 5)
  {
    ss << "atermpp::aterm_appl";
  }
  else
  {
    ss << "make_term_with_many_arguments";
  }
  ss << "(get_appl_afun_value_no_check(" << arity+1 << "),";    // YYYY
  extend_appl_afun_values(arity+1);
  return ss.str();
}

// if total_arity<=5 a term of type atermpp::aterm is generated, otherwise a term of type aterm_appl is generated.
pair<bool,string> RewriterCompilingJitty::calc_inner_term(
                  const data_expression& t,
                  int startarg,
                  aterm_list nnfvars,
                  const bool rewr,
                  const size_t total_arity)
{
  if (is_function_symbol(t))
  {
    stringstream ss;
    bool b = opid_is_nf(aterm_cast<function_symbol>(t),0);
    /* if (total_arity>5)
    {
      ss << "(aterm_appl)";
    } */
    if (rewr && !b)
    {
      ss << "rewr_" << static_cast<aterm_int>(t).value() << "_0_0()";
    }
    else
    {
      // set_rewrappl_value(OpId2Int(static_cast<function_symbol>(t)).value());
      ss << "atermpp::aterm_cast<const atermpp::aterm_appl>(aterm(reinterpret_cast<const atermpp::detail::_aterm*>(" <<
                    atermpp::detail::address(mcrl2::data::detail::get_int2term(OpId2Int(aterm_cast<function_symbol>(t)).value())) << ")))";
                    // atermpp::detail::address(mcrl2::data::detail::get_rewrappl_value_without_check(OpId2Int(aterm_cast<function_symbol>(t)).value())) << ")))";
    }
    return pair<bool,string>(
             rewr || b,
             ss.str()
           );

  }
  else if (gsIsNil((aterm_appl) t))
  {
    assert(0);
    stringstream ss;
    assert(nnfvars!=aterm_list(aterm()));
    // assert(startarg>=0); This may be negative.
    bool b = (std::find(nnfvars.begin(),nnfvars.end(), aterm_int(startarg)) != nnfvars.end());
    if (rewr && b)
    {
      ss << "rewrite(arg_not_nf" << startarg << ")";
    }
    else if (b)
    {
      ss << "arg_not_nf" << startarg;
    }
    else
    {
      ss << "arg" << startarg;
    }
    return pair<bool,string>(rewr || !b, ss.str());

  }
  else if (is_variable((aterm_appl)t))
  {
    assert(t.type_is_appl());
    stringstream ss;
    /* if (total_arity>5)
    {
      ss << "(aterm_appl)";
    } */
    const bool b = (nnfvars!=aterm_list(aterm())) && (std::find(nnfvars.begin(),nnfvars.end(),t) != nnfvars.end());
    const variable v=variable((aterm_appl)t);
    string variable_name=v.name();
    // Remove the initial @ if it is present in the variable name, because then it is an variable introduced
    // by this rewriter.
    if (variable_name[0]=='@')
    {
      if (rewr && b)
      {
        ss << "rewrite(" << variable_name.substr(1) << ")";
      }
      else
      {
        ss << variable_name.substr(1);
      }
    }
    else
    {
      ss << "this_rewriter->bound_variable_get(" << bound_variable_index(v) << ")";
    }
    return pair<bool,string>(rewr || !b, ss.str());
  }
  else if (is_abstraction(t))
  {
    const abstraction& ta(t);
    stringstream ss;
    if (is_lambda(ta.binding_operator()))
    {
      if (rewr)
      {
        // The resulting term must be rewritten.
        pair<bool,string> r=calc_inner_term(ta.body(),startarg,nnfvars,true,total_arity);
        ss << "this_rewriter->rewrite_single_lambda(" <<
               "this_rewriter->binding_variable_list_get(" << binding_variable_list_index(ta.variables()) << ")," <<
               r.second << "," << r.first << ",*(this_rewriter->global_sigma))";
        return pair<bool,string>(true,ss.str());
      }
      else
      {
        pair<bool,string> r=calc_inner_term(ta.body(),startarg,nnfvars,false,total_arity);
        ss << "mcrl2::core::detail::gsMakeBinder(mcrl2::core::detail::gsMakeLambda()," <<
               "this_rewriter->binding_variable_list_get(" << binding_variable_list_index(ta.variables()) << ")," <<
               r.second << ")";
        return pair<bool,string>(false,ss.str());
      }
    }
    else if (is_forall(ta.binding_operator()))
    {
      if (rewr)
      {
        // A result in normal form is requested.
        pair<bool,string> r=calc_inner_term(ta.body(),startarg,nnfvars,false,total_arity);
        ss << "this_rewriter->internal_universal_quantifier_enumeration(" <<
               "this_rewriter->binding_variable_list_get(" << binding_variable_list_index(ta.variables()) << ")," <<
               r.second << "," << r.first << "," << "*(this_rewriter->global_sigma))";
        return pair<bool,string>(true,ss.str());
      }
      else
      {
        // A result which is not a normal form is requested.
        pair<bool,string> r=calc_inner_term(ta.body(),startarg,nnfvars,false,total_arity);
        ss << "mcrl2::core::detail::gsMakeBinder(mcrl2::core::detail::gsMakeForall()," <<
               "this_rewriter->binding_variable_list_get(" << binding_variable_list_index(ta.variables()) << ")," <<
               r.second << ")";
        return pair<bool,string>(false,ss.str());
      }
    }
    else if (is_exists(ta.binding_operator()))
    {
      if (rewr)
      {
        // A result in normal form is requested.
        pair<bool,string> r=calc_inner_term(ta.body(),startarg,nnfvars,false,total_arity);
        ss << "this_rewriter->internal_existential_quantifier_enumeration(" <<
               "this_rewriter->binding_variable_list_get(" << binding_variable_list_index(ta.variables()) << ")," <<
               r.second << "," << r.first << "," << "*(this_rewriter->global_sigma))";
        return pair<bool,string>(true,ss.str());
      }
      else
      {
        // A result which is not a normal form is requested.
        pair<bool,string> r=calc_inner_term(ta.body(),startarg,nnfvars,false,total_arity);
        ss << "mcrl2::core::detail::gsMakeBinder(mcrl2::core::detail::gsMakeExists()," <<
               "this_rewriter->binding_variable_list_get(" << binding_variable_list_index(ta.variables()) << ")," <<
               r.second << ")";
        return pair<bool,string>(false,ss.str());
      }
    }
  }
  else if (is_where_clause(t))
  {

    stringstream ss;
    const where_clause& w(t); // w is the where clause

    if (rewr)
    {
      // A rewritten result is expected.
      pair<bool,string> r=calc_inner_term(w.body(),startarg,nnfvars,true,total_arity);

      ss << "this_rewriter->rewrite_where(mcrl2::data::where_clause(" << r.second << ",";

      const assignment_list& assignments(w.assignments());
      for( size_t no_opening_brackets=assignments.size(); no_opening_brackets>0 ; no_opening_brackets--)
      {
        ss << "jittyc_local_push_front(";
      }
      ss << "aterm_list()";
      for(assignment_list::const_iterator i=assignments.begin() ; i!=assignments.end(); ++i)
      {
        pair<bool,string> r=calc_inner_term(i->rhs(),startarg,nnfvars,true,total_arity);
        ss << ",mcrl2::data::assignment(" <<
                 "this_rewriter->bound_variable_get(" << bound_variable_index(i->lhs()) << ")," <<
                 r.second << "))";
      }

      ss << "),*(this_rewriter->global_sigma))";

      return pair<bool,string>(true,ss.str());
    }
    else
    {
      // The result does not need to be rewritten.
      pair<bool,string> r=calc_inner_term(w.body(),startarg,nnfvars,false,total_arity);
      ss << "mcrl2::data::where_clause(" << r.second << ",";

      const assignment_list& assignments(w.assignments());
      for( size_t no_opening_brackets=assignments.size(); no_opening_brackets>0 ; no_opening_brackets--)
      {
        ss << "jittyc_local_push_front(";
      }
      ss << "aterm_list()";
      for(assignment_list::const_iterator i=assignments.begin() ; i!=assignments.end(); ++i)
      {
        pair<bool,string> r=calc_inner_term(i->rhs(),startarg,nnfvars,true,total_arity);
        ss << ",mcrl2::data::assignment(" <<
                 "this_rewriter->bound_variable_get(" << bound_variable_index(i->lhs()) << ")," <<
                 r.second << "))";
      }

      ss << ")";

      return pair<bool,string>(false,ss.str());
    }
  }

  // t has the shape #REWR#(head,t1,...,tn)
  const application& ta(t);
  stringstream ss;
  bool b;
  size_t arity = ta.size();
  const data_expression& head=ta.head();


  if (is_function_symbol(head))  // Most likely one wants to know also whether the nested symbol on top is
                                 // a function symbol. So this needs to be done recursively. XXXXXXXXXXXX
  {
    const function_symbol headfs(head);
    b = opid_is_nf(headfs,arity);

    if (b || !rewr)
    {
      /* if (total_arity>5)
      {
        ss << "(aterm_appl)";
      } */
      ss << calc_inner_appl_head(arity);
    }

    if (arity == 0)
    {
      if (b || !rewr)
      {
        ss << "atermpp::aterm((const atermpp::detail::_aterm*) " << (void*) atermpp::detail::address(headfs) << "))";
      }
      else
      {
        ss << "rewr_" << OpId2Int(headfs).value() << "_0_0()";
      }
    }
    else
    {
      // arity != 0
      nfs_array args_nfs(arity);
      calc_nfs_list(args_nfs,arity,ta.arguments(),startarg,nnfvars);
      if (!(b || !rewr))
      {
        /* if (arity<=5)
        {
          ss << "/ *" << arity <<  "* / rewr_";
        }
        else */
        {
          ss << "(aterm_appl)rewr_";
        }
        add_base_nfs(args_nfs,headfs,arity);
        extend_nfs(args_nfs,headfs,arity);
      }
      if (arity > NF_MAX_ARITY)
      {
        args_nfs.clear(arity);
      }
      if (args_nfs.is_clear(arity) || b || rewr || (arity > NF_MAX_ARITY))
      {
        if (b || !rewr)
        {
          /* if (arity<=5)
          { */
            ss << "atermpp::aterm((const atermpp::detail::_aterm*) " << (void*) atermpp::detail::address(headfs) << ")";
          /* }
          else
          {
            ss << "(aterm_appl)(atermpp::detail::_aterm*) " << (void*) get_int2aterm_value(headfs);
          } */
        }
        else
          ss << OpId2Int(headfs).value();
      }
      else
      {
        if (b || !rewr)
        {
          const size_t index=OpId2Int(headfs).value();
          const data::function_symbol old_head=get_int2term(index);
          std::stringstream new_name;
          new_name << "@_rewr" << "_" << index << "_" << ta.size() << "_" << args_nfs.get_value(arity)
                               << "_term";
          const data::function_symbol f(new_name.str(),old_head.sort());
          if (partially_rewritten_functions.count(f)==0)
          {
            OpId2Int(f);
            partially_rewritten_functions.insert(f);
          }
          /* if (arity<=5)
          { */
            /* Exclude adding an extra increase of the OpId index, which refers to an OpId that is not available.
               The intention of this increase is to generate an index of an OpId of which it is indicated in args_nfs
               which arguments are guaranteed to be in normal form. For these variables it is not necessary anymore
               to recalculate the normal form. TODO: this needs to be reconstructed. */
            /* ss << "atermpp::aterm( " << (void*) get_int2aterm_value(((aterm_int) ((aterm_list) t).front()).value()
                             + ((1 << arity)-arity-1)+args_nfs.get_value(arity) ) << ")"; */
            ss << "atermpp::aterm((const atermpp::detail::_aterm*) " << (void*) atermpp::detail::address(f) << ")";
          /* }
          else
          {
            / * See the remark above. * /
            / * ss << "(aterm_appl) " << (void*) get_int2aterm_value(((aterm_int) ((aterm_list) t).front()).value()
                               + ((1 << arity)-arity-1)+args_nfs.get_value(arity) ) << ""; * /
            ss << "(aterm_appl)(atermpp::detail::_aterm*) " << (void*) get_int2aterm_value(OpId2Int(f)).value()) << "";
          } */

        }
        else
        {
          // QUE?! Dit stond er vroeger // Sjoerd
          //   ss << (((aterm_int) ((aterm_list) t).front()).value()+((1 << arity)-arity-1)+args_nfs);
          ss << (OpId2Int(headfs).value()+((1 << arity)-arity-1)+args_nfs.getraw(0));
        }
      }
      nfs_array args_first(arity);
      if (rewr && b)
      {
        args_nfs.fill(arity);
      }
      string args_second = calc_inner_terms(args_first,arity,ta.arguments(),startarg,nnfvars,&args_nfs);
      // The assert is not valid anymore, bcause lambda are sometimes returned in normal form, although not strictly
      // asked for...
      assert(!rewr || b || (arity > NF_MAX_ARITY) || args_first.equals(args_nfs,arity));
      if (rewr && !b)
      {
        ss << "_" << arity << "_";
        if (arity <= NF_MAX_ARITY)
        {
          ss << args_first.get_value(arity);
        }
        else
        {
          ss << "0";
        }
        ss << "(";
      }
      else
      {
        ss << ",";
      }
      ss << args_second << ")";
      if (!args_first.is_filled(arity))
      {
        b = false;
      }
    }
    b = b || rewr;

  }
  else
  {
    // ((aterm_list) t).front().type()!=AT_INT). So the first element of this list is
    // either a variable, or a lambda term. It cannot be a forall or exists, because in
    // that case there would be a type error.
    assert(arity > 0);

    if (is_abstraction(ta.head()))
    {
      const abstraction& ta1(ta.head());
      assert(is_lambda(ta1.binding_operator()));

      b = rewr;
      nfs_array args_nfs(arity);
      calc_nfs_list(args_nfs,arity,ta.arguments(),startarg,nnfvars);
      if (arity > NF_MAX_ARITY)
      {
        args_nfs.clear(arity);
      }

      nfs_array args_first(arity);
      if (rewr && b)
      {
        args_nfs.fill(arity);
      }

      pair<bool,string> head = calc_inner_term(ta1,startarg,nnfvars,false,arity);

      if (rewr)
      {
        ss << "rewrite(";
      }

      ss << calc_inner_appl_head(arity) << head.second << "," <<
               calc_inner_terms(args_first,arity,ta.arguments(),startarg,nnfvars,&args_nfs) << ")";
      if (rewr)
      {
        ss << ")";
      }

    }
    else
    {
      // So, the first element of t must be a single variable.
      assert(is_variable(ta.head()));
      b = rewr;
      pair<bool,string> head = calc_inner_term(ta.head(),startarg,nnfvars,false,arity);
      nfs_array tail_first(arity);
      string tail_second = calc_inner_terms(tail_first,arity,ta.arguments(),startarg,nnfvars,NULL);
      /* if (arity>5)
      {
        ss << "(aterm_appl)";
      } */
      ss << "isAppl(" << head.second << ")?";
      if (rewr)
      {
          ss << "rewrite(";
      }
      ss <<"build" << arity << "(" << head.second << "," << tail_second << ")";
      if (rewr)
      {
        ss << ")";
      }
      ss << ":";
      bool c = rewr;
      // assert(startarg>=0); For unclear reasons, this may be negative.
      if (rewr && (nnfvars!=aterm_list(aterm())) && (std::find(nnfvars.begin(),nnfvars.end(), aterm_int(startarg)) != nnfvars.end()))
      {
        ss << "rewrite(";
        c = false;
      }
      else
      {
        ss << "atermpp::aterm_appl(";
      }
      ss << calc_inner_appl_head(arity) << " " << head.second << ",";
      if (c)
      {
        tail_first.clear(arity);
        nfs_array rewrall(arity);
        rewrall.fill(arity);
        tail_second = calc_inner_terms(tail_first,arity,ta.arguments(),startarg,nnfvars,&rewrall);
      }
      ss << tail_second << ")";
      {
        ss << ")";
      }
    }
  }

  return pair<bool,string>(b,ss.str());
}

void RewriterCompilingJitty::calcTerm(FILE* f, const data_expression& t, int startarg, aterm_list nnfvars, bool rewr)
{
  pair<bool,string> p = calc_inner_term(t,startarg,nnfvars,rewr,0);
  fprintf(f,"%s",p.second.c_str());
  return;
}

static data_expression add_args(const data_expression& a, size_t num)
{
  if (num == 0)
  {
    return a;
  }
  else
  {
    // For the time being we add nothing. 

    return a;

    // Code below requires to be revised, but it is unclear how.... JFG
    assert(0);
    /* aterm_list l;

    if (a.type_is_list())
    {
      l = (aterm_list) a;
    }
    else
    {
      l = make_list<aterm>(a);
    }

    while (num > 0)
    {
      l = l+make_list<aterm>(gsMakeNil());
      num--;
    }
    return  l; */
  }
}

static int get_startarg(const aterm &a, int n)
{
  if (a.type_is_list())
  {
    return n-((aterm_list) a).size()+1;
  }
  else
  {
    return n;
  }
}


static int* i_t_st = NULL;
static int i_t_st_s = 0;
static int i_t_st_p = 0;
static void reset_st()
{
  i_t_st_p = 0;
}
static void push_st(int i)
{
  if (i_t_st_s <= i_t_st_p)
  {
    if (i_t_st_s == 0)
    {
      i_t_st_s = 16;
    }
    else
    {
      i_t_st_s = i_t_st_s*2;
    }
    i_t_st = (int*) realloc(i_t_st,i_t_st_s*sizeof(int));
  }
  i_t_st[i_t_st_p] = i;
  i_t_st_p++;
}
static int pop_st()
{
  if (i_t_st_p == 0)
  {
    return 0;
  }
  else
  {
    i_t_st_p--;
    return i_t_st[i_t_st_p];
  }
}
static int peekn_st(int n)
{
  if (i_t_st_p <= n)
  {
    return 0;
  }
  else
  {
    return i_t_st[i_t_st_p-n-1];
  }
}

// #define IT_DEBUG
#define IT_DEBUG_INLINE
#ifdef IT_DEBUG_INLINE
#define IT_DEBUG_FILE f,"//"
#else
#define IT_DEBUG_FILE stderr,
#endif
void RewriterCompilingJitty::implement_tree_aux(
      FILE* f, 
      aterm_appl tree, 
      size_t cur_arg, 
      size_t parent, 
      size_t level, 
      size_t cnt, 
      size_t d, 
      const size_t arity,
      const std::vector<bool> &used, aterm_list nnfvars)
// Print code representing tree to f.
//
// cur_arg   Indices refering to the variable that contains the current
// parent    term. For level 0 this means arg<cur_arg>, for level 1 it
//           means arg<parent>(<cur_arg) and for higher
//           levels it means t<parent>(<cur_arg>)
//
// parent    Index of cur_arg in the previous level
//
// level     Indicates the how deep we are in the term (e.g. in
//           f(.g(x),y) . indicates level 0 and in f(g(.x),y) level 1
//
// cnt       Counter indicating the number of variables t<i> (0<=i<cnt)
//           used so far (in the current scope)
//
// d         Indicates the current scope depth in the code (i.e. new
//           lines need to use at least 2*d spaces for indent)
//
// arity     Arity of the head symbol of the expression where are
//           matching (for construction of return values)
{
  if (isS(tree))
  {
    if (level == 0)
    {
      if (used[cur_arg])
      {
        fprintf(f,"%sconst atermpp::aterm_appl &%s = arg%lu; // S1\n",whitespace(d*2),
                aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(tree[0])[0]).function().name().c_str()+1,cur_arg);
      }
      else
      {
        fprintf(f,"%sconst atermpp::aterm_appl &%s = arg_not_nf%lu; // S1\n",whitespace(d*2),
                aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(tree[0])[0]).function().name().c_str()+1,cur_arg);
        nnfvars.push_front(tree[0]);
      }
    }
    else
    {
      fprintf(f,"%sconst atermpp::aterm_appl &%s = atermpp::aterm_cast<const atermpp::aterm_appl>(%s%lu[%lu]); // S2\n",
              whitespace(d*2),
              aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(tree[0])[0]).function().name().c_str()+1,
              (level==1)?"arg":"t",
              parent,cur_arg);
    }
    implement_tree_aux(f,aterm_cast<aterm_appl>(tree[1]),cur_arg,parent,level,cnt,d,arity,used,nnfvars);
    return;
  }
  else if (isM(tree))
  {
    if (level == 0)
    {
      fprintf(f,"%sif (%s==arg%lu) // M\n"
              "%s{\n",
              whitespace(d*2),aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(tree[0])[0]).function().name().c_str()+1,cur_arg,
              whitespace(d*2)
             );
    }
    else
    {
      fprintf(f,"%sif (%s==static_cast<atermpp::aterm_appl>(%s%lu[%lu])) // M\n"
              "%s{\n",
              whitespace(d*2),aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(tree[0])[0]).function().name().c_str()+1,(level==1)?"arg":"t",parent,cur_arg,
              whitespace(d*2)
             );
    }
    implement_tree_aux(f,aterm_cast<aterm_appl>(tree[1]),cur_arg,parent,level,cnt,d+1,arity,used,nnfvars);
    fprintf(f,"%s}\n%selse\n%s{\n",whitespace(d*2),whitespace(d*2),whitespace(d*2));
    implement_tree_aux(f,aterm_cast<aterm_appl>(tree[2]),cur_arg,parent,level,cnt,d+1,arity,used,nnfvars);
    fprintf(f,"%s}\n",whitespace(d*2));
    return;
  }
  else if (isF(tree))
  {
    if (level == 0)
    {
      fprintf(f,"%sif (atermpp::detail::address(\n"
              "             (mcrl2::data::is_function_symbol(arg%lu)?arg%lu:arg%lu[0]))==reinterpret_cast<const atermpp::detail::_aterm*>(%p)) // F1\n"
              "%s{\n",
              whitespace(d*2),
              cur_arg,
              cur_arg,
              cur_arg,
              // (void*)atermpp::detail::address(get_int2term(OpId2Int(aterm_cast<function_symbol>(tree[0])).value())),
              (void*)atermpp::detail::address(tree[0]),
              whitespace(d*2)
             );
    }
    else
    {
      fprintf(f,"%sif (isAppl(%s%lu[%lu]) && atermpp::detail::address(aterm_cast<const atermpp::aterm_appl >(%s%lu[%lu])[0])==reinterpret_cast<const atermpp::detail::_aterm*>(%p)) // F2\n"
              "%s{\n"
              "%s  atermpp::aterm_appl t%lu (%s%lu[%lu]);\n",
              whitespace(d*2),
              (level==1)?"arg":"t",parent,cur_arg,
              (level==1)?"arg":"t",parent,cur_arg,
              (void*)atermpp::detail::address(aterm_cast<function_symbol>(tree[0])),
              whitespace(d*2),
              whitespace(d*2),cnt,(level==1)?"arg":"t",parent,cur_arg
             );
    }
    push_st(cur_arg);
    push_st(parent);
    implement_tree_aux(f,aterm_cast<aterm_appl>(tree[1]),1,(level==0)?cur_arg:cnt,level+1,cnt+1,d+1,arity,used,nnfvars);
    pop_st();
    pop_st();
    fprintf(f,"%s}\n%selse\n%s{\n",whitespace(d*2),whitespace(d*2),whitespace(d*2));
    implement_tree_aux(f,aterm_cast<aterm_appl>(tree[2]),cur_arg,parent,level,cnt,d+1,arity,used,nnfvars);
    fprintf(f,"%s}\n",whitespace(d*2));
    return;
  }
  else if (isD(tree))
  {
    int i = pop_st();
    int j = pop_st();
    implement_tree_aux(f,aterm_cast<aterm_appl>(tree[0]),j,i,level-1,cnt,d,arity,used,nnfvars);
    push_st(j);
    push_st(i);
    return;
  }
  else if (isN(tree))
  {
    implement_tree_aux(f,aterm_cast<aterm_appl>(tree[0]),cur_arg+1,parent,level,cnt,d,arity,used,nnfvars);
    return;
  }
  else if (isC(tree))
  {
    fprintf(f,"%sif (",whitespace(d*2));
    calcTerm(f,aterm_cast<data_expression>(tree[0]),0,nnfvars);

    fprintf(f,"==atermpp::aterm_appl((const atermpp::detail::_aterm*) %p)) // C\n"
            "%s{\n",
            (void*)atermpp::detail::address(get_int2term(OpId2Int(true_inner).value())),
            whitespace(d*2)
           );

    implement_tree_aux(f,aterm_cast<aterm_appl>(tree[1]),cur_arg,parent,level,cnt,d+1,arity,used,nnfvars);
    fprintf(f,"%s}\n%selse\n%s{\n",whitespace(d*2),whitespace(d*2),whitespace(d*2));
    implement_tree_aux(f,aterm_cast<aterm_appl>(tree[2]),cur_arg,parent,level,cnt,d+1,arity,used,nnfvars);
    fprintf(f,"%s}\n",whitespace(d*2));
    return;
  }
  else if (isR(tree))
  {
    fprintf(f,"%sreturn ",whitespace(d*2));
    if (level > 0)
    {
      //cur_arg = peekn_st(level);
      cur_arg = peekn_st(2*level-1);
    }
    calcTerm(f,add_args(aterm_cast<data_expression>(tree[0]),arity-cur_arg-1),get_startarg(tree[0],cur_arg+1),nnfvars);
    fprintf(f,"; // R1\n");
    return;
  }
  else
  {
    return;
  }
}

void RewriterCompilingJitty::implement_tree(
            FILE* f,
            aterm_appl tree,
            const size_t arity,
            size_t d,
            size_t /* opid */,
            const std::vector<bool> &used)
{
  size_t l = 0;

  aterm_list nnfvars;
  for (size_t i=0; i<arity; i++)
  {
    if (!used[i])
    {
      nnfvars.push_front(aterm_int(i));
    }
  }

  while (isC(tree))
  {
    fprintf(f,"%sif (",whitespace(d*2));
    calcTerm(f,aterm_cast<data_expression>(tree[0]),0,aterm_list());

    fprintf(f,"==atermpp::aterm_appl((const atermpp::detail::_aterm*) %p)) // C\n"
            "%s{\n"
            "%sreturn ",
            (void*)atermpp::detail::address(get_int2term(OpId2Int(true_inner).value())),
            whitespace(d*2),
            whitespace(d*2)
           );

    assert(isR(aterm_cast<aterm_appl>(tree[1])));
    calcTerm(f,add_args(aterm_cast<data_expression>((aterm_cast<aterm_appl>(tree[1]))[0]),arity),
                 get_startarg(aterm_cast<aterm_appl>(tree[1])[0],0),nnfvars);
    fprintf(f,";\n"
            "%s}\n%selse\n%s{\n", whitespace(d*2),whitespace(d*2),whitespace(d*2)
           );
    tree = aterm_cast<aterm_appl>(tree[2]);
    d++;
    l++;
  }
  if (isR(tree))
  {
    if (arity==0)
    { // return a reference to an aterm_appl
      fprintf(f,"%sstatic aterm_appl static_term(rewrite(",whitespace(d*2));
      calcTerm(f,add_args(aterm_cast<data_expression>(tree[0]),arity),get_startarg(tree[0],0),nnfvars);
      fprintf(f,")); \n");
      fprintf(f,"%sreturn static_term",whitespace(d*2));
      fprintf(f,"; // R2a\n");
    }
    else
    { // arity>0
      fprintf(f,"%sreturn ",whitespace(d*2));
      calcTerm(f,add_args(aterm_cast<data_expression>(tree[0]),arity),get_startarg(tree[0],0),nnfvars);
      fprintf(f,"; // R2b\n");
    }
  }
  else
  {
    reset_st();
    implement_tree_aux(f,tree,0,0,0,0,d,arity,used,nnfvars);
  }
  while (l > 0)
  {
    --d;
    fprintf(f,"%s}\n",whitespace(d*2));
    --l;
  }
}

static void finish_function(FILE* f, size_t arity, size_t opid, const std::vector<bool> &used)
{
  if (arity == 0)
  {
    // set_rewrappl_value(opid);
    // fprintf(f,  "  return mcrl2::data::detail::get_rewrappl_value_without_check(%lu", opid);
    fprintf(f,  "  return mcrl2::data::detail::get_int2term(%lu", opid);
  }
  else
  {
    extend_appl_afun_values(arity+1);
    if (arity > 5)
    {
      fprintf(f,  "  return make_term_with_many_arguments(get_appl_afun_value_no_check(%lu),"
              "(const atermpp::detail::_aterm*)%p",
              arity+1,
              // (void*)get_int2aterm_value(opid)
              (void*)atermpp::detail::address(get_int2term(opid))
             );
    }
    else
    {
      fprintf(f,  "  return atermpp::aterm_appl(get_appl_afun_value_no_check(%lu),"
              "(const atermpp::detail::_aterm*) %p",
              arity+1,
              (void*)atermpp::detail::address(get_int2term(opid))
             );
    }
  }
  for (size_t i=0; i<arity; i++)
  {
    if (used[i])
    {
      fprintf(f,                 ", arg%zu",i);
    }
    else
    {
      fprintf(f,                 ", rewrite(arg_not_nf%zu)",i);
    }
  }
  fprintf(f,                 ");\n");
}

void RewriterCompilingJitty::implement_strategy(FILE* f, aterm_list strat, size_t arity, size_t d, size_t opid, size_t nf_args)
{
  std::vector<bool> used;
  for (size_t i=0; i<arity; i++)
  {
    used.push_back((nf_args & (1 << i)) != 0);
  }
  while (!strat.empty())
  {
    if (strat.front().type_is_int())
    {
      size_t arg = static_cast<aterm_int>(strat.front()).value();

      if (!used[arg])
      {
        fprintf(f,"%sconst atermpp::aterm_appl arg%lu = rewrite(arg_not_nf%lu);\n",whitespace(2*d),arg,arg);

        used[arg] = true;
      }
      fprintf(f,"// Considering argument  %ld\n",arg);
    }
    else
    {
      fprintf(f,"%s{\n",whitespace(2*d));
      implement_tree(f,(aterm_appl) strat.front(),arity,d+1,opid,used);
      fprintf(f,"%s}\n",whitespace(2*d));
    }

    strat = strat.tail();
  }

  finish_function(f,arity,opid,used);
}

aterm_appl RewriterCompilingJitty::build_ar_expr_internal(const aterm_appl& expr, const variable& var)
{
  if (is_function_symbol(expr))
  {
    return make_ar_false();
  }

  if (is_variable(expr))
  {
    if (expr==var)
    {
      return make_ar_true();
    }
    else
    {
      return make_ar_false();
    }
  }

  if (is_where_clause(expr) || is_abstraction(expr))
  {
    return make_ar_false();
  }

  // expr has shape #REWR#(t,t1,...,tn);
  const application& expra(expr);
  function_symbol head;
  if (!head_is_function_symbol(expra,head))
  {
    if (head_is_variable(expra))
    {
      if (get_variable_of_head(expra)==var)
      {
        return make_ar_true();
      }
    }
    return make_ar_false();
  }

  aterm_appl result = make_ar_false();

  size_t arity = recursive_number_of_args(expra);
  for (size_t i=0; i<arity; i++)
  {
    const size_t idx = int2ar_idx[OpId2Int(head).value()] + ((arity-1)*arity)/2 + i;
    aterm_appl t = build_ar_expr_internal(get_argument_of_higher_order_term(expra,i),var);
    result = make_ar_or(result,make_ar_and(make_ar_var(idx),t));
  }

  return result;
}

aterm_appl RewriterCompilingJitty::build_ar_expr_aux(const data_equation &eqn, const size_t arg, const size_t arity)
{
  atermpp::aterm_appl lhs = toInner(eqn.lhs(),true); // the lhs in internal format.

  size_t eqn_arity = lhs.function().arity()-1;
  if (eqn_arity > arity)
  {
    return make_ar_true();
  }
  if (eqn_arity <= arg)
  {
    // aterm rhs = toInner_list_odd(eqn.rhs());  // rhs in special internal list format.
    const data_expression rhs = toInner(eqn.rhs(),true);  // rhs in special internal list format.
    function_symbol head;
    if (is_function_symbol(rhs))
    {
      const size_t idx = int2ar_idx[OpId2Int(aterm_cast<function_symbol>(rhs)).value()] + ((arity-1)*arity)/2 + arg;
      return make_ar_var(idx);
    }
    else if (head_is_function_symbol(rhs,head))
    {
      int rhs_arity = recursive_number_of_args(rhs)-1;
      size_t diff_arity = arity-eqn_arity;
      int rhs_new_arity = rhs_arity+diff_arity;
      size_t idx = int2ar_idx[OpId2Int(head).value()] +
                         ((rhs_new_arity-1)*rhs_new_arity)/2 + (arg - eqn_arity + rhs_arity);
      return make_ar_var(idx);
    }
    else
    {
      return make_ar_false();
    }
  }

  aterm_appl arg_term = aterm_cast<aterm_appl>(lhs[arg+1]);
  if (!is_variable(arg_term))
  {
    return make_ar_true();
  }

  const variable v(arg_term);
  const variable_list l=dep_vars(eqn);
  if (std::find(l.begin(),l.end(), v) != l.end())
  {
    return make_ar_true();
  }

  // return build_ar_expr(toInner_list_odd(eqn.rhs()),arg_term);
  return build_ar_expr_internal(toInner(eqn.rhs(),true),v);
}

aterm_appl RewriterCompilingJitty::build_ar_expr(const data_equation_list &eqns, const size_t arg, const size_t arity)
{
  if (eqns.empty())
  {
    return make_ar_true();
  }
  else
  {
    return make_ar_and(build_ar_expr_aux(eqns.front(),arg,arity),build_ar_expr(eqns.tail(),arg,arity));
  }
}

bool RewriterCompilingJitty::always_rewrite_argument(
     const function_symbol& opid,
     const size_t arity,
     const size_t arg)
{
  return !is_ar_false(ar[int2ar_idx[OpId2Int(opid).value()]+((arity-1)*arity)/2+arg]);
}

bool RewriterCompilingJitty::calc_ar(const aterm_appl &expr)
{
  if (is_ar_true(expr))
  {
    return true;
  }
  else if (is_ar_false(expr))
  {
    return false;
  }
  else if (is_ar_and(expr))
  {
    return calc_ar(aterm_cast<aterm_appl>(expr[0])) && calc_ar(aterm_cast<aterm_appl>(expr[1]));
  }
  else if (is_ar_or(expr))
  {
    return calc_ar(aterm_cast<aterm_appl>(expr[0])) || calc_ar(aterm_cast<aterm_appl>(expr[1]));
  }
  else     // is_ar_var(expr)
  {
    return !is_ar_false(ar[static_cast<aterm_int>(expr[0]).value()]);
  }
}

void RewriterCompilingJitty::fill_always_rewrite_array()
{
  ar=std::vector <aterm_appl> (ar_size);
  for(std::map <size_t,size_t> ::const_iterator it=int2ar_idx.begin(); it!=int2ar_idx.end(); ++it)
  {
    size_t arity = getArity(get_int2term(it->first));
    data_equation_list eqns = (size_t(it->first)<jittyc_eqns.size()?jittyc_eqns[it->first]:data_equation_list());
    size_t idx = it->second;
    for (size_t i=1; i<=arity; i++)
    {
      for (size_t j=0; j<i; j++)
      {
        ar[idx+((i-1)*i)/2+j] = build_ar_expr(eqns,j,i);
      }
    }
  }

  bool notdone = true;
  while (notdone)
  {
    notdone = false;
    for (size_t i=0; i<ar_size; i++)
    {
      if (!is_ar_false(ar[i]) && !calc_ar(ar[i]))
      {
        ar[i] = make_ar_false();
        notdone = true;
      }
    }
  }
}

/* static aterm toInner_list_odd(const data_expression &t)
{
  if (is_application(t))
  {
    aterm_list l;
    const application& a=atermpp::aterm_cast<application>(t);
    for (application::const_iterator i=a.begin(); i!=a.end(); ++i )
    {
      l.push_front(toInner_list_odd(*i));
    }

    l = reverse(l);

    aterm arg0 = toInner_list_odd(application(t).head());
    if (arg0.type_is_list())
    {
      l = aterm_cast<const aterm_list>(arg0) + l;
    }
    else
    {
      l.push_front(arg0);
    }
    return  l;
  }
  else if (is_function_symbol(t))
  {
    return aterm_cast<aterm_int>(OpId2Int(aterm_cast<function_symbol>(t)));
  }
  else if (is_variable(t))
  {
    // Here the expression is a binder or a where expression.
    return aterm_cast<aterm_appl>(t);
  }
  else if (is_abstraction(t))
  {
    const abstraction a=t;
    return gsMakeBinder(a.binding_operator(),a.variables(),(aterm_appl)toInner_list_odd(a.body()));
  }
  else if (is_where_clause(t))
  {
    const where_clause& w = core::static_down_cast<const where_clause&>(t);
    assignment_list assignments=w.assignments();
    atermpp::term_list <atermpp::aterm_appl > translated_assignments;
    for (assignment_list::const_iterator i=assignments.begin(); i!=assignments.end(); ++i)
    {
      translated_assignments=push_back(translated_assignments,core::detail::gsMakeDataVarIdInit(i->lhs(),(aterm_appl)toInner_list_odd(i->rhs())));
    }
    return gsMakeWhr((aterm_appl)toInner_list_odd(w.body()),
                     (aterm_list)reverse(translated_assignments));
  }
  assert(0);
  return aterm();
} */


bool RewriterCompilingJitty::addRewriteRule(const data_equation &rule1)
{
  // const data_equation rule=m_conversion_helper.implement(rule1);
  const data_equation rule=rule1;
  try
  {
    CheckRewriteRule(rule);
  }
  catch (std::runtime_error& e)
  {
    mCRL2log(warning) << e.what() << std::endl;
    return false;
  }

  if (rewrite_rules.insert(rule).second)
  {
    // The equation has been added as a rewrite rule, otherwise the equation was already present.
    // Add and number new OpIds, if so required.
    toRewriteFormat(rule.condition());
    toRewriteFormat(rule.lhs());
    toRewriteFormat(rule.rhs());
    data_equation_selector.add_function_symbols(rule.lhs());
    need_rebuild = true;
  }
  return true;
}

bool RewriterCompilingJitty::removeRewriteRule(const data_equation &rule1)
{
  // const data_equation rule=m_conversion_helper.implement(rule1);
  const data_equation rule=rule1;
  if (rewrite_rules.erase(rule)>0) // An equation is erased
  {
    // The equation has been added as a rewrite rule, otherwise the equation was already present.
    need_rebuild = true;
    return true;
  }
  return false;
}

void RewriterCompilingJitty::CompileRewriteSystem(const data_specification& DataSpec)
{
  made_files = false;

  need_rebuild = true;

  true_inner = aterm_cast<function_symbol>(toInner(sort_bool::true_(),true));

  for (function_symbol_vector::const_iterator it=DataSpec.mappings().begin(); it!=DataSpec.mappings().end(); ++it)
  {
    OpId2Int(*it);
  }

  for (function_symbol_vector::const_iterator it=DataSpec.constructors().begin(); it!=DataSpec.constructors().end(); ++it)
  {
    OpId2Int(*it);
  }

  const data_equation_vector l=DataSpec.equations();
  for (data_equation_vector::const_iterator j=l.begin(); j!=l.end(); ++j)
  {
    if (data_equation_selector(*j))
    {
      addRewriteRule(*j);
    }
  }

  int2ar_idx.clear();
  ar=std::vector<aterm_appl>();
}

void RewriterCompilingJitty::CleanupRewriteSystem()
{
  if (so_rewr_cleanup != NULL)
  {
    so_rewr_cleanup();
  }
  if (rewriter_so != NULL)
  {
    delete rewriter_so;
    rewriter_so = NULL;
  }
}

/* Opens a .cpp file, saves filenames to file_c, file_o and file_so.
 *
 */
FILE* RewriterCompilingJitty::MakeTempFiles()
{
	FILE* result;

	std::ostringstream file_base;
        char* file_dir = getenv("MCRL2_COMPILEDIR");
        if (file_dir != NULL)
        {
          size_t l = strlen(file_dir);
          if (file_dir[l - 1] == '/')
          {
            file_dir[l - 1] = 0;
          }
          file_base << file_dir;
        }
        else
        {
          file_base << ".";
        }
	file_base << "/jittyc_" << getpid() << "_" << reinterpret_cast< long >(this) << ".cpp";

	rewriter_source = file_base.str();

	result = fopen(const_cast< char* >(rewriter_source.c_str()),"w");
	if (result == NULL)
	{
		perror("fopen");
		throw mcrl2::runtime_error("Could not create temporary file for rewriter.");
	}

	return result;
}

// The function below yields true if the function indicated by the
// function index can legitemately be used with a arguments.
// Typically a function f:D1x...xDn->D can be used with 0 and n arguments.
// A function f:(D1x...xDn)->(E1x...Em)->F can be used with 0, n, and n+m
// arguments.
static bool arity_is_allowed(
                     const sort_expression s,
                     const size_t a)
{
  if (a==0)
  {
    return true;
  }
  if (is_function_sort(s))
  {
    const function_sort fs(s);
    size_t n=fs.domain().size();
    if (n>a)
    {
      return false;
    }
    return arity_is_allowed(fs.codomain(),a-n);
  }
  return false;
}

static bool arity_is_allowed(
                     const size_t func_index,
                     const size_t a)
{
  return arity_is_allowed(get_int2term(func_index).sort(),a);
}

inline
void declare_rewr_functions(FILE* f, const size_t func_index, const size_t arity)
{
  for (size_t a=0; a<=arity; a++)
  {
    if (arity_is_allowed(func_index,a))
    {
      const size_t b = (a<=NF_MAX_ARITY)?a:0;
      for (size_t nfs=0; (nfs >> b) == 0; nfs++)
      {
        if (a==0)
        {
          // This is a constant function; result can be derived by reference.
          fprintf(f,  "static inline const atermpp::aterm_appl &rewr_%zu_%zu_%zu(",func_index,a,nfs);
        }
        else
        {
          fprintf(f,  "static inline atermpp::aterm_appl rewr_%zu_%zu_%zu(",func_index,a,nfs);
        }
        for (size_t i=0; i<a; i++)
        {
          if (((nfs >> i) & 1) ==1) // nfs indicates in binary form which arguments are in normal form.
          {
            fprintf(f, (i==0)?"const atermpp::aterm_appl &arg%zu":", const atermpp::aterm_appl &arg%zu",i);
          }
          else
          {
            fprintf(f, (i==0)?"const atermpp::aterm_appl &arg_not_nf%zu":", const atermpp::aterm_appl &arg_not_nf%zu",i);
          }
        }
        fprintf(f,  ");\n");

        fprintf(f,  "static inline atermpp::aterm_appl rewr_%zu_%zu_%zu_term(const atermpp::aterm_appl &t) { return rewr_%zu_%zu_%zu(", func_index, a, nfs,func_index, a,nfs);
        for(size_t i = 1; i <= a; ++i)
        {
          fprintf(f,  "%satermpp::aterm_cast<const atermpp::aterm_appl>(t[%zu])", (i == 1?"":", "), i);
        }
        fprintf(f,  "); }\n");
      }
    }
  }
}

void RewriterCompilingJitty::BuildRewriteSystem()
{
  FILE* f;
  CleanupRewriteSystem();

  // Try to find out from environment which compile script to use.
  // If not set, choose one of the following two:
  // * if "mcrl2compilerewriter" is in the same directory as the executable,
  //   this is the version we favour. This is especially needed for single
  //   bundle applications on MacOSX. Furthermore, it is the more foolproof
  //   approach on other platforms.
  // * by default, fall back to the system provided mcrl2compilerewriter script.
  //   in this case, we rely on the script being available in the user's
  //   $PATH environment variable.
  std::string compile_script;
  char* env_compile_script = getenv("MCRL2_COMPILEREWRITER");
  if (env_compile_script != NULL)
  {
    compile_script = env_compile_script;
  }
  else if(mcrl2::utilities::file_exists(mcrl2::utilities::get_executable_basename() + "/mcrl2compilerewriter"))
  {
    compile_script = mcrl2::utilities::get_executable_basename() + "/mcrl2compilerewriter";
  }
  else
  {
    compile_script = "mcrl2compilerewriter";
  }

  rewriter_so = new uncompiled_library(compile_script);
  mCRL2log(verbose) << "using '" << compile_script << "' to compile rewriter." << std::endl;

  jittyc_eqns.clear();

  ar_size = 0;
  int2ar_idx.clear();

  for(std::map< data::function_symbol, atermpp::aterm_int >::const_iterator l = term2int.begin()
        ; l != term2int.end()
        ; ++l)
  {
    size_t i = l->second.value();
    if (int2ar_idx.count(i) == 0)
    {
      size_t arity = getArity(l->first);
      int2ar_idx[i]=ar_size;
      ar_size += (arity*(arity+1))/2;
    }
  }
  for(std::set < data_equation >::const_iterator it=rewrite_rules.begin();
                   it!=rewrite_rules.end(); ++it)
  {
    size_t main_op_id_index=OpId2Int(get_function_symbol_of_head(it->lhs())).value(); // main symbol of equation.
    if (main_op_id_index>=jittyc_eqns.size())
    {
      jittyc_eqns.resize(main_op_id_index+1);
    }
    jittyc_eqns[main_op_id_index].push_front(*it);
  }
  fill_always_rewrite_array();

  f = MakeTempFiles();

#ifdef MCRL2_USE_INDEX_TRAITS
  fprintf(f, "#define MCRL2_USE_INDEX_TRAITS\n");
#endif

  //
  //  Print includes
  //
  fprintf(f, "#include \"mcrl2/data/detail/rewrite/jittycpreamble.h\"\n");

  //
  // Print defs
  //
  fprintf(f,
          "#define isAppl(x) (atermpp::detail::addressf(atermpp::aterm_cast<atermpp::aterm_appl>(x).function()) != %li)\n"
          "\n", atermpp::detail::addressf(data::variable("x",data::sort_bool::bool_()).function())
         );

  // Make a functional push_front to be used in the where clause.
  fprintf(f,"static aterm_list jittyc_local_push_front(aterm_list l, const aterm &e)\n"
            "{\n"
            "  l.push_front(e);\n"
            "  return l;\n"
            "}\n");

  // - Calculate maximum occurring arity
  // - Forward declarations of rewr_* functions
  //
  size_t max_arity = 0;
  for (size_t j=0; j < get_num_opids(); ++j)
  {
    const data::function_symbol fs=get_int2term(j);
    size_t arity = getArity(fs);
    if (arity > max_arity)
    {
      max_arity = arity;
    }
    if (data_equation_selector(fs))
    {
      declare_rewr_functions(f, j, arity);
    }
  }
  fprintf(f,  "\n\n");

  //
  // Declare function types
  //
  fprintf(f,  "typedef atermpp::aterm_appl (*func_type)(const atermpp::aterm_appl &);\n");
  fprintf(f,  "func_type* int2func[%zu];\n", max_arity+2);
  fprintf(f,  "func_type* int2func_head_in_nf[%zu];\n", max_arity+2);

  // Set this rewriter, to use its functions.
  fprintf(f,  "mcrl2::data::detail::RewriterCompilingJitty *this_rewriter;\n");

  // Make functions that construct applications with arity n where 5< n <= max_arity.
  for (size_t i=5; i<=max_arity; ++i)
  {
    fprintf(f,
            "static atermpp::aterm_appl make_term_with_many_arguments(const function_symbol &f");
    for (size_t j=0; j<=i; ++j)
    {
      fprintf(f, ", const atermpp::aterm &arg%zu",j);
    }
    fprintf(f, ")\n"
            "{\n");
    fprintf(f,
//      "const size_t arity = f.arity();\n"
      "MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,const detail::_aterm*,%ld);\n",i+1);

    for (size_t j=0; j<=i; ++j)
    {
      fprintf(f, "buffer[%zu] = atermpp::detail::address(arg%zu);\n",j,j);
    }

    fprintf(f, "return aterm_appl(f,buffer,buffer+%ld);\n",i+1);

    fprintf(f, "}\n\n");
  }


  //
  // "build" functions
  //
  for (size_t i=1; i<=max_arity; ++i)
  {
    fprintf(f,
            "static atermpp::aterm_appl build%zu(const atermpp::aterm_appl &a",i);
    for (size_t j=0; j<i; ++j)
    {
      fprintf(f, ", const atermpp::aterm_appl &arg%zu",j);
    }
    fprintf(f, ")\n"
            "{\n");
    fprintf(f,
            "  size_t arity = a.size();\n"
            "  if (arity == 1 )\n"
            "  {\n"
            "      return %sa[0]", calc_inner_appl_head(i).c_str());

    for (size_t j=0; j<i; j++)
    {
      fprintf(f, ",arg%zu",j);
    }
    fprintf(f,
            ");\n"
            "  }\n"
            "  else if (mcrl2::data::is_abstraction(a))\n"
            "  {\n"
            "    return %sa", calc_inner_appl_head(i).c_str());

    for (size_t j=0; j<i; j++)
    {
      fprintf(f, ",arg%zu",j);
    }
    fprintf(f,
            ");\n"
            "  }\n"
            "  else\n"
            "  {\n"
            "    //atermpp::aterm args[arity+ld];\n"
            "    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,const atermpp::detail::_aterm*,(arity+%zu));\n"
            "\n"
            "    for (size_t i=0; i<arity; i++)\n"
            "    {\n"
            "      args[i]=atermpp::detail::address(a[i]);\n"
            "    }\n",i);
    for (size_t j=0; j<i; j++)
    {
      fprintf(f,
              "    args[arity+%zu]=atermpp::detail::address(arg%zu);\n",j,j);
    }
    fprintf(f,
            "\n"
            "    return atermpp::aterm_appl(mcrl2::data::detail::get_appl_afun_value(arity+%zu),&args[0],&args[0]+(arity+%zu));\n"
            "  }\n"
            "}\n"
            "\n",i,i);
  }

  //
  // Implement the equations of every operation.
  //
  for (size_t j=0; j < get_num_opids(); j++)
  {
    const data::function_symbol fs=get_int2term(j);
    const size_t arity = getArity(fs);

    if (data_equation_selector(fs))
    {
      fprintf(f,  "// %s\n",to_string(fs).c_str());

      for (size_t a=0; a<=arity; a++)
      {
        if (arity_is_allowed(j,a))
        {
          nfs_array nfs_a(a);
          int b = (a<=NF_MAX_ARITY)?a:0;
          for (size_t nfs=0; (nfs >> b) == 0; nfs++)
          {
            if (a==0)
            {
              fprintf(f,  "static const atermpp::aterm_appl &rewr_%zu_%zu_%zu(",j,a,nfs);
            }
            else
            {
              fprintf(f,  "static atermpp::aterm_appl rewr_%zu_%zu_%zu(",j,a,nfs);
            }
            for (size_t i=0; i<a; i++)
            {
              if (((nfs >> i) & 1) ==1) // nfs indicates in binary form which arguments are in normal form.
              {
                fprintf(f, (i==0)?"const atermpp::aterm_appl &arg%zu":", const atermpp::aterm_appl &arg%zu",i);
              }
              else
              {
                fprintf(f, (i==0)?"const atermpp::aterm_appl &arg_not_nf%zu":", const atermpp::aterm_appl &arg_not_nf%zu",i);
              }
            }
            fprintf(f,  ")\n"
                    "{\n"
                   );
//for (size_t i=0; i<a; i++)
//{
//  if (((nfs >> i) & 1) ==1) // nfs indicates in binary form which arguments are in normal form.
//  {
//    fprintf(f, "  std::cerr << \"ARG\%zu \"  << arg%zu << \"\\n\";\n",i,i);
//  }
//  else
//  {
//    fprintf(f, "  std::cerr << \"ARg\%zu \" << arg_not_nf%zu << \"\\n\";\n",i,i);
//  }
//}
            if (j<jittyc_eqns.size() && !jittyc_eqns[j].empty() )
            {
            // Implement strategy
              if (0 < a)
              {
                nfs_a.set_value(nfs);
              }
              implement_strategy(f,create_strategy(jittyc_eqns[j],j,a,nfs_a,true_inner),a,1,j,nfs);
            }
            else
            {
              std::vector<bool> used;
              for (size_t k=0; k<a; k++)
              {
                used.push_back((nfs & ((size_t)1 << k)) != 0);
              }
              finish_function(f,a,j,used);
            }

            fprintf(f,                 "}\n");
          }
        }
      }
      fprintf(f,  "\n");
    }
  }

  extend_appl_afun_values(max_arity+1);
  fprintf(f,
          "void rewrite_init(RewriterCompilingJitty *r)\n"
          "{\n"
          "  this_rewriter=r;\n"
          "  mcrl2::data::detail::get_appl_afun_value_no_check(%zu);\n"
          "  assert(this_rewriter->rewriter_binding_variable_lists.size()==%zu);\n"
          "  assert(this_rewriter->rewriter_bound_variables.size()==%zu);\n",
          max_arity+1,rewriter_binding_variable_lists.size(),rewriter_bound_variables.size()
         );
  fprintf(f,  "\n");

  /* put the functions that start the rewriting in the array int2func */
  fprintf(f,  "\n");
  fprintf(f,  "\n");
  // Generate the entries for int2func.
  for (size_t i=0; i<=max_arity; i++)
  {
    fprintf(f,  "  int2func[%zu] = (func_type *) malloc(%zu*sizeof(func_type));\n",i+1,get_num_opids());
    for (size_t j=0; j < get_num_opids(); j++)
    {
      const data::function_symbol fs=get_int2term(j);

      if (partially_rewritten_functions.count(fs)>0)
      {
        if (arity_is_allowed(j,i))
        {
          // We are dealing with a partially rewritten function here. Remove the "@_" at
          // the beginning of the string.
          const string c_function_name=pp(fs.name());
          fprintf(f,  "  int2func[%zu][%zu] = %s;\n",i+1,j,c_function_name.substr(2,c_function_name.size()-2).c_str());
        }
      }
      else if (data_equation_selector(fs) && arity_is_allowed(j,i))
      {
        fprintf(f,  "  int2func[%zu][%zu] = rewr_%zu_%zu_0_term;\n",i+1,j,j,i);
      }
    }
  }
  // Generate the entries for int2func_head_in_nf. Entries for constants (with arity 0) are not required.
  for (size_t i=1; i<=max_arity; i++)
  {
    fprintf(f,  "  int2func_head_in_nf[%zu] = (func_type *) malloc(%zu*sizeof(func_type));\n",i+1,get_num_opids());
    for (size_t j=0; j < get_num_opids(); j++)
    {
      const data::function_symbol fs=get_int2term(j);

      if (partially_rewritten_functions.count(fs)>0)
      {
        if (arity_is_allowed(j,i))
        {
          // We are dealing with a partially rewritten function here.
          // This cannot be invoked for any normal function.
        }
      }
      else if (data_equation_selector(fs) && arity_is_allowed(j,i))
      {
        if (i<=NF_MAX_ARITY)
        {
          fprintf(f,  "  int2func_head_in_nf[%zu][%zu] = rewr_%zu_%zu_1_term;\n",i+1,j,j,i);
        }
        else
        {
          // If i>NF_MAX_ARITY no compiled rewrite function where the head is already in nf is available.
          fprintf(f,  "  int2func_head_in_nf[%zu][%zu] = rewr_%zu_%zu_0_term;\n",i+1,j,j,i);
        }
      }
    }
  }
  // Generate code to cleanup int2func.
  fprintf(f,  "}\n"
          "\n"
          "void rewrite_cleanup()\n"
          "{\n"
         );
  fprintf(f,  "\n");
  for (size_t i=0; i<=max_arity; i++)
  {
    fprintf(f,  "  free(int2func[%zu]);\n",i+1);
    fprintf(f,  "  free(int2func_head_in_nf[%zu]);\n",i+1);
  }
  fprintf(f,  "}\n"
          "\n"

         );

  fprintf(f,
      "struct argument_rewriter_struct\n"
      "{\n"
      "  argument_rewriter_struct()\n"
      "  {}\n"
      "\n"
      "  atermpp::aterm_appl operator()(const atermpp::aterm &arg) const\n"
      "  {\n"
      "    return rewrite(aterm_cast<const atermpp::aterm_appl >(arg));\n"
      "  }\n"
      "};\n"
      "\n"
      "atermpp::aterm_appl rewrite_int_aux(const atermpp::aterm_appl &t)\n"
      "{\n"
      "  const argument_rewriter_struct argument_rewriter;\n"
      "  return atermpp::aterm_appl(t.function(),t.begin(),t.end(),argument_rewriter);\n"
      "}\n\n");

  fprintf(f,
      "atermpp::aterm_appl rewrite_appl_aux(\n"
      "     const atermpp::aterm_appl& t)\n"
      "{\n"
      "  using namespace mcrl2::core::detail;\n"
      "  using namespace atermpp;\n"
      "  const aterm_appl& head0=aterm_cast<const aterm_appl>(t[0]);\n"
      "  aterm_appl head=\n"
      "       (mcrl2::data::is_variable(head0)?\n"
      "            (*(this_rewriter->global_sigma))(aterm_cast<const mcrl2::data::variable>(head0)):\n"
      "       (mcrl2::data::is_where_clause(head0)?\n"
      "            this_rewriter->rewrite_where(head0,*(this_rewriter->global_sigma)):\n"
      "             head0));\n"
      "  \n"
      "  // Here head has the shape\n"
      "  // variable, u(u1,...,um), lambda y1,....,ym.u, forall y1,....,ym.u or exists y1,....,ym.u,\n"
      "  if (mcrl2::data::is_abstraction(head))\n"
      "  {\n"
      "    const aterm_appl binder(head[0]);\n"
      "    if (is_lambda(binder))\n"
      "    {\n"
      "      return this_rewriter->rewrite_lambda_application(head,t,*(this_rewriter->global_sigma));\n"
      "    }\n"
      "    if (is_exists(binder))\n"
      "    {\n"
      "      return this_rewriter->internal_existential_quantifier_enumeration(head,*(this_rewriter->global_sigma));\n"
      "    }\n"
      "    if (is_forall(binder)))\n"
      "    {\n"
      "      return this_rewriter->internal_universal_quantifier_enumeration(head,*(this_rewriter->global_sigma));\n"
      "    }\n"
      "    assert(0); // One cannot end up here.\n"
      "  }\n"
      "  \n"
      "  const size_t arity=t.size();\n"
      "  if (mcrl2::data::is_variable(head))\n"
      "  {\n"
      "    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,aterm, arity);\n"
      "    new (&args[0]) aterm(head);\n"
      "    for(size_t i=1; i<arity; ++i)\n"
      "    {\n"
      "      new (&args[i]) aterm(rewrite(aterm_cast<const aterm_appl>(t[i])));\n"
      "    }\n"
      "    const aterm_appl result=ApplyArray(arity,&args[0],&args[0]+arity);\n"
      "    for(size_t i=0; i<arity; ++i)\n"
      "    {\n"
      "      args[i].~aterm();\n"
      "    }\n"
      "    return result;\n"
      "  }\n"
      "  \n"
      "  \n"
      "  // Here head has the shape #REWR#(u0,u1,...,un).\n"

      "  const aterm_appl u=head;\n"
      "  const size_t arity_t = t.size();\n"
      "  const aterm head1 = u[0];\n"
      "  const size_t arity_u=u.size();\n"
      "  MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,aterm,(arity_u+arity_t-1));\n"
//       "  std::vector<aterm> args(arity_u+arity_t-1);\n"
      "  new (&args[0]) aterm(head1);\n"
      "  size_t function_index;\n"
      "  if ((atermpp::detail::addressf(aterm_cast<aterm_appl>(head1).function())==%ld) && ((function_index = aterm_int(head1).value()) < %zu) )\n"
      "  {\n"
      "    for (size_t i=1; i<arity_u; ++i)\n"
      "    {\n"
      "      new (&args[i]) aterm(u[i]);\n"
      "    }\n"
      "    size_t k = arity_u;\n"
      "    for (size_t i=1; i<arity_t; ++i,++k)\n"
      "    {\n"
      "      new (&args[k]) aterm(t[i]);\n"
      "    }\n"
      "    size_t arity = arity_u+arity_t-2;\n"
      "    const aterm_appl intermediate(mcrl2::data::detail::get_appl_afun_value(arity+1),&args[0],&args[0]+arity_u+arity_t-1);\n"   // YYYY+
      "    for(size_t i=0; i<arity_u+arity_t-1; ++i)\n"
      "    {\n"
      "      args[i].~aterm();\n"
      "    }\n"
      "    assert(arity <= %zu);\n"
      "    assert(int2func_head_in_nf[arity+1][function_index] != NULL);\n"
      "    return int2func_head_in_nf[arity+1][function_index](intermediate);\n"
      "  }\n"
      "  else\n"
      "  {\n"
      "    for (size_t i=1; i<arity_u; ++i)\n"
      "    {\n"
      "      new (&args[i]) aterm(rewrite(aterm_cast<const aterm_appl>(u[i])));\n"
      "    }\n"
      "    size_t k = arity_u;\n"
      "    for (size_t i=1; i<arity_t; ++i,++k)\n"
      "    {\n"
      "      new (&args[k]) aterm(rewrite(aterm_cast<const aterm_appl>(t[i])));\n"
      "    }\n"
      "    const aterm_appl result(mcrl2::data::detail::get_appl_afun_value(arity_u+arity_t-1),&args[0],&args[0]+arity_u+arity_t-1);\n"   // YYYY+
      "    for(size_t i=0; i<arity_u+arity_t-1; ++i)\n"
      "    {\n"
      "      args[i].~aterm();\n"
      "    }\n"
      "    return result;\n"
      "  }\n"
      "}\n\n",
      atermpp::detail::addressf(atermpp::detail::function_adm.AS_INT),get_num_opids(), max_arity+1
      );

  fprintf(f,
      "atermpp::aterm_appl rewrite_external(const atermpp::aterm_appl &t)\n"
      "{\n"
      "  return rewrite(t);\n"
      "}\n\n"
       );

  // Moved part of the rewrite function to rewrite_aux, such that the compiler
  // can inline rewrite more often, and so gain some performance.
  fprintf(f,
      "atermpp::aterm_appl rewrite_aux(const atermpp::aterm_appl &t)\n"
      "{\n"
      "  using namespace mcrl2::core::detail;\n"
      "  // Term t does not have the shape #REWR#(t1,...,tn)\n"
      "  if (mcrl2::data::is_variable(t))\n"
      "  {\n"
      "    return (*(this_rewriter->global_sigma))(atermpp::aterm_cast<const mcrl2::data::variable>(t));\n"
      "  }\n"
      "  if (mcrl2::data::is_abstraction(t))\n"
      "  {\n"
      "    atermpp::aterm_appl binder(t[0]);\n"
      "    if (is_exists(binder))\n"
      "    {\n"
      "      return this_rewriter->internal_existential_quantifier_enumeration(t,*(this_rewriter->global_sigma));\n"
      "    }\n"
      "    if (is_forall(binder))\n"
      "    {\n"
      "      return this_rewriter->internal_universal_quantifier_enumeration(t,*(this_rewriter->global_sigma));\n"
      "    }\n"
      "    if (is_lambda(binder))\n"
      "    {\n"
      "      return this_rewriter->rewrite_single_lambda(\n"
      "               atermpp::aterm_cast<const mcrl2::data::variable_list>(t[1]),\n"
      "               atermpp::aterm_cast<const atermpp::aterm_appl>(t[2]),false,*(this_rewriter->global_sigma));\n"
      "    }\n"
      "    assert(0);\n"
      "    return t;\n"
      "  }\n"
      "  assert(mcrl2::data::is_where_clause(t));\n"
      "  return this_rewriter->rewrite_where(t,*(this_rewriter->global_sigma));\n"
      "}\n");

  fprintf(f,
      "static inline atermpp::aterm_appl rewrite(const atermpp::aterm_appl &t)\n"
      "{\n"
      "  using namespace mcrl2::core::detail;\n"
      "  if (mcrl2::data::is_function_symbol(t))\n"
      "  {\n"
      "    // Term t is a function_symbol\n"
      "    const size_t function_index = mcrl2::data::detail::OpId2Int(aterm_cast<mcrl2::data::function_symbol>(t)).value();\n"
      "    if (function_index < %zu )\n"
      "    {\n"
      "      const size_t arity=1;\n"
      "      assert(int2func[arity][function_index] != NULL);\n"
      "      return int2func[arity][function_index](t);\n"
      "    }\n"
      "    else\n"
      "    {\n"
      "      return rewrite_int_aux(t);"
      "    }\n"
      "  }\n"
      "  const function_symbol &fun=t.function();\n"
      "  const size_t arity=fun.arity();\n"
      "  if (fun==apples[arity])\n"
      "  {\n"
      "    // Term t has the shape #REWR#(t1,...,tn)\n"
      "    mcrl2::data::function_symbol head;\n"
      "    if (mcrl2::data::detail::head_is_function_symbol(t,head))\n"
      "    {\n"
      "      const size_t function_index = mcrl2::data::detail::OpId2Int(head).value();\n"
      "      if (function_index < %zu )\n"
      "      {\n"
      "        assert(arity <= %zu);\n"
      "        assert(int2func[arity][function_index] != NULL);\n"
      "        return int2func[arity][function_index](t);\n"
      "      }\n"
      "      else\n"
      "      {\n"
      "        return rewrite_int_aux(t);"
      "      }\n"
      "    }\n"
      "    else\n"
      "    {\n"
      "      return rewrite_appl_aux(t);\n"
      "    }\n"
      "  }\n"
      "  \n"
      "  return rewrite_aux(t);\n"
      "}\n",
      get_num_opids(), get_num_opids(), max_arity+1);


  fclose(f);

  mCRL2log(verbose) << "compiling rewriter..." << std::endl;

  try
  {
    rewriter_so->compile(rewriter_source);
  }
  catch(std::runtime_error &e)
  {
    rewriter_so->leave_files();
    throw mcrl2::runtime_error(std::string("Could not compile rewriter: ") + e.what());
  }

  mCRL2log(verbose) << "loading rewriter..." << std::endl;

  bool (*init)(rewriter_interface*);
  rewriter_interface interface = { mcrl2::utilities::get_toolset_version(), "Unknown error when loading rewriter.", this, NULL, NULL };
  try
  {
    init = reinterpret_cast<bool(*)(rewriter_interface *)>(rewriter_so->proc_address("init"));
  }
  catch(std::runtime_error &e)
  {
    rewriter_so->leave_files();
    throw mcrl2::runtime_error(std::string("Could not load rewriter: ") + e.what());
  }

#ifdef NDEBUG // In non debug mode clear compiled files directly after loading.
  try
  {
    rewriter_so->cleanup();
  }
  catch (std::runtime_error &error)
  {
    mCRL2log(mcrl2::log::error) << "Could not cleanup temporary files: " << error.what() << std::endl;
  }
#endif

  if (!init(&interface))
  {
    throw mcrl2::runtime_error(std::string("Could not load rewriter: ") + interface.status);
  }
  so_rewr_cleanup = interface.rewrite_cleanup;
  so_rewr = interface.rewrite_external;

  mCRL2log(verbose) << interface.status << std::endl;

  need_rebuild = false;
}

RewriterCompilingJitty::RewriterCompilingJitty(
                          const data_specification& data_spec, 
                          const used_data_equation_selector &equation_selector):
   Rewriter(data_spec,equation_selector)
{
  // data_equation_selector=equations_selector;
  so_rewr_cleanup = NULL;
  rewriter_so = NULL;
  // m_data_specification_for_enumeration = DataSpec;
  initialise_common();
  CompileRewriteSystem(data_spec);
}

RewriterCompilingJitty::~RewriterCompilingJitty()
{
  CleanupRewriteSystem();
  finalise_common();
}

data_expression RewriterCompilingJitty::rewrite(
     const data_expression &term,
     substitution_type &sigma)
{
  internal_substitution_type internal_sigma = apply(sigma, boost::bind(&RewriterCompilingJitty::toRewriteFormat, this, _1));
  // Code below is not accepted by all compilers.
  // internal_substitution_type internal_sigma = apply(sigma, [this](const data_expression& t){return this->toRewriteFormat(t);});
  return fromRewriteFormat(rewrite_internal(toRewriteFormat(term),internal_sigma));
}

data_expression RewriterCompilingJitty::rewrite_internal(
     const data_expression& term,
     internal_substitution_type& sigma)
{
  if (need_rebuild)
  {
    BuildRewriteSystem();
  }
  // Save global sigma and restore it afterwards, as rewriting might be recursive with different
  // substitutions, due to the enumerator.
  internal_substitution_type *saved_sigma=global_sigma;
  global_sigma= &sigma;
  const data_expression result=so_rewr(term);
  global_sigma=saved_sigma;
  return result;
}

rewrite_strategy RewriterCompilingJitty::getStrategy()
{
  return jitty_compiling;
}

}
}
}

#endif
