// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/regular_formula.h
/// \brief Add your file description here.

#ifndef MCRL2_MODAL_REGULAR_FORMULA_H
#define MCRL2_MODAL_REGULAR_FORMULA_H

#include <iostream> // for debugging

#include <string>
#include <cassert>
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"

namespace mcrl2 {

namespace regular_formulas {

/// \brief Returns true if the term t is a regular formula
/// \param t A term
/// \return True if the term is a regular formula
// TODO: generate this function
inline
bool is_regular_formula(const atermpp::aterm_appl& t)
{
  return core::detail::gsIsRegFrm(t);
}

//--- start generated classes ---//
/// \brief class regular_formula
class regular_formula: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    regular_formula()
      : atermpp::aterm_appl(core::detail::constructRegFrm())
    {}

    /// \brief Constructor.
    /// \param term A term
    regular_formula(const atermpp::aterm_appl& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_RegFrm(m_term));
    }
};

/// \brief list of regular_formulas
typedef atermpp::term_list<regular_formula> regular_formula_list;

/// \brief vector of regular_formulas
typedef atermpp::vector<regular_formula>    regular_formula_vector;

/// \brief The value nil for regular formulas
class nil: public regular_formula
{
  public:
    /// \brief Default constructor.
    nil()
      : regular_formula(core::detail::constructRegNil())
    {}

    /// \brief Constructor.
    /// \param term A term
    nil(const atermpp::aterm_appl& term)
      : regular_formula(term)
    {
      assert(core::detail::check_term_RegNil(m_term));
    }
};

/// \brief The seq operator for regular formulas
class seq: public regular_formula
{
  public:
    /// \brief Default constructor.
    seq()
      : regular_formula(core::detail::constructRegSeq())
    {}

    /// \brief Constructor.
    /// \param term A term
    seq(const atermpp::aterm_appl& term)
      : regular_formula(term)
    {
      assert(core::detail::check_term_RegSeq(m_term));
    }

    /// \brief Constructor.
    seq(const regular_formula& left, const regular_formula& right)
      : regular_formula(core::detail::gsMakeRegSeq(left, right))
    {}

    regular_formula left() const
    {
      return atermpp::arg1(*this);
    }

    regular_formula right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The alt operator for regular formulas
class alt: public regular_formula
{
  public:
    /// \brief Default constructor.
    alt()
      : regular_formula(core::detail::constructRegAlt())
    {}

    /// \brief Constructor.
    /// \param term A term
    alt(const atermpp::aterm_appl& term)
      : regular_formula(term)
    {
      assert(core::detail::check_term_RegAlt(m_term));
    }

    /// \brief Constructor.
    alt(const regular_formula& left, const regular_formula& right)
      : regular_formula(core::detail::gsMakeRegAlt(left, right))
    {}

    regular_formula left() const
    {
      return atermpp::arg1(*this);
    }

    regular_formula right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The trans operator for regular formulas
class trans: public regular_formula
{
  public:
    /// \brief Default constructor.
    trans()
      : regular_formula(core::detail::constructRegTrans())
    {}

    /// \brief Constructor.
    /// \param term A term
    trans(const atermpp::aterm_appl& term)
      : regular_formula(term)
    {
      assert(core::detail::check_term_RegTrans(m_term));
    }

    /// \brief Constructor.
    trans(const regular_formula& operand)
      : regular_formula(core::detail::gsMakeRegTrans(operand))
    {}

    regular_formula operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief The 'trans or nil' operator for regular formulas
class trans_or_nil: public regular_formula
{
  public:
    /// \brief Default constructor.
    trans_or_nil()
      : regular_formula(core::detail::constructRegTransOrNil())
    {}

    /// \brief Constructor.
    /// \param term A term
    trans_or_nil(const atermpp::aterm_appl& term)
      : regular_formula(term)
    {
      assert(core::detail::check_term_RegTransOrNil(m_term));
    }

    /// \brief Constructor.
    trans_or_nil(const regular_formula& operand)
      : regular_formula(core::detail::gsMakeRegTransOrNil(operand))
    {}

    regular_formula operand() const
    {
      return atermpp::arg1(*this);
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a nil expression
    /// \param t A term
    /// \return True if it is a nil expression
    inline
    bool is_nil(const regular_formula& t)
    {
      return core::detail::gsIsRegNil(t);
    }

    /// \brief Test for a seq expression
    /// \param t A term
    /// \return True if it is a seq expression
    inline
    bool is_seq(const regular_formula& t)
    {
      return core::detail::gsIsRegSeq(t);
    }

    /// \brief Test for a alt expression
    /// \param t A term
    /// \return True if it is a alt expression
    inline
    bool is_alt(const regular_formula& t)
    {
      return core::detail::gsIsRegAlt(t);
    }

    /// \brief Test for a trans expression
    /// \param t A term
    /// \return True if it is a trans expression
    inline
    bool is_trans(const regular_formula& t)
    {
      return core::detail::gsIsRegTrans(t);
    }

    /// \brief Test for a trans_or_nil expression
    /// \param t A term
    /// \return True if it is a trans_or_nil expression
    inline
    bool is_trans_or_nil(const regular_formula& t)
    {
      return core::detail::gsIsRegTransOrNil(t);
    }
//--- end generated is-functions ---//

} // namespace regular_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_REGULAR_FORMULA_H
