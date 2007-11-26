// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_operation.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DATA_OPERATION_H
#define MCRL2_DATA_DATA_OPERATION_H

#include <cassert>
#include "atermpp/aterm_appl.h"
#include "atermpp/aterm_traits.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/data_expression.h"

namespace lps {

using atermpp::aterm_appl;
using atermpp::term_list;
using atermpp::arg1;

///////////////////////////////////////////////////////////////////////////////
// data_operation
/// \brief operation on data.
///
class data_operation: public data_expression
{
  public:
    data_operation()
      : data_expression(detail::constructOpId())
    {}

    data_operation(aterm_appl t)
     : data_expression(t)
    {
      assert(detail::check_rule_OpId(m_term));
    }

    data_operation(identifier_string name, sort_expression s)
     : data_expression(gsMakeOpId(name, s))
    {}

    /// Returns the name of the data_operation.
    ///
    identifier_string name() const
    {
      return arg1(*this);
    }
  };
                                                            
///////////////////////////////////////////////////////////////////////////////
// data_operation_list
/// \brief singly linked list of data operations
///
typedef term_list<data_operation> data_operation_list;

/// \brief Returns true if the term t is a data operation
inline
bool is_data_operation(aterm_appl t)
{
  return gsIsOpId(t);
}

} // namespace lps

/// \internal
namespace atermpp
{
using lps::data_operation;

template<>
struct aterm_traits<data_operation>
{
  typedef ATermAppl aterm_type;
  static void protect(data_operation t)   { t.protect(); }
  static void unprotect(data_operation t) { t.unprotect(); }
  static void mark(data_operation t)      { t.mark(); }
  static ATerm term(data_operation t)     { return t.term(); }
  static ATerm* ptr(data_operation& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // MCRL2_DATA_DATA_OPERATION_H
