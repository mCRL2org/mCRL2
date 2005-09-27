///////////////////////////////////////////////////////////////////////////////
/// \file function.h
/// Contains function data structures for the mcrl2 library.

#ifndef MCRL2_SUBSTITUTE_H
#define MCRL2_SUBSTITUTE_H

#include <vector>
#include "atermpp/aterm.h"
#include "mcrl2/aterm_wrapper.h"
#include "mcrl2/term_list.h"

namespace mcrl2 {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::make_term;

    /// Applies a sequence of substitutions to the term init and returns the result.
    /// The SubstIter objects must supply the method aterm_appl operator()(aterm_appl).
    ///
    template <typename SubstIter>
    aterm_appl aterm_appl_substitute(aterm_appl init, SubstIter first, SubstIter last)
    {
      aterm_appl result = init;
      for (SubstIter i = first; i != last; ++i)
      {
        result = (*i)(result);
      }
      return result;
    }

/*
    /// Applies a sequence of substitutions to the term_list t and returns the result.
    /// The SubstIter objects must supply the method aterm_appl operator()(aterm_appl).
    ///
    template <typename Term, typename SubstIter>
    term_list<Term> term_list_substitute(term_list<Term> t, SubstIter first, SubstIter last)
    {
      std::vector<aterm_appl> tmp;
      tmp.reserve(t.size());
      for (term_list<Term>::iterator i = t.begin(); i != t.end(); ++i)
      {
        tmp.push_back(aterm_appl_substitute(i->term(), first, last));
      }
      return term_list<Term>(tmp.begin(), tmp.end());
    }
*/

} // namespace mcrl

#endif // MCRL2_SUBSTITUTE_H
