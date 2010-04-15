// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/lambda_expression_impl.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_LAMBDA_EXPRESSION_IMPL_H
#define MCRL2_FDR_LAMBDA_EXPRESSION_IMPL_H

#include "mcrl2/fdr/lambda_expression_decl.h"

namespace mcrl2 {

namespace fdr {

//--- start generated class definitions ---//
    /// \brief Default constructor.
    inline
    lambdaexpr::lambdaexpr()
      : lambda_expression(fdr::detail::constructLambdaExpr())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    lambdaexpr::lambdaexpr(atermpp::aterm_appl term)
      : lambda_expression(term)
    {
      assert(fdr::detail::check_term_LambdaExpr(m_term));
    }

    /// \brief Constructor.
    inline
    lambdaexpr::lambdaexpr(const expression_list& exprs, const any& function)
      : lambda_expression(fdr::detail::gsMakeLambdaExpr(exprs, function))
    {}

    inline
    expression_list lambdaexpr::exprs() const
    {
      return atermpp::list_arg1(*this);
    }

    inline
    any lambdaexpr::function() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated class definitions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_LAMBDA_EXPRESSION_IMPL_H
