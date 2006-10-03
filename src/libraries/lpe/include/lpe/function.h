///////////////////////////////////////////////////////////////////////////////
/// \file function.h
/// Contains function data structures for the LPE Library.

#ifndef LPE_FUNCTION_H
#define LPE_FUNCTION_H

#include <iostream>
#include <cassert>
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "lpe/aterm_wrapper.h"
#include "lpe/sort.h"
#include "libstruct.h"
#include "liblowlevel.h"

namespace lpe {

using atermpp::aterm_appl;
using atermpp::term_list;
using atermpp::make_term;

///////////////////////////////////////////////////////////////////////////////
// function
/// \brief Represents mappings and constructors of a mCRL2 specification.
///
class function: public aterm_appl_wrapper
{
  public:
    function()
    {}

    function(aterm_appl t)
      : aterm_appl_wrapper(t)
    {
      assert(gsIsOpId(t));
    }
       
    // Added 16-11-2005 by Frank S.
    // 
    sort_list input_types() const
    {
      sort_list result;
      aterm_appl t = aterm_appl(*this);
      while (gsIsSortArrow(lpe::sort(aterm_appl(t.argument(1))))) {
        t = ATAgetArgument(t , 1);
        result = push_front(result, lpe::sort(aterm_appl(t.argument(0))));
      }
      return reverse(result);
    }

    // Added 16-11-2005 by Frank S.
    //     
    sort result_type() const
    {
      aterm_appl t = aterm_appl(*this);
      while (gsIsSortArrow(lpe::sort(aterm_appl(t.argument(1))))) {
        t = ATAgetArgument(t , 1);
      }
      return lpe::sort(aterm_appl(t.argument(1)));
    }
};

typedef term_list<function> function_list;

} // namespace mcrl

#endif // LPE_FUNCTION_H
