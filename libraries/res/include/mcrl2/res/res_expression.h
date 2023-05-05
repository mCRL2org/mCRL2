// Author(s): Jan Friso Groote. Based on boolean_expression by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/res/res_expression.h
/// \brief add your file description here.

#ifndef MCRL2_RES_RES_EXPRESSION_H
#define MCRL2_RES_RES_EXPRESSION_H

#include "mcrl2/core/detail/default_values.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/term_traits.h"
#include "mcrl2/data/data_expression.h"

namespace mcrl2
{

namespace res
{

typedef core::identifier_string res_variable_key_type;

template <typename T> std::string pp(const T& x);

//--- start generated classes ---//
/// \\brief A res expression
class res_expression: public atermpp::aterm_appl
{
  public:
    /// \\brief Default constructor.
    res_expression()
      : atermpp::aterm_appl(core::detail::default_values::RESExpression)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit res_expression(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_RESExpression(*this));
    }

    /// Move semantics
    res_expression(const res_expression&) noexcept = default;
    res_expression(res_expression&&) noexcept = default;
    res_expression& operator=(const res_expression&) noexcept = default;
    res_expression& operator=(res_expression&&) noexcept = default;
};

/// \\brief list of res_expressions
typedef atermpp::term_list<res_expression> res_expression_list;

/// \\brief vector of res_expressions
typedef std::vector<res_expression>    res_expression_vector;

// prototypes
inline bool is_true(const atermpp::aterm_appl& x);
inline bool is_false(const atermpp::aterm_appl& x);
inline bool is_res_variable(const atermpp::aterm_appl& x);
inline bool is_minus(const atermpp::aterm_appl& x);
inline bool is_and(const atermpp::aterm_appl& x);
inline bool is_or(const atermpp::aterm_appl& x);
inline bool is_imp(const atermpp::aterm_appl& x);
inline bool is_plus(const atermpp::aterm_appl& x);
inline bool is_const_multiply(const atermpp::aterm_appl& x);
inline bool is_const_multiply_alt(const atermpp::aterm_appl& x);
inline bool is_eqinf(const atermpp::aterm_appl& x);
inline bool is_eqninf(const atermpp::aterm_appl& x);
inline bool is_condsm(const atermpp::aterm_appl& x);
inline bool is_condeq(const atermpp::aterm_appl& x);

/// \\brief Test for a res_expression expression
/// \\param x A term
/// \\return True if \\a x is a res_expression expression
inline
bool is_res_expression(const atermpp::aterm_appl& x)
{
  return res::is_true(x) ||
         res::is_false(x) ||
         res::is_res_variable(x) ||
         res::is_minus(x) ||
         res::is_and(x) ||
         res::is_or(x) ||
         res::is_imp(x) ||
         res::is_plus(x) ||
         res::is_const_multiply(x) ||
         res::is_const_multiply_alt(x) ||
         res::is_eqinf(x) ||
         res::is_eqninf(x) ||
         res::is_condsm(x) ||
         res::is_condeq(x);
}

// prototype declaration
std::string pp(const res_expression& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const res_expression& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(res_expression& t1, res_expression& t2)
{
  t1.swap(t2);
}


/// \\brief The value true for res expressions
class true_: public res_expression
{
  public:
    /// \\brief Default constructor.
    true_()
      : res_expression(core::detail::default_values::RESTrue)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit true_(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESTrue(*this));
    }

    /// Move semantics
    true_(const true_&) noexcept = default;
    true_(true_&&) noexcept = default;
    true_& operator=(const true_&) noexcept = default;
    true_& operator=(true_&&) noexcept = default;
};

/// \\brief Test for a true expression
/// \\param x A term
/// \\return True if \\a x is a true expression
inline
bool is_true(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESTrue;
}

// prototype declaration
std::string pp(const true_& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const true_& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(true_& t1, true_& t2)
{
  t1.swap(t2);
}


/// \\brief The value false for res expressions
class false_: public res_expression
{
  public:
    /// \\brief Default constructor.
    false_()
      : res_expression(core::detail::default_values::RESFalse)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit false_(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESFalse(*this));
    }

    /// Move semantics
    false_(const false_&) noexcept = default;
    false_(false_&&) noexcept = default;
    false_& operator=(const false_&) noexcept = default;
    false_& operator=(false_&&) noexcept = default;
};

/// \\brief Test for a false expression
/// \\param x A term
/// \\return True if \\a x is a false expression
inline
bool is_false(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESFalse;
}

// prototype declaration
std::string pp(const false_& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const false_& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(false_& t1, false_& t2)
{
  t1.swap(t2);
}


/// \\brief A res variable
class res_variable: public res_expression
{
  public:


    /// Move semantics
    res_variable(const res_variable&) noexcept = default;
    res_variable(res_variable&&) noexcept = default;
    res_variable& operator=(const res_variable&) noexcept = default;
    res_variable& operator=(res_variable&&) noexcept = default;

    const core::identifier_string& name() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[0]);
    }
//--- start user section res_variable ---//
    /// \brief Default constructor.
    res_variable()
      : res_expression(core::detail::default_values::BooleanVariable)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit res_variable(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_BooleanVariable(*this));
    }

    /// \brief Constructor.
    res_variable(const core::identifier_string& name)
    {
      atermpp::make_term_appl<res_variable,core::identifier_string>
                      (*this,core::detail::function_symbol_BooleanVariable(), name);
    }

    /// \brief Constructor.
    res_variable(const std::string& name)
    {
      atermpp::make_term_appl<res_variable,core::identifier_string>
                      (*this,core::detail::function_symbol_BooleanVariable(), core::identifier_string(name));
    }
