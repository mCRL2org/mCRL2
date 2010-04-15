// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/seq_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_SEQ_EXPRESSION_H
#define MCRL2_FDR_SEQ_EXPRESSION_H

#include "mcrl2/fdr/seq_expression_decl.h"
#include "mcrl2/fdr/seq_expression_impl.h"
                                      
namespace mcrl2 {

namespace fdr {

//--- start generated is-functions ---//

    /// \brief Test for a cat expression
    /// \param t A term
    /// \return True if it is a cat expression
    inline
    bool is_cat(const seq_expression& t)
    {
      return fdr::detail::gsIsCat(t);
    }

    /// \brief Test for a concat expression
    /// \param t A term
    /// \return True if it is a concat expression
    inline
    bool is_concat(const seq_expression& t)
    {
      return fdr::detail::gsIsConcat(t);
    }

    /// \brief Test for a head expression
    /// \param t A term
    /// \return True if it is a head expression
    inline
    bool is_head(const seq_expression& t)
    {
      return fdr::detail::gsIsHead(t);
    }

    /// \brief Test for a tail expression
    /// \param t A term
    /// \return True if it is a tail expression
    inline
    bool is_tail(const seq_expression& t)
    {
      return fdr::detail::gsIsTail(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_SEQ_EXPRESSION_H
