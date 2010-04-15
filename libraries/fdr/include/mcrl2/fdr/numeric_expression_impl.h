// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/numeric_expression_impl.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_NUMERIC_EXPRESSION_IMPL_H
#define MCRL2_FDR_NUMERIC_EXPRESSION_IMPL_H

#include "mcrl2/fdr/numeric_expression_decl.h"

namespace mcrl2 {

namespace fdr {

//--- start generated class definitions ---//
    /// \brief Default constructor.
    inline
    number::number()
      : numeric_expression(fdr::detail::constructNumber())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    number::number(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Number(m_term));
    }

    /// \brief Constructor.
    inline
    number::number(const number& operand)
      : numeric_expression(fdr::detail::gsMakeNumber(operand))
    {}

    inline
    number number::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    card::card()
      : numeric_expression(fdr::detail::constructCard())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    card::card(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Card(m_term));
    }

    /// \brief Constructor.
    inline
    card::card(const set_expression& set)
      : numeric_expression(fdr::detail::gsMakeCard(set))
    {}

    inline
    set_expression card::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    length::length()
      : numeric_expression(fdr::detail::constructLength())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    length::length(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Length(m_term));
    }

    /// \brief Constructor.
    inline
    length::length(const seq_expression& seq)
      : numeric_expression(fdr::detail::gsMakeLength(seq))
    {}

    inline
    seq_expression length::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    plus::plus()
      : numeric_expression(fdr::detail::constructPlus())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    plus::plus(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Plus(m_term));
    }

    /// \brief Constructor.
    inline
    plus::plus(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakePlus(left, right))
    {}

    inline
    numeric_expression plus::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    numeric_expression plus::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    minus::minus()
      : numeric_expression(fdr::detail::constructMinus())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    minus::minus(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Minus(m_term));
    }

    /// \brief Constructor.
    inline
    minus::minus(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakeMinus(left, right))
    {}

    inline
    numeric_expression minus::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    numeric_expression minus::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    times::times()
      : numeric_expression(fdr::detail::constructTimes())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    times::times(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Times(m_term));
    }

    /// \brief Constructor.
    inline
    times::times(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakeTimes(left, right))
    {}

    inline
    numeric_expression times::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    numeric_expression times::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    div::div()
      : numeric_expression(fdr::detail::constructDiv())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    div::div(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Div(m_term));
    }

    /// \brief Constructor.
    inline
    div::div(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakeDiv(left, right))
    {}

    inline
    numeric_expression div::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    numeric_expression div::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    mod::mod()
      : numeric_expression(fdr::detail::constructMod())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    mod::mod(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Mod(m_term));
    }

    /// \brief Constructor.
    inline
    mod::mod(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakeMod(left, right))
    {}

    inline
    numeric_expression mod::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    numeric_expression mod::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    min::min()
      : numeric_expression(fdr::detail::constructMin())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    min::min(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Min(m_term));
    }

    /// \brief Constructor.
    inline
    min::min(const numeric_expression& operand)
      : numeric_expression(fdr::detail::gsMakeMin(operand))
    {}

    inline
    numeric_expression min::operand() const
    {
      return atermpp::arg1(*this);
    }
//--- end generated class definitions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_NUMERIC_EXPRESSION_IMPL_H