//--- end user section res_variable ---//
};

/// \\brief Make_res_variable constructs a new term into a given address.
/// \\ \param t The reference into which the new res_variable is constructed. 
template <class... ARGUMENTS>
inline void make_res_variable(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESVariable(), args...);
}

/// \\brief Test for a res_variable expression
/// \\param x A term
/// \\return True if \\a x is a res_variable expression
inline
bool is_res_variable(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESVariable;
}

// prototype declaration
std::string pp(const res_variable& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const res_variable& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(res_variable& t1, res_variable& t2)
{
  t1.swap(t2);
}


/// \\brief The minus operator for res expressions
class minus: public res_expression
{
  public:
    /// \\brief Default constructor.
    minus()
      : res_expression(core::detail::default_values::RESMinus)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit minus(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESMinus(*this));
    }

    /// \\brief Constructor.
    explicit minus(const res_expression& operand)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESMinus(), operand))
    {}

    /// Move semantics
    minus(const minus&) noexcept = default;
    minus(minus&&) noexcept = default;
    minus& operator=(const minus&) noexcept = default;
    minus& operator=(minus&&) noexcept = default;

    const res_expression& operand() const
    {
      return atermpp::down_cast<res_expression>((*this)[0]);
    }
};

/// \\brief Make_minus constructs a new term into a given address.
/// \\ \param t The reference into which the new minus is constructed. 
template <class... ARGUMENTS>
inline void make_minus(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESMinus(), args...);
}

/// \\brief Test for a minus expression
/// \\param x A term
/// \\return True if \\a x is a minus expression
inline
bool is_minus(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESMinus;
}

// prototype declaration
std::string pp(const minus& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const minus& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(minus& t1, minus& t2)
{
  t1.swap(t2);
}


/// \\brief The and operator for res expressions
class and_: public res_expression
{
  public:
    /// \\brief Default constructor.
    and_()
      : res_expression(core::detail::default_values::RESAnd)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit and_(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESAnd(*this));
    }

    /// \\brief Constructor.
    and_(const res_expression& left, const res_expression& right)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESAnd(), left, right))
    {}

    /// Move semantics
    and_(const and_&) noexcept = default;
    and_(and_&&) noexcept = default;
    and_& operator=(const and_&) noexcept = default;
    and_& operator=(and_&&) noexcept = default;

    const res_expression& left() const
    {
      return atermpp::down_cast<res_expression>((*this)[0]);
    }

    const res_expression& right() const
    {
      return atermpp::down_cast<res_expression>((*this)[1]);
    }
};

