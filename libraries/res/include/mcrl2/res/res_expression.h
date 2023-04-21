// Author(s): Jan Friso Groote. Based on boolean_expression by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/res_expression.h
/// \brief add your file description here.

#ifndef MCRL2_RES_RES_EXPRESSION_H
#define MCRL2_RES_RES_EXPRESSION_H

#include "mcrl2/core/detail/default_values.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/term_traits.h"

namespace mcrl2
{

namespace bes
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
inline bool is_not(const atermpp::aterm_appl& x);
inline bool is_and(const atermpp::aterm_appl& x);
inline bool is_or(const atermpp::aterm_appl& x);
inline bool is_imp(const atermpp::aterm_appl& x);
inline bool is_plus(const atermpp::aterm_appl& x);
inline bool is_multiply(const atermpp::aterm_appl& x);
inline bool is_rescondand(const atermpp::aterm_appl& x);
inline bool is_rescondor(const atermpp::aterm_appl& x);
inline bool is_reseqinf(const atermpp::aterm_appl& x);
inline bool is_reseqninf(const atermpp::aterm_appl& x);

/// \\brief Test for a res_expression expression
/// \\param x A term
/// \\return True if \\a x is a res_expression expression
inline
bool is_res_expression(const atermpp::aterm_appl& x)
{
  return res::is_true(x) ||
         res::is_false(x) ||
         res::is_res_variable(x) ||
         res::is_not(x) ||
         res::is_and(x) ||
         res::is_or(x) ||
         res::is_imp(x) ||
         res::is_plus(x) ||
         res::is_multiply(x) ||
         res::is_rescondand(x) ||
         res::is_rescondor(x) ||
         res::is_reseqinf(x) ||
         res::is_reseqninf(x);
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


/// \\brief The not operator for res expressions
class not_: public res_expression
{
  public:
    /// \\brief Default constructor.
    not_()
      : res_expression(core::detail::default_values::RESNot)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit not_(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESNot(*this));
    }

    /// \\brief Constructor.
    explicit not_(const res_expression& operand)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESNot(), operand))
    {}

    /// Move semantics
    not_(const not_&) noexcept = default;
    not_(not_&&) noexcept = default;
    not_& operator=(const not_&) noexcept = default;
    not_& operator=(not_&&) noexcept = default;

    const res_expression& operand() const
    {
      return atermpp::down_cast<res_expression>((*this)[0]);
    }
};

/// \\brief Make_not_ constructs a new term into a given address.
/// \\ \param t The reference into which the new not_ is constructed. 
template <class... ARGUMENTS>
inline void make_not_(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESNot(), args...);
}

/// \\brief Test for a not expression
/// \\param x A term
/// \\return True if \\a x is a not expression
inline
bool is_not(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESNot;
}

// prototype declaration
std::string pp(const not_& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const not_& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(not_& t1, not_& t2)
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


/// \\brief Multiplication with a positive constant for res expressions
class multiply: public res_expression
{
  public:
    /// \\brief Default constructor.
    multiply()
      : res_expression(core::detail::default_values::RESConstantMultiply)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit multiply(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESConstantMultiply(*this));
    }

    /// \\brief Constructor.
    multiply(const data::data_expression& left, const res_expression& right)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESConstantMultiply(), left, right))
    {}

    /// Move semantics
    multiply(const multiply&) noexcept = default;
    multiply(multiply&&) noexcept = default;
    multiply& operator=(const multiply&) noexcept = default;
    multiply& operator=(multiply&&) noexcept = default;

    const data::data_expression& left() const
    {
      return atermpp::down_cast<data::data_expression>((*this)[0]);
    }

    const res_expression& right() const
    {
      return atermpp::down_cast<res_expression>((*this)[1]);
    }
};

/// \\brief Make_multiply constructs a new term into a given address.
/// \\ \param t The reference into which the new multiply is constructed. 
template <class... ARGUMENTS>
inline void make_multiply(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESConstantMultiply(), args...);
}

