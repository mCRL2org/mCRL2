// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/set_expression_impl.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_SET_EXPRESSION_IMPL_H
#define MCRL2_FDR_SET_EXPRESSION_IMPL_H

#include "mcrl2/fdr/set_expression_decl.h"

namespace mcrl2 {

namespace fdr {

//--- start generated class definitions ---//
    /// \brief Default constructor.
    inline
    chanset::chanset()
      : set_expression(fdr::detail::constructChanSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    chanset::chanset(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_ChanSet(m_term));
    }

    /// \brief Constructor.
    inline
    chanset::chanset(const targ& argument)
      : set_expression(fdr::detail::gsMakeChanSet(argument))
    {}

    inline
    targ chanset::argument() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    union::union()
      : set_expression(fdr::detail::constructunion())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    union::union(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_union(m_term));
    }

    /// \brief Constructor.
    inline
    union::union(const set_expression& left, const set_expression& right)
      : set_expression(fdr::detail::gsMakeunion(left, right))
    {}

    inline
    set_expression union::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    set_expression union::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    inter::inter()
      : set_expression(fdr::detail::constructinter())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    inter::inter(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_inter(m_term));
    }

    /// \brief Constructor.
    inline
    inter::inter(const set_expression& left, const set_expression& right)
      : set_expression(fdr::detail::gsMakeinter(left, right))
    {}

    inline
    set_expression inter::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    set_expression inter::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    diff::diff()
      : set_expression(fdr::detail::constructdiff())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    diff::diff(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_diff(m_term));
    }

    /// \brief Constructor.
    inline
    diff::diff(const set_expression& left, const set_expression& right)
      : set_expression(fdr::detail::gsMakediff(left, right))
    {}

    inline
    set_expression diff::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    set_expression diff::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    union::union()
      : set_expression(fdr::detail::constructUnion())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    union::union(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Union(m_term));
    }

    /// \brief Constructor.
    inline
    union::union(const set_expression& set)
      : set_expression(fdr::detail::gsMakeUnion(set))
    {}

    inline
    set_expression union::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    inter::inter()
      : set_expression(fdr::detail::constructInter())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    inter::inter(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Inter(m_term));
    }

    /// \brief Constructor.
    inline
    inter::inter(const set_expression& set)
      : set_expression(fdr::detail::gsMakeInter(set))
    {}

    inline
    set_expression inter::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    set::set()
      : set_expression(fdr::detail::constructset())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    set::set(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_set(m_term));
    }

    /// \brief Constructor.
    inline
    set::set(const seq_expression& seq)
      : set_expression(fdr::detail::gsMakeset(seq))
    {}

    inline
    seq_expression set::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    set::set()
      : set_expression(fdr::detail::constructSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    set::set(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Set(m_term));
    }

    /// \brief Constructor.
    inline
    set::set(const set_expression& set)
      : set_expression(fdr::detail::gsMakeSet(set))
    {}

    inline
    set_expression set::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    seq::seq()
      : set_expression(fdr::detail::constructSeq())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    seq::seq(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_Seq(m_term));
    }

    /// \brief Constructor.
    inline
    seq::seq(const seq_expression& seq)
      : set_expression(fdr::detail::gsMakeSeq(seq))
    {}

    inline
    seq_expression seq::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    extensions::extensions()
      : set_expression(fdr::detail::constructextensions())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    extensions::extensions(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_extensions(m_term));
    }

    /// \brief Constructor.
    inline
    extensions::extensions(const expression& expr)
      : set_expression(fdr::detail::gsMakeextensions(expr))
    {}

    inline
    expression extensions::expr() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    productions::productions()
      : set_expression(fdr::detail::constructproductions())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    productions::productions(atermpp::aterm_appl term)
      : set_expression(term)
    {
      assert(fdr::detail::check_term_productions(m_term));
    }

    /// \brief Constructor.
    inline
    productions::productions(const expression& expr)
      : set_expression(fdr::detail::gsMakeproductions(expr))
    {}

    inline
    expression productions::expr() const
    {
      return atermpp::arg1(*this);
    }
//--- end generated class definitions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_SET_EXPRESSION_IMPL_H