/// \\brief Make_and_ constructs a new term into a given address.
/// \\ \param t The reference into which the new and_ is constructed. 
template <class... ARGUMENTS>
inline void make_and_(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESAnd(), args...);
}

/// \\brief Test for a and expression
/// \\param x A term
/// \\return True if \\a x is a and expression
inline
bool is_and(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESAnd;
}

// prototype declaration
std::string pp(const and_& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const and_& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(and_& t1, and_& t2)
{
  t1.swap(t2);
}


/// \\brief The or operator for res expressions
class or_: public res_expression
{
  public:
    /// \\brief Default constructor.
    or_()
      : res_expression(core::detail::default_values::RESOr)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit or_(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESOr(*this));
    }

    /// \\brief Constructor.
    or_(const res_expression& left, const res_expression& right)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESOr(), left, right))
    {}

    /// Move semantics
    or_(const or_&) noexcept = default;
    or_(or_&&) noexcept = default;
    or_& operator=(const or_&) noexcept = default;
    or_& operator=(or_&&) noexcept = default;

    const res_expression& left() const
    {
      return atermpp::down_cast<res_expression>((*this)[0]);
    }

    const res_expression& right() const
    {
      return atermpp::down_cast<res_expression>((*this)[1]);
    }
};

/// \\brief Make_or_ constructs a new term into a given address.
/// \\ \param t The reference into which the new or_ is constructed. 
template <class... ARGUMENTS>
inline void make_or_(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESOr(), args...);
}

/// \\brief Test for a or expression
/// \\param x A term
/// \\return True if \\a x is a or expression
inline
bool is_or(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESOr;
}

// prototype declaration
std::string pp(const or_& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const or_& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(or_& t1, or_& t2)
{
  t1.swap(t2);
}


/// \\brief The implication operator for res expressions
class imp: public res_expression
{
  public:
    /// \\brief Default constructor.
    imp()
      : res_expression(core::detail::default_values::RESImp)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit imp(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESImp(*this));
    }

    /// \\brief Constructor.
    imp(const res_expression& left, const res_expression& right)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESImp(), left, right))
    {}

    /// Move semantics
    imp(const imp&) noexcept = default;
    imp(imp&&) noexcept = default;
    imp& operator=(const imp&) noexcept = default;
    imp& operator=(imp&&) noexcept = default;

    const res_expression& left() const
    {
      return atermpp::down_cast<res_expression>((*this)[0]);
    }

    const res_expression& right() const
    {
      return atermpp::down_cast<res_expression>((*this)[1]);
    }
};

/// \\brief Make_imp constructs a new term into a given address.
/// \\ \param t The reference into which the new imp is constructed. 
template <class... ARGUMENTS>
inline void make_imp(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESImp(), args...);
}

/// \\brief Test for a imp expression
/// \\param x A term
/// \\return True if \\a x is a imp expression
inline
bool is_imp(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESImp;
}

// prototype declaration
std::string pp(const imp& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const imp& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(imp& t1, imp& t2)
{
  t1.swap(t2);
}


/// \\brief The plus operator for res expressions
class plus: public res_expression
{
  public:
    /// \\brief Default constructor.
    plus()
      : res_expression(core::detail::default_values::RESPlus)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit plus(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESPlus(*this));
    }

    /// \\brief Constructor.
    plus(const res_expression& left, const res_expression& right)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESPlus(), left, right))
    {}

    /// Move semantics
    plus(const plus&) noexcept = default;
    plus(plus&&) noexcept = default;
    plus& operator=(const plus&) noexcept = default;
    plus& operator=(plus&&) noexcept = default;

    const res_expression& left() const
    {
      return atermpp::down_cast<res_expression>((*this)[0]);
    }

    const res_expression& right() const
    {
      return atermpp::down_cast<res_expression>((*this)[1]);
    }
};

