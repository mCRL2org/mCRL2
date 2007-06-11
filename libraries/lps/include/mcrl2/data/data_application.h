// Copyright (c) 2007 Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_application.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DATA_APPLICATION_H
#define MCRL2_DATA_DATA_APPLICATION_H

#include <cassert>
#include "atermpp/aterm_appl.h"
#include "atermpp/aterm_traits.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/basic/detail/soundness_checks.h"

namespace lps {

using atermpp::aterm_appl;
using atermpp::term_list;

///////////////////////////////////////////////////////////////////////////////
// data_application
/// \brief data application.
///
// DataAppl(<DataExpr>, <DataExpr>+)
class data_application: public data_expression
{
  public:
    data_application()
      : data_expression(detail::constructDataAppl())
    {}

    data_application(aterm_appl t)
     : data_expression(t)
    {
      assert(detail::check_term_DataAppl(m_term));
    }

    data_application(data_expression expr, data_expression_list args)
     : data_expression(gsMakeDataAppl(expr, args))
    {}
  };
                                                            
///////////////////////////////////////////////////////////////////////////////
// data_application_list
/// \brief singly linked list of data applications
///
typedef term_list<data_application> data_application_list;

/// \brief Returns true if the term t is a data application
inline
bool is_data_application(aterm_appl t)
{
  return gsIsDataAppl(t);
}

} // namespace lps

/// INTERNAL ONLY
namespace atermpp
{
using lps::data_application;

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

#endif // MCRL2_DATA_DATA_APPLICATION_H
