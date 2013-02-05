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
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/precedence.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/data_specification.h"

namespace mcrl2
{

namespace regular_formulas
{

//--- start generated classes ---//
/// \brief A regular formula
class regular_formula: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    regular_formula()
      : atermpp::aterm_appl(core::detail::constructRegFrm())
    {}

    /// \brief Constructor.
    /// \param term A term
    regular_formula(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_RegFrm(*this));
    }
};

/// \brief list of regular_formulas
typedef atermpp::term_list<regular_formula> regular_formula_list;

/// \brief vector of regular_formulas
typedef std::vector<regular_formula>    regular_formula_vector;


/// \brief Test for a regular_formula expression
/// \param t A term
/// \return True if it is a regular_formula expression
inline
bool is_regular_formula(const atermpp::aterm_appl& t)
{
  return core::detail::gsIsRegFrm(t);
}


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
    nil(const atermpp::aterm& term)
      : regular_formula(term)
    {
      assert(core::detail::check_term_RegNil(*this));
    }
};

/// \brief Test for a nil expression
/// \param t A term
/// \return True if it is a nil expression
inline
bool is_nil(const regular_formula& t)
{
  return core::detail::gsIsRegNil(t);
}


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
    seq(const atermpp::aterm& term)
      : regular_formula(term)
    {
      assert(core::detail::check_term_RegSeq(*this));
    }

    /// \brief Constructor.
    seq(const regular_formula& left, const regular_formula& right)
      : regular_formula(core::detail::gsMakeRegSeq(left, right))
    {}

    const regular_formula& left() const
    {
      return atermpp::aterm_cast<const regular_formula>(atermpp::arg1(*this));
    }

    const regular_formula& right() const
    {
      return atermpp::aterm_cast<const regular_formula>(atermpp::arg2(*this));
    }
};

/// \brief Test for a seq expression
/// \param t A term
/// \return True if it is a seq expression
inline
bool is_seq(const regular_formula& t)
{
  return core::detail::gsIsRegSeq(t);
}


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
    alt(const atermpp::aterm& term)
      : regular_formula(term)
    {
      assert(core::detail::check_term_RegAlt(*this));
    }

    /// \brief Constructor.
    alt(const regular_formula& left, const regular_formula& right)
      : regular_formula(core::detail::gsMakeRegAlt(left, right))
    {}

    const regular_formula& left() const
    {
      return atermpp::aterm_cast<const regular_formula>(atermpp::arg1(*this));
    }

    const regular_formula& right() const
    {
      return atermpp::aterm_cast<const regular_formula>(atermpp::arg2(*this));
    }
};

/// \brief Test for a alt expression
/// \param t A term
/// \return True if it is a alt expression
inline
bool is_alt(const regular_formula& t)
{
  return core::detail::gsIsRegAlt(t);
}


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
    trans(const atermpp::aterm& term)
      : regular_formula(term)
    {
      assert(core::detail::check_term_RegTrans(*this));
    }

    /// \brief Constructor.
    trans(const regular_formula& operand)
      : regular_formula(core::detail::gsMakeRegTrans(operand))
    {}

    const regular_formula& operand() const
    {
      return atermpp::aterm_cast<const regular_formula>(atermpp::arg1(*this));
    }
};

/// \brief Test for a trans expression
/// \param t A term
/// \return True if it is a trans expression
inline
bool is_trans(const regular_formula& t)
{
  return core::detail::gsIsRegTrans(t);
}


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
    trans_or_nil(const atermpp::aterm& term)
      : regular_formula(term)
    {
      assert(core::detail::check_term_RegTransOrNil(*this));
    }

    /// \brief Constructor.
    trans_or_nil(const regular_formula& operand)
      : regular_formula(core::detail::gsMakeRegTransOrNil(operand))
    {}

    const regular_formula& operand() const
    {
      return atermpp::aterm_cast<const regular_formula>(atermpp::arg1(*this));
    }
};

/// \brief Test for a trans_or_nil expression
/// \param t A term
/// \return True if it is a trans_or_nil expression
inline
bool is_trans_or_nil(const regular_formula& t)
{
  return core::detail::gsIsRegTransOrNil(t);
}

//--- end generated classes ---//

inline
int precedence(const regular_formula& x)
{
  if (is_seq(x))
  {
    return 1;
  }
  else if (is_alt(x))
  {
    return 2;
  }
  else if (is_trans(x) || is_trans_or_nil(x))
  {
    return 3;
  }
  return core::detail::precedences::max_precedence;
}

// TODO: is there a cleaner way to make the precedence function work for derived classes?
inline int precedence(const seq& x) { return precedence(static_cast<const regular_formula&>(x)); }
inline int precedence(const alt& x) { return precedence(static_cast<const regular_formula&>(x)); }
inline int precedence(const trans& x) { return precedence(static_cast<const regular_formula&>(x)); }
inline int precedence(const trans_or_nil& x) { return precedence(static_cast<const regular_formula&>(x)); }

// template function overloads
std::string pp(const regular_formula& x);

} // namespace regular_formulas

} // namespace mcrl2

namespace std {
//--- start generated swap functions ---//
template <>
inline void swap(mcrl2::regular_formulas::regular_formula& t1, mcrl2::regular_formulas::regular_formula& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::regular_formulas::nil& t1, mcrl2::regular_formulas::nil& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::regular_formulas::seq& t1, mcrl2::regular_formulas::seq& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::regular_formulas::alt& t1, mcrl2::regular_formulas::alt& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::regular_formulas::trans& t1, mcrl2::regular_formulas::trans& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::regular_formulas::trans_or_nil& t1, mcrl2::regular_formulas::trans_or_nil& t2)
{
  t1.swap(t2);
}
//--- end generated swap functions ---//
} // namespace std

#endif // MCRL2_MODAL_REGULAR_FORMULA_H
