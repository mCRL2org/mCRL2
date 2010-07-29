// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_EXPRESSION_H
#define MCRL2_FDR_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/boolean_expression_fwd.h"
#include "mcrl2/fdr/dotted_expression_fwd.h"
#include "mcrl2/fdr/lambda_expression_fwd.h"
#include "mcrl2/fdr/seq_expression_fwd.h"
#include "mcrl2/fdr/set_expression_fwd.h"
#include "mcrl2/fdr/numeric_expression_fwd.h"
#include "mcrl2/fdr/tuple_expression_fwd.h"

namespace mcrl2 {

namespace fdr {

//--- start generated  expression class declarations ---//
/// \brief class expression
class expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    expression()
      : atermpp::aterm_appl(fdr::detail::constructExpr())
    {}

    /// \brief Constructor.
    /// \param term A term
    expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_Expr(m_term));
    }
};

/// \brief list of expressions
typedef atermpp::term_list<expression> expression_list;

/// \brief vector of expressions
typedef atermpp::vector<expression>    expression_vector;

//--- end generated  expression class declarations ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_EXPRESSION_H
