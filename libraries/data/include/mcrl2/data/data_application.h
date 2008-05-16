// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING.
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

using atermpp::aterm_appl;
using atermpp::term_list;

/// \brief Data application.
///
// DataAppl(<DataExpr>, <DataExpr>+)
class data_application: public data_expression
{
  public:
    /// Constructor.
    ///
    data_application()
      : data_expression(core::detail::constructDataAppl())
    {}

    /// Constructor.
    ///
    data_application(aterm_appl t)
     : data_expression(t)
    {
      assert(core::detail::check_term_DataAppl(m_term));
    }

    /// Constructor.
    ///
    data_application(data_expression expr, data_expression_list args)
     : data_expression(core::detail::gsMakeDataAppl(expr, args))
    {}

    /// Returns the head of the data application.
    ///
    data_expression head() const
    {
      return core::gsGetDataExprHead(*this);
    }

    /// Returns the arguments of the data expression.
    ///
    data_expression_list arguments() const
    {
      return core::gsGetDataExprArgs(*this);
    }

    sort_expression sort() const
    {
      sort_expression s = this->head().sort();
      assert(is_sort_arrow(s));
      return static_cast<const sort_arrow&>(s).result_sort();
    }

  };

/// \brief singly linked list of data applications
///
typedef term_list<data_application> data_application_list;

/// \brief Returns true if the term t is a data application
inline
bool is_data_application(aterm_appl t)
{
  return core::detail::gsIsDataAppl(t);
}

} // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{
using mcrl2::data::data_application;

template<>
struct aterm_traits<data_application>
{
  typedef ATermAppl aterm_type;
  static void protect(data_application t)   { t.protect(); }
  static void unprotect(data_application t) { t.unprotect(); }
  static void mark(data_application t)      { t.mark(); }
  static ATerm term(data_application t)     { return t.term(); }
  static ATerm* ptr(data_application& t)    { return &t.term(); }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_DATA_DATA_APPLICATION_H
