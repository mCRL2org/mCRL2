// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/tuple_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_TUPLE_EXPRESSION_H
#define MCRL2_FDR_TUPLE_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"

namespace mcrl2 {

namespace fdr {

//--- start generated tuple expression class declarations ---//
/// \brief class tuple_expression
class tuple_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    tuple_expression()
      : atermpp::aterm_appl(fdr::detail::constructTuple())
    {}

    /// \brief Constructor.
    /// \param term A term
    tuple_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_Tuple(m_term));
    }
};

/// \brief list of tuple_expressions
typedef atermpp::term_list<tuple_expression> tuple_expression_list;

/// \brief vector of tuple_expressions
typedef atermpp::vector<tuple_expression>    tuple_expression_vector;

/// \brief A tuple expression
class exprs: public tuple_expression
{
  public:
    /// \brief Default constructor.
    exprs();

    /// \brief Constructor.
    /// \param term A term
    exprs(atermpp::aterm_appl term);

    /// \brief Constructor.
    exprs(const expression_list& elements);

    expression_list elements() const;
};
//--- end generated tuple expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a exprs expression
    /// \param t A term
    /// \return True if it is a exprs expression
    inline
    bool is_exprs(const tuple_expression& t)
    {
      return fdr::detail::gsIsExprs(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_TUPLE_EXPRESSION_H
