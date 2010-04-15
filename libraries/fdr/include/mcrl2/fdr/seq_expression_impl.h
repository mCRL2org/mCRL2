// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/seq_expression_impl.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_SEQ_EXPRESSION_IMPL_H
#define MCRL2_FDR_SEQ_EXPRESSION_IMPL_H

#include "mcrl2/fdr/seq_expression_decl.h"

namespace mcrl2 {

namespace fdr {

//--- start generated class definitions ---//
    /// \brief Default constructor.
    inline
    cat::cat()
      : seq_expression(fdr::detail::constructCat())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    cat::cat(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Cat(m_term));
    }

    /// \brief Constructor.
    inline
    cat::cat(const seq_expression& left, const seq_expression& right)
      : seq_expression(fdr::detail::gsMakeCat(left, right))
    {}

    inline
    seq_expression cat::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    seq_expression cat::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    concat::concat()
      : seq_expression(fdr::detail::constructConcat())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    concat::concat(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Concat(m_term));
    }

    /// \brief Constructor.
    inline
    concat::concat(const seq_expression& seq)
      : seq_expression(fdr::detail::gsMakeConcat(seq))
    {}

    inline
    seq_expression concat::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    head::head()
      : seq_expression(fdr::detail::constructHead())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    head::head(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Head(m_term));
    }

    /// \brief Constructor.
    inline
    head::head(const seq_expression& seq)
      : seq_expression(fdr::detail::gsMakeHead(seq))
    {}

    inline
    seq_expression head::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    tail::tail()
      : seq_expression(fdr::detail::constructTail())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    tail::tail(atermpp::aterm_appl term)
      : seq_expression(term)
    {
      assert(fdr::detail::check_term_Tail(m_term));
    }

    /// \brief Constructor.
    inline
    tail::tail(const seq_expression& seq)
      : seq_expression(fdr::detail::gsMakeTail(seq))
    {}

    inline
    seq_expression tail::seq() const
    {
      return atermpp::arg1(*this);
    }
//--- end generated class definitions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_SEQ_EXPRESSION_IMPL_H
