// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/common_expression_impl.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_COMMON_EXPRESSION_IMPL_H
#define MCRL2_FDR_COMMON_EXPRESSION_IMPL_H

#include "mcrl2/fdr/common_expression_decl.h"

namespace mcrl2 {

namespace fdr {

//--- start generated class definitions ---//
    /// \brief Default constructor.
    inline
    conditional::conditional()
      : common_expression(fdr::detail::constructConditional())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    conditional::conditional(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_Conditional(m_term));
    }

    /// \brief Constructor.
    inline
    conditional::conditional(const boolean_expression& guard, const any& thenpart, const any& elsepart)
      : common_expression(fdr::detail::gsMakeConditional(guard, thenpart, elsepart))
    {}

    inline
    boolean_expression conditional::guard() const
    {
      return atermpp::arg1(*this);
    }

    inline
    any conditional::thenpart() const
    {
      return atermpp::arg2(*this);
    }

    inline
    any conditional::elsepart() const
    {
      return atermpp::arg3(*this);
    }

    /// \brief Default constructor.
    inline
    identifier::identifier()
      : common_expression(fdr::detail::constructName())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    identifier::identifier(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_Name(m_term));
    }

    /// \brief Constructor.
    inline
    identifier::identifier(const name& id)
      : common_expression(fdr::detail::gsMakeName(id))
    {}

    inline
    name identifier::id() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    lambdaappl::lambdaappl()
      : common_expression(fdr::detail::constructLambdaAppl())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    lambdaappl::lambdaappl(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_LambdaAppl(m_term));
    }

    /// \brief Constructor.
    inline
    lambdaappl::lambdaappl(const lambda_expression& lambda, const expression_list& exprs)
      : common_expression(fdr::detail::gsMakeLambdaAppl(lambda, exprs))
    {}

    inline
    lambda_expression lambdaappl::lambda() const
    {
      return atermpp::arg1(*this);
    }

    inline
    expression_list lambdaappl::exprs() const
    {
      return atermpp::list_arg2(*this);
    }

    /// \brief Default constructor.
    inline
    localdef::localdef()
      : common_expression(fdr::detail::constructLocalDef())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    localdef::localdef(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_LocalDef(m_term));
    }

    /// \brief Constructor.
    inline
    localdef::localdef(const definition_list& defs, const any& within)
      : common_expression(fdr::detail::gsMakeLocalDef(defs, within))
    {}

    inline
    definition_list localdef::defs() const
    {
      return atermpp::list_arg1(*this);
    }

    inline
    any localdef::within() const
    {
      return atermpp::arg2(*this);
    }

    /// \brief Default constructor.
    inline
    bracketed::bracketed()
      : common_expression(fdr::detail::constructBracketed())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    bracketed::bracketed(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_Bracketed(m_term));
    }

    /// \brief Constructor.
    inline
    bracketed::bracketed(const any& operand)
      : common_expression(fdr::detail::gsMakeBracketed(operand))
    {}

    inline
    any bracketed::operand() const
    {
      return atermpp::arg1(*this);
    }

    /// \brief Default constructor.
    inline
    pattern::pattern()
      : common_expression(fdr::detail::constructPattern())
    {}

    /// \brief Constructor.
    /// \param term A term
    inline
    pattern::pattern(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_Pattern(m_term));
    }

    /// \brief Constructor.
    inline
    pattern::pattern(const any& left, const any& right)
      : common_expression(fdr::detail::gsMakePattern(left, right))
    {}

    inline
    any pattern::left() const
    {
      return atermpp::arg1(*this);
    }

    inline
    any pattern::right() const
    {
      return atermpp::arg2(*this);
    }
//--- end generated class definitions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_COMMON_EXPRESSION_IMPL_H