/// \\brief Make_plus constructs a new term into a given address.
/// \\ \param t The reference into which the new plus is constructed. 
template <class... ARGUMENTS>
inline void make_plus(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESPlus(), args...);
}

/// \\brief Test for a plus expression
/// \\param x A term
/// \\return True if \\a x is a plus expression
inline
bool is_plus(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESPlus;
}

// prototype declaration
std::string pp(const plus& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const plus& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(plus& t1, plus& t2)
{
  t1.swap(t2);
}


/// \\brief Left multiplication with a positive constant for res expressions
class const_multiply: public res_expression
{
  public:
    /// \\brief Default constructor.
    const_multiply()
      : res_expression(core::detail::default_values::RESConstantMultiply)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit const_multiply(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESConstantMultiply(*this));
    }

    /// \\brief Constructor.
    const_multiply(const data::data_expression& left, const res_expression& right)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESConstantMultiply(), left, right))
    {}

    /// Move semantics
    const_multiply(const const_multiply&) noexcept = default;
    const_multiply(const_multiply&&) noexcept = default;
    const_multiply& operator=(const const_multiply&) noexcept = default;
    const_multiply& operator=(const_multiply&&) noexcept = default;

    const data::data_expression& left() const
    {
      return atermpp::down_cast<data::data_expression>((*this)[0]);
    }

    const res_expression& right() const
    {
      return atermpp::down_cast<res_expression>((*this)[1]);
    }
};

/// \\brief Make_const_multiply constructs a new term into a given address.
/// \\ \param t The reference into which the new const_multiply is constructed. 
template <class... ARGUMENTS>
inline void make_const_multiply(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESConstantMultiply(), args...);
}

/// \\brief Test for a const_multiply expression
/// \\param x A term
/// \\return True if \\a x is a const_multiply expression
inline
bool is_const_multiply(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESConstantMultiply;
}

// prototype declaration
std::string pp(const const_multiply& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const const_multiply& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(const_multiply& t1, const_multiply& t2)
{
  t1.swap(t2);
}


/// \\brief Right multiplication with a positive constant for res expressions
class const_multiply_alt: public res_expression
{
  public:
    /// \\brief Default constructor.
    const_multiply_alt()
      : res_expression(core::detail::default_values::RESConstantMultiplyAlt)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit const_multiply_alt(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESConstantMultiplyAlt(*this));
    }

    /// \\brief Constructor.
    const_multiply_alt(const res_expression& left, const data::data_expression& right)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESConstantMultiplyAlt(), left, right))
    {}

    /// Move semantics
    const_multiply_alt(const const_multiply_alt&) noexcept = default;
    const_multiply_alt(const_multiply_alt&&) noexcept = default;
    const_multiply_alt& operator=(const const_multiply_alt&) noexcept = default;
    const_multiply_alt& operator=(const_multiply_alt&&) noexcept = default;

    const res_expression& left() const
    {
      return atermpp::down_cast<res_expression>((*this)[0]);
    }

    const data::data_expression& right() const
    {
      return atermpp::down_cast<data::data_expression>((*this)[1]);
    }
};

/// \\brief Make_const_multiply_alt constructs a new term into a given address.
/// \\ \param t The reference into which the new const_multiply_alt is constructed. 
template <class... ARGUMENTS>
inline void make_const_multiply_alt(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESConstantMultiplyAlt(), args...);
}

/// \\brief Test for a const_multiply_alt expression
/// \\param x A term
/// \\return True if \\a x is a const_multiply_alt expression
inline
bool is_const_multiply_alt(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESConstantMultiplyAlt;
}

