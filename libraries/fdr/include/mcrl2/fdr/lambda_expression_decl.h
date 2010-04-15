// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/lambda_expression_decl.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_LAMBDA_EXPRESSION_DECL_H
#define MCRL2_FDR_LAMBDA_EXPRESSION_DECL_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

#include "mcrl2/fdr/expression_decl.h"

namespace mcrl2 {

namespace fdr {

//--- start generated class declarations ---//
/// \brief class lambda_expression
class lambda_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    lambda_expression()
      : atermpp::aterm_appl(fdr::detail::constructLambdaExpression())
    {}

    /// \brief Constructor.
    /// \param term A term
    lambda_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_LambdaExpression(m_term));
    }
};

/// \brief list of lambda_expressions
typedef atermpp::term_list<lambda_expression> lambda_expression_list;

/// \brief vector of lambda_expressions
typedef atermpp::vector<lambda_expression>    lambda_expression_vector;

/// \brief A lambda expression
class lambdaexpr: public lambda_expression
{
  public:
    /// \brief Default constructor.
    lambdaexpr();

    /// \brief Constructor.
    /// \param term A term
    lambdaexpr(atermpp::aterm_appl term);

    /// \brief Constructor.
    lambdaexpr(const expression_list& exprs, const any& function);

    expression_list exprs() const;

    any function() const;
};
//--- end generated class declarations ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_LAMBDA_EXPRESSION_DECL_H
