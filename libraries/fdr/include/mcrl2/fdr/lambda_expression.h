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

#include "mcrl2/fdr/lambda_expression_decl.h"
#include "mcrl2/fdr/lambda_expression_impl.h"
                                      
namespace mcrl2 {

namespace fdr {

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