/// \\brief Test for a multiply expression
/// \\param x A term
/// \\return True if \\a x is a multiply expression
inline
bool is_multiply(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESConstantMultiply;
}

// prototype declaration
std::string pp(const multiply& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const multiply& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(multiply& t1, multiply& t2)
{
  t1.swap(t2);
}


/// \\brief The conditional and expression for res expressions
class rescondand: public res_expression
{
  public:
    /// \\brief Default constructor.
    rescondand()
      : res_expression(core::detail::default_values::RESCondAnd)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit rescondand(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESCondAnd(*this));
    }

    /// \\brief Constructor.
    rescondand(const res_expression& arg1, const res_expression& arg2, const res_expression& arg3)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESCondAnd(), arg1, arg2, arg3))
    {}

    /// Move semantics
    rescondand(const rescondand&) noexcept = default;
    rescondand(rescondand&&) noexcept = default;
    rescondand& operator=(const rescondand&) noexcept = default;
    rescondand& operator=(rescondand&&) noexcept = default;

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

/// \\brief Make_rescondand constructs a new term into a given address.
/// \\ \param t The reference into which the new rescondand is constructed. 
template <class... ARGUMENTS>
inline void make_rescondand(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESCondAnd(), args...);
}

/// \\brief Test for a rescondand expression
/// \\param x A term
/// \\return True if \\a x is a rescondand expression
inline
bool is_rescondand(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESCondAnd;
}

// prototype declaration
std::string pp(const rescondand& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const rescondand& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(rescondand& t1, rescondand& t2)
{
  t1.swap(t2);
}


/// \\brief The conditional or expression for res expressions
class rescondor: public res_expression
{
  public:
    /// \\brief Default constructor.
    rescondor()
      : res_expression(core::detail::default_values::RESCondOr)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit rescondor(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESCondOr(*this));
    }

    /// \\brief Constructor.
    rescondor(const res_expression& arg1, const res_expression& arg2, const res_expression& arg3)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESCondOr(), arg1, arg2, arg3))
    {}

    /// Move semantics
    rescondor(const rescondor&) noexcept = default;
    rescondor(rescondor&&) noexcept = default;
    rescondor& operator=(const rescondor&) noexcept = default;
    rescondor& operator=(rescondor&&) noexcept = default;

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

/// \\brief Make_rescondor constructs a new term into a given address.
/// \\ \param t The reference into which the new rescondor is constructed. 
template <class... ARGUMENTS>
inline void make_rescondor(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESCondOr(), args...);
}

/// \\brief Test for a rescondor expression
/// \\param x A term
/// \\return True if \\a x is a rescondor expression
inline
bool is_rescondor(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESCondOr;
}

// prototype declaration
std::string pp(const rescondor& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const rescondor& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(rescondor& t1, rescondor& t2)
{
  t1.swap(t2);
}


/// \\brief The operator to check for infinity
class reseqinf: public res_expression
{
  public:
    /// \\brief Default constructor.
    reseqinf()
      : res_expression(core::detail::default_values::RESEqInf)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit reseqinf(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESEqInf(*this));
    }

    /// \\brief Constructor.
    explicit reseqinf(const res_expression& operand)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESEqInf(), operand))
    {}

    /// Move semantics
    reseqinf(const reseqinf&) noexcept = default;
    reseqinf(reseqinf&&) noexcept = default;
    reseqinf& operator=(const reseqinf&) noexcept = default;
    reseqinf& operator=(reseqinf&&) noexcept = default;

    const res_expression& operand() const
    {
      return atermpp::down_cast<res_expression>((*this)[0]);
    }
};

/// \\brief Make_reseqinf constructs a new term into a given address.
/// \\ \param t The reference into which the new reseqinf is constructed. 
template <class... ARGUMENTS>
inline void make_reseqinf(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESEqInf(), args...);
}

