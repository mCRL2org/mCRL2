// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/dotted_expression_impl.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_DOTTED_EXPRESSION_IMPL_H
#define MCRL2_FDR_DOTTED_EXPRESSION_IMPL_H

#include "mcrl2/fdr/dotted_expression_decl.h"

namespace mcrl2 {

namespace fdr {

//--- start generated class definitions ---//
    /// \brief Default constructor.
    inline
    dot::dot()
      : dotted_expression(fdr::detail::constructDot())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    dot::dot(atermpp::aterm_appl term)
      : dotted_expression(term)
    {
      assert(fdr::detail::check_term_Dot(m_term));
    }

    /// \brief Constructor.
    inline
    dot::dot(const expression& left, expression& right)
      : dotted_expression(fdr::detail::gsMakeDot(left, right))
    {}

    inline
    expression dot::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    expression dot::right() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated class definitions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_DOTTED_EXPRESSION_IMPL_H
