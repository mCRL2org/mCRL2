// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstring>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <cctype>

#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/aterm/aterm.h"

namespace aterm_deprecated
{

//Substitutions on ATerm's
//------------------------

static AFun substafun;
static bool substafun_notset = true;
ATermAppl gsMakeSubst(const ATerm &OldValue, const ATerm &NewValue)
{
  if (substafun_notset)
  {
    substafun = AFun("subst", 2);
    substafun_notset = false;
  }
  return ATmakeAppl2(substafun, OldValue, NewValue);
}

ATerm gsSubstValues(const ATermList &Substs, const ATerm &t, bool Recursive)
{
  ATerm Term=t;
  ATermList l = Substs;
  while (!ATisEmpty(l))
  {
    ATermAppl Subst = ATAgetFirst(l);
    if (ATgetArgument(Subst, 0)==Term)
    {
      return ATgetArgument(Subst, 1);
    }
    l = ATgetNext(l);
  }
  if (!Recursive)
  {
    return Term;
  }
  else
  {
    //Recursive; distribute substitutions over the arguments/elements of Term
    if (Term.type() == AT_APPL)
    {
      //Term is an ATermAppl; distribute substitutions over the arguments
      AFun Head = Term.function();
      const size_t NrArgs = Head.arity();
      if (NrArgs > 0)
      {
        MCRL2_SYSTEM_SPECIFIC_ALLOCA(Args,aterm,NrArgs);
        // std::vector<ATerm> Args(NrArgs);
        for (size_t i = 0; i < NrArgs; ++i)
        {
          new (&Args[i]) aterm(gsSubstValues(Substs, ATgetArgument((ATermAppl) Term, i), Recursive));
        }
        const aterm a = ATmakeAppl_iterator(Head, &Args[0],&Args[0]+NrArgs);
        for (size_t i = 0; i < NrArgs; ++i)
        {
          Args[i].~ATerm();
        }
        return a;
      }
      else
      {
        return Term;
      }
    }
    else if (Term.type() == AT_LIST)
    {
      //Term is an ATermList; distribute substitutions over the elements
      ATermList Result = ATmakeList0();
      while (!ATisEmpty((ATermList) Term))
      {
        Result = ATinsert(Result,
                          gsSubstValues(Substs, ATgetFirst((ATermList) Term), Recursive));
        Term = ATgetNext((ATermList) Term);
      }
      return reverse(Result);
    }
    else
    {
      return Term;
    }
  }
}

ATerm gsSubstValuesTable(const ATermTable &Substs, const ATerm &t, const bool Recursive)
{
  ATerm Term=t;
  ATerm Result = ATtableGet(Substs, Term);
  if (Result != aterm())
  {
    return Result;
  }
  if (!Recursive)
  {
    return Term;
  }
  else
  {
    //Recursive; distribute substitutions over the arguments/elements of Term
    if (Term.type() == AT_APPL)
    {
      //Term is an ATermAppl; distribute substitutions over the arguments
      AFun Head = Term.function();
      const size_t NrArgs = Head.arity();
      if (NrArgs > 0)
      {
        MCRL2_SYSTEM_SPECIFIC_ALLOCA(Args,aterm,NrArgs);
        // std::vector <ATerm> Args(NrArgs);
        for (size_t i = 0; i < NrArgs; i++)
        {
           new (&Args[i]) aterm(gsSubstValuesTable(Substs, ATgetArgument((ATermAppl) Term, i), Recursive));
        }
        const aterm a = ATmakeAppl_iterator(Head, &Args[0],&Args[0]+NrArgs);
        for (size_t i = 0; i < NrArgs; i++)
        {
          Args[i].~aterm(); 
        }
        return a;
      }
      else
      {
        return Term;
      }
    }
    else if (Term.type() == AT_LIST)
    {
      //Term is an ATermList; distribute substitutions over the elements
      ATermList Result = ATmakeList0();
      while (!ATisEmpty((ATermList) Term))
      {
        Result = ATinsert(Result,
                          gsSubstValuesTable(Substs, ATgetFirst((ATermList) Term), Recursive));
        Term = ATgetNext((ATermList) Term);
      }
      return reverse(Result);
    }
    else
    {
      return Term;
    }
  }
}

//Occurrences of ATerm's
//----------------------

bool gsOccurs(const ATerm &Elt, const ATerm &t)
{
  ATerm Term=t;
  bool Result = false;
  if (Elt==Term)
  {
    Result = true;
  }
  else
  {
    //check occurrences of Elt in the arguments/elements of Term
    if (Term.type() == AT_APPL)
    {
      AFun Head = Term.function();
      const size_t NrArgs = Head.arity();
      for (size_t i = 0; i < NrArgs && !Result; i++)
      {
        Result = gsOccurs(Elt, ATgetArgument((ATermAppl) Term, i));
      }
    }
    else if (Term.type() == AT_LIST)
    {
      while (!ATisEmpty((ATermList) Term) && !Result)
      {
        Result = gsOccurs(Elt, ATgetFirst((ATermList) Term));
        Term = ATgetNext((ATermList) Term);
      }
    }
  }
  return Result;
}

} // namespace aterm_deprecated

