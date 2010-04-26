// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/field_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_FIELD_EXPRESSION_H
#define MCRL2_FDR_FIELD_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/expression_fwd.h"
#include "mcrl2/fdr/set_expression_fwd.h"

namespace mcrl2 {

namespace fdr {

//--- start generated field expression class declarations ---//
/// \brief class field_expression
class field_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    field_expression()
      : atermpp::aterm_appl(fdr::detail::constructField())
    {}

    /// \brief Constructor.
    /// \param term A term
    field_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_Field(m_term));
    }
};

/// \brief list of field_expressions
typedef atermpp::term_list<field_expression> field_expression_list;

/// \brief vector of field_expressions
typedef atermpp::vector<field_expression>    field_expression_vector;

/// \brief A simple input
class simpleinput: public field_expression
{
  public:
    /// \brief Default constructor.
    simpleinput();

    /// \brief Constructor.
    /// \param term A term
    simpleinput(atermpp::aterm_appl term);

    /// \brief Constructor.
    simpleinput(const expression& expr);

    expression expr() const;
};

/// \brief An input
class input: public field_expression
{
  public:
    /// \brief Default constructor.
    input();

    /// \brief Constructor.
    /// \param term A term
    input(atermpp::aterm_appl term);

    /// \brief Constructor.
    input(const expression& expr, const set_expression& restriction);

    expression expr() const;

    set_expression restriction() const;
};

/// \brief An output
class output: public field_expression
{
  public:
    /// \brief Default constructor.
    output();

    /// \brief Constructor.
    /// \param term A term
    output(atermpp::aterm_appl term);

    /// \brief Constructor.
    output(const expression& expr);

    expression expr() const;
};
//--- end generated field expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a simpleinput expression
    /// \param t A term
    /// \return True if it is a simpleinput expression
    inline
    bool is_simpleinput(const field_expression& t)
    {
      return fdr::detail::gsIsSimpleInput(t);
    }

    /// \brief Test for a input expression
    /// \param t A term
    /// \return True if it is a input expression
    inline
    bool is_input(const field_expression& t)
    {
      return fdr::detail::gsIsInput(t);
    }

    /// \brief Test for a output expression
    /// \param t A term
    /// \return True if it is a output expression
    inline
    bool is_output(const field_expression& t)
    {
      return fdr::detail::gsIsOutput(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_FIELD_EXPRESSION_H
