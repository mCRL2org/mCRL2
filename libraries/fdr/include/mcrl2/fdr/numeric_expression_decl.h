// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/numeric_expression_decl.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_NUMERIC_EXPRESSION_DECL_H
#define MCRL2_FDR_NUMERIC_EXPRESSION_DECL_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

#include "mcrl2/fdr/set_expression_decl.h"

namespace mcrl2 {

namespace fdr {

//--- start generated class declarations ---//
/// \brief class numeric_expression
class numeric_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    numeric_expression()
      : atermpp::aterm_appl(fdr::detail::constructNumericExpression())
    {}

    /// \brief Constructor.
    /// \param term A term
    numeric_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_NumericExpression(m_term));
    }
};

/// \brief list of numeric_expressions
typedef atermpp::term_list<numeric_expression> numeric_expression_list;

/// \brief vector of numeric_expressions
typedef atermpp::vector<numeric_expression>    numeric_expression_vector;

/// \brief A number
class number: public numeric_expression
{
  public:
    /// \brief Default constructor.
    number();

    /// \brief Constructor.
    /// \param term A term
    number(atermpp::aterm_appl term);

    /// \brief Constructor.
    number(const number& operand);

    number operand() const;
};

/// \brief The cardinality of a set
class card: public numeric_expression
{
  public:
    /// \brief Default constructor.
    card();

    /// \brief Constructor.
    /// \param term A term
    card(atermpp::aterm_appl term);

    /// \brief Constructor.
    card(const set_expression& set);

    set_expression set() const;
};

/// \brief The length of a sequence
class length: public numeric_expression
{
  public:
    /// \brief Default constructor.
    length();

    /// \brief Constructor.
    /// \param term A term
    length(atermpp::aterm_appl term);

    /// \brief Constructor.
    length(const seq_expression& seq);

    seq_expression seq() const;
};

/// \brief An addition
class plus: public numeric_expression
{
  public:
    /// \brief Default constructor.
    plus();

    /// \brief Constructor.
    /// \param term A term
    plus(atermpp::aterm_appl term);

    /// \brief Constructor.
    plus(const numeric_expression& left, const numeric_expression& right);

    numeric_expression left() const;

    numeric_expression right() const;
};

/// \brief A subtraction
class minus: public numeric_expression
{
  public:
    /// \brief Default constructor.
    minus();

    /// \brief Constructor.
    /// \param term A term
    minus(atermpp::aterm_appl term);

    /// \brief Constructor.
    minus(const numeric_expression& left, const numeric_expression& right);

    numeric_expression left() const;

    numeric_expression right() const;
};

/// \brief A multiplication
class times: public numeric_expression
{
  public:
    /// \brief Default constructor.
    times();

    /// \brief Constructor.
    /// \param term A term
    times(atermpp::aterm_appl term);

    /// \brief Constructor.
    times(const numeric_expression& left, const numeric_expression& right);

    numeric_expression left() const;

    numeric_expression right() const;
};

/// \brief A division
class div: public numeric_expression
{
  public:
    /// \brief Default constructor.
    div();

    /// \brief Constructor.
    /// \param term A term
    div(atermpp::aterm_appl term);

    /// \brief Constructor.
    div(const numeric_expression& left, const numeric_expression& right);

    numeric_expression left() const;

    numeric_expression right() const;
};

/// \brief A modulo operation
class mod: public numeric_expression
{
  public:
    /// \brief Default constructor.
    mod();

    /// \brief Constructor.
    /// \param term A term
    mod(atermpp::aterm_appl term);

    /// \brief Constructor.
    mod(const numeric_expression& left, const numeric_expression& right);

    numeric_expression left() const;

    numeric_expression right() const;
};

/// \brief A negative number
class min: public numeric_expression
{
  public:
    /// \brief Default constructor.
    min();

    /// \brief Constructor.
    /// \param term A term
    min(atermpp::aterm_appl term);

    /// \brief Constructor.
    min(const numeric_expression& operand);

    numeric_expression operand() const;
};
//--- end generated class declarations ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_NUMERIC_EXPRESSION_DECL_H
