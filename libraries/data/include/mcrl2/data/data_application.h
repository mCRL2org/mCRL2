// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_application.h
/// \brief The class data_application.

#ifndef MCRL2_DATA_DATA_APPLICATION_H
#define MCRL2_DATA_DATA_APPLICATION_H

#include <cassert>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/sort_arrow.h"
#include "mcrl2/core/detail/soundness_checks.h"

namespace mcrl2 {

namespace data {

/// \brief Data application.
///
// DataAppl(<DataExpr>, <DataExpr>+)
class data_application: public data_expression
{
  public:

    /// \brief Constructor.
    data_application()
      : data_expression(core::detail::constructDataAppl())
    {}

    /// \brief Constructor.
    /// \param t A term.
    data_application(atermpp::aterm_appl t)
     : data_expression(t)
    {
      assert(core::detail::check_term_DataAppl(m_term));
    }

    /// \brief Constructor.
    /// \param expr A data expression.
    /// \param args A sequence of data expressions.
    data_application(data_expression expr, data_expression_list args)
     : data_expression(core::detail::gsMakeDataAppl(expr, args))
    {}

    /// \brief Returns the head of the data application.
    /// \return The head of the data application.
    data_expression head() const
    {
      return core::detail::gsGetDataExprHead(*this);
    }

    /// \brief Returns the arguments of the data application.
    /// \return The arguments of the data application.
    data_expression_list arguments() const
    {
      return core::detail::gsGetDataExprArgs(*this);
    }

    /// \brief Returns the sort of the data application.
    /// \return The sort of the data application.
    sort_expression sort() const
    {
      sort_expression s = this->head().sort();
      assert(is_sort_arrow(s));
      return static_cast<const sort_arrow&>(s).result_sort();
    }

  };

/// \brief Read-only singly linked list of data applications
typedef atermpp::term_list<data_application> data_application_list;

/// \brief Returns true if the term t is a data application
/// \param t A term.
/// \return True if the term is a data application.
inline
bool is_data_application(atermpp::aterm_appl t)
{
  return core::detail::gsIsDataAppl(t);
}

} // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::data_application)
/// \endcond

#endif // MCRL2_DATA_DATA_APPLICATION_H
