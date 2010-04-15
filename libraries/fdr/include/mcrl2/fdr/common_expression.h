// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/common_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_COMMON_EXPRESSION_H
#define MCRL2_FDR_COMMON_EXPRESSION_H

#include "mcrl2/fdr/common_expression_decl.h"
#include "mcrl2/fdr/common_expression_impl.h"
                                      
namespace mcrl2 {

namespace fdr {

//--- start generated is-functions ---//

    /// \brief Test for a conditional expression
    /// \param t A term
    /// \return True if it is a conditional expression
    inline
    bool is_conditional(const common_expression& t)
    {
      return fdr::detail::gsIsConditional(t);
    }

    /// \brief Test for a identifier expression
    /// \param t A term
    /// \return True if it is a identifier expression
    inline
    bool is_identifier(const common_expression& t)
    {
      return fdr::detail::gsIsName(t);
    }

    /// \brief Test for a lambdaappl expression
    /// \param t A term
    /// \return True if it is a lambdaappl expression
    inline
    bool is_lambdaappl(const common_expression& t)
    {
      return fdr::detail::gsIsLambdaAppl(t);
    }

    /// \brief Test for a localdef expression
    /// \param t A term
    /// \return True if it is a localdef expression
    inline
    bool is_localdef(const common_expression& t)
    {
      return fdr::detail::gsIsLocalDef(t);
    }

    /// \brief Test for a bracketed expression
    /// \param t A term
    /// \return True if it is a bracketed expression
    inline
    bool is_bracketed(const common_expression& t)
    {
      return fdr::detail::gsIsBracketed(t);
    }

    /// \brief Test for a pattern expression
    /// \param t A term
    /// \return True if it is a pattern expression
    inline
    bool is_pattern(const common_expression& t)
    {
      return fdr::detail::gsIsPattern(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_COMMON_EXPRESSION_H