/// \\brief Test for a reseqinf expression
/// \\param x A term
/// \\return True if \\a x is a reseqinf expression
inline
bool is_reseqinf(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESEqInf;
}

// prototype declaration
std::string pp(const reseqinf& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const reseqinf& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(reseqinf& t1, reseqinf& t2)
{
  t1.swap(t2);
}


/// \\brief The operator to check for negative infinity
class reseqninf: public res_expression
{
  public:
    /// \\brief Default constructor.
    reseqninf()
      : res_expression(core::detail::default_values::RESEqInf)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit reseqninf(const atermpp::aterm& term)
      : res_expression(term)
    {
      assert(core::detail::check_term_RESEqInf(*this));
    }

    /// \\brief Constructor.
    explicit reseqninf(const res_expression& operand)
      : res_expression(atermpp::aterm_appl(core::detail::function_symbol_RESEqInf(), operand))
    {}

    /// Move semantics
    reseqninf(const reseqninf&) noexcept = default;
    reseqninf(reseqninf&&) noexcept = default;
    reseqninf& operator=(const reseqninf&) noexcept = default;
    reseqninf& operator=(reseqninf&&) noexcept = default;

    const res_expression& operand() const
    {
      return atermpp::down_cast<res_expression>((*this)[0]);
    }
};

/// \\brief Make_reseqninf constructs a new term into a given address.
/// \\ \param t The reference into which the new reseqninf is constructed. 
template <class... ARGUMENTS>
inline void make_reseqninf(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_RESEqInf(), args...);
}

/// \\brief Test for a reseqninf expression
/// \\param x A term
/// \\return True if \\a x is a reseqninf expression
inline
bool is_reseqninf(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RESEqInf;
}

// prototype declaration
std::string pp(const reseqninf& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const reseqninf& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(reseqninf& t1, reseqninf& t2)
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

} // namespace bes

} // namespace mcrl2

namespace mcrl2
{

namespace core
{

/// \brief Contains type information for res expressions
template <>
struct term_traits<bes::res_expression>
{
  /// The term type
  typedef bes::res_expression term_type;

  /// \brief The variable type
  typedef bes::res_variable variable_type;

  /// \brief The string type
  typedef core::identifier_string string_type;

  /// \brief The value true
  /// \return The value true
  static inline
  bes::res_expression true_()
  {
    return bes::true_();
  }

  /// \brief The value false
  /// \return The value false
  static inline
  bes::res_expression false_()
  {
    return bes::false_();
  }

  /// \brief Operator not
  /// \param x A term
  /// \return Operator not applied to
  static inline
  bes::res_expression not_(const bes::res_expression& x)
  {
    return bes::not_(x);
  }

  /// \brief Operator not
  /// \param result Will contain not applied to x. 
  /// \param x A term
  static inline
  void make_not_(bes::res_expression& result, const bes::res_expression& x)
  {
    bes::make_not_(result, x);
  }

  /// \brief Operator and
  /// \param p A term
  /// \param q A term
  /// \return Operator and applied to p and q
  static inline
  bes::res_expression and_(const bes::res_expression& p, const bes::res_expression& q)
  {
    return bes::and_(p, q);
  }

  /// \brief Operator and
  /// \param result Operator and applied to p and q
  /// \param p A term
  /// \param q A term
  static inline
  void make_and_(bes::res_expression& result, const bes::res_expression& p, const bes::res_expression& q)
  {
    bes::make_and_(result, p, q);
  }

  /// \brief Operator or
  /// \param p A term
  /// \param q A term
  /// \return Operator or applied to p and q
  static inline
  bes::res_expression or_(const bes::res_expression& p, const bes::res_expression& q)
  {
    return bes::or_(p, q);
  }
  //
  /// \brief Operator or
  /// \param result Operator or applied to p and q
  /// \param p A term
  /// \param q A term
  static inline
  void make_or_(bes::res_expression& result, const bes::res_expression& p, const bes::res_expression& q)
  {
    bes::make_or_(result, p, q);
  }

