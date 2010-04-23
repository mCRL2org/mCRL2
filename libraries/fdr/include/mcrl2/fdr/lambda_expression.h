// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/lambda_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_LAMBDA_EXPRESSION_H
#define MCRL2_FDR_LAMBDA_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/any.h"
                                      
namespace mcrl2 {

namespace fdr {

class expression;
typedef atermpp::term_list<expression> expression_list;

//--- start generated lambda expression class declarations ---//
/// \brief class lambda_expression
class lambda_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    lambda_expression()
      : atermpp::aterm_appl(fdr::detail::constructLambda())
    {}

    /// \brief Constructor.
    /// \param term A term
    lambda_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_Lambda(m_term));
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
//--- end generated lambda expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a lambdaexpr expression
    /// \param t A term
    /// \return True if it is a lambdaexpr expression
    inline
    bool is_lambdaexpr(const lambda_expression& t)
    {
      return fdr::detail::gsIsLambdaExpr(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_LAMBDA_EXPRESSION_H
