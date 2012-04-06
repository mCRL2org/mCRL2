// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file jittyc.cpp

#include "boost.hpp" // precompiled headers

#include "mcrl2/data/detail/rewrite.h"

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
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/aterm_ext.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/detail/rewrite/jittyc.h"
#include "mcrl2/data/traverser.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace std;
using namespace aterm;
using namespace mcrl2::log;

namespace mcrl2
{
namespace data
{
namespace detail
{

static AFun afunS, afunM, afunF, afunN, afunD, afunR, afunCR, afunC, afunX, afunRe, afunCRe, afunMe;
static ATerm dummy;
static AFun afunARtrue, afunARfalse, afunARand, afunARor, afunARvar;
static ATermAppl ar_true, ar_false;

#define isS(x) ATisEqualAFun(ATgetAFun(x),afunS)
#define isM(x) ATisEqualAFun(ATgetAFun(x),afunM)
#define isF(x) ATisEqualAFun(ATgetAFun(x),afunF)
#define isN(x) ATisEqualAFun(ATgetAFun(x),afunN)
#define isD(x) ATisEqualAFun(ATgetAFun(x),afunD)
#define isR(x) ATisEqualAFun(ATgetAFun(x),afunR)
#define isCR(x) ATisEqualAFun(ATgetAFun(x),afunCR)
#define isC(x) ATisEqualAFun(ATgetAFun(x),afunC)
#define isX(x) ATisEqualAFun(ATgetAFun(x),afunX)
#define isRe(x) ATisEqualAFun(ATgetAFun(x),afunRe)
#define isCRe(x) ATisEqualAFun(ATgetAFun(x),afunCRe)
#define isMe(x) ATisEqualAFun(ATgetAFun(x),afunMe)

static size_t is_initialised = 0;

static void initialise_common()
{
  if (is_initialised == 0)
  {
    afunS = ATmakeAFun("@@S",2,false); // Store term ( target_variable, result_tree )
    ATprotectAFun(afunS);
    afunM = ATmakeAFun("@@M",3,false); // Match term ( match_variable, true_tree , false_tree )
    ATprotectAFun(afunM);
    afunF = ATmakeAFun("@@F",3,false); // Match function ( match_function, true_tree, false_tree )
    ATprotectAFun(afunF);
    afunN = ATmakeAFun("@@N",1,false); // Go to next parameter ( result_tree )
    ATprotectAFun(afunN);
    afunD = ATmakeAFun("@@D",1,false); // Go down a level ( result_tree )
    ATprotectAFun(afunD);
    afunR = ATmakeAFun("@@R",1,false); // End of tree ( matching_rule )
    ATprotectAFun(afunR);
    afunCR = ATmakeAFun("@@CR",2,false); // End of tree ( condition, matching_rule )
    ATprotectAFun(afunCR);
    afunC = ATmakeAFun("@@C",3,false); // Check condition ( condition, true_tree, false_tree )
    ATprotectAFun(afunC);
    afunX = ATmakeAFun("@@X",0,false); // End of tree
    ATprotectAFun(afunX);
    afunRe = ATmakeAFun("@@Re",2,false); // End of tree ( matching_rule , vars_of_rule)
    ATprotectAFun(afunRe);
    afunCRe = ATmakeAFun("@@CRe",4,false); // End of tree ( condition, matching_rule, vars_of_condition, vars_of_rule )
    ATprotectAFun(afunCRe);
    afunMe = ATmakeAFun("@@Me",2,false); // Match term ( match_variable, variable_index )
    ATprotectAFun(afunMe);

    dummy=NULL;
    ATprotect(&dummy);
    dummy = (ATerm) gsMakeNil();

    afunARtrue = ATmakeAFun("@@true",0,false);
    ATprotectAFun(afunARtrue);
    afunARfalse = ATmakeAFun("@@false",0,false);
    ATprotectAFun(afunARfalse);
    afunARand = ATmakeAFun("@@and",2,false);
    ATprotectAFun(afunARand);
    afunARor = ATmakeAFun("@@or",2,false);
    ATprotectAFun(afunARor);
    afunARvar = ATmakeAFun("@@var",1,false);
    ATprotectAFun(afunARvar);
    ar_true = NULL;
    ATprotectAppl(&ar_true);
    ar_true = ATmakeAppl0(afunARtrue);
    ar_false = NULL;
    ATprotectAppl(&ar_false);
    ar_false = ATmakeAppl0(afunARfalse);
  }

  is_initialised++;
}

static void finalise_common()
{
  assert(is_initialised > 0);
  is_initialised--;

  if (is_initialised == 0)
  {
    ATunprotectAppl(&ar_false);
    ATunprotectAppl(&ar_true);
    ATunprotectAFun(afunARvar);
    ATunprotectAFun(afunARor);
    ATunprotectAFun(afunARand);
    ATunprotectAFun(afunARfalse);
    ATunprotectAFun(afunARtrue);

    ATunprotect(&dummy);

    ATunprotectAFun(afunMe);
    ATunprotectAFun(afunCRe);
    ATunprotectAFun(afunRe);
    ATunprotectAFun(afunX);
    ATunprotectAFun(afunC);
    ATunprotectAFun(afunCR);
    ATunprotectAFun(afunR);
    ATunprotectAFun(afunD);
    ATunprotectAFun(afunN);
    ATunprotectAFun(afunF);
    ATunprotectAFun(afunM);
    ATunprotectAFun(afunS);
  }
}

#define is_ar_true(x) (ATisEqual((x),ar_true))
#define is_ar_false(x) (ATisEqual((x),ar_false))
#define is_ar_and(x) (ATisEqualAFun(ATgetAFun(x),afunARand))
#define is_ar_or(x) (ATisEqualAFun(ATgetAFun(x),afunARor))
#define is_ar_var(x) (ATisEqualAFun(ATgetAFun(x),afunARvar))

// Prototype
static ATerm toInner_list_odd(const data_expression t);

static ATermAppl make_ar_true()
{
  return ar_true;
}

static ATermAppl make_ar_false()
{
  return ar_false;
}

static ATermAppl make_ar_and(ATermAppl x, ATermAppl y)
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

  return ATmakeAppl2(afunARand,(ATerm) x,(ATerm) y);
}

static ATermAppl make_ar_or(ATermAppl x, ATermAppl y)
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

