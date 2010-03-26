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
#include "mcrl2/fdr/detail/syntax_checks.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief An expression
class expr
{
  public:
    /// \brief Default constructor.
    expr()
      : atermpp::aterm_appl(fdr::detail::constructExpr())
    {}

    /// \brief Constructor.
    /// \param term A term
    expr(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Expr(m_term));
    }

    /// \brief Constructor.
    expr(const expression& operand)
      : atermpp::aterm_appl(fdr::detail::gsMakeExpr(operand))
    {}

    expression operand() const
    {
      return atermpp::arg1(*this);
    }
};/// \brief list of exprs
    typedef atermpp::term_list<expr> expr_list;

    /// \brief vector of exprs
    typedef atermpp::vector<expr>    expr_vector;


/// \brief A process
class proc
{
  public:
    /// \brief Default constructor.
    proc()
      : atermpp::aterm_appl(fdr::detail::constructProc())
    {}

    /// \brief Constructor.
    /// \param term A term
    proc(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Proc(m_term));
    }

    /// \brief Constructor.
    proc(const process& operand)
      : atermpp::aterm_appl(fdr::detail::gsMakeProc(operand))
    {}

    process operand() const
    {
      return atermpp::arg1(*this);
    }
};/// \brief list of procs
    typedef atermpp::term_list<proc> proc_list;

    /// \brief vector of procs
    typedef atermpp::vector<proc>    proc_vector;

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
