// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/numeric_expression.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_NUMERIC_EXPRESSION_H
#define MCRL2_FDR_NUMERIC_EXPRESSION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Number expression
  class numeric_expression: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      numeric_expression()
        : atermpp::aterm_appl(fdr::atermpp::detail::constructNumb())
      {}

      /// \brief Constructor.
      /// \param term A term
      numeric_expression(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Numb(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A number
class number: public numeric_expression
{
  public:
    /// \brief Default constructor.
    number()
      : numeric_expression(fdr::atermpp::detail::constructNumber())
    {}

    /// \brief Constructor.
    /// \param term A term
    number(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Number(m_term));
    }

    /// \brief Constructor.
    number(const number_expression& number)
      : numeric_expression(fdr::detail::gsMakeNumber(number))
    {}

    number_expression number() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief The cardinality of a set
class card: public numeric_expression
{
  public:
    /// \brief Default constructor.
    card()
      : numeric_expression(fdr::atermpp::detail::constructCard())
    {}

    /// \brief Constructor.
    /// \param term A term
    card(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Card(m_term));
    }

    /// \brief Constructor.
    card(const set_expression& set)
      : numeric_expression(fdr::detail::gsMakeCard(set))
    {}

    set_expression set() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief The length of a sequence
class length: public numeric_expression
{
  public:
    /// \brief Default constructor.
    length()
      : numeric_expression(fdr::atermpp::detail::constructLength())
    {}

    /// \brief Constructor.
    /// \param term A term
    length(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Length(m_term));
    }

    /// \brief Constructor.
    length(const seq_expression& seq)
      : numeric_expression(fdr::detail::gsMakeLength(seq))
    {}

    seq_expression seq() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief An addition
class plus: public numeric_expression
{
  public:
    /// \brief Default constructor.
    plus()
      : numeric_expression(fdr::atermpp::detail::constructPlus())
    {}

    /// \brief Constructor.
    /// \param term A term
    plus(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Plus(m_term));
    }

    /// \brief Constructor.
    plus(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakePlus(left, right))
    {}

    numeric_expression left() const
    {
      return atermpp::arg1(*this);
    }

    numeric_expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A subtraction
class minus: public numeric_expression
{
  public:
    /// \brief Default constructor.
    minus()
      : numeric_expression(fdr::atermpp::detail::constructMinus())
    {}

    /// \brief Constructor.
    /// \param term A term
    minus(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Minus(m_term));
    }

    /// \brief Constructor.
    minus(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakeMinus(left, right))
    {}

    numeric_expression left() const
    {
      return atermpp::arg1(*this);
    }

    numeric_expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A multiplication
class times: public numeric_expression
{
  public:
    /// \brief Default constructor.
    times()
      : numeric_expression(fdr::atermpp::detail::constructTimes())
    {}

    /// \brief Constructor.
    /// \param term A term
    times(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Times(m_term));
    }

    /// \brief Constructor.
    times(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakeTimes(left, right))
    {}

    numeric_expression left() const
    {
      return atermpp::arg1(*this);
    }

    numeric_expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A division
class div: public numeric_expression
{
  public:
    /// \brief Default constructor.
    div()
      : numeric_expression(fdr::atermpp::detail::constructDiv())
    {}

    /// \brief Constructor.
    /// \param term A term
    div(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Div(m_term));
    }

    /// \brief Constructor.
    div(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakeDiv(left, right))
    {}

    numeric_expression left() const
    {
      return atermpp::arg1(*this);
    }

    numeric_expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A modulo operation
class mod: public numeric_expression
{
  public:
    /// \brief Default constructor.
    mod()
      : numeric_expression(fdr::atermpp::detail::constructMod())
    {}

    /// \brief Constructor.
    /// \param term A term
    mod(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Mod(m_term));
    }

    /// \brief Constructor.
    mod(const numeric_expression& left, const numeric_expression& right)
      : numeric_expression(fdr::detail::gsMakeMod(left, right))
    {}

    numeric_expression left() const
    {
      return atermpp::arg1(*this);
    }

    numeric_expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A negative number
class min: public numeric_expression
{
  public:
    /// \brief Default constructor.
    min()
      : numeric_expression(fdr::atermpp::detail::constructMin())
    {}

    /// \brief Constructor.
    /// \param term A term
    min(atermpp::aterm_appl term)
      : numeric_expression(term)
    {
      assert(fdr::detail::check_term_Min(m_term));
    }

    /// \brief Constructor.
    min(const numeric_expression& operand)
      : numeric_expression(fdr::detail::gsMakeMin(operand))
    {}

    numeric_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};
//--- end generated classes ---//

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
