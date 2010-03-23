// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/any.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_ANY_H
#define MCRL2_FDR_ANY_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Any
  class any: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      any()
        : atermpp::aterm_appl(fdr::detail::constructAny())
      {}

      /// \brief Constructor.
      /// \param term A term
      any(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Any(m_term));
      }
  };

//--- start generated classes ---//
/// \brief An expression
class expr: public any
{
  public:
    /// \brief Default constructor.
    expr()
      : any(fdr::detail::constructExpr())
    {}

    /// \brief Constructor.
    /// \param term A term
    expr(atermpp::aterm_appl term)
      : any(term)
    {
      assert(fdr::detail::check_term_Expr(m_term));
    }

    /// \brief Constructor.
    expr(const expression& operand)
      : any(fdr::detail::gsMakeExpr(operand))
    {}

    expression operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A process
class proc: public any
{
  public:
    /// \brief Default constructor.
    proc()
      : any(fdr::detail::constructProc())
    {}

    /// \brief Constructor.
    /// \param term A term
    proc(atermpp::aterm_appl term)
      : any(term)
    {
      assert(fdr::detail::check_term_Proc(m_term));
    }

    /// \brief Constructor.
    proc(const process& operand)
      : any(fdr::detail::gsMakeProc(operand))
    {}

    process operand() const
    {
      return atermpp::arg1(*this);
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a expr expression
    /// \param t A term
    /// \return True if it is a expr expression
    inline
    bool is_expr(const any& t)
    {
      return fdr::detail::gsIsExpr(t);
    }

    /// \brief Test for a proc expression
    /// \param t A term
    /// \return True if it is a proc expression
    inline
    bool is_proc(const any& t)
    {
      return fdr::detail::gsIsProc(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_ANY_H