  return ATmakeAppl2(afunARor,(ATerm) x,(ATerm) y);
}

static ATermAppl make_ar_var(int var)
{
  return ATmakeAppl1(afunARvar,(ATerm) ATmakeInt(var));
}

static size_t num_int2aterms = 0;
static _ATerm** int2aterms = NULL; // An array with prepared ATermInt's.
static _ATerm* get_int2aterm_value(int i)
{
  assert(i >= 0);
  if (((size_t) i) >= num_int2aterms)
  {
    size_t old_num = num_int2aterms;
    num_int2aterms = i+1;
    if (int2aterms != NULL)
    {
      ATunprotectArray(reinterpret_cast<ATerm*>(int2aterms));
    }
    int2aterms = (_ATerm**) realloc(int2aterms,num_int2aterms*sizeof(ATerm));
    if (int2aterms == NULL)
    {
      throw mcrl2::runtime_error("Cannot allocate enough memory.");
    }
    for (size_t j=old_num; j < num_int2aterms; j++)
    {
      int2aterms[j] = NULL;
    }
    ATprotectArray((ATerm*)int2aterms,num_int2aterms);
    for (; old_num < num_int2aterms; old_num++)
    {
      int2aterms[old_num] = (_ATerm*) &*ATmakeInt(old_num);
    }
  }
  return int2aterms[i];
}

static _ATerm* get_int2aterm_value(ATermInt i)
{
  return get_int2aterm_value(ATgetInt(i));
}

static _ATermAppl* get_rewrappl_value(const size_t i)
{
  static atermpp::vector <_ATermAppl*> rewr_appls;
  while (rewr_appls.size()<i+1)
  {
    rewr_appls.push_back(&*(ATermAppl)Apply0(atermpp::aterm_int(rewr_appls.size())));
  }
  return rewr_appls[i];
}

static _ATermAppl* get_rewrappl_value(const atermpp::aterm_int i)
{
  return get_rewrappl_value(i.value());
}

atermpp::aterm_appl RewriterCompilingJitty::toRewriteFormat(const data_expression t)
{
  size_t old_opids = get_num_opids();

  atermpp::aterm_appl r = toInner(t,true);

  if (old_opids != get_num_opids())
  {
    need_rebuild = true;
  }

  return r;
}

/* data_expression RewriterCompilingJitty::fromRewriteFormat(const atermpp::aterm_appl t)
{
  return fromInner(t);
} */

static char* whitespace_str = NULL;
static int whitespace_len;
static int whitespace_pos;
static char* whitespace(int len)
{
  int i;

  if (whitespace_str == NULL)
  {
    whitespace_str = (char*) malloc((2*len+1)*sizeof(char));
    for (i=0; i<2*len; i++)
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
      for (i=whitespace_len; i<2*len; i++)
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


static void term2seq(ATerm t, ATermList* s, int* var_cnt)
{
  if (ATisInt(t))
  {
    term2seq((ATerm) ATmakeList1(t),s,var_cnt);
  }
  else if (ATisAppl(t))
  {
    if (gsIsDataVarId((ATermAppl) t))
    {
      ATerm store = (ATerm) ATmakeAppl2(afunS,(ATerm) t,dummy);

      if (ATindexOf(*s,store,0) != ATERM_NON_EXISTING_POSITION)
      {
        *s = ATinsert(*s, (ATerm) ATmakeAppl3(afunM,(ATerm) t,dummy,dummy));
      }
      else
      {
        (*var_cnt)++;
        *s = ATinsert(*s, store);
      }
    }
    else
    {
      int arity = ATgetArity(ATgetAFun((ATermAppl) t));

      *s = ATinsert(*s, (ATerm) ATmakeAppl3(afunF,ATgetArgument((ATermAppl) t,0),dummy,dummy));

      for (int i=1; i<arity; ++i)
      {
        term2seq(ATgetArgument((ATermAppl) t,i),s,var_cnt);
        if (i<arity-1)
        {
          *s = ATinsert(*s, (ATerm) ATmakeAppl1(afunN,dummy));
        }
      }
      *s = ATinsert(*s, (ATerm) ATmakeAppl1(afunD,dummy));
    }
  }
  else
  {
    assert(0);
  }

}

static void get_used_vars_aux(ATerm t, ATermList* vars)
{
  if (ATisList(t))
  {
    for (; !ATisEmpty((ATermList) t); t=(ATerm) ATgetNext((ATermList) t))
    {
      get_used_vars_aux(ATgetFirst((ATermList) t),vars);
    }
  }
  else if (ATisAppl(t))
  {
    if (gsIsDataVarId((ATermAppl) t))
    {
      if (ATindexOf(*vars,t,0) == ATERM_NON_EXISTING_POSITION)
      {
        *vars = ATinsert(*vars,t);
      }
    }
    else
    {
      int a = ATgetArity(ATgetAFun((ATermAppl) t));
      for (int i=0; i<a; i++)
      {
        get_used_vars_aux(ATgetArgument((ATermAppl) t,i),vars);
      }
    }
  }
}

static ATermList get_used_vars(ATerm t)
{
  ATermList l = ATmakeList0();

  get_used_vars_aux(t,&l);

  return l;
}

static ATermList create_sequence(const data_equation rule, int* var_cnt, ATermInt true_inner)
{
  ATermAppl pat = toInner(rule.lhs(),true);
  int pat_arity = ATgetArity(ATgetAFun(pat));
  ATerm cond = toInner_list_odd(rule.condition());
  ATerm rslt = toInner_list_odd(rule.rhs());
  ATermList rseq = ATmakeList0();

  for (int i=1; i<pat_arity; ++i)
  {
    term2seq(ATgetArgument(pat,i),&rseq,var_cnt);
    if (i<pat_arity-1)
    {
      rseq = ATinsert(rseq, (ATerm) ATmakeAppl1(afunN,dummy));
    }
  }

  if (ATisInt(cond) && ATisEqual(cond, true_inner))
  {
    rseq = ATinsert(rseq,(ATerm) ATmakeAppl2(afunRe,rslt,(ATerm) get_used_vars(rslt)));
  }
  else
  {
    rseq = ATinsert(rseq,(ATerm) ATmakeAppl4(afunCRe,cond,rslt,(ATerm) get_used_vars(cond),(ATerm) get_used_vars(rslt)));
  }

  return ATreverse(rseq);
}


// Structure for build_tree paramters
typedef struct
{
  ATermList Flist;   // List of sequences of which the first action is an F
  ATermList Slist;   // List of sequences of which the first action is an S
  ATermList Mlist;   // List of sequences of which the first action is an M
  ATermList stack;   // Stack to maintain the sequences that do not have to
  // do anything in the current term
  ATermList upstack; // List of sequences that have done an F at the current
  // level
} build_pars;

static void initialise_build_pars(build_pars* p)
{
  p->Flist = ATmakeList0();
  p->Slist = ATmakeList0();
  p->Mlist = ATmakeList0();
  p->stack = ATmakeList1((ATerm) ATmakeList0());
  p->upstack = ATmakeList0();
}

static ATermList add_to_stack(ATermList stack, ATermList seqs, ATermAppl* r, ATermList* cr)
{
  if (ATisEmpty(stack))
  {
    return stack;
  }

  ATermList l = ATmakeList0();
  ATermList h = ATLgetFirst(stack);

  for (; !ATisEmpty(seqs); seqs=ATgetNext(seqs))
  {
    ATermList e = ATLgetFirst(seqs);

    if (isD(ATAgetFirst(e)))
    {
      l = ATinsert(l,(ATerm) ATgetNext(e));
    }
    else if (isN(ATAgetFirst(e)))
    {
      h = ATinsert(h,(ATerm) ATgetNext(e));
    }
    else if (isRe(ATAgetFirst(e)))
    {
      *r = ATAgetFirst(e);
    }
    else
    {
      *cr = ATinsert(*cr,ATgetFirst(e));
    }
  }

  return ATinsert(add_to_stack(ATgetNext(stack),l,r,cr),(ATerm) h);
}

static void add_to_build_pars(build_pars* pars,ATermList seqs, ATermAppl* r, ATermList* cr)
{
  ATermList l = ATmakeList0();

  for (; !ATisEmpty(seqs); seqs=ATgetNext(seqs))
  {
    ATermList e = ATLgetFirst(seqs);

    if (isD(ATAgetFirst(e)) || isN(ATAgetFirst(e)))
    {
      l = ATinsert(l,(ATerm) e);
    }
    else if (isS(ATAgetFirst(e)))
    {
      pars->Slist = ATinsert(pars->Slist,(ATerm) e);
    }
    else if (isMe(ATAgetFirst(e)))     // M should not appear at the head of a seq
    {
      pars->Mlist = ATinsert(pars->Mlist,(ATerm) e);
    }
    else if (isF(ATAgetFirst(e)))
    {
      pars->Flist = ATinsert(pars->Flist,(ATerm) e);
    }
    else if (isRe(ATAgetFirst(e)))
    {
      *r = ATAgetFirst(e);
    }
    else
    {
      *cr = ATinsert(*cr,ATgetFirst(e));
    }
  }

  pars->stack = add_to_stack(pars->stack,l,r,cr);
}

static char tree_var_str[20];
static ATermAppl createFreshVar(ATermAppl sort,int* i)
{
  sprintf(tree_var_str,"@var_%i",(*i)++);
  return gsMakeDataVarId(gsString2ATermAppl(tree_var_str),sort);
}

static ATermList subst_var(ATermList l, ATermAppl old, ATerm new_val, ATerm num, ATermList substs)
{
  if (ATisEmpty(l))
  {
    return l;
  }

  ATermAppl head = (ATermAppl) ATgetFirst(l);
  l = ATgetNext(l);

  if (isM(head))
  {
    if (ATisEqual(ATgetArgument(head,0),old))
    {
      head = ATmakeAppl2(afunMe,new_val,num);
    }
  }
  else if (isCRe(head))
  {
    ATermList l = (ATermList) ATgetArgument(head,2);
    ATermList m = ATmakeList0();
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      if (ATisEqual(ATgetFirst(l),old))
      {
        m = ATinsert(m,num);
      }
      else
      {
        m = ATinsert(m,ATgetFirst(l));
      }
    }
    l = (ATermList) ATgetArgument(head,3);
    ATermList n = ATmakeList0();
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      if (ATisEqual(ATgetFirst(l),old))
      {
        n = ATinsert(n,num);
      }
      else
      {
        n = ATinsert(n,ATgetFirst(l));
      }
    }
    head = ATmakeAppl4(afunCRe,gsSubstValues(substs,ATgetArgument(head,0),true),gsSubstValues(substs,ATgetArgument(head,1),true),(ATerm) m, (ATerm) n);
  }
  else if (isRe(head))
  {
    ATermList l = (ATermList) ATgetArgument(head,1);
    ATermList m = ATmakeList0();
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      if (ATisEqual(ATgetFirst(l),old))
      {
        m = ATinsert(m,num);
      }
      else
      {
        m = ATinsert(m,ATgetFirst(l));
      }
    }
    head = ATmakeAppl2(afunRe,gsSubstValues(substs,ATgetArgument(head,0),true),(ATerm) m);
  }

  return ATinsert(subst_var(l,old,new_val,num,substs),(ATerm) head);
}

static int* treevars_usedcnt;

static void inc_usedcnt(ATermList l)
{
  for (; !ATisEmpty(l); l=ATgetNext(l))
  {
    ATerm first=ATgetFirst(l);
    if (ATisInt(first))
    {
      treevars_usedcnt[ATgetInt((ATermInt)first)]++;
    }
  }
}

static ATermAppl build_tree(build_pars pars, int i)
{
  if (!ATisEmpty(pars.Slist))
  {
    ATermList l,m;

    int k = i;
    ATermAppl v = createFreshVar(ATAgetArgument(ATAgetArgument(ATAgetFirst(ATLgetFirst(pars.Slist)),0),1),&i);
    treevars_usedcnt[k] = 0;

    l = ATmakeList0();
    m = ATmakeList0();
    for (; !ATisEmpty(pars.Slist); pars.Slist=ATgetNext(pars.Slist))
    {
      ATermList e = ATLgetFirst(pars.Slist);

      e = subst_var(e,ATAgetArgument(ATAgetFirst(e),0),(ATerm) v,(ATerm) ATmakeInt(k),ATmakeList1((ATerm) gsMakeSubst(ATgetArgument(ATAgetFirst(e),0),(ATerm) v)));

      l = ATinsert(l,ATgetFirst(e));
      m = ATinsert(m,(ATerm) ATgetNext(e));
    }

    ATermAppl r = NULL;
    ATermList readies = ATmakeList0();

    pars.stack = add_to_stack(pars.stack,m,&r,&readies);

    if (r==NULL)
    {
      ATermAppl tree;

      tree = build_tree(pars,i);
      for (; !ATisEmpty(readies); readies=ATgetNext(readies))
      {
        inc_usedcnt((ATermList) ATgetArgument(ATAgetFirst(readies),2));
        inc_usedcnt((ATermList) ATgetArgument(ATAgetFirst(readies),3));
        tree = ATmakeAppl3(afunC,ATgetArgument(ATAgetFirst(readies),0),(ATerm) ATmakeAppl1(afunR,ATgetArgument(ATAgetFirst(readies),1)),(ATerm) tree);
      }
      r = tree;
    }
    else
    {
      inc_usedcnt((ATermList) ATgetArgument(r,1));
      r = ATmakeAppl1(afunR,ATgetArgument(r,0));
    }

    if ((treevars_usedcnt[k] > 0) || ((k == 0) && isR(r)))
    {
       return ATmakeAppl2(afunS,(ATerm) v,(ATerm) r);
    }
    else
    {
       return r;
    }
  }
  else if (!ATisEmpty(pars.Mlist))
  {
    ATerm M = ATgetFirst(ATLgetFirst(pars.Mlist));

    ATermList l = ATmakeList0();
    ATermList m = ATmakeList0();
    for (; !ATisEmpty(pars.Mlist); pars.Mlist=ATgetNext(pars.Mlist))
    {
      if (ATisEqual(M,ATgetFirst(ATLgetFirst(pars.Mlist))))
      {
        l = ATinsert(l,(ATerm) ATgetNext(ATLgetFirst(pars.Mlist)));
      }
      else
      {
        m = ATinsert(m,ATgetFirst(pars.Mlist));
      }
    }
    pars.Mlist = m;

    ATermAppl true_tree,false_tree;
    ATermAppl r = NULL;
    ATermList readies = ATmakeList0();

    ATermList newstack = add_to_stack(pars.stack,l,&r,&readies);

    false_tree = build_tree(pars,i);

    if (r==NULL)
    {
      pars.stack = newstack;
      true_tree = build_tree(pars,i);
      for (; !ATisEmpty(readies); readies=ATgetNext(readies))
      {
        inc_usedcnt((ATermList) ATgetArgument(ATAgetFirst(readies),2));
        inc_usedcnt((ATermList) ATgetArgument(ATAgetFirst(readies),3));
        true_tree = ATmakeAppl3(afunC,ATgetArgument(ATAgetFirst(readies),0),(ATerm) ATmakeAppl1(afunR,ATgetArgument(ATAgetFirst(readies),1)),(ATerm) true_tree);
      }
    }
    else
    {
      inc_usedcnt((ATermList) ATgetArgument(r,1));
      true_tree = ATmakeAppl1(afunR,ATgetArgument(r,0));
    }

    if (ATisEqual(true_tree,false_tree))
    {
       return true_tree;
    }
    else
    {
      treevars_usedcnt[ATgetInt((ATermInt) ATgetArgument((ATermAppl) M,1))]++;
      return ATmakeAppl3(afunM,ATgetArgument((ATermAppl) M,0),(ATerm) true_tree,(ATerm) false_tree);
    }
  }
  else if (!ATisEmpty(pars.Flist))
  {
    ATermList F = ATLgetFirst(pars.Flist);
    ATermAppl true_tree,false_tree;

    ATermList newupstack = pars.upstack;
    ATermList l = ATmakeList0();

    for (; !ATisEmpty(pars.Flist); pars.Flist=ATgetNext(pars.Flist))
    {
      if (ATisEqual(ATgetFirst(ATLgetFirst(pars.Flist)),ATgetFirst(F)))
      {
        newupstack = ATinsert(newupstack, (ATerm) ATgetNext(ATLgetFirst(pars.Flist)));
      }
      else
      {
        l = ATinsert(l,ATgetFirst(pars.Flist));
      }
    }

    pars.Flist = l;
    false_tree = build_tree(pars,i);
    pars.Flist = ATmakeList0();
    pars.upstack = newupstack;
    true_tree = build_tree(pars,i);

    if (ATisEqual(true_tree,false_tree))
    {
      return true_tree;
    }
    else
    {
      return ATmakeAppl3(afunF,ATgetArgument(ATAgetFirst(F),0),(ATerm) true_tree,(ATerm) false_tree);
    }
  }
  else if (!ATisEmpty(pars.upstack))
  {
    ATermList l;

    ATermAppl r = NULL;
    ATermList readies = ATmakeList0();

    pars.stack = ATinsert(pars.stack,(ATerm) ATmakeList0());
    l = pars.upstack;
    pars.upstack = ATmakeList0();
    add_to_build_pars(&pars,l,&r,&readies);


    if (r==NULL)
    {
      ATermAppl t = build_tree(pars,i);

      for (; !ATisEmpty(readies); readies=ATgetNext(readies))
      {
        inc_usedcnt((ATermList) ATgetArgument(ATAgetFirst(readies),2));
        inc_usedcnt((ATermList) ATgetArgument(ATAgetFirst(readies),3));
        t = ATmakeAppl3(afunC,ATgetArgument(ATAgetFirst(readies),0),(ATerm) ATmakeAppl1(afunR,ATgetArgument(ATAgetFirst(readies),1)),(ATerm) t);
      }

      return t;
    }
    else
    {
      inc_usedcnt((ATermList) ATgetArgument(r,1));
      return ATmakeAppl1(afunR,ATgetArgument(r,0));
    }
  }
  else
  {
    if (ATisEmpty(ATLgetFirst(pars.stack)))
    {
      if (ATisEmpty(ATgetNext(pars.stack)))
      {
        return ATmakeAppl0(afunX);
      }
      else
      {
        pars.stack = ATgetNext(pars.stack);
        return ATmakeAppl1(afunD,(ATerm) build_tree(pars,i));
      }
    }
    else
    {
      ATermList l = ATLgetFirst(pars.stack);
      ATermAppl r = NULL;
      ATermList readies = ATmakeList0();

      pars.stack = ATinsert(ATgetNext(pars.stack),(ATerm) ATmakeList0());
      add_to_build_pars(&pars,l,&r,&readies);

      ATermAppl tree;
      if (r==NULL)
      {
        tree = build_tree(pars,i);
        for (; !ATisEmpty(readies); readies=ATgetNext(readies))
        {
          inc_usedcnt((ATermList) ATgetArgument(ATAgetFirst(readies),2));
          inc_usedcnt((ATermList) ATgetArgument(ATAgetFirst(readies),3));
          tree = ATmakeAppl3(afunC,ATgetArgument(ATAgetFirst(readies),0),(ATerm) ATmakeAppl1(afunR,ATgetArgument(ATAgetFirst(readies),1)),(ATerm) tree);
        }
      }
      else
      {
        inc_usedcnt((ATermList) ATgetArgument(r,1));
        tree = ATmakeAppl1(afunR,ATgetArgument(r,0));
      }

      return ATmakeAppl1(afunN,(ATerm) tree);
    }
  }
}

