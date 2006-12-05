///////////////////////////////////////////////////////////////////////////////
/// \file function.h
/// Contains function data structures for the LPE Library.

#ifndef LPE_FUNCTION_H
#define LPE_FUNCTION_H

#include <iostream>
#include <cassert>
#include "atermpp/aterm_list.h"
#include "atermpp/aterm_appl.h"
#include "atermpp/aterm_string.h"
#include "atermpp/aterm_access.h"
#include "lpe/sort.h"

namespace lpe {

using atermpp::term_list;
using atermpp::aterm_appl;
using atermpp::aterm_string;

///////////////////////////////////////////////////////////////////////////////
// function
/// \brief Represents mappings and constructors of a mCRL2 specification.
///
// <OpId>         ::= OpId(<String>, <SortExpr>)
class function: public aterm_appl
{
  public:
    function()
    {}

    function(aterm_appl t)
      : aterm_appl(t)
    {
      assert(gsIsOpId(t));
    }

    function(std::string name, sort s)
      : aterm_appl(gsMakeOpId(gsString2ATermAppl(name.c_str()), s))
    {
    }

    aterm_string name() const
    {
      return arg1(*this);
    }
      
    sort type() const
    {
      return arg2(*this);
    }

    sort_list domain_sorts() const
    {
      return type().domain_sorts();
    }

    sort range_sort() const
    {
      return type().range_sort();
    }

    /// Applies a substitution to this function and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    function substitute(Substitution f)
    {
      return function(f(*this));
    }     

    /// Returns a pretty print representation of the term.
    ///                                                   
    std::string pp() const                                
    {                                                     
      return pretty_print(term());                        
    }
};

typedef term_list<function> function_list;

inline
bool is_function(aterm_appl t)
{
  return gsIsOpId(t);
}

} // namespace mcrl

#endif // LPE_FUNCTION_H
