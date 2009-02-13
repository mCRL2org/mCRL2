// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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

/// \brief Operation on data.
class data_operation: public data_expression
{
  public:

    /// \brief Constructor.
    data_operation()
      : data_expression(core::detail::constructOpId())
    {}

    /// \brief Constructor.
    /// \param t A term
    data_operation(atermpp::aterm_appl t)
     : data_expression(t)
    {
      assert(core::detail::check_rule_OpId(m_term));
    }

    /// \brief Constructor.
    /// \param name A
    /// \param s A sort expression
    data_operation(core::identifier_string name, sort_expression s)
     : data_expression(core::detail::gsMakeOpId(name, s))
    {}

    /// \brief Returns the name of the data_operation.
    /// \return The name of the data operation.
    core::identifier_string name() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Returns the sort of the data_operation.
    /// \return The sort of the data operation.
    sort_expression sort() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Apply the data_operation to a list of arguments.
    /// \param args A sequence of data expressions
    /// \return The application of the operation to the arguments.
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

/// \brief Read-only singly linked list of data operations
typedef atermpp::term_list<data_operation> data_operation_list;

/// \brief Returns true if the term t is a data operation
/// \param t A term
/// \return True if the term is a data operation.
inline
bool is_data_operation(atermpp::aterm_appl t)
{
  return core::detail::gsIsOpId(t);
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DATA_OPERATION_H