// prototype declaration
std::string pp(const const_multiply_alt& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const const_multiply_alt& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(const_multiply_alt& t1, const_multiply_alt& t2)
{
  t1.swap(t2);
}


/// \\brief The operator to check for infinity
class eqinf: public res_expression
{
  public:
    /// \\brief Default constructor.
    eqinf()
      : res_expression(core::detail::default_values::RESEqInf)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit eqinf(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESEqInf(*this));
    }

    /// \\brief Constructor.
    explicit eqinf(const res_expression& operand)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESEqInf(), operand))
    {}

    /// Move semantics
    eqinf(const eqinf&) noexcept = default;
    eqinf(eqinf&&) noexcept = default;
    eqinf& operator=(const eqinf&) noexcept = default;
    eqinf& operator=(eqinf&&) noexcept = default;

    const res_expression& operand() const
    {
      return atermpp::down_cast<res_expression>((*this)[0]);
    }
};

/// \\brief Make_eqinf constructs a new term into a given address.
/// \\ \param t The reference into which the new eqinf is constructed. 
template <class... ARGUMENTS>
inline void make_eqinf(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESEqInf(), args...);
}

/// \\brief Test for a eqinf expression
/// \\param x A term
/// \\return True if \\a x is a eqinf expression
inline
bool is_eqinf(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESEqInf;
}

// prototype declaration
std::string pp(const eqinf& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const eqinf& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(eqinf& t1, eqinf& t2)
{
  t1.swap(t2);
}


/// \\brief The operator to check for negative infinity
class eqninf: public res_expression
{
  public:
    /// \\brief Default constructor.
    eqninf()
      : res_expression(core::detail::default_values::RESEqInf)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit eqninf(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESEqInf(*this));
    }

    /// \\brief Constructor.
    explicit eqninf(const res_expression& operand)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESEqInf(), operand))
    {}

    /// Move semantics
    eqninf(const eqninf&) noexcept = default;
    eqninf(eqninf&&) noexcept = default;
    eqninf& operator=(const eqninf&) noexcept = default;
    eqninf& operator=(eqninf&&) noexcept = default;

    const res_expression& operand() const
    {
      return atermpp::down_cast<res_expression>((*this)[0]);
    }
};

/// \\brief Make_eqninf constructs a new term into a given address.
/// \\ \param t The reference into which the new eqninf is constructed. 
template <class... ARGUMENTS>
inline void make_eqninf(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESEqInf(), args...);
}

/// \\brief Test for a eqninf expression
/// \\param x A term
/// \\return True if \\a x is a eqninf expression
inline
bool is_eqninf(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESEqInf;
}

// prototype declaration
std::string pp(const eqninf& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const eqninf& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(eqninf& t1, eqninf& t2)
{
  t1.swap(t2);
}


/// \\brief The conditional and expression for res expressions
class condsm: public res_expression
{
  public:
    /// \\brief Default constructor.
    condsm()
      : res_expression(core::detail::default_values::RESCondSm)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit condsm(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESCondSm(*this));
    }

    /// \\brief Constructor.
    condsm(const res_expression& arg1, const res_expression& arg2, const res_expression& arg3)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESCondSm(), arg1, arg2, arg3))
    {}

    /// Move semantics
    condsm(const condsm&) noexcept = default;
    condsm(condsm&&) noexcept = default;
    condsm& operator=(const condsm&) noexcept = default;
    condsm& operator=(condsm&&) noexcept = default;

    const res_expression& arg1() const
    {
      return atermpp::down_cast<res_expression>((*this)[0]);
    }

    const res_expression& arg2() const
    {
      return atermpp::down_cast<res_expression>((*this)[1]);
    }

    const res_expression& arg3() const
    {
      return atermpp::down_cast<res_expression>((*this)[2]);
    }
};

/// \\brief Make_condsm constructs a new term into a given address.
/// \\ \param t The reference into which the new condsm is constructed. 
template <class... ARGUMENTS>
inline void make_condsm(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESCondSm(), args...);
}

/// \\brief Test for a condsm expression
/// \\param x A term
/// \\return True if \\a x is a condsm expression
inline
bool is_condsm(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESCondSm;
}

