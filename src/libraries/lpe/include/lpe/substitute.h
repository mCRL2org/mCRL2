///////////////////////////////////////////////////////////////////////////////
/// \file substitute.h
/// Contains function data structures for the LPE Library.

#ifndef LPE_SUBSTITUTE_H
#define LPE_SUBSTITUTE_H

#include <vector>
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "librewrite_c.h"

namespace lpe {

using atermpp::aterm_appl;
using atermpp::term_list;

    /// Applies a substitution to this aterm_appl_wrapper and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    aterm_appl_wrapper substitute(aterm_appl_wrapper t, Substitution f)
    {
      return aterm_appl_wrapper(f(t));
    }     

    /// Applies a substitution to this term_list and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Term, typename Substitution>
    term_list<Term> substitute(term_list<Term> l, Substitution f)
    {
      return term_list<Term>(f(l));
    }     

    /// Applies a sequence of data assignments to this ATermAppl and returns the result.
    /// It uses the rewriter
    /// EXPERIMENTAL!
    ///
    template <typename SubstIter>
    ATermAppl p_substitute(ATermAppl t, SubstIter first, SubstIter last)
    {
      for(SubstIter i = first; i != last; ++i)
      {
        RWsetVariable(aterm(i->lhs()) , aterm(i->rhs()));
      }
      ATermAppl result = gsRewriteTerm(t);
      for(SubstIter i = first; i != last; ++i)
      {
        RWclearVariable(aterm(i->lhs()));
      }
      return result;
    }

} // namespace mcrl

#endif // LPE_SUBSTITUTE_H
