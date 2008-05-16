// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_operation.h
/// \brief The class data_operation.

#ifndef MCRL2_DATA_DATA_OPERATION_H
#define MCRL2_DATA_DATA_OPERATION_H

#include <cassert>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/data_application.h"

namespace mcrl2 {

namespace data {

using atermpp::aterm_appl;
using atermpp::term_list;
using atermpp::arg1;

/// \brief An operation on data.
///
class data_operation: public data_expression
{
  public:
    /// Constructor.
    ///             
    data_operation()
      : data_expression(core::detail::constructOpId())
    {}

    /// Constructor.
    ///             
    data_operation(aterm_appl t)
     : data_expression(t)
    {
      assert(core::detail::check_rule_OpId(m_term));
    }

    /// Constructor.
    ///             
    data_operation(core::identifier_string name, sort_expression s)
     : data_expression(core::detail::gsMakeOpId(name, s))
    {}

    /// Returns the name of the data_operation.
    ///
    core::identifier_string name() const
    {
      return arg1(*this);
    }
    
    /// Returns the sort of the data_operation.
    ///
    sort_expression sort() const
    {
      return arg2(*this);
    }

    /// Apply the data_operation to a list of arguments.
    ///
    data_expression operator()(data_expression_list args) const
    {
      if (args.empty())
      {
        return ATermAppl(*this);
      }
      else
      {
        return data_application(ATermAppl(*this), args);
      }
    }
  };
                                                            
/// \brief singly linked list of data operations
///
typedef term_list<data_operation> data_operation_list;

/// \brief Returns true if the term t is a data operation
inline
bool is_data_operation(aterm_appl t)
{
  return core::detail::gsIsOpId(t);
}

} // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{
using mcrl2::data::data_operation;

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
/// \endcond

#endif // MCRL2_DATA_DATA_OPERATION_H