// prototype declaration
std::string pp(const condsm& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const condsm& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(condsm& t1, condsm& t2)
{
  t1.swap(t2);
}


/// \\brief The conditional or expression for res expressions
class condeq: public res_expression
{
  public:
    /// \\brief Default constructor.
    condeq()
      : res_expression(core::detail::default_values::RESCondEq)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit condeq(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESCondEq(*this));
    }

    /// \\brief Constructor.
    condeq(const res_expression& arg1, const res_expression& arg2, const res_expression& arg3)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESCondEq(), arg1, arg2, arg3))
    {}

    /// Move semantics
    condeq(const condeq&) noexcept = default;
    condeq(condeq&&) noexcept = default;
    condeq& operator=(const condeq&) noexcept = default;
    condeq& operator=(condeq&&) noexcept = default;

    const res_expression& arg1() const
    {
      return atermpp::down_cast<res_expression>((*this)[0]);
    }

    const res_expression& arg2() const
    {
      return atermpp::down_cast<res_expression>((*this)[1]);
    }

    const res_expression& arg3() const
    {
      return atermpp::down_cast<res_expression>((*this)[2]);
    }
};

/// \\brief Make_condeq constructs a new term into a given address.
/// \\ \param t The reference into which the new condeq is constructed. 
template <class... ARGUMENTS>
inline void make_condeq(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESCondEq(), args...);
}

/// \\brief Test for a condeq expression
/// \\param x A term
/// \\return True if \\a x is a condeq expression
inline
bool is_condeq(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESCondEq;
}

// prototype declaration
std::string pp(const condeq& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const condeq& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(condeq& t1, condeq& t2)
{
  t1.swap(t2);
}
//--- end generated classes ---//

namespace accessors
{
inline
const res_expression& left(res_expression const& e)
{
  assert(is_and(e) || is_or(e) || is_imp(e));
  return atermpp::down_cast<res_expression>(e[0]);
}

inline
const res_expression& right(res_expression const& e)
{
  assert(is_and(e) || is_or(e) || is_imp(e));
  return atermpp::down_cast<res_expression>(e[1]);
}

} // namespace accessors

} // namespace res

} // namespace mcrl2

namespace mcrl2
{

namespace core
{

/// \brief Contains type information for res expressions
template <>
struct term_traits<res::res_expression>
{
  /// The term type
  typedef res::res_expression term_type;

  /// \brief The variable type
  typedef res::res_variable variable_type;

  /// \brief The string type
  typedef core::identifier_string string_type;

  /// \brief The value true
  /// \return The value true
  static inline
  res::res_expression true_()
  {
    return res::true_();
  }

  /// \brief The value false
  /// \return The value false
  static inline
  res::res_expression false_()
  {
    return res::false_();
  }

  /// \brief Operator not
  /// \param x A term
  /// \return Operator not applied to
  static inline
  res::res_expression minus(const res::res_expression& x)
  {
    return res::minus(x);
  }

  /// \brief Operator not
  /// \param result Will contain not applied to x. 
  /// \param x A term
  static inline
  void make_minus(res::res_expression& result, const res::res_expression& x)
  {
    res::make_minus(result, x);
  }

  /// \brief Operator and
  /// \param p A term
  /// \param q A term
  /// \return Operator and applied to p and q
  static inline
  res::res_expression and_(const res::res_expression& p, const res::res_expression& q)
  {
    return res::and_(p, q);
  }

  /// \brief Operator and
  /// \param result Operator and applied to p and q
  /// \param p A term
  /// \param q A term
  static inline
  void make_and_(res::res_expression& result, const res::res_expression& p, const res::res_expression& q)
  {
    res::make_and_(result, p, q);
  }

  /// \brief Operator or
  /// \param p A term
  /// \param q A term
  /// \return Operator or applied to p and q
  static inline
  res::res_expression or_(const res::res_expression& p, const res::res_expression& q)
  {
    return res::or_(p, q);
  }
  //
  /// \brief Operator or
  /// \param result Operator or applied to p and q
  /// \param p A term
  /// \param q A term
  static inline
  void make_or_(res::res_expression& result, const res::res_expression& p, const res::res_expression& q)
  {
    res::make_or_(result, p, q);
  }

