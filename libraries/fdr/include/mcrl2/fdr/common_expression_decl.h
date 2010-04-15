// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/common_expression_decl.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_COMMON_EXPRESSION_DECL_H
#define MCRL2_FDR_COMMON_EXPRESSION_DECL_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

#include "mcrl2/fdr/boolean_expression_decl.h"

namespace mcrl2 {

namespace fdr {

//--- start generated class declarations ---//
/// \brief class common_expression
class common_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    common_expression()
      : atermpp::aterm_appl(fdr::detail::constructCommonExpression())
    {}

    /// \brief Constructor.
    /// \param term A term
    common_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_CommonExpression(m_term));
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
//--- end generated class declarations ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_COMMON_EXPRESSION_DECL_H
