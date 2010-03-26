// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/dotted_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_DOTTED_EXPRESSION_H
#define MCRL2_FDR_DOTTED_EXPRESSION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"
#include "mcrl2/fdr/common_expression.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Dotted expression
  class dotted_expression: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      dotted_expression()
        : atermpp::aterm_appl(fdr::detail::constructDotted())
      {}

      /// \brief Constructor.
      /// \param term A term
      dotted_expression(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Dotted(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A common expression
class common: public dotted_expression
{
  public:
    /// \brief Default constructor.
    common()
      : dotted_expression(fdr::detail::constructCommon())
    {}

    /// \brief Constructor.
    /// \param term A term
    common(atermpp::aterm_appl term)
      : dotted_expression(term)
    {
      assert(fdr::detail::check_term_Common(m_term));
    }

    /// \brief Constructor.
    common(const common_expression& operand)
      : dotted_expression(fdr::detail::gsMakeCommon(operand))
    {}

    common_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A dotted expression
class dot: public dotted_expression
{
  public:
    /// \brief Default constructor.
    dot()
      : dotted_expression(fdr::detail::constructDot())
    {}

    /// \brief Constructor.
    /// \param term A term
    dot(atermpp::aterm_appl term)
      : dotted_expression(term)
    {
      assert(fdr::detail::check_term_Dot(m_term));
    }

    /// \brief Constructor.
    dot(const expression& left, expression& right)
      : dotted_expression(fdr::detail::gsMakeDot(left, right))
    {}

    expression left() const
    {
      return atermpp::arg1(*this);
    }

    expression right() const
    {
      return atermpp::arg2(*this);
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a common expression
    /// \param t A term
    /// \return True if it is a common expression
    inline
    bool is_common(const dotted_expression& t)
    {
      return fdr::detail::gsIsCommon(t);
    }

    /// \brief Test for a dot expression
    /// \param t A term
    /// \return True if it is a dot expression
    inline
    bool is_dot(const dotted_expression& t)
    {
      return fdr::detail::gsIsDot(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_DOTTED_EXPRESSION_H