  /// \brief Implication
  /// \param p A term
  /// \param q A term
  /// \return Implication applied to p and q
  static inline
  bes::res_expression imp(const bes::res_expression& p, const bes::res_expression& q)
  {
    return bes::imp(p, q);
  }
  
  /// \brief Implication
  /// \param result Implication applied to p and q
  /// \param p A term
  /// \param q A term
  static inline
  void make_imp(bes::res_expression& result, const bes::res_expression& p, const bes::res_expression& q)
  {
    bes::make_imp(result, p, q);
  }

  /// \brief Test for value true
  /// \param t A term
  /// \return True if the term has the value true
  static inline
  bool is_true(const bes::res_expression& t)
  {
    return bes::is_true(t);
  }

  /// \brief Test for value false
  /// \param t A term
  /// \return True if the term has the value false
  static inline
  bool is_false(const bes::res_expression& t)
  {
    return bes::is_false(t);
  }

  /// \brief Test for operator not
  /// \param t A term
  /// \return True if the term is of type and
  static inline
  bool is_not(const bes::res_expression& t)
  {
    return bes::is_not(t);
  }

  /// \brief Test for operator and
  /// \param t A term
  /// \return True if the term is of type and
  static inline
  bool is_and(const bes::res_expression& t)
  {
    return bes::is_and(t);
  }

  /// \brief Test for operator or
  /// \param t A term
  /// \return True if the term is of type or
  static inline
  bool is_or(const bes::res_expression& t)
  {
    return bes::is_or(t);
  }

  /// \brief Test for implication
  /// \param t A term
  /// \return True if the term is an implication
  static inline
  bool is_imp(const bes::res_expression& t)
  {
    return bes::is_imp(t);
  }

  /// \brief Test for propositional variable
  /// \param t A term
  /// \return True if the term is a propositional variable
  static inline
  bool is_prop_var(const bes::res_expression& t)
  {
    return bes::is_res_variable(t);
  }

  /// \brief Returns the left argument of a term of type and, or or imp
  /// \param t A term
  /// \return The left argument of the term
  static inline
  const bes::res_expression& left(const bes::res_expression& t)
  {
    assert(is_and(t) || is_or(t) || is_imp(t));
    return atermpp::down_cast<const bes::res_expression>(t[0]);
  }

  /// \brief Returns the right argument of a term of type and, or or imp
  /// \param t A term
  /// \return The right argument of the term
  static inline
  const bes::res_expression& right(const bes::res_expression& t)
  {
    assert(is_and(t) || is_or(t) || is_imp(t));
    return atermpp::down_cast<const bes::res_expression>(t[1]);
  }

  /// \brief Returns the argument of a term of type not
  /// \param t A term
  static inline
  const bes::res_expression& not_arg(const bes::res_expression& t)
  {
    assert(is_not(t));
    return atermpp::down_cast<bes::not_>(t).operand();
  }

  /// \brief Returns the name of a res variable
  /// \param t A term
  /// \return The name of the res variable
  static inline
  const core::identifier_string& name(const bes::res_expression& t)
  {
    assert(bes::is_res_variable(t));
    return atermpp::down_cast<bes::res_variable>(t).name();
  }

  /// \brief Conversion from variable to term
  /// \param v A variable
  /// \returns The converted variable
  static inline
  const bes::res_expression& variable2term(const bes::res_variable& v)
  {
    return v;
  }

  /// \brief Conversion from term to variable
  /// \param t a term
  /// \returns The converted term
  static inline
  const bes::res_variable& term2variable(const bes::res_expression& t)
  {
    return atermpp::down_cast<bes::res_variable>(t);
  }

  /// \brief Pretty print function
  /// \param t A term
  /// \return Returns a pretty print representation of the term
  static inline
  std::string pp(const bes::res_expression& t)
  {
    return bes::pp(t);
  }
};

} // namespace core

} // namespace mcrl2

#endif // MCRL2_RES_RES_EXPRESSION_H
