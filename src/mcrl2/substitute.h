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

using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::make_term;

    /// Applies a sequence of substitutions to the term init and returns the result.
    /// The SubstIter objects must supply the method aterm operator()(aterm).
    ///
    template <typename SubstIter>
    aterm_appl substitute(aterm_appl init, SubstIter first, SubstIter last)
    {
      aterm_appl result = init;
      for (SubstIter i = first; i != last; ++i)
      {
        result = (*i)(result);
      }
      return result;
    }

} // namespace mcrl

#endif // MCRL2_SUBSTITUTE_H
