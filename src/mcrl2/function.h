///////////////////////////////////////////////////////////////////////////////
/// \file function.h
/// Contains function data structures for the mcrl2 library.

#ifndef MCRL2_FUNCTION_H
#define MCRL2_FUNCTION_H

#include "atermpp/aterm.h"
#include "mcrl2/aterm_wrapper.h"
#include "mcrl2/term_list.h"

namespace mcrl2 {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::make_term;

///////////////////////////////////////////////////////////////////////////////
// function
/// \brief Represents mappings and constructors of a mCRL2 specification.
///
class function: public aterm_wrapper
{
  public:
    function()
    {}

    function(aterm_appl t)
      : aterm_wrapper(t)
    {}
};

typedef term_list<function> function_list;

} // namespace mcrl

#endif // MCRL2_FUNCTION_H
