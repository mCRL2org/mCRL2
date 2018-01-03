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

#ifndef MCRL2_MODAL_FORMULA_REGULAR_FORMULA_H
#define MCRL2_MODAL_FORMULA_REGULAR_FORMULA_H

#include <iostream> // for debugging

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/detail/default_values.h"
#include "mcrl2/core/detail/precedence.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/modal_formula/action_formula.h"
#include <cassert>
#include <string>

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
      : atermpp::aterm_appl(core::detail::default_values::RegFrm)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit regular_formula(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_RegFrm(*this));
    }

    /// \brief Constructor.
    regular_formula(const action_formulas::action_formula& x)
      : atermpp::aterm_appl(x)
    {}

    /// \brief Constructor.
    regular_formula(const data::data_expression& x)
      : atermpp::aterm_appl(x)
    {}

    /// Move semantics
    regular_formula(const regular_formula&) noexcept = default;
    regular_formula(regular_formula&&) noexcept = default;
    regular_formula& operator=(const regular_formula&) noexcept = default;
    regular_formula& operator=(regular_formula&&) noexcept = default;
};

/// \brief list of regular_formulas
typedef atermpp::term_list<regular_formula> regular_formula_list;

/// \brief vector of regular_formulas
typedef std::vector<regular_formula>    regular_formula_vector;

// prototypes
inline bool is_seq(const atermpp::aterm_appl& x);
inline bool is_alt(const atermpp::aterm_appl& x);
inline bool is_trans(const atermpp::aterm_appl& x);
inline bool is_trans_or_nil(const atermpp::aterm_appl& x);
inline bool is_untyped_regular_formula(const atermpp::aterm_appl& x);

/// \brief Test for a regular_formula expression
/// \param x A term
/// \return True if \a x is a regular_formula expression
inline
bool is_regular_formula(const atermpp::aterm_appl& x)
{
  return action_formulas::is_action_formula(x) ||
         data::is_data_expression(x) ||
         regular_formulas::is_seq(x) ||
         regular_formulas::is_alt(x) ||
         regular_formulas::is_trans(x) ||
         regular_formulas::is_trans_or_nil(x) ||
         regular_formulas::is_untyped_regular_formula(x);
}

// prototype declaration
std::string pp(const regular_formula& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const regular_formula& x)
{
  return out << regular_formulas::pp(x);
}

/// \brief swap overload
inline void swap(regular_formula& t1, regular_formula& t2)
{
  t1.swap(t2);
}


/// \brief The seq operator for regular formulas
class seq: public regular_formula
{
  public:
    /// \brief Default constructor.
    seq()
      : regular_formula(core::detail::default_values::RegSeq)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit seq(const atermpp::aterm& term)
      : regular_formula(term)
    {
      assert(core::detail::check_term_RegSeq(*this));
    }

    /// \brief Constructor.
    seq(const regular_formula& left, const regular_formula& right)
      : regular_formula(atermpp::aterm_appl(core::detail::function_symbol_RegSeq(), left, right))
    {}

    /// Move semantics
    seq(const seq&) noexcept = default;
    seq(seq&&) noexcept = default;
    seq& operator=(const seq&) noexcept = default;
    seq& operator=(seq&&) noexcept = default;

    const regular_formula& left() const
    {
      return atermpp::down_cast<regular_formula>((*this)[0]);
    }

    const regular_formula& right() const
    {
      return atermpp::down_cast<regular_formula>((*this)[1]);
    }
};

/// \brief Test for a seq expression
/// \param x A term
/// \return True if \a x is a seq expression
inline
bool is_seq(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RegSeq;
}

// prototype declaration
std::string pp(const seq& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const seq& x)
{
  return out << regular_formulas::pp(x);
}

/// \brief swap overload
inline void swap(seq& t1, seq& t2)
{
  t1.swap(t2);
}


/// \brief The alt operator for regular formulas
class alt: public regular_formula
{
  public:
    /// \brief Default constructor.
    alt()
      : regular_formula(core::detail::default_values::RegAlt)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit alt(const atermpp::aterm& term)
      : regular_formula(term)
    {
      assert(core::detail::check_term_RegAlt(*this));
    }

    /// \brief Constructor.
    alt(const regular_formula& left, const regular_formula& right)
      : regular_formula(atermpp::aterm_appl(core::detail::function_symbol_RegAlt(), left, right))
    {}

    /// Move semantics
    alt(const alt&) noexcept = default;
    alt(alt&&) noexcept = default;
    alt& operator=(const alt&) noexcept = default;
    alt& operator=(alt&&) noexcept = default;

    const regular_formula& left() const
    {
      return atermpp::down_cast<regular_formula>((*this)[0]);
    }

    const regular_formula& right() const
    {
      return atermpp::down_cast<regular_formula>((*this)[1]);
    }
};

/// \brief Test for a alt expression
/// \param x A term
/// \return True if \a x is a alt expression
inline
bool is_alt(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RegAlt;
}

// prototype declaration
std::string pp(const alt& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const alt& x)
{
  return out << regular_formulas::pp(x);
}

/// \brief swap overload
inline void swap(alt& t1, alt& t2)
{
  t1.swap(t2);
}


/// \brief The trans operator for regular formulas
class trans: public regular_formula
{
  public:
    /// \brief Default constructor.
    trans()
      : regular_formula(core::detail::default_values::RegTrans)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit trans(const atermpp::aterm& term)
      : regular_formula(term)
    {
      assert(core::detail::check_term_RegTrans(*this));
    }

    /// \brief Constructor.
    trans(const regular_formula& operand)
      : regular_formula(atermpp::aterm_appl(core::detail::function_symbol_RegTrans(), operand))
    {}

