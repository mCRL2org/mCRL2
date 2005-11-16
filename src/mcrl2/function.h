///////////////////////////////////////////////////////////////////////////////
/// \file function.h
/// Contains function data structures for the mcrl2 library.

#ifndef MCRL2_FUNCTION_H
#define MCRL2_FUNCTION_H

#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "mcrl2/aterm_wrapper.h"
#include "mcrl2/sort.h"

#include "libstruct.h"
#include "liblowlevel.h"

#include "iostream"

namespace mcrl2 {

using atermpp::aterm_appl;
using atermpp::term_list;
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
    
    
    // Added 16-11-2005 by Frank S.
    // 
    sort_list input_types() const
    {
      sort_list result;
      aterm_appl t = aterm_appl(*this);
      while (gsIsSortArrow(mcrl2::sort(t.argument(1)))) {
        t = ATAgetArgument(t , 1);
        result = push_front(result, mcrl2::sort(t.argument(0)));
      }
      return reverse(result);
    }

    // Added 16-11-2005 by Frank S.
    //     
    sort result_type() const
    {
      aterm_appl t = aterm_appl(*this);
      while (gsIsSortArrow(mcrl2::sort(t.argument(1)))) {
        t = ATAgetArgument(t , 1);
      }
      return mcrl2::sort(t.argument(1));
    }
};

typedef term_list<function> function_list;

} // namespace mcrl

#endif // MCRL2_FUNCTION_H