  /// \brief Implication
  /// \param p A term
  /// \param q A term
  /// \return Implication applied to p and q
  static inline
  res::res_expression imp(const res::res_expression& p, const res::res_expression& q)
  {
    return res::imp(p, q);
  }
  
  /// \brief Implication
  /// \param result Implication applied to p and q
  /// \param p A term
  /// \param q A term
  static inline
  void make_imp(res::res_expression& result, const res::res_expression& p, const res::res_expression& q)
  {
    res::make_imp(result, p, q);
  }

  /// \brief Test for value true
  /// \param t A term
  /// \return True if the term has the value true
  static inline
  bool is_true(const res::res_expression& t)
  {
    return res::is_true(t);
  }

  /// \brief Test for value false
  /// \param t A term
  /// \return True if the term has the value false
  static inline
  bool is_false(const res::res_expression& t)
  {
    return res::is_false(t);
  }

  /// \brief Test for operator not
  /// \param t A term
  /// \return True if the term is of type and
  static inline
  bool is_minus(const res::res_expression& t)
  {
    return res::is_minus(t);
  }

  /// \brief Test for operator and
  /// \param t A term
  /// \return True if the term is of type and
  static inline
  bool is_and(const res::res_expression& t)
  {
    return res::is_and(t);
  }

  /// \brief Test for operator or
  /// \param t A term
  /// \return True if the term is of type or
  static inline
  bool is_or(const res::res_expression& t)
  {
    return res::is_or(t);
  }

  /// \brief Test for implication
  /// \param t A term
  /// \return True if the term is an implication
  static inline
  bool is_imp(const res::res_expression& t)
  {
    return res::is_imp(t);
  }

  /// \brief Test for propositional variable
  /// \param t A term
  /// \return True if the term is a propositional variable
  static inline
  bool is_prop_var(const res::res_expression& t)
  {
    return res::is_res_variable(t);
  }

  /// \brief Returns the left argument of a term of type and, or or imp
  /// \param t A term
  /// \return The left argument of the term
  static inline
  const res::res_expression& left(const res::res_expression& t)
  {
    assert(is_and(t) || is_or(t) || is_imp(t));
    return atermpp::down_cast<const res::res_expression>(t[0]);
  }

  /// \brief Returns the right argument of a term of type and, or or imp
  /// \param t A term
  /// \return The right argument of the term
  static inline
  const res::res_expression& right(const res::res_expression& t)
  {
    assert(is_and(t) || is_or(t) || is_imp(t));
    return atermpp::down_cast<const res::res_expression>(t[1]);
  }

  /// \brief Returns the argument of a term of type not
  /// \param t A term
  static inline
  const res::res_expression& minus_arg(const res::res_expression& t)
  {
    assert(is_minus(t));
    return atermpp::down_cast<res::minus>(t).operand();
  }

  /// \brief Returns the name of a res variable
  /// \param t A term
  /// \return The name of the res variable
  static inline
  const core::identifier_string& name(const res::res_expression& t)
  {
    assert(res::is_res_variable(t));
    return atermpp::down_cast<res::res_variable>(t).name();
  }

  /// \brief Conversion from variable to term
  /// \param v A variable
  /// \returns The converted variable
  static inline
  const res::res_expression& variable2term(const res::res_variable& v)
  {
    return v;
  }

  /// \brief Conversion from term to variable
  /// \param t a term
  /// \returns The converted term
  static inline
  const res::res_variable& term2variable(const res::res_expression& t)
  {
    return atermpp::down_cast<res::res_variable>(t);
  }

  /// \brief Pretty print function
  /// \param t A term
  /// \return Returns a pretty print representation of the term
  static inline
  std::string pp(const res::res_expression& t)
  {
    return res::pp(t);
  }
};

} // namespace core

} // namespace mcrl2

#endif // MCRL2_RES_RES_EXPRESSION_H
