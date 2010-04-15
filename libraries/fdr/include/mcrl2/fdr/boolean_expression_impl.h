// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/boolean_expression_impl.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_BOOLEAN_EXPRESSION_IMPL_H
#define MCRL2_FDR_BOOLEAN_EXPRESSION_IMPL_H

#include "mcrl2/fdr/boolean_expression_decl.h"

namespace mcrl2 {

namespace fdr {

//--- start generated class definitions ---//
    /// \brief Default constructor.
    inline
    true_::true_()
      : boolean_expression(fdr::detail::constructtrue())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    true_::true_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_true(m_term));
    }

    /// \brief Default constructor.
    inline
    false_::false_()
      : boolean_expression(fdr::detail::constructfalse())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    false_::false_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_false(m_term));
    }

    /// \brief Default constructor.
    inline
    and_::and_()
      : boolean_expression(fdr::detail::constructAnd())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    and_::and_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_And(m_term));
    }

    /// \brief Constructor.
    inline
    and_::and_(const boolean_expression& left, const boolean_expression& right)
      : boolean_expression(fdr::detail::gsMakeAnd(left, right))
    {}

    inline
    boolean_expression and_::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    boolean_expression and_::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    or_::or_()
      : boolean_expression(fdr::detail::constructOr())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    or_::or_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Or(m_term));
    }

    /// \brief Constructor.
    inline
    or_::or_(const boolean_expression& left, const boolean_expression& right)
      : boolean_expression(fdr::detail::gsMakeOr(left, right))
    {}

    inline
    boolean_expression or_::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    boolean_expression or_::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    not_::not_()
      : boolean_expression(fdr::detail::constructNot())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    not_::not_(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Not(m_term));
    }

    /// \brief Constructor.
    inline
    not_::not_(const boolean_expression& operand)
      : boolean_expression(fdr::detail::gsMakeNot(operand))
    {}

    inline
    boolean_expression not_::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    null::null()
      : boolean_expression(fdr::detail::constructNull())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    null::null(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Null(m_term));
    }

    /// \brief Constructor.
    inline
    null::null(const seq_expression& seq)
      : boolean_expression(fdr::detail::gsMakeNull(seq))
    {}

    inline
    seq_expression null::seq() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    elem::elem()
      : boolean_expression(fdr::detail::constructElem())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    elem::elem(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Elem(m_term));
    }

    /// \brief Constructor.
    inline
    elem::elem(const expression& expr, const seq_expression& seq)
      : boolean_expression(fdr::detail::gsMakeElem(expr, seq))
    {}

    inline
    expression elem::expr() const
    {
      return atermpp::arg1(*this);
    }

    inline
    seq_expression elem::seq() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    member::member()
      : boolean_expression(fdr::detail::constructMember())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    member::member(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Member(m_term));
    }

    /// \brief Constructor.
    inline
    member::member(const expression& expr, set_expression& set)
      : boolean_expression(fdr::detail::gsMakeMember(expr, set))
    {}

    inline
    expression member::expr() const
    {
      return atermpp::arg1(*this);
    }

    inline
    set_expression member::set() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    empty::empty()
      : boolean_expression(fdr::detail::constructEmpty())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    empty::empty(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Empty(m_term));
    }

    /// \brief Constructor.
    inline
    empty::empty(const set_expression& set)
      : boolean_expression(fdr::detail::gsMakeEmpty(set))
    {}

    inline
    set_expression empty::set() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    equal::equal()
      : boolean_expression(fdr::detail::constructEqual())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    equal::equal(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Equal(m_term));
    }

    /// \brief Constructor.
    inline
    equal::equal(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeEqual(left, right))
    {}

    inline
    expression equal::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    expression equal::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    notequal::notequal()
      : boolean_expression(fdr::detail::constructNotEqual())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    notequal::notequal(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_NotEqual(m_term));
    }

    /// \brief Constructor.
    inline
    notequal::notequal(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeNotEqual(left, right))
    {}

    inline
    expression notequal::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    expression notequal::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    less::less()
      : boolean_expression(fdr::detail::constructLess())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    less::less(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Less(m_term));
    }

    /// \brief Constructor.
    inline
    less::less(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeLess(left, right))
    {}

    inline
    expression less::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    expression less::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    lessorequal::lessorequal()
      : boolean_expression(fdr::detail::constructLessOrEqual())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    lessorequal::lessorequal(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_LessOrEqual(m_term));
    }

    /// \brief Constructor.
    inline
    lessorequal::lessorequal(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeLessOrEqual(left, right))
    {}

    inline
    expression lessorequal::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    expression lessorequal::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    greater::greater()
      : boolean_expression(fdr::detail::constructGreater())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    greater::greater(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_Greater(m_term));
    }

    /// \brief Constructor.
    inline
    greater::greater(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeGreater(left, right))
    {}

    inline
    expression greater::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    expression greater::right() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    greaterorequal::greaterorequal()
      : boolean_expression(fdr::detail::constructGreaterOrEqual())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    greaterorequal::greaterorequal(atermpp::aterm_appl term)
      : boolean_expression(term)
    {
      assert(fdr::detail::check_term_GreaterOrEqual(m_term));
    }

    /// \brief Constructor.
    inline
    greaterorequal::greaterorequal(const expression& left, const expression& right)
      : boolean_expression(fdr::detail::gsMakeGreaterOrEqual(left, right))
    {}

    inline
    expression greaterorequal::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    expression greaterorequal::right() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated class definitions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_BOOLEAN_EXPRESSION_IMPL_H
