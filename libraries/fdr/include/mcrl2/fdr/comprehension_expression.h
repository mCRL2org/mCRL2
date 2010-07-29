// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/comprehension_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_COMPREHENSION_EXPRESSION_H
#define MCRL2_FDR_COMPREHENSION_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/expression_fwd.h"
#include "mcrl2/fdr/boolean_expression_fwd.h"

namespace mcrl2 {

namespace fdr {

//--- start generated comprehension expression class declarations ---//
/// \brief class comprehension_expression
class comprehension_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    comprehension_expression()
      : atermpp::aterm_appl(fdr::detail::constructComprehension())
    {}

    /// \brief Constructor.
    /// \param term A term
    comprehension_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_Comprehension(m_term));
    }
};

/// \brief list of comprehension_expressions
typedef atermpp::term_list<comprehension_expression> comprehension_expression_list;

/// \brief vector of comprehension_expressions
typedef atermpp::vector<comprehension_expression>    comprehension_expression_vector;

/// \brief An empty
class nil: public comprehension_expression
{
  public:
    /// \brief Default constructor.
    nil();

    /// \brief Constructor.
    /// \param term A term
    nil(atermpp::aterm_appl term);
};

/// \brief A boolean
class bcomprehension: public comprehension_expression
{
  public:
    /// \brief Default constructor.
    bcomprehension();

    /// \brief Constructor.
    /// \param term A term
    bcomprehension(atermpp::aterm_appl term);

    /// \brief Constructor.
    bcomprehension(const boolean_expression& operand);

    boolean_expression operand() const;
};

/// \brief A comprehension
class ecomprehension: public comprehension_expression
{
  public:
    /// \brief Default constructor.
    ecomprehension();

    /// \brief Constructor.
    /// \param term A term
    ecomprehension(atermpp::aterm_appl term);

    /// \brief Constructor.
    ecomprehension(const expression& left, const expression& right);

    expression left() const;

    expression right() const;
};
//--- end generated comprehension expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a nil expression
    /// \param t A term
    /// \return True if it is a nil expression
    inline
    bool is_nil(const comprehension_expression& t)
    {
      return fdr::detail::gsIsNil(t);
    }

    /// \brief Test for a bcomprehension expression
    /// \param t A term
    /// \return True if it is a bcomprehension expression
    inline
    bool is_bcomprehension(const comprehension_expression& t)
    {
      return fdr::detail::gsIsBComprehension(t);
    }

    /// \brief Test for a ecomprehension expression
    /// \param t A term
    /// \return True if it is a ecomprehension expression
    inline
    bool is_ecomprehension(const comprehension_expression& t)
    {
      return fdr::detail::gsIsEComprehension(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_COMPREHENSION_EXPRESSION_H
