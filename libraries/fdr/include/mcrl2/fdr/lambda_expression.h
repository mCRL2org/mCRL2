// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/lambda_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_LAMBDA_EXPRESSION_H
#define MCRL2_FDR_LAMBDA_EXPRESSION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Lambda expression
  class lambda_expression: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      lambda_expression()
        : atermpp::aterm_appl(fdr::atermpp::detail::constructLambda())
      {}

      /// \brief Constructor.
      /// \param term A term
      lambda_expression(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Lambda(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A lambda expression
class lambdaexpr: public lambda_expression
{
  public:
    /// \brief Default constructor.
    lambdaexpr()
      : lambda_expression(fdr::atermpp::detail::constructLambdaExpr())
    {}

    /// \brief Constructor.
    /// \param term A term
    lambdaexpr(atermpp::aterm_appl term)
      : lambda_expression(term)
    {
      assert(fdr::detail::check_term_LambdaExpr(m_term));
    }

    /// \brief Constructor.
    lambdaexpr(const expression_list& exprs, const any& any)
      : lambda_expression(fdr::detail::gsMakeLambdaExpr(exprs, any))
    {}

    expression_list exprs() const
    {
      return atermpp::list_arg1(*this);
    }

    any any() const
    {
      return atermpp::arg2(*this);
    }
};
//--- end generated classes ---//

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
