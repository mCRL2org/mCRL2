// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/function.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_FUNCTION_H
#define MCRL2_DATA_FUNCTION_H

#include <cassert>
#include "atermpp/aterm_appl.h"
#include "atermpp/aterm_traits.h"
#include "mcrl2/basic/identifier_string.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/basic/detail/soundness_checks.h"

namespace lps {

using atermpp::aterm_appl;
using atermpp::term_list;
using atermpp::arg1;

///////////////////////////////////////////////////////////////////////////////
// function
/// \brief operation on data.
///
class function: public data_expression
{
  public:
    function()
      : data_expression(detail::constructOpId())
    {}

    function(aterm_appl t)
     : data_expression(t)
    {
      assert(detail::check_rule_OpId(m_term));
    }

    function(identifier_string name, lps::sort s)
     : data_expression(gsMakeOpId(name, s))
    {}

    /// Returns the name of the function.
    ///
    identifier_string name() const
    {
      return arg1(*this);
    }
  };
                                                            
///////////////////////////////////////////////////////////////////////////////
// function_list
/// \brief singly linked list of data operations
///
typedef term_list<function> function_list;

/// \brief Returns true if the term t is a data operation
inline
bool is_function(aterm_appl t)
{
  return gsIsOpId(t);
}

} // namespace lps

/// \internal
namespace atermpp
{
using lps::function;

template<>
struct aterm_traits<function>
{
  typedef ATermAppl aterm_type;
  static void protect(function t)   { t.protect(); }
  static void unprotect(function t) { t.unprotect(); }
  static void mark(function t)      { t.mark(); }
  static ATerm term(function t)     { return t.term(); }
  static ATerm* ptr(function& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // MCRL2_DATA_FUNCTION_H