static ATermAppl create_tree(const data_equation_list rules, int /*opid*/, int /*arity*/, ATermInt true_inner)
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
  ATermList rule_seqs = ATmakeList0();
  int total_rule_vars = 0;
  for (data_equation_list::const_iterator it=rules.begin(); it!=rules.end(); ++it)
  {
    rule_seqs = ATinsert(rule_seqs, (ATerm) create_sequence(*it,&total_rule_vars, true_inner));
  }

  // Generate initial parameters for built_tree
  build_pars init_pars;
  ATermAppl r = NULL;
  ATermList readies = ATmakeList0();

  initialise_build_pars(&init_pars);
  add_to_build_pars(&init_pars,rule_seqs,&r,&readies);

  ATermAppl tree;
  if (r==NULL)
  {
    MCRL2_SYSTEM_SPECIFIC_ALLOCA(a,int,total_rule_vars);
    treevars_usedcnt = a;
    tree = build_tree(init_pars,0);
    for (; !ATisEmpty(readies); readies=ATgetNext(readies))
    {
      tree = ATmakeAppl3(afunC,ATgetArgument(ATAgetFirst(readies),0),(ATerm) ATmakeAppl1(afunR,ATgetArgument(ATAgetFirst(readies),1)),(ATerm) tree);
    }
  }
  else
  {
    tree = ATmakeAppl1(afunR,ATgetArgument(r,0));
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

  void operator ()(const variable &v)
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

static variable_list get_doubles(const data_expression t)
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
      result=atermpp::push_front(result,i->first);
    }
  }
  return result;
}


static variable_list get_vars(const data_expression &t)
{
  std::set < variable > s=find_free_variables(t);
  variable_list result;
  for(std::set < variable >::const_iterator i=s.begin(); i!=s.end(); ++i)
  {
    result=atermpp::push_front(result,*i);
  }
  return result;
}


static ATermList get_vars(ATerm a)
{
  if (ATisInt(a))
  {
    return ATmakeList0();
  }
  else if (ATisAppl(a) && gsIsDataVarId((ATermAppl) a))
  {
    return ATmakeList1(a);
  }
  else if (ATisList(a))
  {
    ATermList l = ATmakeList0();
    for (ATermList m=(ATermList) a; !ATisEmpty(m); m=ATgetNext(m))
    {
      l = ATconcat(get_vars(ATgetFirst(m)),l);
    }
    return l;
  }
  else     // ATisAppl(a)
  {
    ATermList l = ATmakeList0();
    int arity = ATgetArity(ATgetAFun((ATermAppl) a));
    for (int i=0; i<arity; ++i)
    {
      l = ATconcat(get_vars(ATgetArgument((ATermAppl) a,i)),l);
    }
    return l;
  }
}

