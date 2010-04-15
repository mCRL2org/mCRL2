// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/numeric_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_NUMERIC_EXPRESSION_H
#define MCRL2_FDR_NUMERIC_EXPRESSION_H

#include "mcrl2/fdr/numeric_expression_decl.h"
#include "mcrl2/fdr/numeric_expression_impl.h"
                                      
namespace mcrl2 {

namespace fdr {

//--- start generated is-functions ---//

    /// \brief Test for a number expression
    /// \param t A term
    /// \return True if it is a number expression
    inline
    bool is_number(const numeric_expression& t)
    {
      return fdr::detail::gsIsNumber(t);
    }

    /// \brief Test for a card expression
    /// \param t A term
    /// \return True if it is a card expression
    inline
    bool is_card(const numeric_expression& t)
    {
      return fdr::detail::gsIsCard(t);
    }

    /// \brief Test for a length expression
    /// \param t A term
    /// \return True if it is a length expression
    inline
    bool is_length(const numeric_expression& t)
    {
      return fdr::detail::gsIsLength(t);
    }

    /// \brief Test for a plus expression
    /// \param t A term
    /// \return True if it is a plus expression
    inline
    bool is_plus(const numeric_expression& t)
    {
      return fdr::detail::gsIsPlus(t);
    }

    /// \brief Test for a minus expression
    /// \param t A term
    /// \return True if it is a minus expression
    inline
    bool is_minus(const numeric_expression& t)
    {
      return fdr::detail::gsIsMinus(t);
    }

    /// \brief Test for a times expression
    /// \param t A term
    /// \return True if it is a times expression
    inline
    bool is_times(const numeric_expression& t)
    {
      return fdr::detail::gsIsTimes(t);
    }

    /// \brief Test for a div expression
    /// \param t A term
    /// \return True if it is a div expression
    inline
    bool is_div(const numeric_expression& t)
    {
      return fdr::detail::gsIsDiv(t);
    }

    /// \brief Test for a mod expression
    /// \param t A term
    /// \return True if it is a mod expression
    inline
    bool is_mod(const numeric_expression& t)
    {
      return fdr::detail::gsIsMod(t);
    }

    /// \brief Test for a min expression
    /// \param t A term
    /// \return True if it is a min expression
    inline
    bool is_min(const numeric_expression& t)
    {
      return fdr::detail::gsIsMin(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_NUMERIC_EXPRESSION_H
