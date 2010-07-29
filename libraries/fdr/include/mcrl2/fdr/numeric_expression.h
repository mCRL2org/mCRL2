// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/numeric_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_NUMERIC_EXPRESSION_H
#define MCRL2_FDR_NUMERIC_EXPRESSION_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/seq_expression.h"
#include "mcrl2/fdr/set_expression.h"
                                      
namespace mcrl2 {

namespace fdr {

//--- start generated numeric expression class declarations ---//
/// \brief class numeric_expression
class numeric_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    numeric_expression()
      : atermpp::aterm_appl(fdr::detail::constructNumber())
    {}

    /// \brief Constructor.
    /// \param term A term
    numeric_expression(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_rule_Number(m_term));
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
//--- end generated numeric expression class declarations ---//

//--- start generated is-functions ---//

    /// \brief Test for a number expression
    /// \param t A term
    /// \return True if it is a number expression
    inline
    bool is_number(const numeric_expression& t)
    {
      return fdr::detail::gsIsNumber(t);
    }

    /// \brief Test for a card expression
    /// \param t A term
    /// \return True if it is a card expression
    inline
    bool is_card(const numeric_expression& t)
    {
      return fdr::detail::gsIsCard(t);
    }

    /// \brief Test for a length expression
    /// \param t A term
    /// \return True if it is a length expression
    inline
    bool is_length(const numeric_expression& t)
    {
      return fdr::detail::gsIsLength(t);
    }

    /// \brief Test for a plus expression
    /// \param t A term
    /// \return True if it is a plus expression
    inline
    bool is_plus(const numeric_expression& t)
    {
      return fdr::detail::gsIsPlus(t);
    }

    /// \brief Test for a minus expression
    /// \param t A term
    /// \return True if it is a minus expression
    inline
    bool is_minus(const numeric_expression& t)
    {
      return fdr::detail::gsIsMinus(t);
    }

    /// \brief Test for a times expression
    /// \param t A term
    /// \return True if it is a times expression
    inline
    bool is_times(const numeric_expression& t)
    {
      return fdr::detail::gsIsTimes(t);
    }

    /// \brief Test for a div expression
    /// \param t A term
    /// \return True if it is a div expression
    inline
    bool is_div(const numeric_expression& t)
    {
      return fdr::detail::gsIsDiv(t);
    }

    /// \brief Test for a mod expression
    /// \param t A term
    /// \return True if it is a mod expression
    inline
    bool is_mod(const numeric_expression& t)
    {
      return fdr::detail::gsIsMod(t);
    }

    /// \brief Test for a min expression
    /// \param t A term
    /// \return True if it is a min expression
    inline
    bool is_min(const numeric_expression& t)
    {
      return fdr::detail::gsIsMin(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_NUMERIC_EXPRESSION_H