static ATermList dep_vars(const data_equation eqn)
{
  size_t rule_arity = ATgetArity(ATgetAFun(toInner(eqn.lhs(),true)))-1;
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(bs,bool,rule_arity);

  ATermAppl pars = toInner(eqn.lhs(),true); // pars is actually the lhs of the equation.
  ATermList vars = ATmakeList1((ATerm) ATconcat(
                                 get_doubles(eqn.rhs()),
                                 get_vars(eqn.condition())
                               )); // List of variables occurring in each argument of the lhs
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
    if (!gsIsDataVarId(ATAgetArgument(pars,i+1)))
    {
      // Argument is not a variable, so it needs to be rewritten
      bs[i] = true;
      ATermList evars = get_vars(ATgetArgument(pars,i+1));
      for (; !ATisEmpty(evars); evars=ATgetNext(evars))
      {
        int j=i-1; // ATgetLength(ATgetNext(vars))-1
        for (ATermList o=ATgetNext(vars); !ATisEmpty(o); o=ATgetNext(o))
        {
          if (ATindexOf(ATLgetFirst(o),ATgetFirst(evars),0) != ATERM_NON_EXISTING_POSITION)
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
      int j = i-1; // ATgetLength(vars)-1-1
      bool b = false;
      for (ATermList o=vars; !ATisEmpty(o); o=ATgetNext(o))
      {
        if (ATindexOf(ATLgetFirst(o),ATgetArgument(pars,i+1),0) != ATERM_NON_EXISTING_POSITION)
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
    vars = ATinsert(vars,(ATerm) get_vars(ATgetArgument(pars,i+1)));
  }

  ATermList deps = ATmakeList0();
  for (size_t i = 0; i < rule_arity; i++)
  {
    if (bs[i] && gsIsDataVarId(ATAgetArgument(pars,i+1)))
    {
      deps = ATinsert(deps,ATgetArgument(pars,i+1));
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
size_t RewriterCompilingJitty::bound_variable_index(const variable v)
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
size_t RewriterCompilingJitty::binding_variable_list_index(const variable_list vl)
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

static ATermList create_strategy(
        const data_equation_list rules,
        const int opid,
        const size_t arity,
        nfs_array &nfs,
        ATermInt true_inner)
{
  ATermList strat = ATmakeList0();
  // Array to keep note of the used parameters
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(used,bool,arity);
  for (size_t i = 0; i < arity; i++)
  {
    used[i] = nfs.get(i);
  }

  // Maintain dependency count (i.e. the number of rules that depend on a given argument)
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,int,arity);
  for (size_t i = 0; i < arity; i++)
  {
    args[i] = -1;
  }

  // Process all (applicable) rules
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(bs,bool,arity);
  ATermList dep_list = ATmakeList0();
  for (data_equation_list::const_iterator it=rules.begin(); it!=rules.end(); ++it)
  {
    size_t rule_arity = ATgetArity(ATgetAFun(toInner(it->lhs(),true)))-1;
    if (rule_arity > arity)
    {
      continue;
    }

    ATermAppl pars = toInner(it->lhs(),true);  // the lhs, pars is an odd name.
    ATermList vars = ATmakeList1((ATerm) ATconcat(
                                   get_doubles(it->rhs()),
                                   get_vars(it->condition())
                                 )); // List of variables occurring in each argument of the lhs
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
      if (!gsIsDataVarId(ATAgetArgument(pars,i+1)))
      {
        // Argument is not a variable, so it needs to be rewritten
        bs[i] = true;
        ATermList evars = get_vars(ATgetArgument(pars,i+1));
        for (; !ATisEmpty(evars); evars=ATgetNext(evars))
        {
          int j=i-1;
          for (ATermList o=vars; !ATisEmpty(ATgetNext(o)); o=ATgetNext(o))
          {
            if (ATindexOf(ATLgetFirst(o),ATgetFirst(evars),0) != ATERM_NON_EXISTING_POSITION)
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
        int j = i-1; // ATgetLength(vars)-1-1
        bool b = false;
        for (ATermList o=vars; !ATisEmpty(o); o=ATgetNext(o))
        {
          if (ATindexOf(ATLgetFirst(o),ATgetArgument(pars,i+1),0) != ATERM_NON_EXISTING_POSITION)
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
      vars = ATinsert(vars,(ATerm) get_vars(ATgetArgument(pars,i+1)));
    }

    // Create dependency list for this rule
    ATermList deps = ATmakeList0();
    for (size_t i = 0; i < rule_arity; i++)
    {
      // Only if needed and not already rewritten
      if (bs[i] && !used[i])
      {
        deps = ATinsert(deps,(ATerm) ATmakeInt(i));
        // Increase dependency count
        args[i] += 1;
        //fprintf(stderr,"dep of arg %i\n",i);
      }
    }
    deps = ATreverse(deps);

    // Add rule with its dependencies
    dep_list = ATinsert(dep_list,(ATerm) ATmakeList2((ATerm) deps, (ATerm)(ATermAppl)*it));
  }

  // Process all rules with their dependencies
  while (1)
  {
    // First collect rules without dependencies to the strategy
    data_equation_list no_deps = ATmakeList0();
    ATermList has_deps = ATmakeList0();
    for (; !ATisEmpty(dep_list); dep_list=ATgetNext(dep_list))
    {
      if (ATisEmpty(ATLgetFirst(ATLgetFirst(dep_list))))
      {
        no_deps = push_front(no_deps, data_equation(ATgetFirst(ATgetNext(ATLgetFirst(dep_list)))));
      }
      else
      {
        has_deps = ATinsert(has_deps,ATgetFirst(dep_list));
      }
    }
    dep_list = ATreverse(has_deps);

    // Create and add tree of collected rules
    if (!no_deps.empty())
    {
      strat = ATinsert(strat, (ATerm) create_tree(no_deps,opid,arity,true_inner));
    }

    // Stop if there are no more rules left
    if (ATisEmpty(dep_list))
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
      ATermInt rewr_arg = ATmakeInt(maxidx);

      strat = ATinsert(strat,(ATerm) rewr_arg);

      ATermList l = ATmakeList0();
      for (; !ATisEmpty(dep_list); dep_list=ATgetNext(dep_list))
      {
        l = ATinsert(l,(ATerm) ATinsert(ATgetNext(ATLgetFirst(dep_list)),
                   (ATerm) ATremoveElement(ATLgetFirst(ATLgetFirst(dep_list)),(ATerm) rewr_arg)));
      }
      dep_list = ATreverse(l);
    }
  }

  return ATreverse(strat);
}

void RewriterCompilingJitty::add_base_nfs(nfs_array &nfs, const atermpp::aterm_int opid, size_t arity)
{
  for (size_t i=0; i<arity; i++)
  {
    if (always_rewrite_argument(opid,arity,i))
    {
      nfs.set(i);
    }
  }
}

void RewriterCompilingJitty::extend_nfs(nfs_array &nfs, const atermpp::aterm_int opid, size_t arity)
{
  data_equation_list eqns = (size_t(opid.value())<jittyc_eqns.size()?jittyc_eqns[opid.value()]:data_equation_list());
  if (eqns.empty())
  {
    nfs.fill(arity);
    return;
  }
  ATermList strat = create_strategy(eqns,opid.value(),arity,nfs,true_inner);
  while (!ATisEmpty(strat) && ATisInt(ATgetFirst(strat)))
  {
    nfs.set(ATgetInt((ATermInt) ATgetFirst(strat)));
    strat = ATgetNext(strat);
  }
}

// Determine whether the opid is a normal form, with the given number of arguments.
bool RewriterCompilingJitty::opid_is_nf(const atermpp::aterm_int opid, size_t num_args)
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
  data_equation_list l = (size_t(opid.value())<jittyc_eqns.size()?jittyc_eqns[opid.value()]:data_equation_list());

  if (l.empty())
  {
    return true;
  }

  for (data_equation_list::const_iterator it=l.begin(); it!=l.end(); ++it)
  {
    if (ATgetArity(ATgetAFun(toInner(it->lhs(),true)))-1 <= num_args)
    {
      return false;
    }
  }

  return true;
}

void RewriterCompilingJitty::calc_nfs_list(nfs_array &nfs, size_t arity, ATermList args, int startarg, ATermList nnfvars)
{
  if (ATisEmpty(args))
  {
    return;
  }

  nfs.set(arity-ATgetLength(args),calc_nfs(ATgetFirst(args),startarg,nnfvars));
  calc_nfs_list(nfs,arity,ATgetNext(args),startarg+1,nnfvars);
}

bool RewriterCompilingJitty::calc_nfs(ATerm t, int startarg, ATermList nnfvars)
{
  if (ATisList(t))
  {
    int arity = ATgetLength((ATermList) t)-1;
    if (ATisInt(ATgetFirst((ATermList) t)))
    {
      if (opid_is_nf((ATermInt) ATgetFirst((ATermList) t),arity) && arity != 0)
      {
        nfs_array args(arity);
        calc_nfs_list(args,arity,ATgetNext((ATermList) t),startarg,nnfvars);
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
        return calc_nfs(ATgetFirst((ATermList) t), startarg, nnfvars);
      }
      return false;
    }
  }
  else if (ATisInt(t))
  {
    return opid_is_nf((ATermInt) t,0);
  }
  else if (/*ATisAppl(t) && */ gsIsNil((ATermAppl) t))
  {
    return (nnfvars==NULL) || (ATindexOf(nnfvars,(ATerm) ATmakeInt(startarg),0) == ATERM_NON_EXISTING_POSITION);
  }
  else if (/* ATisAppl(t) && */ gsIsDataVarId((ATermAppl) t))
  {
    assert(ATisAppl(t) && gsIsDataVarId((ATermAppl) t));
    return (nnfvars==NULL) || (ATindexOf(nnfvars,t,0) == ATERM_NON_EXISTING_POSITION);
  }
  else if (is_abstraction(t))
  {
    assert(ATisAppl(t));
    return false; // I assume that lambda, forall and exists are not in normal form by default.
                  // This might be too weak, and may require to be reinvestigated later.
  }
  else
  {
    assert(ATisAppl(t) && is_where_clause(t));
    return false; // I assume that a where clause is not in normal form by default.
                  // This might be too weak, and may require to be reinvestigated later.
  }
}

string RewriterCompilingJitty::calc_inner_terms(nfs_array &nfs, size_t arity, ATermList args, int startarg, ATermList nnfvars, nfs_array *rewr)
{
  if (ATisEmpty(args))
  {
    return "";
  }

  pair<bool,string> head = calc_inner_term(ATgetFirst(args),startarg,nnfvars,rewr?(rewr->get(arity-ATgetLength(args))):false,arity);
  nfs.set(arity-ATgetLength(args),head.first);
  string tail = calc_inner_terms(nfs,arity,ATgetNext(args),startarg+1,nnfvars,rewr);
  return head.second+(ATisEmpty(ATgetNext(args))?"":",")+tail;
}

static string calc_inner_appl_head(size_t arity)
{
  stringstream ss;
  if (arity <= 5)
  {
    ss << "makeAppl" << arity;
  }
  else
  {
    ss << "ATmakeAppl";
  }
  ss << "(" << ((long int) get_appl_afun_value(arity+1)) << ",";    // YYYY
  return ss.str();
}

// if total_arity<=5 a term of type atermpp::aterm is generated, otherwise a term of type ATermAppl is generated.
pair<bool,string> RewriterCompilingJitty::calc_inner_term(
                  ATerm t,
                  int startarg,
                  ATermList nnfvars,
                  const bool rewr,
                  const size_t total_arity)
{
  if (ATisList(t))
  {
    stringstream ss;
    bool b;
    size_t arity = ATgetLength((ATermList) t)-1;


    if (ATisInt(ATgetFirst((ATermList) t)))
    {
      b = opid_is_nf((ATermInt) ATgetFirst((ATermList) t),arity);

      if (b || !rewr)
      {
        if (total_arity>5)
        {
          ss << "(ATermAppl)";
        }
        ss << calc_inner_appl_head(arity);
      }

      if (arity == 0)
      {
        if (b || !rewr)
        {
          ss << "atermpp::aterm((_ATerm*) " << (void*) get_int2aterm_value((ATermInt) ATgetFirst((ATermList) t)) << "))";
        }
        else
        {
          ss << "rewr_" << ATgetInt((ATermInt) ATgetFirst((ATermList) t)) << "_0_0()";
        }
      }
      else
      {
        // arity != 0
        nfs_array args_nfs(arity);
        calc_nfs_list(args_nfs,arity,ATgetNext((ATermList) t),startarg,nnfvars);
        if (!(b || !rewr))
        {
          if (arity<=5)
          {
            ss << "rewr_";
          }
          else
          {
            ss << "(ATermAppl)rewr_";
          }
          add_base_nfs(args_nfs,(ATermInt) ATgetFirst((ATermList) t),arity);
          extend_nfs(args_nfs,(ATermInt) ATgetFirst((ATermList) t),arity);
        }
        if (arity > NF_MAX_ARITY)
        {
          args_nfs.clear(arity);
        }
        if (args_nfs.is_clear(arity) || b || rewr || (arity > NF_MAX_ARITY))
        {
          if (b || !rewr)
          {
            if (arity<=5)
            {
              ss << "atermpp::aterm((_ATerm*) " << (void*) get_int2aterm_value((ATermInt) ATgetFirst((ATermList) t)) << ")";
            }
            else
            {
              ss << "(ATermAppl)(_ATerm*) " << (void*) get_int2aterm_value((ATermInt) ATgetFirst((ATermList) t));
            }
          }
          else
            ss << ATgetInt((ATermInt) ATgetFirst((ATermList) t));
        }
        else
        {
          if (b || !rewr)
          {
            const int index=ATgetInt((ATermInt) ATgetFirst((ATermList) t));
            const function_symbol old_head=get_int2term(index);
            std::stringstream new_name;
            new_name << "@_rewr" << "_" << index << "_" << ATgetLength((ATermList)t)-1 << "_" << args_nfs.get_value(arity)
                                 << "_term";
            const function_symbol f(new_name.str(),old_head.sort());
            if (partially_rewritten_functions.count(f)==0)
            {
              OpId2Int(f);
              partially_rewritten_functions.insert(f);
            }
            if (arity<=5)
            {
              /* Exclude adding an extra increase of the OpId index, which refers to an OpId that is not available.
                 The intention of this increase is to generate an index of an OpId of which it is indicated in args_nfs
                 which arguments are guaranteed to be in normal form. For these variables it is not necessary anymore
                 to recalculate the normal form. TODO: this needs to be reconstructed. */
              /* ss << "atermpp::aterm((ATerm) " << (void*) get_int2aterm_value(ATgetInt((ATermInt) ATgetFirst((ATermList) t))
                               + ((1 << arity)-arity-1)+args_nfs.get_value(arity) ) << ")"; */
              ss << "atermpp::aterm((_ATerm*) " << (void*) get_int2aterm_value(ATgetInt(OpId2Int(f))) << ")";
            }
            else
            {
              /* See the remark above. */
              /* ss << "(ATermAppl) " << (void*) get_int2aterm_value(ATgetInt((ATermInt) ATgetFirst((ATermList) t))
                                 + ((1 << arity)-arity-1)+args_nfs.get_value(arity) ) << ""; */
              ss << "(ATermAppl)(_ATerm*) " << (void*) get_int2aterm_value(ATgetInt(OpId2Int(f))) << "";
            }

          }
          else
          {
            // QUE?! Dit stond er vroeger // Sjoerd
            //   ss << (ATgetInt((ATermInt) ATgetFirst((ATermList) t))+((1 << arity)-arity-1)+args_nfs);
            ss << (ATgetInt((ATermInt) ATgetFirst((ATermList) t))+((1 << arity)-arity-1)+args_nfs.getraw(0));
          }
        }
        nfs_array args_first(arity);
        if (rewr && b)
        {
          args_nfs.fill(arity);
        }
        string args_second = calc_inner_terms(args_first,arity,ATgetNext((ATermList) t),startarg,nnfvars,&args_nfs);
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
      // !ATisInt(ATgetFirst((ATermList) t)). So the first element of this list is
      // either a variable, or a lambda term. It cannot be a forall or exists, because in
      // that case there would be a type error.
      if (arity == 0)
      {
        assert(false);
        // return calc_inner_term(ATgetFirst((ATermList) t), startarg, nnfvars,true,total_arity);
      }
      if (is_abstraction(ATgetFirst((ATermList) t)))
      {
        atermpp::aterm_appl lambda_term=ATgetFirst((ATermList) t);
        assert(lambda_term(0)=gsMakeLambda());

        b = rewr;
        nfs_array args_nfs(arity);
        calc_nfs_list(args_nfs,arity,ATgetNext((ATermList) t),startarg,nnfvars);
        if (arity > NF_MAX_ARITY)
        {
          args_nfs.clear(arity);
        }

        nfs_array args_first(arity);
        if (rewr && b)
        {
          args_nfs.fill(arity);
        }

        pair<bool,string> head = calc_inner_term((ATerm)(ATermAppl)lambda_term,startarg,nnfvars,false,arity);

        if (rewr)
        {
          ss << "rewrite(";
        }

        ss << calc_inner_appl_head(arity) << head.second << "," <<
                 calc_inner_terms(args_first,arity,ATgetNext((ATermList) t),startarg,nnfvars,&args_nfs) << ")";
        if (rewr)
        {
          ss << ")";
        }

      }
      else
      {
        // So, t must be a single variable.
        assert(gsIsDataVarId(ATgetFirst((ATermList) t)));
        b = rewr;
        pair<bool,string> head = calc_inner_term(ATgetFirst((ATermList) t),startarg,nnfvars,false,arity);
        nfs_array tail_first(arity);
        string tail_second = calc_inner_terms(tail_first,arity,ATgetNext((ATermList) t),startarg,nnfvars,NULL);
        if (arity>5)
        {
          ss << "(ATermAppl)";
        }
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
        if (rewr && (nnfvars!=NULL) && (ATindexOf(nnfvars,(ATerm) ATmakeInt(startarg),0) != ATERM_NON_EXISTING_POSITION))
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
          tail_second = calc_inner_terms(tail_first,arity,ATgetNext((ATermList) t),startarg,nnfvars,&rewrall);
        }
        ss << tail_second << ")";
        {
          ss << ")";
        }
      }
    }

    return pair<bool,string>(b,ss.str());

  }
  else if (ATisInt(t))
  {
    stringstream ss;
    bool b = opid_is_nf((ATermInt) t,0);
    if (total_arity>5)
    {
      ss << "(ATermAppl)";
    }
    if (rewr && !b)
    {
      ss << "rewr_" << ATgetInt((ATermInt) t) << "_0_0()";
    }
    else
    {
      ss << "atermpp::aterm_appl((_ATerm*) " << (void*) get_rewrappl_value((ATermInt) t) << ")";
    }
    return pair<bool,string>(
             rewr || b,
             ss.str()
           );

  }
  else if (gsIsNil((ATermAppl) t))
  {
    stringstream ss;
    if (total_arity>5)
    {
      ss << "(ATermAppl)";
    }
    bool b = (nnfvars!=NULL) && (ATindexOf(nnfvars,(ATerm) ATmakeInt(startarg),0) != ATERM_NON_EXISTING_POSITION);
    if (rewr && b)
    {
      ss << "rewrite(arg" << startarg << ")";
    }
    else
    {
      ss << "arg" << startarg;
    }
    return pair<bool,string>(rewr || !b, ss.str());

  }
  else if (gsIsDataVarId((ATermAppl)t))
  {
    assert(ATisAppl(t));
    stringstream ss;
    if (total_arity>5)
    {
      ss << "(ATermAppl)";
    }
    const bool b = (nnfvars!=NULL) && (ATindexOf(nnfvars,t,0) != ATERM_NON_EXISTING_POSITION);
    const variable v=variable((ATermAppl)t);
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
    stringstream ss;
    if (ATAgetArgument((ATermAppl)t,0)==gsMakeLambda())
    {
      if (rewr)
      {
        // The resulting term must be rewritten.
        pair<bool,string> r=calc_inner_term(ATgetArgument((ATermAppl)t,2),startarg,nnfvars,true,total_arity);
        ss << "this_rewriter->rewrite_single_lambda(" <<
               "this_rewriter->binding_variable_list_get(" << binding_variable_list_index(variable_list(ATgetArgument((ATermAppl)t,1))) << ")," <<
               r.second << "," << r.first << ",*(this_rewriter->global_sigma))";
        return pair<bool,string>(true,ss.str());
      }
      else
      {
        pair<bool,string> r=calc_inner_term(ATgetArgument((ATermAppl)t,2),startarg,nnfvars,false,total_arity);
        ss << "mcrl2::core::detail::gsMakeBinder(mcrl2::core::detail::gsMakeLambda()," <<
               "this_rewriter->binding_variable_list_get(" << binding_variable_list_index(variable_list(ATgetArgument((ATermAppl)t,1))) << ")," <<
               r.second << ")";
        return pair<bool,string>(false,ss.str());
      }
    }
    else if (ATAgetArgument((ATermAppl)t,0)==gsMakeForall())
    {
      if (rewr)
      {
        // A result in normal form is requested.
        pair<bool,string> r=calc_inner_term(ATgetArgument((ATermAppl)t,2),startarg,nnfvars,false,total_arity);
        ss << "this_rewriter->internal_universal_quantifier_enumeration(" <<
               "this_rewriter->binding_variable_list_get(" << binding_variable_list_index(variable_list(ATgetArgument((ATermAppl)t,1))) << ")," <<
               r.second << "," << r.first << "," << "*(this_rewriter->global_sigma))";
        return pair<bool,string>(true,ss.str());
      }
      else
      {
        // A result which is not a normal form is requested.
        pair<bool,string> r=calc_inner_term(ATgetArgument((ATermAppl)t,2),startarg,nnfvars,false,total_arity);
        ss << "mcrl2::core::detail::gsMakeBinder(mcrl2::core::detail::gsMakeForall()," <<
               "this_rewriter->binding_variable_list_get(" << binding_variable_list_index(variable_list(ATgetArgument((ATermAppl)t,1))) << ")," <<
               r.second << ")";
        return pair<bool,string>(false,ss.str());
      }
    }
    else if (ATAgetArgument((ATermAppl)t,0)==gsMakeExists())
    {
      if (rewr)
      {
        // A result in normal form is requested.
        pair<bool,string> r=calc_inner_term(ATgetArgument((ATermAppl)t,2),startarg,nnfvars,false,total_arity);
        ss << "this_rewriter->internal_existential_quantifier_enumeration(" <<
               "this_rewriter->binding_variable_list_get(" << binding_variable_list_index(variable_list(ATgetArgument((ATermAppl)t,1))) << ")," <<
               r.second << "," << r.first << "," << "*(this_rewriter->global_sigma))";
        return pair<bool,string>(true,ss.str());
      }
      else
      {
        // A result which is not a normal form is requested.
        pair<bool,string> r=calc_inner_term(ATgetArgument((ATermAppl)t,2),startarg,nnfvars,false,total_arity);
        ss << "mcrl2::core::detail::gsMakeBinder(mcrl2::core::detail::gsMakeExists()," <<
               "this_rewriter->binding_variable_list_get(" << binding_variable_list_index(variable_list(ATgetArgument((ATermAppl)t,1))) << ")," <<
               r.second << ")";
        return pair<bool,string>(false,ss.str());
      }
    }
  }
  else if (is_where_clause(t))
  {

    stringstream ss;
    const ATermAppl w=(ATermAppl)t; // w is the where clause

    if (rewr)
    {
      // A rewritten result is expected.
      pair<bool,string> r=calc_inner_term(ATgetArgument(w,0),startarg,nnfvars,true,total_arity);

      // TODO REMOVE gsMakeWhr.
      ss << "this_rewriter->rewrite_where(mcrl2::core::detail::gsMakeWhr(" << r.second << ",";

      ATermList assignments=ATLgetArgument(w,1);
      for( size_t no_opening_brackets=ATgetLength(assignments); no_opening_brackets>0 ; no_opening_brackets--)
      {
        ss << "ATinsert(";
      }
      ss << "ATempty";
      for( ; !ATisEmpty(assignments) ; assignments=ATgetNext(assignments))
      {
        ATermAppl assignment=ATAgetFirst(assignments);
        pair<bool,string> r=calc_inner_term(ATgetArgument(assignment,1),startarg,nnfvars,true,total_arity);
        ss << ",(ATerm)mcrl2::core::detail::gsMakeDataVarIdInit(" <<
                 "this_rewriter->bound_variable_get(" << bound_variable_index(variable(ATAgetArgument(assignment,0))) << ")," <<
                 r.second << "))";
      }

      ss << "),*(this_rewriter->global_sigma))";

      return pair<bool,string>(true,ss.str());
    }
    else
    {
      // The result does not need to be rewritten.
      pair<bool,string> r=calc_inner_term(ATgetArgument(w,0),startarg,nnfvars,false,total_arity);
      ss << "mcrl2::core::detail::gsMakeWhr(" << r.second << ",";

      ATermList assignments=ATLgetArgument(w,1);
      for( size_t no_opening_brackets=ATgetLength(assignments); no_opening_brackets>0 ; no_opening_brackets--)
      {
        ss << "ATinsert(";
      }
      ss << "ATempty";
      for( ; !ATisEmpty(assignments) ; assignments=ATgetNext(assignments))
      {
        ATermAppl assignment=ATAgetFirst(assignments);
        pair<bool,string> r=calc_inner_term(ATgetArgument(assignment,1),startarg,nnfvars,true,total_arity);
        ss << ",(ATerm)mcrl2::core::detail::gsMakeDataVarIdInit(" <<
                 "this_rewriter->bound_variable_get(" << bound_variable_index(variable(ATAgetArgument(assignment,0))) << ")," <<
                 r.second << "))";
      }

      ss << ")";

      return pair<bool,string>(false,ss.str());
    }
  }
  assert(0);
  return pair<bool,string>(false,"----");
}

void RewriterCompilingJitty::calcTerm(FILE* f, ATerm t, int startarg, ATermList nnfvars, bool rewr)
{
  pair<bool,string> p = calc_inner_term(t,startarg,nnfvars,rewr,0);
  fprintf(f,"%s",p.second.c_str());
  return;
}

static ATerm add_args(ATerm a, int num)
{
  if (num == 0)
  {
    return a;
  }
  else
  {
    ATermList l;

    if (ATisList(a))
    {
      l = (ATermList) a;
    }
    else
    {
      l = ATmakeList1(a);
    }

    while (num > 0)
    {
      l = ATappend(l,(ATerm) gsMakeNil());
      num--;
    }
    return (ATerm) l;
  }
}

static int get_startarg(ATerm a, int n)
{
  if (ATisList(a))
  {
    return n-ATgetLength((ATermList) a)+1;
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
void RewriterCompilingJitty::implement_tree_aux(FILE* f, ATermAppl tree, int cur_arg, int parent, int level, int cnt, int d, int arity, bool* used, ATermList nnfvars)
// Print code representing tree to f.
//
// cur_arg   Indices refering to the variable that contains the current
// parent    term. For level 0 this means arg<cur_arg>, for level 1 it
//           means ATgetArgument(arg<parent>,<cur_arg) and for higher
//           levels it means ATgetArgument(t<parent>,<cur_arg>)
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
      fprintf(f,"%sconst atermpp::aterm_appl %s = arg%i; // S\n",whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(tree,0),0)))+1,cur_arg);
      if (!used[cur_arg])
      {
        nnfvars = ATinsert(nnfvars,ATgetArgument(tree,0));
      }
    }
    else
    {
      fprintf(f,"%sconst atermpp::aterm_appl %s = atermpp::aterm_appl(%s%i(%i)); // S\n",whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(tree,0),0)))+1,(level==1)?"arg":"t",parent,cur_arg);
    }
    implement_tree_aux(f,ATAgetArgument(tree,1),cur_arg,parent,level,cnt,d,arity,used,nnfvars);
    return;
  }
  else if (isM(tree))
  {
    if (level == 0)
    {
      fprintf(f,"%sif (%s==arg%i) // M\n"
              "%s{\n",
              whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(tree,0),0)))+1,cur_arg,
              whitespace(d*2)
             );
    }
    else
    {
      fprintf(f,"%sif (%s==%s%i(%i)) // M\n"
              "%s{\n",
              whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(tree,0),0)))+1,(level==1)?"arg":"t",parent,cur_arg,
              whitespace(d*2)
             );
    }
    implement_tree_aux(f,ATAgetArgument(tree,1),cur_arg,parent,level,cnt,d+1,arity,used,nnfvars);
    fprintf(f,"%s}\n%selse\n%s{\n",whitespace(d*2),whitespace(d*2),whitespace(d*2));
    implement_tree_aux(f,ATAgetArgument(tree,2),cur_arg,parent,level,cnt,d+1,arity,used,nnfvars);
    fprintf(f,"%s}\n",whitespace(d*2));
    return;
  }
  else if (isF(tree))
  {
    if (level == 0)
    {
      // TODO: if parent was also an F of the same level we can omit isAppl
      fprintf(f,"%sif (arg%i(0)==atermpp::aterm((_ATerm*) %p)) // F\n"
              "%s{\n",
              whitespace(d*2),
              cur_arg,
              (void*)get_int2aterm_value((ATermInt) ATgetArgument(tree,0)),
              whitespace(d*2)
             );
    }
    else
    {
      fprintf(f,"%sif (isAppl(%s%i(%i)) && atermpp::aterm_appl(%s%i(%i))(0)==atermpp::aterm((_ATerm*) %p)) // F\n"
              "%s{\n"
              "%s  atermpp::aterm_appl t%i = %s%i(%i);\n",
              whitespace(d*2),
              (level==1)?"arg":"t",parent,cur_arg,
              (level==1)?"arg":"t",parent,cur_arg,
              (void*)get_int2aterm_value((ATermInt) ATgetArgument(tree,0)),
              whitespace(d*2),
              whitespace(d*2),cnt,(level==1)?"arg":"t",parent,cur_arg
             );
    }
    push_st(cur_arg);
    push_st(parent);
    implement_tree_aux(f,ATAgetArgument(tree,1),1,(level==0)?cur_arg:cnt,level+1,cnt+1,d+1,arity,used,nnfvars);
    pop_st();
    pop_st();
    fprintf(f,"%s}\n%selse\n%s{\n",whitespace(d*2),whitespace(d*2),whitespace(d*2));
    implement_tree_aux(f,ATAgetArgument(tree,2),cur_arg,parent,level,cnt,d+1,arity,used,nnfvars);
    fprintf(f,"%s}\n",whitespace(d*2));
    return;
  }
  else if (isD(tree))
  {
    int i = pop_st();
    int j = pop_st();
    implement_tree_aux(f,ATAgetArgument(tree,0),j,i,level-1,cnt,d,arity,used,nnfvars);
    push_st(j);
    push_st(i);
    return;
  }
  else if (isN(tree))
  {
    implement_tree_aux(f,ATAgetArgument(tree,0),cur_arg+1,parent,level,cnt,d,arity,used,nnfvars);
    return;
  }
  else if (isC(tree))
  {
    fprintf(f,"%sif (",whitespace(d*2));
    calcTerm(f,ATgetArgument(tree,0),0,nnfvars);

    fprintf(f,"==atermpp::aterm_appl((_ATerm*) %p)) // C\n"
            "%s{\n",
            (void*)get_rewrappl_value(true_num),
            whitespace(d*2)
           );
    implement_tree_aux(f,ATAgetArgument(tree,1),cur_arg,parent,level,cnt,d+1,arity,used,nnfvars);
    fprintf(f,"%s}\n%selse\n%s{\n",whitespace(d*2),whitespace(d*2),whitespace(d*2));
    implement_tree_aux(f,ATAgetArgument(tree,2),cur_arg,parent,level,cnt,d+1,arity,used,nnfvars);
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
    calcTerm(f,add_args(ATgetArgument(tree,0),arity-cur_arg-1),get_startarg(ATgetArgument(tree,0),cur_arg+1),nnfvars);
    fprintf(f,"; // R\n");
    return;
  }
  else
  {
    return;
  }
}

