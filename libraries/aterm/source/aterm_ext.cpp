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

//Substitutions on aterm's
//------------------------

static function_symbol substafun;
static bool substafun_notset = true;
aterm_appl gsMakeSubst(const aterm &OldValue, const aterm &NewValue)
{
  if (substafun_notset)
  {
    substafun = function_symbol("subst", 2);
    substafun_notset = false;
  }
  return aterm_appl(substafun, OldValue, NewValue);
}

aterm gsSubstValues(const aterm_list &Substs, const aterm &t, bool Recursive)
{
  aterm Term=t;
  aterm_list l = Substs;
  while (!l.empty())
  {
    aterm_appl Subst = aterm_cast<aterm_appl>(l.front());
    if (Subst(0)==Term)
    {
      return Subst(1);
    }
    l = l.tail();
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
      //Term is an aterm_appl; distribute substitutions over the arguments
      function_symbol Head = Term.function();
      const size_t NrArgs = Head.arity();
      if (NrArgs > 0)
      {
        MCRL2_SYSTEM_SPECIFIC_ALLOCA(Args,aterm,NrArgs);
        // std::vector<aterm> Args(NrArgs);
        for (size_t i = 0; i < NrArgs; ++i)
        {
          new (&Args[i]) aterm(gsSubstValues(Substs, ((aterm_appl) Term)(i), Recursive));
        }
        const aterm a = aterm_appl(Head, &Args[0],&Args[0]+NrArgs);
        for (size_t i = 0; i < NrArgs; ++i)
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
      //Term is an aterm_list; distribute substitutions over the elements
      aterm_list Result;
      while (!((aterm_list) Term).empty())
      {
        Result = push_front(Result,
                          gsSubstValues(Substs, ((aterm_list) Term).front(), Recursive));
        Term = ((aterm_list) Term).tail();
      }
      return reverse(Result);
    }
    else
    {
      return Term;
    }
  }
}

} // namespace atermpp

