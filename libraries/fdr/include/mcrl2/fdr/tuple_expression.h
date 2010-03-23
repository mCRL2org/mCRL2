// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/tuple_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_TUPLE_EXPRESSION_H
#define MCRL2_FDR_TUPLE_EXPRESSION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Tuple expression
  class tuple_expression: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      tuple_expression()
        : atermpp::aterm_appl(fdr::detail::constructTuple())
      {}

      /// \brief Constructor.
      /// \param term A term
      tuple_expression(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Tuple(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A common expression
class common: public tuple_expression
{
  public:
    /// \brief Default constructor.
    common()
      : tuple_expression(fdr::detail::constructCommon())
    {}

    /// \brief Constructor.
    /// \param term A term
    common(atermpp::aterm_appl term)
      : tuple_expression(term)
    {
      assert(fdr::detail::check_term_Common(m_term));
    }

    /// \brief Constructor.
    common(const common_expression& operand)
      : tuple_expression(fdr::detail::gsMakeCommon(operand))
    {}

    common_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A tuple expression
class exprs: public tuple_expression
{
  public:
    /// \brief Default constructor.
    exprs()
      : tuple_expression(fdr::detail::constructExprs())
    {}

    /// \brief Constructor.
    /// \param term A term
    exprs(atermpp::aterm_appl term)
      : tuple_expression(term)
    {
      assert(fdr::detail::check_term_Exprs(m_term));
    }

    /// \brief Constructor.
    exprs(const expression_list& exprs)
      : tuple_expression(fdr::detail::gsMakeExprs(exprs))
    {}

    expression_list exprs() const
    {
      return atermpp::list_arg1(*this);
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a common expression
    /// \param t A term
    /// \return True if it is a common expression
    inline
    bool is_common(const tuple_expression& t)
    {
      return fdr::detail::gsIsCommon(t);
    }

    /// \brief Test for a exprs expression
    /// \param t A term
    /// \return True if it is a exprs expression
    inline
    bool is_exprs(const tuple_expression& t)
    {
      return fdr::detail::gsIsExprs(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_TUPLE_EXPRESSION_H