void RewriterCompilingJitty::implement_tree(FILE* f, ATermAppl tree, int arity, int d, int /* opid */, bool* used)
{
  int l = 0;

  ATermList nnfvars = ATmakeList0();
  for (int i=0; i<arity; i++)
  {
    if (!used[i])
    {
      nnfvars = ATinsert(nnfvars,(ATerm) ATmakeInt(i));
    }
  }

  while (isC(tree))
  {
    fprintf(f,"%sif (",whitespace(d*2));
    calcTerm(f,ATgetArgument(tree,0),0,ATmakeList0());

    fprintf(f,"==atermpp::aterm_appl((_ATerm*) %p)) // C\n"
            "%s{\n"
            "%sreturn ",
            (void*)get_rewrappl_value(true_num),
            whitespace(d*2),
            whitespace(d*2)
           );
    assert(isR(ATAgetArgument(tree,1)));
    calcTerm(f,add_args(ATgetArgument(ATAgetArgument(tree,1),0),arity),get_startarg(ATgetArgument(ATAgetArgument(tree,1),0),0),nnfvars);
    fprintf(f,";\n"
            "%s}\n%selse\n%s{\n", whitespace(d*2),whitespace(d*2),whitespace(d*2)
           );
    tree = ATAgetArgument(tree,2);
    d++;
    l++;
  }
  if (isR(tree))
  {
    fprintf(f,"%sreturn ",whitespace(d*2));
    calcTerm(f,add_args(ATgetArgument(tree,0),arity),get_startarg(ATgetArgument(tree,0),0),nnfvars);
    fprintf(f,"; // R\n");
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

static void finish_function(FILE* f, size_t arity, int opid, bool* used)
{
  if (arity == 0)
  {
    fprintf(f,  "  return (atermpp::aterm_appl((_ATerm*) %p)",
            (void*)get_rewrappl_value(opid)
           );
  }
  else
  {
    if (arity > 5)
    {
      fprintf(f,  "  return ATmakeAppl("
              "%li,"
              "&*(_ATerm*)%p",
              (long int) get_appl_afun_value(arity+1),   // YYYY
              (void*)get_int2aterm_value(opid)
             );
    }
    else
    {
      fprintf(f,  "  return ATmakeAppl%zu("
              "%li,"
              "(_ATerm*) %p",
              arity+1,
              (long int) get_appl_afun_value(arity+1),    // YYYY
              (void*)get_int2aterm_value(opid)
             );
    }
  }
  for (size_t i=0; i<arity; i++)
  {
    if ((used != NULL) && used[i])
    {
      if (arity > 5)
      {
        fprintf(f,                 ",(_ATerm*)&*(ATermAppl) arg%zu",i);
      }
      else
      {
        fprintf(f,                 ",(ATerm)(ATermAppl) arg%zu",i);
      }
    }
    else
    {
      if (arity > 5)
      {
        fprintf(f,                 ",(_ATerm*)&*(ATermAppl) rewrite(arg%zu)",i);
      }
      else
      {
        fprintf(f,                 ",(ATerm)(ATermAppl) rewrite(arg%zu)",i);
      }
    }
  }
  fprintf(f,                 ");\n");
}

void RewriterCompilingJitty::implement_strategy(FILE* f, ATermList strat, int arity, int d, int opid, size_t nf_args)
{
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(used,bool,arity);
  for (int i=0; i<arity; i++)
  {
    used[i] = ((nf_args & (1 << i)) != 0);
  }

  while (!ATisEmpty(strat))
  {
    if (ATisInt(ATgetFirst(strat)))
    {
      int arg = ATgetInt((ATermInt) ATgetFirst(strat));

      if (!used[arg])
      {
        fprintf(f,"%sarg%i = rewrite(arg%i);\n",whitespace(2*d),arg,arg);

        used[arg] = true;
      }
    }
    else
    {
      fprintf(f,"%s{\n",whitespace(2*d));
      implement_tree(f,(ATermAppl) ATgetFirst(strat),arity,d+1,opid,used);
      fprintf(f,"%s}\n",whitespace(2*d));
    }

    strat = ATgetNext(strat);
  }

  finish_function(f,arity,opid,used);
}

ATermAppl RewriterCompilingJitty::build_ar_expr(ATerm expr, ATermAppl var)
{
  if (ATisInt(expr))
  {
    return make_ar_false();
  }

  if (ATisAppl(expr) && gsIsDataVarId((ATermAppl) expr))
  {
    if (ATisEqual(expr,var))
    {
      return make_ar_true();
    }
    else
    {
      return make_ar_false();
    }
  }

  ATerm head = ATgetFirst((ATermList) expr);
  if (!ATisInt(head))
  {
    return ATisEqual(head,var)?make_ar_true():make_ar_false();
  }

  ATermAppl result = make_ar_false();

  ATermList args = ATgetNext((ATermList) expr);
  size_t arity = ATgetLength(args);
  for (size_t i=0; i<arity; i++, args=ATgetNext(args))
  {
    int idx = int2ar_idx[atermpp::aterm_int(head).value()] + ((arity-1)*arity)/2 + i;
    ATermAppl t = build_ar_expr(ATgetFirst(args),var);
    result = make_ar_or(result,make_ar_and(make_ar_var(idx),t));
  }

  return result;
}

ATermAppl RewriterCompilingJitty::build_ar_expr_aux(const data_equation eqn, const size_t arg, const size_t arity)
{
  atermpp::aterm_appl lhs = toInner(eqn.lhs(),true); // the lhs in internal format.

  size_t eqn_arity = ATgetArity(ATgetAFun(lhs))-1;
  if (eqn_arity > arity)
  {
    return make_ar_true();
  }
  if (eqn_arity <= arg)
  {
    ATerm rhs = toInner_list_odd(eqn.rhs());  // rhs in special internal list format.
    if (ATisInt(rhs))
    {
      int idx = int2ar_idx[atermpp::aterm_int(rhs).value()] + ((arity-1)*arity)/2 + arg;
      return make_ar_var(idx);
    }
    else if (ATisList(rhs) && ATisInt(ATgetFirst((ATermList) rhs)))
    {
      int rhs_arity = ATgetLength((ATermList) rhs)-1;
      size_t diff_arity = arity-eqn_arity;
      int rhs_new_arity = rhs_arity+diff_arity;
      size_t idx = int2ar_idx[atermpp::aterm_int(ATgetFirst((ATermList) rhs)).value()] +
                         ((rhs_new_arity-1)*rhs_new_arity)/2 + (arg - eqn_arity + rhs_arity);
      return make_ar_var(idx);
    }
    else
    {
      return make_ar_false();
    }
  }

  ATermAppl arg_term = ATAgetArgument(lhs,arg+1);
  if (!gsIsDataVarId(arg_term))
  {
    return make_ar_true();
  }

  if (ATindexOf(dep_vars(eqn),(ATerm) arg_term,0) != ATERM_NON_EXISTING_POSITION)
  {
    return make_ar_true();
  }

  return build_ar_expr(toInner_list_odd(eqn.rhs()),arg_term);
}

ATermAppl RewriterCompilingJitty::build_ar_expr(const data_equation_list eqns, const size_t arg, const size_t arity)
{
  if (eqns.empty())
  {
    return make_ar_true();
  }
  else
  {
    return make_ar_and(build_ar_expr_aux(eqns.front(),arg,arity),build_ar_expr(pop_front(eqns),arg,arity));
  }
}

bool RewriterCompilingJitty::always_rewrite_argument(
     const atermpp::aterm_int opid,
     const size_t arity,
     const size_t arg)
{
  return !is_ar_false(ar[int2ar_idx[atermpp::aterm_int(opid).value()]+((arity-1)*arity)/2+arg]);
}

bool RewriterCompilingJitty::calc_ar(ATermAppl expr)
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
    return calc_ar(ATAgetArgument(expr,0)) && calc_ar(ATAgetArgument(expr,1));
  }
  else if (is_ar_and(expr))
  {
    return calc_ar(ATAgetArgument(expr,0)) || calc_ar(ATAgetArgument(expr,1));
  }
  else     // is_ar_var(expr)
  {
    return !is_ar_false(ar[ATgetInt((ATermInt) ATgetArgument(expr,0))]);
  }
}

