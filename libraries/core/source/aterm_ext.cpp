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

#include "boost/scoped_array.hpp"
#include "mcrl2/core/aterm_ext.h"

namespace mcrl2 {
  namespace core {

      //Substitutions on ATerm's
      //------------------------

      static AFun substafun;
      static bool substafun_notset = true;
      ATermAppl gsMakeSubst(ATerm OldValue, ATerm NewValue)
      {
        if ( substafun_notset )
        {
                substafun = ATmakeAFun("subst", 2, ATfalse);
                ATprotectAFun(substafun);
                substafun_notset = false;
        }
        return ATmakeAppl2(substafun, OldValue, NewValue);
      }

      ATerm gsSubstValues(ATermList Substs, ATerm Term, bool Recursive)
      {
        ATermList l = Substs;
        while (!ATisEmpty(l)) {
          ATermAppl Subst = ATAgetFirst(l);
          if (ATisEqual(ATgetArgument(Subst, 0), Term)) {
            return ATgetArgument(Subst, 1);
          }
          l = ATgetNext(l);
        }
        if (!Recursive) {
          return Term;
        } else {
          //Recursive; distribute substitutions over the arguments/elements of Term
          if (ATgetType(Term) == AT_APPL) {
            //Term is an ATermAppl; distribute substitutions over the arguments
            AFun Head = ATgetAFun((ATermAppl) Term);
            int NrArgs = ATgetArity(Head);
            if (NrArgs > 0) {
              boost::scoped_array< ATerm > Args(new ATerm[NrArgs]);
              for (int i = 0; i < NrArgs; i++) {
                Args[i] = gsSubstValues(Substs, ATgetArgument((ATermAppl) Term, i),
                  Recursive);
              }
              ATerm a = (ATerm) ATmakeApplArray(Head, Args.get());
              return a;
            } else {
              return Term;
            }
          } else if (ATgetType(Term) == AT_LIST) {
            //Term is an ATermList; distribute substitutions over the elements
            ATermList Result = ATmakeList0();
            while (!ATisEmpty((ATermList) Term)) {
              Result = ATinsert(Result,
                gsSubstValues(Substs, ATgetFirst((ATermList) Term), Recursive));
              Term = (ATerm) ATgetNext((ATermList) Term);
            }
            return (ATerm) ATreverse(Result);
          } else {
            return Term;
          }
        }
      }

      ATerm gsSubstValuesTable(ATermTable Substs, ATerm Term, bool Recursive)
      {
        ATerm Result = ATtableGet(Substs, Term);
        if ((Result) != NULL) {
          return Result;
        }
        if (!Recursive) {
          return Term;
        } else {
          //Recursive; distribute substitutions over the arguments/elements of Term
          if (ATgetType(Term) == AT_APPL) {
            //Term is an ATermAppl; distribute substitutions over the arguments
            AFun Head = ATgetAFun((ATermAppl) Term);
            int NrArgs = ATgetArity(Head);
            if (NrArgs > 0) {
              boost::scoped_array< ATerm > Args(new ATerm[NrArgs]);
              for (int i = 0; i < NrArgs; i++) {
                Args[i] = gsSubstValuesTable(Substs, ATgetArgument((ATermAppl) Term, i),
                  Recursive);
              }
              ATerm a = (ATerm) ATmakeApplArray(Head, Args.get());
              return a;
            } else {
              return Term;
            }
          } else if (ATgetType(Term) == AT_LIST) {
            //Term is an ATermList; distribute substitutions over the elements
            ATermList Result = ATmakeList0();
            while (!ATisEmpty((ATermList) Term)) {
              Result = ATinsert(Result,
                gsSubstValuesTable(Substs, ATgetFirst((ATermList) Term), Recursive));
              Term = (ATerm) ATgetNext((ATermList) Term);
            }
            return (ATerm) ATreverse(Result);
          } else {
            return Term;
          }
        }
      }

      ATermList gsAddSubstToSubsts(ATermAppl Subst, ATermList Substs)
      {
        //add Subst to Substs in which Subst is performed on the RHS's
        ATermList Result = ATmakeList0();
        while (!ATisEmpty(Substs)) {
          ATermAppl SubstsElt = ATAgetFirst(Substs);
          Result = ATinsert(Result, (ATerm) ATsetArgument(SubstsElt,
            (ATerm) gsSubstValues(ATmakeList1((ATerm) Subst),
              ATgetArgument(SubstsElt, 1) , true), 1));
          Substs = ATgetNext(Substs);
        }
        return ATinsert(ATreverse(Result), (ATerm) Subst);
      }

      //Occurrences of ATerm's
      //----------------------

      bool gsOccurs(ATerm Elt, ATerm Term)
      {
        bool Result = false;
        if (ATisEqual(Elt, Term)) {
          Result = true;
        } else {
          //check occurrences of Elt in the arguments/elements of Term
          if (ATgetType(Term) == AT_APPL) {
            AFun Head = ATgetAFun((ATermAppl) Term);
            int NrArgs = ATgetArity(Head);
            for (int i = 0; i < NrArgs && !Result; i++) {
              Result = gsOccurs(Elt, ATgetArgument((ATermAppl) Term, i));
            }
          } else if (ATgetType(Term) == AT_LIST) {
            while (!ATisEmpty((ATermList) Term) && !Result)
            {
              Result = gsOccurs(Elt, ATgetFirst((ATermList) Term));
              Term = (ATerm) ATgetNext((ATermList) Term);
            }
          }
        }
        return Result;
      }

      int gsCount(ATerm Elt, ATerm Term)
      {
        int Result = 0;
        if (ATisEqual(Elt, Term)) {
          Result = 1;
        } else {
          //count occurrences of Elt in the arguments/elements of Term
          if (ATgetType(Term) == AT_APPL) {
            AFun Head = ATgetAFun((ATermAppl) Term);
            int NrArgs = ATgetArity(Head);
            for (int i = 0; i < NrArgs; i++) {
              Result += gsCount(Elt, ATgetArgument((ATermAppl) Term, i));
            }
          } else if (ATgetType(Term) == AT_LIST) {
            while (!ATisEmpty((ATermList) Term))
            {
              Result += gsCount(Elt, ATgetFirst((ATermList) Term));
              Term = (ATerm) ATgetNext((ATermList) Term);
            }
          }
        }
        return Result;
      }

      int gsCountAFun(AFun fun, ATerm term)
      {
        int result = 0;
        if (ATgetType(term) == AT_APPL) {
          AFun head = ATgetAFun((ATermAppl) term);
          if (fun == head) {
            result += 1;
          }
          int nr_args = ATgetArity(head);
          for (int i = 0; i < nr_args; i++) {
            result += gsCountAFun(fun, ATgetArgument((ATermAppl) term, i));
          }
        } else if (ATgetType(term) == AT_LIST) {
          while (!ATisEmpty((ATermList) term))
          {
            result += gsCountAFun(fun, ATgetFirst((ATermList) term));
            term = (ATerm) ATgetNext((ATermList) term);
          }
        }
        return result;
      }
  }
}

