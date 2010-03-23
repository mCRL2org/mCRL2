// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/common_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_COMMON_EXPRESSION_H
#define MCRL2_FDR_COMMON_EXPRESSION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Set expression
  class common_expression: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      common_expression()
        : atermpp::aterm_appl(fdr::detail::constructCommon())
      {}

      /// \brief Constructor.
      /// \param term A term
      common_expression(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Common(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A conditional
class conditional: public common_expression
{
  public:
    /// \brief Default constructor.
    conditional()
      : common_expression(fdr::detail::constructConditional())
    {}

    /// \brief Constructor.
    /// \param term A term
    conditional(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_Conditional(m_term));
    }

    /// \brief Constructor.
    conditional(const boolean_expression& guard, const any& thenpart, const any& elsepart)
      : common_expression(fdr::detail::gsMakeConditional(guard, thenpart, elsepart))
    {}

    boolean_expression guard() const
    {
      return atermpp::arg1(*this);
    }

    any thenpart() const
    {
      return atermpp::arg2(*this);
    }

    any elsepart() const
    {
      return atermpp::arg3(*this);
    }
};

/// \brief A name
class name: public common_expression
{
  public:
    /// \brief Default constructor.
    name()
      : common_expression(fdr::detail::constructName())
    {}

    /// \brief Constructor.
    /// \param term A term
    name(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_Name(m_term));
    }

    /// \brief Constructor.
    name(const name& name)
      : common_expression(fdr::detail::gsMakeName(name))
    {}

    name name() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A lambda application
class lambdaappl: public common_expression
{
  public:
    /// \brief Default constructor.
    lambdaappl()
      : common_expression(fdr::detail::constructLambdaAppl())
    {}

    /// \brief Constructor.
    /// \param term A term
    lambdaappl(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_LambdaAppl(m_term));
    }

    /// \brief Constructor.
    lambdaappl(const lambda_expression& lambda, const expression_list& exprs)
      : common_expression(fdr::detail::gsMakeLambdaAppl(lambda, exprs))
    {}

    lambda_expression lambda() const
    {
      return atermpp::arg1(*this);
    }

    expression_list exprs() const
    {
      return atermpp::list_arg2(*this);
    }
};

/// \brief A local definition
class localdef: public common_expression
{
  public:
    /// \brief Default constructor.
    localdef()
      : common_expression(fdr::detail::constructLocalDef())
    {}

    /// \brief Constructor.
    /// \param term A term
    localdef(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_LocalDef(m_term));
    }

    /// \brief Constructor.
    localdef(const definition_list& defs, const any& any)
      : common_expression(fdr::detail::gsMakeLocalDef(defs, any))
    {}

    definition_list defs() const
    {
      return atermpp::list_arg1(*this);
    }

    any any() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A bracketed process or expression
class bracketed: public common_expression
{
  public:
    /// \brief Default constructor.
    bracketed()
      : common_expression(fdr::detail::constructBracketed())
    {}

    /// \brief Constructor.
    /// \param term A term
    bracketed(atermpp::aterm_appl term)
      : common_expression(term)
    {
      assert(fdr::detail::check_term_Bracketed(m_term));
    }

    /// \brief Constructor.
    bracketed(const any& operand)
      : common_expression(fdr::detail::gsMakeBracketed(operand))
    {}

    any operand() const
    {
      return atermpp::arg1(*this);
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a conditional expression
    /// \param t A term
    /// \return True if it is a conditional expression
    inline
    bool is_conditional(const common_expression& t)
    {
      return fdr::detail::gsIsConditional(t);
    }

    /// \brief Test for a name expression
    /// \param t A term
    /// \return True if it is a name expression
    inline
    bool is_name(const common_expression& t)
    {
      return fdr::detail::gsIsName(t);
    }

    /// \brief Test for a lambdaappl expression
    /// \param t A term
    /// \return True if it is a lambdaappl expression
    inline
    bool is_lambdaappl(const common_expression& t)
    {
      return fdr::detail::gsIsLambdaAppl(t);
    }

    /// \brief Test for a localdef expression
    /// \param t A term
    /// \return True if it is a localdef expression
    inline
    bool is_localdef(const common_expression& t)
    {
      return fdr::detail::gsIsLocalDef(t);
    }

    /// \brief Test for a bracketed expression
    /// \param t A term
    /// \return True if it is a bracketed expression
    inline
    bool is_bracketed(const common_expression& t)
    {
      return fdr::detail::gsIsBracketed(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_COMMON_EXPRESSION_H