void RewriterCompilingJitty::fill_always_rewrite_array()
{
  ar = (ATermAppl*) malloc(ar_size*sizeof(ATermAppl));
  if (ar == NULL)
  {
    throw mcrl2::runtime_error("cannot allocate enough memory (" + utilities::to_string(ar_size*sizeof(ATermAppl)) + "bytes)");
  }
  for (size_t i=0; i<ar_size; i++)
  {
    ar[i] = NULL;
  }
  ATprotectArray((ATerm*) ar,ar_size);

  for(std::map <int,int> ::const_iterator it=int2ar_idx.begin(); it!=int2ar_idx.end(); ++it)
  {
    size_t arity = getArity(get_int2term(it->first));
    data_equation_list eqns = (size_t(it->first)<jittyc_eqns.size()?jittyc_eqns[it->first]:data_equation_list());
    int idx = it->second;
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

static ATerm toInner_list_odd(const data_expression t)
{
  if (is_application(t))
  {
    ATermList l = ATmakeList0();
    const data_expression_list a=application(t).arguments();
    for (data_expression_list::const_iterator i=a.begin(); i!=a.end(); ++i )
    {
      l = ATinsert(l,toInner_list_odd(*i));
    }

    l = ATreverse(l);

    ATerm arg0 = toInner_list_odd(application(t).head());
    if (ATisList(arg0))
    {
      l = ATconcat((ATermList) arg0, l);
    }
    else
    {
      l = ATinsert(l, arg0);
    }
    return (ATerm) l;
  }
  else if (is_function_symbol(t))
  {
    return (ATerm)(ATermInt)OpId2Int(data_expression(t));
  }
  else if (is_variable(t))
  {
    // Here the expression is a binder or a where expression.
    return (ATerm)(ATermAppl) t;
  }
  else if (is_abstraction(t))
  {
    const abstraction a=t;
    return (ATerm)gsMakeBinder(a.binding_operator(),a.variables(),(ATermAppl)toInner_list_odd(a.body()));
  }
  else if (is_where_clause(t))
  {
    const where_clause w=t;
    assignment_expression_list assignments=w.declarations();
    atermpp::term_list <atermpp::aterm_appl > translated_assignments;
    for (assignment_expression_list::const_iterator i=assignments.begin(); i!=assignments.end(); ++i)
    {
      translated_assignments=push_back(translated_assignments,
                                   atermpp::aterm_appl(core::detail::gsMakeDataVarIdInit(i->lhs(),(ATermAppl)toInner_list_odd(i->rhs()))));
    }
    return (ATerm)gsMakeWhr((ATermAppl)toInner_list_odd(w.body()),
                     (ATermList)reverse(translated_assignments));
  }
  assert(0);
  return NULL;
}


bool RewriterCompilingJitty::addRewriteRule(const data_equation rule1)
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

bool RewriterCompilingJitty::removeRewriteRule(const data_equation rule1)
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

  true_inner = OpId2Int(sort_bool::true_());
  true_num = ATgetInt(true_inner);

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
  ar = NULL;
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
  if (ar != NULL)
  {
    ATunprotectArray((ATerm*) ar);
    free(ar);
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
          int l = strlen(file_dir);
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
  /* If generate_code is false, only the variable aux is increased to calculate the
     return value. TODO. This can be optimized.
     Declare the function that gets function func_index in normal form */
  // int aux = 0;
  for (size_t a=0; a<=arity; a++)
  {
    if (arity_is_allowed(func_index,a))
    {
      const size_t b = (a<=NF_MAX_ARITY)?a:0;
      for (size_t nfs=0; (nfs >> b) == 0; nfs++)
      {
        fprintf(f,  "static inline atermpp::aterm_appl rewr_%zu_%zu_%zu(",func_index,a,nfs);
        for (size_t i=0; i<a; i++)
        {
          fprintf(f, (i==0)?"atermpp::aterm_appl arg%zu":", atermpp::aterm_appl arg%zu",i);
        }
        fprintf(f,  ");\n");

        fprintf(f,  "static inline atermpp::aterm_appl rewr_%zu_%zu_%zu_term(const atermpp::aterm_appl t) { return rewr_%zu_%zu_%zu(", func_index, a, nfs,func_index, a,nfs);
        for(size_t i = 1; i <= a; ++i)
        {
          fprintf(f,  "%st(%zu)", (i == 1?"":", "), i);
        }
        fprintf(f,  "); }\n");
      }
    }
  }
  // return aux;
}

void RewriterCompilingJitty::BuildRewriteSystem()
{
  FILE* f;
  CleanupRewriteSystem();

  // Try to find out from environment which compile script to use. Use
  // default script called "mcrl2compilerewriter" if not set.
  std::string compile_script;
  char* env_compile_script = getenv("MCRL2_COMPILEREWRITER");
  if (env_compile_script != NULL)
    compile_script = env_compile_script;
  else
    compile_script = "mcrl2compilerewriter";
  rewriter_so = new uncompiled_library(compile_script);
  mCRL2log(verbose) << "using '" << compile_script << "' to compile rewriter." << std::endl;

  jittyc_eqns.clear();

  ar_size = 0;
  int2ar_idx.clear();

  for(atermpp::map< function_symbol, atermpp::aterm_int >::const_iterator l = term2int_begin()
        ; l != term2int_end()
        ; ++l)
  {
    int i = l->second.value();
    if (int2ar_idx.count(i) == 0)
    {
      size_t arity = getArity(l->first);
      int2ar_idx[i]=ar_size;
      ar_size += (arity*(arity+1))/2;
    }
  }
  for(atermpp::set < data_equation >::const_iterator it=rewrite_rules.begin();
                   it!=rewrite_rules.end(); ++it)
  {
    size_t main_op_id_index=atermpp::aterm_int(toInner(it->lhs(),true)(0)).value(); // main symbol of equation.
    if (main_op_id_index>=jittyc_eqns.size())
    {
      jittyc_eqns.resize(main_op_id_index+1);
    }
    jittyc_eqns[main_op_id_index] = push_front(jittyc_eqns[main_op_id_index],*it);
  }
  fill_always_rewrite_array();

  f = MakeTempFiles();

  //
  //  Print includes
  //
  fprintf(f, "#include \"mcrl2/data/detail/rewrite/jittycpreamble.h\"\n");

  //
  // Print defs
  //
  fprintf(f,
          "#define isAppl(x) (ATgetAFun((ATermAppl)x) != %li)\n"
          "\n", (long int) ATgetAFun(static_cast<ATermAppl>(data::variable("x", data::sort_bool::bool_())))
         );

  //
  // - Calculate maximum occurring arity
  // - Forward declarations of rewr_* functions
  //
  size_t max_arity = 0;
  for (size_t j=0; j < get_num_opids(); ++j)
  {
    const function_symbol fs=get_int2term(j);
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
  fprintf(f,  "typedef atermpp::aterm_appl (*func_type)(const atermpp::aterm_appl);\n");
  fprintf(f,  "func_type* int2func[%zu];\n", max_arity+2);

  // Set this rewriter, to use its functions.
  fprintf(f,  "mcrl2::data::detail::RewriterCompilingJitty *this_rewriter;\n");

  //
  // "build" functions
  //
  for (size_t i=1; i<=max_arity; ++i)
  {
    fprintf(f,
            "static atermpp::aterm_appl build%zu(const atermpp::aterm_appl a",i);
    for (size_t j=0; j<i; ++j)
    {
      fprintf(f, ", const atermpp::aterm_appl arg%zu",j);
    }
    fprintf(f, ")\n"
            "{\n");
    fprintf(f,
            "  size_t arity = a.size();\n"
            "  if (arity == 1 )\n"
            "  {\n"
            "      return ATmakeAppl(%li,&*(ATerm)a(0)", (long int) get_appl_afun_value(i+1)); // YYYY

    for (size_t j=0; j<i; j++)
    {
      fprintf(f, ",&*(ATermAppl)arg%zu",j);
    }
    fprintf(f,
            ");\n"
            "  }\n"
            "  else if (mcrl2::data::is_abstraction(a))\n"
            "  {\n"
            "    return ATmakeAppl(%li,(_ATerm*)&*(ATermAppl)a", (long int) get_appl_afun_value(i+1)); // YYYY

    for (size_t j=0; j<i; j++)
    {
      fprintf(f, ",&*(ATermAppl)arg%zu",j);
    }
    fprintf(f,
            ");\n"
            "  }\n"
            "  else\n"
            "  {\n"
            "    //atermpp::aterm args[arity+ld];\n"
            "    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,atermpp::aterm,(arity+%zu));\n"
            "\n"
            "    for (size_t i=0; i<arity; i++)\n"
            "    {\n"
            "      args[i] = a(i);\n"
            "    }\n",i);
    for (size_t j=0; j<i; j++)
    {
      fprintf(f,
              "    args[arity+%zu] = arg%zu;\n",j,j);
    }
    fprintf(f,
            "\n"
            "    return ATmakeApplArray(mcrl2::data::detail::get_appl_afun_value(arity+%zu),(ATerm*)args);\n"  // YYYY+
            "  }\n"
            "}\n"
            "\n",i);
  }

  //
  // Implement the equations of every operation.
  //
  for (size_t j=0; j < get_num_opids(); j++)
  {
    const function_symbol fs=get_int2term(j);
    const size_t arity = getArity(fs);

    if (data_equation_selector(fs))
    {
      fprintf(f,  "// %s\n",atermpp::aterm(fs).to_string().c_str());

      for (size_t a=0; a<=arity; a++)
      {
        if (arity_is_allowed(j,a))
        {
          nfs_array nfs_a(a);
          int b = (a<=NF_MAX_ARITY)?a:0;
          for (size_t nfs=0; (nfs >> b) == 0; nfs++)
          {
            fprintf(f,  "static atermpp::aterm_appl rewr_%zu_%zu_%zu(",j,a,nfs);
            for (size_t i=0; i<a; i++)
            {
              fprintf(f, (i==0)?"atermpp::aterm_appl arg%zu":", atermpp::aterm_appl arg%zu",i);
            }
            fprintf(f,  ")\n"
                    "{\n"
                   );
            if (j<jittyc_eqns.size() && !jittyc_eqns[j].empty() )
            {
            // Implement strategy
              if (0 < a)
              {
                nfs_a.set_value(a,nfs);
              }
              implement_strategy(f,create_strategy(jittyc_eqns[j],j,a,nfs_a,true_inner),a,1,j,nfs);
            }
            else
            {
              MCRL2_SYSTEM_SPECIFIC_ALLOCA(used,bool,a);
              for (size_t k=0; k<a; k++)
              {
                used[k] = ((nfs & ((size_t)1 << k)) != 0);
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

  fprintf(f,
          "void rewrite_init(RewriterCompilingJitty *r)\n"
          "{\n"
          "  this_rewriter=r;\n"
          "  mcrl2::data::detail::get_appl_afun_value(%zu);\n"
          "  assert(this_rewriter->rewriter_binding_variable_lists.size()==%zu);\n"
          "  assert(this_rewriter->rewriter_bound_variables.size()==%zu);\n",
          max_arity+1,rewriter_binding_variable_lists.size(),rewriter_bound_variables.size()
         );
  fprintf(f,  "\n");

  /* put the functions that start the rewriting in the array int2func */
  fprintf(f,  "\n");
  fprintf(f,  "\n");
  for (size_t i=0; i<=max_arity; i++)
  {
    fprintf(f,  "  int2func[%zu] = (func_type *) malloc(%zu*sizeof(func_type));\n",i+1,get_num_opids());
    for (size_t j=0; j < get_num_opids(); j++)
    {
      const function_symbol fs=get_int2term(j);
      // const size_t arity = getArity(fs);
      if (partially_rewritten_functions.count(fs)>0)
      {
        if (arity_is_allowed(j,i))
        {
          // We are dealing with a partially rewritten function here. Remove the "@_" at
          // the beginning of the string.
          const string c_function_name=pp(fs.name());
          fprintf(f,  "  int2func[%zu][%zu] = %s;\n",i+1,j,c_function_name.substr(3,c_function_name.size()-4).c_str());
        }
      }
      else if (/* (i <= arity) && */ data_equation_selector(fs) && arity_is_allowed(j,i))
      {
        fprintf(f,  "  int2func[%zu][%zu] = rewr_%zu_%zu_0_term;\n",i+1,j,j,i);
      }
    }
  }
  fprintf(f,  "}\n"
          "\n"
          "void rewrite_cleanup()\n"
          "{\n"
         );
  fprintf(f,  "\n");
  for (size_t i=0; i<=max_arity; i++)
  {
    fprintf(f,  "  free(int2func[%zu]);\n",i+1);
  }
  fprintf(f,  "}\n"
          "\n"

         );

  fprintf(f,
      "static atermpp::aterm_appl rewrite_int_aux(const atermpp::aterm head, const atermpp::aterm_appl t)\n"
      "{\n"
      "  const size_t arity = t.size();\n"
      "  //atermpp::aterm args[arity];\n"
      "  MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,atermpp::aterm,(arity));\n"
      "  args[0] = head;\n"
      "  for (size_t i=1; i<arity; ++i)\n"
      "  {\n"
      "    args[i] = rewrite(t(i));\n"
      "  }\n"
      "  return ATmakeApplArray(ATgetAFun(t),(ATerm*)args);\n"
      "}\n\n");

  fprintf(f,
      "static atermpp::aterm_appl rewrite_appl_aux(\n"
      "     atermpp::aterm_appl head,\n"
      "     const atermpp::aterm_appl t)\n"
      "{\n"
      "  using namespace mcrl2::core::detail;\n"
      "  if (mcrl2::core::detail::gsIsDataVarId(head))\n"
      "  {\n"
      "    head=(*(this_rewriter->global_sigma))(mcrl2::data::variable(head));\n"
      "  }\n"
      "  else if (mcrl2::data::is_where_clause(head))\n"
      "  {\n"
      "    head = this_rewriter->rewrite_where(head,*(this_rewriter->global_sigma));\n"
      "  }\n"
      "  \n"
      "  // Here head has the shape\n"
      "  // variable, u(u1,...,um), lambda y1,....,ym.u, forall y1,....,ym.u or exists y1,....,ym.u,\n"
      "  if (mcrl2::data::is_abstraction(head))\n"
      "  {\n"
      "    const atermpp::aterm_appl binder=head(0);\n"
      "    if (binder==gsMakeLambda())\n"
      "    {\n"
      "      return this_rewriter->rewrite_lambda_application(head,t,*(this_rewriter->global_sigma));\n"
      "    }\n"
      "    if (binder==gsMakeExists())\n"
      "    {\n"
      "      return this_rewriter->internal_existential_quantifier_enumeration(head,*(this_rewriter->global_sigma));\n"
      "    }\n"
      "    if (binder==gsMakeForall())\n"
      "    {\n"
      "      return this_rewriter->internal_universal_quantifier_enumeration(head,*(this_rewriter->global_sigma));\n"
      "    }\n"
      "    assert(0); // One cannot end up here.\n"
      "  }\n"
      "  \n"
      "  const size_t arity=t.size();\n"
      "  if (gsIsDataVarId(head))\n"
      "  {\n"
      "    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,atermpp::aterm, arity);\n"
      "    args[0]=head;\n"
      "    for(size_t i=1; i<arity; ++i)\n"
      "    {\n"
      "      args[i]=rewrite(atermpp::aterm_appl(t(i)));\n"
      "    }\n"
      "    return ApplyArray(arity,args);\n"
      "  }\n"
      "  \n"
      "  \n"
      "  // Here head has the shape #REWR#(u0,u1,...,un).\n"

      "  const atermpp::aterm_appl u=head;\n"
      "  const size_t arity_t = t.size();\n"
      "  const atermpp::aterm head1 = u(0);\n"
      "  const size_t arity_u=u.size();\n"
      "  MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,atermpp::aterm,(arity_u+arity_t-1));\n"
      "  args[0] = head1;\n"
      "  int function_index;\n"
      "  if ((head1.type()==AT_INT) && ((function_index = atermpp::aterm_int(head1).value()) < %zu) )\n"
      "  {\n"
      "    for (size_t i=1; i<arity_u; ++i)\n"
      "    {\n"
      "      args[i] = u(i);\n"
      "    }\n"
      "    size_t k = arity_u;\n"
      "    for (size_t i=1; i<arity_t; ++i,++k)\n"
      "    {\n"
      "      args[k] = t(i);\n"
      "    }\n"
      "    size_t arity = arity_u+arity_t-2;\n"
      "    const atermpp::aterm_appl intermediate = ATmakeApplArray(mcrl2::data::detail::get_appl_afun_value(arity+1),(ATerm*)args);\n"   // YYYY+
      "    assert(arity <= %zu);\n"
      "    assert(int2func[arity+1][function_index] != NULL);\n"
      "    return int2func[arity+1][function_index](intermediate);\n"
      "  }\n"
      "  else\n"
      "  {\n"
      "    for (size_t i=1; i<arity_u; ++i)\n"
      "    {\n"
      "      args[i] = rewrite(atermpp::aterm_appl(u(i)));\n"
      "    }\n"
      "    size_t k = arity_u;\n"
      "    for (size_t i=1; i<arity_t; ++i,++k)\n"
      "    {\n"
      "      args[k] = rewrite(atermpp::aterm_appl(t(i)));\n"
      "    }\n"
      "    return ATmakeApplArray(mcrl2::data::detail::get_appl_afun_value(arity_u+arity_t-1),(ATerm*)args);\n"   // YYYY+
      "  }\n"
      "}\n\n",
      get_num_opids(), max_arity
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
      "static atermpp::aterm_appl rewrite_aux(const atermpp::aterm_appl t)\n"
      "{\n"
      "  using namespace mcrl2::core::detail;\n"
      "  if (mcrl2::data::is_abstraction(t))\n"
      "  {\n"
      "    atermpp::aterm_appl binder=t(0);\n"
      "    if (binder==gsMakeExists())\n"
      "    {\n"
      "      return this_rewriter->internal_existential_quantifier_enumeration(t,*(this_rewriter->global_sigma));\n"
      "    }\n"
      "    if (binder==gsMakeForall())\n"
      "    {\n"
      "      return this_rewriter->internal_universal_quantifier_enumeration(t,*(this_rewriter->global_sigma));\n"
      "    }\n"
      "    if (binder==gsMakeLambda())\n"
      "    {\n"
      "      return this_rewriter->rewrite_single_lambda(\n"
      "               mcrl2::data::variable_list(atermpp::aterm_list(t(1))),\n"
      "               atermpp::aterm_appl(t(2)),false,*(this_rewriter->global_sigma));\n"
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
      "  if (t.function()==apples[t.size()])\n"
      "  {\n"
      "    // Term t has the shape #REWR#(t1,...,tn)\n"
      "    const atermpp::aterm head = t(0);\n"
      "    if (head.type()==AT_INT)\n"
      "    {\n"
      "      const int function_index = atermpp::aterm_int(head).value();\n"
      "      if (function_index < %zu )\n"
      "      {\n"
      "        const size_t arity = t.size();\n"
      "        assert(arity <= %zu);\n"
      "        assert(int2func[arity][function_index] != NULL);\n"
      "        return int2func[arity][function_index](t);\n"
      "      }\n"
      "      else\n"
      "      {\n"
      "        return rewrite_int_aux(head, t);"
      "      }\n"
      "    }\n"
      "    else\n"
      "    {\n"
      "      return rewrite_appl_aux(head, t);\n"
      "    }\n"
      "  }\n"
      "  \n"
      "  // Term t does not have the shape #REWR#(t1,...,tn)\n"
      "  if (gsIsDataVarId(t))\n"
      "  {\n"
      "    return (*(this_rewriter->global_sigma))(t);\n"
      "  }\n"
      "  return rewrite_aux(t);\n"
      "}\n",
      get_num_opids(), max_arity);


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

  try
  {
    so_rewr_init = reinterpret_cast<void(*)(RewriterCompilingJitty *)>(rewriter_so->proc_address("rewrite_init"));
    so_rewr_cleanup = reinterpret_cast<void (*)()>(rewriter_so->proc_address("rewrite_cleanup"));
    so_rewr = reinterpret_cast<atermpp::aterm_appl(*)(const atermpp::aterm_appl &)> (rewriter_so->proc_address("rewrite_external"));
    // so_rewr = reinterpret_cast<atermpp::aterm_appl(*)(const _ATermAppl*)> (rewriter_so->proc_address("rewrite_external"));

  }
  catch(std::runtime_error &e)
  {
    rewriter_so->leave_files();
    throw mcrl2::runtime_error(std::string("Could not load rewriter: ") + e.what());
  }

  so_rewr_init(this);
  need_rebuild = false;
}

RewriterCompilingJitty::RewriterCompilingJitty(const data_specification& DataSpec, const used_data_equation_selector &equations_selector):
   Rewriter()
{
  data_equation_selector=equations_selector;
  so_rewr_cleanup = NULL;
  rewriter_so = NULL;
  m_data_specification_for_enumeration = DataSpec;
  initialise_common();
  CompileRewriteSystem(DataSpec);
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
  return fromRewriteFormat(rewrite_internal(toRewriteFormat(term),internal_sigma));
}

atermpp::aterm_appl RewriterCompilingJitty::rewrite_internal(
     const atermpp::aterm_appl &term,
     internal_substitution_type &sigma)
{
  if (need_rebuild)
  {
    BuildRewriteSystem();
  }
  // Save global sigma and restore it afterwards, as rewriting might be recursive with different
  // substitutions, due to the enumerator.
  internal_substitution_type *saved_sigma=global_sigma;
  global_sigma= &sigma;
  const atermpp::aterm_appl result=so_rewr(term);
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
