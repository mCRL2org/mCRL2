// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/tuple_expression_impl.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_TUPLE_EXPRESSION_IMPL_H
#define MCRL2_FDR_TUPLE_EXPRESSION_IMPL_H

#include "mcrl2/fdr/tuple_expression_decl.h"

namespace mcrl2 {

namespace fdr {

//--- start generated class definitions ---//
    /// \brief Default constructor.
    inline
    exprs::exprs()
      : tuple_expression(fdr::detail::constructExprs())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    exprs::exprs(atermpp::aterm_appl term)
      : tuple_expression(term)
    {
      assert(fdr::detail::check_term_Exprs(m_term));
    }

    /// \brief Constructor.
    inline
    exprs::exprs(const expression_list& elements)
      : tuple_expression(fdr::detail::gsMakeExprs(elements))
    {}

    inline
    expression_list exprs::elements() const
    {
      return atermpp::list_arg1(*this);
    }
//--- end generated class definitions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_TUPLE_EXPRESSION_IMPL_H
