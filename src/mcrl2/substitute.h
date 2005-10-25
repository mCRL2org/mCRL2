///////////////////////////////////////////////////////////////////////////////
/// \file substitute.h
/// Contains function data structures for the mcrl2 library.

#ifndef MCRL2_SUBSTITUTE_H
#define MCRL2_SUBSTITUTE_H

#include <vector>
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"

namespace mcrl2 {

using atermpp::aterm_appl;
using atermpp::term_list;

    /// Applies a substitution to this term_list and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Term, typename Substitution>
    term_list<Term> substitute(term_list<Term> l, Substitution f)
    {
      return term_list<Term>(f(l));
    }     

    /// Applies a sequence of substitutions to this term_list and returns the result.
    /// The SubstIter objects must supply the method aterm operator()(aterm).
    ///
    template <typename Term, typename SubstIter>
    term_list<Term> substitute(term_list<Term> l, SubstIter first, SubstIter last)
    {
      aterm_appl result = l;
      for (SubstIter i = first; i != last; ++i)
          result = (*i)(result);
      return term_list<Term>(result);
    }     

} // namespace mcrl

#endif // MCRL2_SUBSTITUTE_H
