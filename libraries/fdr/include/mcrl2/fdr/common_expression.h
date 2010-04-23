// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/common_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_COMMON_EXPRESSION_H
#define MCRL2_FDR_COMMON_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/any.h"
#include "mcrl2/fdr/boolean_expression.h"
                                      
namespace mcrl2 {

namespace fdr {

//--- start generated common expression class declarations ---//
/// \brief class common_expression
class common_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
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

/// \brief list of common_expressions
typedef atermpp::term_list<common_expression> common_expression_list;

/// \brief vector of common_expressions
typedef atermpp::vector<common_expression>    common_expression_vector;

/// \brief A conditional
class conditional: public common_expression
{
  public:
    /// \brief Default constructor.
    conditional();

    /// \brief Constructor.
    /// \param term A term
    conditional(atermpp::aterm_appl term);

    /// \brief Constructor.
    conditional(const boolean_expression& guard, const any& thenpart, const any& elsepart);

    boolean_expression guard() const;

    any thenpart() const;

    any elsepart() const;
};

/// \brief A name
class identifier: public common_expression
{
  public:
    /// \brief Default constructor.
    identifier();

    /// \brief Constructor.
    /// \param term A term
    identifier(atermpp::aterm_appl term);

    /// \brief Constructor.
    identifier(const name& id);

    name id() const;
};

/// \brief A lambda application
class lambdaappl: public common_expression
{
  public:
    /// \brief Default constructor.
    lambdaappl();

    /// \brief Constructor.
    /// \param term A term
    lambdaappl(atermpp::aterm_appl term);

    /// \brief Constructor.
    lambdaappl(const lambda_expression& lambda, const expression_list& exprs);

    lambda_expression lambda() const;

    expression_list exprs() const;
};

/// \brief A local definition
class localdef: public common_expression
{
  public:
    /// \brief Default constructor.
    localdef();

    /// \brief Constructor.
    /// \param term A term
    localdef(atermpp::aterm_appl term);

    /// \brief Constructor.
    localdef(const definition_list& defs, const any& within);

    definition_list defs() const;

    any within() const;
};

/// \brief A bracketed process or expression
class bracketed: public common_expression
{
  public:
    /// \brief Default constructor.
    bracketed();

    /// \brief Constructor.
    /// \param term A term
    bracketed(atermpp::aterm_appl term);

    /// \brief Constructor.
    bracketed(const any& operand);

    any operand() const;
};

/// \brief A combination of two patterns
class pattern: public common_expression
{
  public:
    /// \brief Default constructor.
    pattern();

    /// \brief Constructor.
    /// \param term A term
    pattern(atermpp::aterm_appl term);

    /// \brief Constructor.
    pattern(const any& left, const any& right);

    any left() const;

    any right() const;
};
//--- end generated common expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a conditional expression
    /// \param t A term
    /// \return True if it is a conditional expression
    inline
    bool is_conditional(const common_expression& t)
    {
      return fdr::detail::gsIsConditional(t);
    }

    /// \brief Test for a identifier expression
    /// \param t A term
    /// \return True if it is a identifier expression
    inline
    bool is_identifier(const common_expression& t)
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

    /// \brief Test for a pattern expression
    /// \param t A term
    /// \return True if it is a pattern expression
    inline
    bool is_pattern(const common_expression& t)
    {
      return fdr::detail::gsIsPattern(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_COMMON_EXPRESSION_H
