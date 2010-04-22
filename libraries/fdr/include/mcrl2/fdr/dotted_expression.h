// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/dotted_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_DOTTED_EXPRESSION_H
#define MCRL2_FDR_DOTTED_EXPRESSION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

#include "mcrl2/fdr/expression.h"
                                      
namespace mcrl2 {

namespace fdr {

//--- start generated dotted expression class declarations ---//
/// \brief class dotted_expression
class dotted_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    dotted_expression()
      : atermpp::aterm_appl(fdr::detail::constructDotted())
    {}

    /// \brief Constructor.
    /// \param term A term
    dotted_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_Dotted(m_term));
    }
};

/// \brief list of dotted_expressions
typedef atermpp::term_list<dotted_expression> dotted_expression_list;

/// \brief vector of dotted_expressions
typedef atermpp::vector<dotted_expression>    dotted_expression_vector;

/// \brief A dotted expression
class dot: public dotted_expression
{
  public:
    /// \brief Default constructor.
    dot();

    /// \brief Constructor.
    /// \param term A term
    dot(atermpp::aterm_appl term);

    /// \brief Constructor.
    dot(const expression& left, expression& right);

    expression left() const;

    expression right() const;
};
//--- end generated dotted expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a dot expression
    /// \param t A term
    /// \return True if it is a dot expression
    inline
    bool is_dot(const dotted_expression& t)
    {
      return fdr::detail::gsIsDot(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_DOTTED_EXPRESSION_H
