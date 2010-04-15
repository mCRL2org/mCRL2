// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/expression_impl.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_EXPRESSION_IMPL_H
#define MCRL2_FDR_EXPRESSION_IMPL_H

#include "mcrl2/fdr/expression_decl.h"

namespace mcrl2 {

namespace fdr {

//--- start generated class definitions ---//
    /// \brief Default constructor.
    inline
    numb::numb()
      : expression(fdr::detail::constructNumb())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    numb::numb(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Numb(m_term));
    }

    /// \brief Constructor.
    inline
    numb::numb(const numeric_expression& operand)
      : expression(fdr::detail::gsMakeNumb(operand))
    {}

    inline
    numeric_expression numb::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    bool_::bool_()
      : expression(fdr::detail::constructBool())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    bool_::bool_(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Bool(m_term));
    }

    /// \brief Constructor.
    inline
    bool_::bool_(const boolean_expression& operand)
      : expression(fdr::detail::gsMakeBool(operand))
    {}

    inline
    boolean_expression bool_::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    set::set()
      : expression(fdr::detail::constructSet())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    set::set(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Set(m_term));
    }

    /// \brief Constructor.
    inline
    set::set(const set_expression& operand)
      : expression(fdr::detail::gsMakeSet(operand))
    {}

    inline
    set_expression set::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    seq::seq()
      : expression(fdr::detail::constructSeq())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    seq::seq(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Seq(m_term));
    }

    /// \brief Constructor.
    inline
    seq::seq(const seq_expression& operand)
      : expression(fdr::detail::gsMakeSeq(operand))
    {}

    inline
    seq_expression seq::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    tuple::tuple()
      : expression(fdr::detail::constructTuple())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    tuple::tuple(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Tuple(m_term));
    }

    /// \brief Constructor.
    inline
    tuple::tuple(const tuple_expression& operand)
      : expression(fdr::detail::gsMakeTuple(operand))
    {}

    inline
    tuple_expression tuple::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    dotted::dotted()
      : expression(fdr::detail::constructDotted())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    dotted::dotted(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Dotted(m_term));
    }

    /// \brief Constructor.
    inline
    dotted::dotted(const dotted_expression& operand)
      : expression(fdr::detail::gsMakeDotted(operand))
    {}

    inline
    dotted_expression dotted::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    lambda::lambda()
      : expression(fdr::detail::constructLambda())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    lambda::lambda(atermpp::aterm_appl term)
      : expression(term)
    {
      assert(fdr::detail::check_term_Lambda(m_term));
    }

    /// \brief Constructor.
    inline
    lambda::lambda(const lambda_expression& operand)
      : expression(fdr::detail::gsMakeLambda(operand))
    {}

    inline
    lambda_expression lambda::operand() const
    {
      return atermpp::arg1(*this);
    }
//--- end generated class definitions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_EXPRESSION_IMPL_H
