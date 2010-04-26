// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/any_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_ANY_EXPRESSION_H
#define MCRL2_FDR_ANY_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/expression_fwd.h"
#include "mcrl2/fdr/process_expression_fwd.h"

namespace mcrl2 {

namespace fdr {

//--- start generated any expression class declarations ---//
/// \brief class any_expression
class any_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    any_expression()
      : atermpp::aterm_appl(fdr::detail::constructAny())
    {}

    /// \brief Constructor.
    /// \param term A term
    any_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_Any(m_term));
    }
};

/// \brief list of any_expressions
typedef atermpp::term_list<any_expression> any_expression_list;

/// \brief vector of any_expressions
typedef atermpp::vector<any_expression>    any_expression_vector;

/// \brief An expression
class expr: public any_expression
{
  public:
    /// \brief Default constructor.
    expr();

    /// \brief Constructor.
    /// \param term A term
    expr(atermpp::aterm_appl term);

    /// \brief Constructor.
    expr(const expression& operand);

    expression operand() const;
};

/// \brief A process
class proc: public any_expression
{
  public:
    /// \brief Default constructor.
    proc();

    /// \brief Constructor.
    /// \param term A term
    proc(atermpp::aterm_appl term);

    /// \brief Constructor.
    proc(const process_expression& operand);

    process_expression operand() const;
};
//--- end generated any expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a expr expression
    /// \param t A term
    /// \return True if it is a expr expression
    inline
    bool is_expr(const any_expression& t)
    {
      return fdr::detail::gsIsExpr(t);
    }

    /// \brief Test for a proc expression
    /// \param t A term
    /// \return True if it is a proc expression
    inline
    bool is_proc(const any_expression& t)
    {
      return fdr::detail::gsIsProc(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_ANY_EXPRESSION_H