    /// Move semantics
    trans(const trans&) noexcept = default;
    trans(trans&&) noexcept = default;
    trans& operator=(const trans&) noexcept = default;
    trans& operator=(trans&&) noexcept = default;

    const regular_formula& operand() const
    {
      return atermpp::down_cast<regular_formula>((*this)[0]);
    }
};

/// \brief Test for a trans expression
/// \param x A term
/// \return True if \a x is a trans expression
inline
bool is_trans(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RegTrans;
}

// prototype declaration
std::string pp(const trans& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const trans& x)
{
  return out << regular_formulas::pp(x);
}

/// \brief swap overload
inline void swap(trans& t1, trans& t2)
{
  t1.swap(t2);
}


/// \brief The 'trans or nil' operator for regular formulas
class trans_or_nil: public regular_formula
{
  public:
    /// \brief Default constructor.
    trans_or_nil()
      : regular_formula(core::detail::default_values::RegTransOrNil)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit trans_or_nil(const atermpp::aterm& term)
      : regular_formula(term)
    {
      assert(core::detail::check_term_RegTransOrNil(*this));
    }

    /// \brief Constructor.
    trans_or_nil(const regular_formula& operand)
      : regular_formula(atermpp::aterm_appl(core::detail::function_symbol_RegTransOrNil(), operand))
    {}

    /// Move semantics
    trans_or_nil(const trans_or_nil&) noexcept = default;
    trans_or_nil(trans_or_nil&&) noexcept = default;
    trans_or_nil& operator=(const trans_or_nil&) noexcept = default;
    trans_or_nil& operator=(trans_or_nil&&) noexcept = default;

    const regular_formula& operand() const
    {
      return atermpp::down_cast<regular_formula>((*this)[0]);
    }
};

/// \brief Test for a trans_or_nil expression
/// \param x A term
/// \return True if \a x is a trans_or_nil expression
inline
bool is_trans_or_nil(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RegTransOrNil;
}

// prototype declaration
std::string pp(const trans_or_nil& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const trans_or_nil& x)
{
  return out << regular_formulas::pp(x);
}

/// \brief swap overload
inline void swap(trans_or_nil& t1, trans_or_nil& t2)
{
  t1.swap(t2);
}


/// \brief An untyped regular formula or action formula
class untyped_regular_formula: public regular_formula
{
  public:
    /// \brief Default constructor.
    untyped_regular_formula()
      : regular_formula(core::detail::default_values::UntypedRegFrm)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit untyped_regular_formula(const atermpp::aterm& term)
      : regular_formula(term)
    {
      assert(core::detail::check_term_UntypedRegFrm(*this));
    }

    /// \brief Constructor.
    untyped_regular_formula(const core::identifier_string& name, const regular_formula& left, const regular_formula& right)
      : regular_formula(atermpp::aterm_appl(core::detail::function_symbol_UntypedRegFrm(), name, left, right))
    {}

    /// \brief Constructor.
    untyped_regular_formula(const std::string& name, const regular_formula& left, const regular_formula& right)
      : regular_formula(atermpp::aterm_appl(core::detail::function_symbol_UntypedRegFrm(), core::identifier_string(name), left, right))
    {}

    /// Move semantics
    untyped_regular_formula(const untyped_regular_formula&) noexcept = default;
    untyped_regular_formula(untyped_regular_formula&&) noexcept = default;
    untyped_regular_formula& operator=(const untyped_regular_formula&) noexcept = default;
    untyped_regular_formula& operator=(untyped_regular_formula&&) noexcept = default;

    const core::identifier_string& name() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[0]);
    }

    const regular_formula& left() const
    {
      return atermpp::down_cast<regular_formula>((*this)[1]);
    }

    const regular_formula& right() const
    {
      return atermpp::down_cast<regular_formula>((*this)[2]);
    }
};

/// \brief Test for a untyped_regular_formula expression
/// \param x A term
/// \return True if \a x is a untyped_regular_formula expression
inline
bool is_untyped_regular_formula(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::UntypedRegFrm;
}

// prototype declaration
std::string pp(const untyped_regular_formula& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const untyped_regular_formula& x)
{
  return out << regular_formulas::pp(x);
}

/// \brief swap overload
inline void swap(untyped_regular_formula& t1, untyped_regular_formula& t2)
{
  t1.swap(t2);
}
//--- end generated classes ---//

inline int left_precedence(const seq&)            { return 31; }
inline int left_precedence(const alt&)            { return 32; }
inline int left_precedence(const trans&)          { return 33; }
inline int left_precedence(const trans_or_nil&)   { return 33; }
inline int left_precedence(const regular_formula& x)
{
  if      (is_seq(x))          { return left_precedence(static_cast<const seq&>(x)); }
  else if (is_alt(x))          { return left_precedence(static_cast<const alt&>(x)); }
  else if (is_trans(x))        { return left_precedence(static_cast<const trans&>(x)); }
  else if (is_trans_or_nil(x)) { return left_precedence(static_cast<const trans_or_nil&>(x)); }
  return core::detail::precedences::max_precedence;
}

inline int right_precedence(const regular_formula& x)
{
  return left_precedence(x);
}

inline const regular_formula& unary_operand(const trans& x)        { return x.operand(); }
inline const regular_formula& unary_operand(const trans_or_nil& x) { return x.operand(); }
inline const regular_formula& binary_left(const seq& x)            { return x.left(); }
inline const regular_formula& binary_right(const seq& x)           { return x.right(); }
inline const regular_formula& binary_left(const alt& x)            { return x.left(); }
inline const regular_formula& binary_right(const alt& x)           { return x.right(); }

} // namespace regular_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_REGULAR_FORMULA_H
