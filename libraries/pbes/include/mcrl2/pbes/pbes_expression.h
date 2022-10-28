// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_expression.h
/// \brief The class pbes_expression.

#ifndef MCRL2_PBES_PBES_EXPRESSION_H
#define MCRL2_PBES_PBES_EXPRESSION_H

#include "mcrl2/data/expression_traits.h"
#include "mcrl2/data/optimized_boolean_operators.h"
#include "mcrl2/pbes/propositional_variable.h"

namespace mcrl2
{

namespace pbes_system
{

typedef std::pair<core::identifier_string, data::data_expression_list> propositional_variable_key_type;

//--- start generated classes ---//
/// \\brief A pbes expression
class pbes_expression: public atermpp::aterm_appl
{
  public:
    /// \\brief Default constructor.
    pbes_expression()
      : atermpp::aterm_appl(core::detail::default_values::PBExpr)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit pbes_expression(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_PBExpr(*this));
    }

    /// \\brief Constructor.
    pbes_expression(const data::data_expression& x)
      : atermpp::aterm_appl(x)
    {}

    /// \\brief Constructor.
    pbes_expression(const data::variable& x)
      : atermpp::aterm_appl(x)
    {}

    /// \\brief Constructor.
    pbes_expression(const data::untyped_data_parameter& x)
      : atermpp::aterm_appl(x)
    {}

    /// Move semantics
    pbes_expression(const pbes_expression&) noexcept = default;
    pbes_expression(pbes_expression&&) noexcept = default;
    pbes_expression& operator=(const pbes_expression&) noexcept = default;
    pbes_expression& operator=(pbes_expression&&) noexcept = default;
};

/// \\brief list of pbes_expressions
typedef atermpp::term_list<pbes_expression> pbes_expression_list;

/// \\brief vector of pbes_expressions
typedef std::vector<pbes_expression>    pbes_expression_vector;

// prototypes
inline bool is_propositional_variable_instantiation(const atermpp::aterm_appl& x);
inline bool is_not(const atermpp::aterm_appl& x);
inline bool is_and(const atermpp::aterm_appl& x);
inline bool is_or(const atermpp::aterm_appl& x);
inline bool is_imp(const atermpp::aterm_appl& x);
inline bool is_forall(const atermpp::aterm_appl& x);
inline bool is_exists(const atermpp::aterm_appl& x);

/// \\brief Test for a pbes_expression expression
/// \\param x A term
/// \\return True if \\a x is a pbes_expression expression
inline
bool is_pbes_expression(const atermpp::aterm_appl& x)
{
  return data::is_data_expression(x) ||
         data::is_variable(x) ||
         data::is_untyped_data_parameter(x) ||
         pbes_system::is_propositional_variable_instantiation(x) ||
         pbes_system::is_not(x) ||
         pbes_system::is_and(x) ||
         pbes_system::is_or(x) ||
         pbes_system::is_imp(x) ||
         pbes_system::is_forall(x) ||
         pbes_system::is_exists(x);
}

// prototype declaration
std::string pp(const pbes_expression& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const pbes_expression& x)
{
  return out << pbes_system::pp(x);
}

/// \\brief swap overload
inline void swap(pbes_expression& t1, pbes_expression& t2)
{
  t1.swap(t2);
}


/// \\brief A propositional variable instantiation
class propositional_variable_instantiation: public pbes_expression
{
  public:


    /// Move semantics
    propositional_variable_instantiation(const propositional_variable_instantiation&) noexcept = default;
    propositional_variable_instantiation(propositional_variable_instantiation&&) noexcept = default;
    propositional_variable_instantiation& operator=(const propositional_variable_instantiation&) noexcept = default;
    propositional_variable_instantiation& operator=(propositional_variable_instantiation&&) noexcept = default;

    const core::identifier_string& name() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[0]);
    }

    const data::data_expression_list& parameters() const
    {
      return atermpp::down_cast<data::data_expression_list>((*this)[1]);
    }
//--- start user section propositional_variable_instantiation ---//
    /// \brief Default constructor.
    propositional_variable_instantiation()
      : pbes_expression(core::detail::default_values::PropVarInst)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit propositional_variable_instantiation(const atermpp::aterm& term)
      : pbes_expression(term)
    {
      assert(core::detail::check_term_PropVarInst(*this));
    }

    /// \brief Constructor.
    propositional_variable_instantiation(const core::identifier_string& name, const data::data_expression_list& parameters)
    {
      atermpp::make_term_appl_with_index<propositional_variable_instantiation, std::pair<core::identifier_string, data::data_expression_list> >
                   (*this,core::detail::function_symbol_PropVarInst(), name, parameters);
    }

    /// \brief Constructor.
    propositional_variable_instantiation(const std::string& name, const data::data_expression_list& parameters)
    {
      atermpp::make_term_appl_with_index<propositional_variable_instantiation, std::pair<core::identifier_string, data::data_expression_list> >
                   (*this,core::detail::function_symbol_PropVarInst(), core::identifier_string(name), parameters);
    }

//--- end user section propositional_variable_instantiation ---//
};

/// \\brief Make_propositional_variable_instantiation constructs a new term into a given address.
/// \\ \param t The reference into which the new propositional_variable_instantiation is constructed. 
template <class... ARGUMENTS>
inline void make_propositional_variable_instantiation(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl_with_index<propositional_variable_instantiation,std::pair<core::identifier_string, data::data_expression_list>>(t, core::detail::function_symbol_PropVarInst(), args...);
}

/// \\brief list of propositional_variable_instantiations
typedef atermpp::term_list<propositional_variable_instantiation> propositional_variable_instantiation_list;

/// \\brief vector of propositional_variable_instantiations
typedef std::vector<propositional_variable_instantiation>    propositional_variable_instantiation_vector;

/// \\brief Test for a propositional_variable_instantiation expression
/// \\param x A term
/// \\return True if \\a x is a propositional_variable_instantiation expression
inline
bool is_propositional_variable_instantiation(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::PropVarInst;
}

// prototype declaration
std::string pp(const propositional_variable_instantiation& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const propositional_variable_instantiation& x)
{
  return out << pbes_system::pp(x);
}

/// \\brief swap overload
inline void swap(propositional_variable_instantiation& t1, propositional_variable_instantiation& t2)
{
  t1.swap(t2);
}


/// \\brief The not operator for pbes expressions
class not_: public pbes_expression
{
  public:
    /// \\brief Default constructor.
    not_()
      : pbes_expression(core::detail::default_values::PBESNot)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit not_(const atermpp::aterm& term)
      : pbes_expression(term)
    {
      assert(core::detail::check_term_PBESNot(*this));
    }

    /// \\brief Constructor.
    explicit not_(const pbes_expression& operand)
      : pbes_expression(atermpp::aterm_appl(core::detail::function_symbol_PBESNot(), operand))
    {}

    /// Move semantics
    not_(const not_&) noexcept = default;
    not_(not_&&) noexcept = default;
    not_& operator=(const not_&) noexcept = default;
    not_& operator=(not_&&) noexcept = default;

    const pbes_expression& operand() const
    {
      return atermpp::down_cast<pbes_expression>((*this)[0]);
    }
};

/// \\brief Make_not_ constructs a new term into a given address.
/// \\ \param t The reference into which the new not_ is constructed. 
template <class... ARGUMENTS>
inline void make_not_(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_PBESNot(), args...);
}

/// \\brief Test for a not expression
/// \\param x A term
/// \\return True if \\a x is a not expression
inline
bool is_not(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::PBESNot;
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
  return out << pbes_system::pp(x);
}

/// \\brief swap overload
inline void swap(not_& t1, not_& t2)
{
  t1.swap(t2);
}


/// \\brief The and operator for pbes expressions
class and_: public pbes_expression
{
  public:
    /// \\brief Default constructor.
    and_()
      : pbes_expression(core::detail::default_values::PBESAnd)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit and_(const atermpp::aterm& term)
      : pbes_expression(term)
    {
      assert(core::detail::check_term_PBESAnd(*this));
    }

    /// \\brief Constructor.
    and_(const pbes_expression& left, const pbes_expression& right)
      : pbes_expression(atermpp::aterm_appl(core::detail::function_symbol_PBESAnd(), left, right))
    {}

    /// Move semantics
    and_(const and_&) noexcept = default;
    and_(and_&&) noexcept = default;
    and_& operator=(const and_&) noexcept = default;
    and_& operator=(and_&&) noexcept = default;

    const pbes_expression& left() const
    {
      return atermpp::down_cast<pbes_expression>((*this)[0]);
    }

    const pbes_expression& right() const
    {
      return atermpp::down_cast<pbes_expression>((*this)[1]);
    }
};

/// \\brief Make_and_ constructs a new term into a given address.
/// \\ \param t The reference into which the new and_ is constructed. 
template <class... ARGUMENTS>
inline void make_and_(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_PBESAnd(), args...);
}

/// \\brief Test for a and expression
/// \\param x A term
/// \\return True if \\a x is a and expression
inline
bool is_and(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::PBESAnd;
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
  return out << pbes_system::pp(x);
}

/// \\brief swap overload
inline void swap(and_& t1, and_& t2)
{
  t1.swap(t2);
}


/// \\brief The or operator for pbes expressions
class or_: public pbes_expression
{
  public:
    /// \\brief Default constructor.
    or_()
      : pbes_expression(core::detail::default_values::PBESOr)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit or_(const atermpp::aterm& term)
      : pbes_expression(term)
    {
      assert(core::detail::check_term_PBESOr(*this));
    }

    /// \\brief Constructor.
    or_(const pbes_expression& left, const pbes_expression& right)
      : pbes_expression(atermpp::aterm_appl(core::detail::function_symbol_PBESOr(), left, right))
    {}

    /// Move semantics
    or_(const or_&) noexcept = default;
    or_(or_&&) noexcept = default;
    or_& operator=(const or_&) noexcept = default;
    or_& operator=(or_&&) noexcept = default;

    const pbes_expression& left() const
    {
      return atermpp::down_cast<pbes_expression>((*this)[0]);
    }

    const pbes_expression& right() const
    {
      return atermpp::down_cast<pbes_expression>((*this)[1]);
    }
};

/// \\brief Make_or_ constructs a new term into a given address.
/// \\ \param t The reference into which the new or_ is constructed. 
template <class... ARGUMENTS>
inline void make_or_(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_PBESOr(), args...);
}

/// \\brief Test for a or expression
/// \\param x A term
/// \\return True if \\a x is a or expression
inline
bool is_or(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::PBESOr;
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
  return out << pbes_system::pp(x);
}

/// \\brief swap overload
inline void swap(or_& t1, or_& t2)
{
  t1.swap(t2);
}


/// \\brief The implication operator for pbes expressions
class imp: public pbes_expression
{
  public:
    /// \\brief Default constructor.
    imp()
      : pbes_expression(core::detail::default_values::PBESImp)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit imp(const atermpp::aterm& term)
      : pbes_expression(term)
    {
      assert(core::detail::check_term_PBESImp(*this));
    }

    /// \\brief Constructor.
    imp(const pbes_expression& left, const pbes_expression& right)
      : pbes_expression(atermpp::aterm_appl(core::detail::function_symbol_PBESImp(), left, right))
    {}

    /// Move semantics
    imp(const imp&) noexcept = default;
    imp(imp&&) noexcept = default;
    imp& operator=(const imp&) noexcept = default;
    imp& operator=(imp&&) noexcept = default;

    const pbes_expression& left() const
    {
      return atermpp::down_cast<pbes_expression>((*this)[0]);
    }

    const pbes_expression& right() const
    {
      return atermpp::down_cast<pbes_expression>((*this)[1]);
    }
};

/// \\brief Make_imp constructs a new term into a given address.
/// \\ \param t The reference into which the new imp is constructed. 
template <class... ARGUMENTS>
inline void make_imp(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_PBESImp(), args...);
}

/// \\brief Test for a imp expression
/// \\param x A term
/// \\return True if \\a x is a imp expression
inline
bool is_imp(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::PBESImp;
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
  return out << pbes_system::pp(x);
}

/// \\brief swap overload
inline void swap(imp& t1, imp& t2)
{
  t1.swap(t2);
}


/// \\brief The universal quantification operator for pbes expressions
class forall: public pbes_expression
{
  public:
    /// \\brief Default constructor.
    forall()
      : pbes_expression(core::detail::default_values::PBESForall)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit forall(const atermpp::aterm& term)
      : pbes_expression(term)
    {
      assert(core::detail::check_term_PBESForall(*this));
    }

    /// \\brief Constructor.
    forall(const data::variable_list& variables, const pbes_expression& body)
      : pbes_expression(atermpp::aterm_appl(core::detail::function_symbol_PBESForall(), variables, body))
    {}

    /// Move semantics
    forall(const forall&) noexcept = default;
    forall(forall&&) noexcept = default;
    forall& operator=(const forall&) noexcept = default;
    forall& operator=(forall&&) noexcept = default;

    const data::variable_list& variables() const
    {
      return atermpp::down_cast<data::variable_list>((*this)[0]);
    }

    const pbes_expression& body() const
    {
      return atermpp::down_cast<pbes_expression>((*this)[1]);
    }
};

/// \\brief Make_forall constructs a new term into a given address.
/// \\ \param t The reference into which the new forall is constructed. 
template <class... ARGUMENTS>
inline void make_forall(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_PBESForall(), args...);
}

/// \\brief Test for a forall expression
/// \\param x A term
/// \\return True if \\a x is a forall expression
inline
bool is_forall(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::PBESForall;
}

// prototype declaration
std::string pp(const forall& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const forall& x)
{
  return out << pbes_system::pp(x);
}

/// \\brief swap overload
inline void swap(forall& t1, forall& t2)
{
  t1.swap(t2);
}


/// \\brief The existential quantification operator for pbes expressions
class exists: public pbes_expression
{
  public:
    /// \\brief Default constructor.
    exists()
      : pbes_expression(core::detail::default_values::PBESExists)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit exists(const atermpp::aterm& term)
      : pbes_expression(term)
    {
      assert(core::detail::check_term_PBESExists(*this));
    }

    /// \\brief Constructor.
    exists(const data::variable_list& variables, const pbes_expression& body)
      : pbes_expression(atermpp::aterm_appl(core::detail::function_symbol_PBESExists(), variables, body))
    {}

    /// Move semantics
    exists(const exists&) noexcept = default;
    exists(exists&&) noexcept = default;
    exists& operator=(const exists&) noexcept = default;
    exists& operator=(exists&&) noexcept = default;

    const data::variable_list& variables() const
    {
      return atermpp::down_cast<data::variable_list>((*this)[0]);
    }

    const pbes_expression& body() const
    {
      return atermpp::down_cast<pbes_expression>((*this)[1]);
    }
};

/// \\brief Make_exists constructs a new term into a given address.
/// \\ \param t The reference into which the new exists is constructed. 
template <class... ARGUMENTS>
inline void make_exists(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_PBESExists(), args...);
}

/// \\brief Test for a exists expression
/// \\param x A term
/// \\return True if \\a x is a exists expression
inline
bool is_exists(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::PBESExists;
}

// prototype declaration
std::string pp(const exists& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const exists& x)
{
  return out << pbes_system::pp(x);
}

/// \\brief swap overload
inline void swap(exists& t1, exists& t2)
{
  t1.swap(t2);
}
//--- end generated classes ---//

// template function overloads
std::string pp(const pbes_expression_list& x);
std::string pp(const pbes_expression_vector& x);
std::string pp(const propositional_variable_instantiation_list& x);
std::string pp(const propositional_variable_instantiation_vector& x);
std::set<pbes_system::propositional_variable_instantiation> find_propositional_variable_instantiations(const pbes_system::pbes_expression& x);
std::set<core::identifier_string> find_identifiers(const pbes_system::pbes_expression& x);
std::set<data::variable> find_free_variables(const pbes_system::pbes_expression& x);
bool search_variable(const pbes_system::pbes_expression& x, const data::variable& v);
pbes_system::pbes_expression normalize_sorts(const pbes_system::pbes_expression& x, const data::sort_specification& sortspec);
pbes_system::pbes_expression translate_user_notation(const pbes_system::pbes_expression& x);

/// \return Returns the value true
inline
const pbes_expression& true_()
{
  /* The dynamic cast is required, to prevent copying the data term true to
     a local term on the stack. */
  return reinterpret_cast<const pbes_expression&>(data::sort_bool::true_());
}

/// \return Returns the value false
inline
const pbes_expression& false_()
{
  /* The dynamic cast is required, to prevent copying the data term false to
     a local term on the stack. */
  return reinterpret_cast<const pbes_expression&>(data::sort_bool::false_());
}

/// \brief Test for the value true
/// \param t A PBES expression
/// \return True if it is the value \p true
inline bool is_true(const pbes_expression& t)
{
  return data::sort_bool::is_true_function_symbol(t);
}

/// \brief Test for the value false
/// \param t A PBES expression
/// \return True if it is the value \p false
inline bool is_false(const pbes_expression& t)
{
  return data::sort_bool::is_false_function_symbol(t);
}

/// \brief Returns true if the term t is a not expression
/// \param t A PBES expression
/// \return True if the term t is a not expression
inline bool is_pbes_not(const pbes_expression& t)
{
  return pbes_system::is_not(t);
}

/// \brief Returns true if the term t is an and expression
/// \param t A PBES expression
/// \return True if the term t is an and expression
inline bool is_pbes_and(const pbes_expression& t)
{
  return pbes_system::is_and(t);
}

/// \brief Returns true if the term t is an or expression
/// \param t A PBES expression
/// \return True if the term t is an or expression
inline bool is_pbes_or(const pbes_expression& t)
{
  return pbes_system::is_or(t);
}

/// \brief Returns true if the term t is an imp expression
/// \param t A PBES expression
/// \return True if the term t is an imp expression
inline bool is_pbes_imp(const pbes_expression& t)
{
  return pbes_system::is_imp(t);
}

/// \brief Returns true if the term t is a universal quantification
/// \param t A PBES expression
/// \return True if the term t is a universal quantification
inline bool is_pbes_forall(const pbes_expression& t)
{
  return pbes_system::is_forall(t);
}

/// \brief Returns true if the term t is an existential quantification
/// \param t A PBES expression
/// \return True if the term t is an existential quantification
inline bool is_pbes_exists(const pbes_expression& t)
{
  return pbes_system::is_exists(t);
}

/// \brief Test for a conjunction
/// \param t A PBES expression or a data expression
/// \return True if it is a conjunction
inline bool is_universal_not(const pbes_expression& t)
{
  return is_pbes_not(t) || data::sort_bool::is_not_application(t);
}

/// \brief Test for a conjunction
/// \param t A PBES expression or a data expression
/// \return True if it is a conjunction
inline bool is_universal_and(const pbes_expression& t)
{
  return is_pbes_and(t) || data::sort_bool::is_and_application(t);
}

/// \brief Test for a disjunction
/// \param t A PBES expression or a data expression
/// \return True if it is a disjunction
inline bool is_universal_or(const pbes_expression& t)
{
  return is_pbes_or(t) || data::sort_bool::is_or_application(t);
}

/// \brief Returns true if the term t is a data expression
/// \param t A PBES expression
/// \return True if the term t is a data expression
inline bool is_data(const pbes_expression& t)
{
  return data::is_data_expression(t);
}

/// \brief The namespace for accessor functions on pbes expressions.
namespace accessors
{

/// \brief Returns the pbes expression argument of expressions of type not, exists and forall.
/// \param t A PBES expression
/// \return The pbes expression argument of expressions of type not, exists and forall.
inline
const pbes_expression& arg(const pbes_expression& t)
{
  if (is_pbes_not(t))
  {
    return atermpp::down_cast<const pbes_expression>(t[0]);
  }
  else
  {
    assert(is_forall(t) || is_exists(t));
    return atermpp::down_cast<const pbes_expression>(t[1]);
  }
}

/// \brief Returns the pbes expression argument of expressions of type not, exists and forall.
/// \param t A PBES expression or a data expression
/// \return The pbes expression argument of expressions of type not, exists and forall.
inline
pbes_expression data_arg(const pbes_expression& t)
{
  if (data::is_data_expression(t))
  {
    assert(data::is_application(t));
    const auto& a = atermpp::down_cast<const data::application>(t);
    return *(a.begin());
  }
  else
  {
    return arg(t);
  }
}

/// \brief Returns the left hand side of an expression of type and, or or imp.
/// \param t A PBES expression
/// \return The left hand side of an expression of type and, or or imp.
inline
const pbes_expression& left(const pbes_expression& t)
{
  assert(is_and(t) || is_or(t) || is_imp(t));
  return atermpp::down_cast<pbes_expression>(t[0]);
}

/// \brief Returns the left hand side of an expression of type and, or or imp.
/// \param x A PBES expression or a data expression
/// \return The left hand side of an expression of type and, or or imp.
inline
pbes_expression data_left(const pbes_expression& x)
{
  if (data::is_data_expression(x))
  {
    return data::binary_left(atermpp::down_cast<data::application>(x));
  }
  else
  {
    return left(x);
  }
}

/// \brief Returns the right hand side of an expression of type and, or or imp.
/// \param t A PBES expression
/// \return The right hand side of an expression of type and, or or imp.
inline
const pbes_expression& right(const pbes_expression& t)
{
  return atermpp::down_cast<pbes_expression>(t[1]);
}

/// \brief Returns the left hand side of an expression of type and, or or imp.
/// \param x A PBES expression or a data expression
/// \return The left hand side of an expression of type and, or or imp.
inline
pbes_expression data_right(const pbes_expression& x)
{
  if (data::is_data_expression(x))
  {
    return data::binary_right(atermpp::down_cast<data::application>(x));
  }
  else
  {
    return right(x);
  }
}

/// \brief Returns the variables of a quantification expression
/// \param t A PBES expression
/// \return The variables of a quantification expression
inline
const data::variable_list& var(const pbes_expression& t)
{
  assert(is_forall(t) || is_exists(t));
  return atermpp::down_cast<data::variable_list>(t[0]);
}

/// \brief Returns the name of a propositional variable expression
/// \param t A PBES expression
/// \return The name of a propositional variable expression
inline
const core::identifier_string& name(const pbes_expression& t)
{
  assert(is_propositional_variable_instantiation(t));
  return atermpp::down_cast<core::identifier_string>(t[0]);
}

/// \brief Returns the parameters of a propositional variable instantiation.
/// \param t A PBES expression
/// \return The parameters of a propositional variable instantiation.
inline
const data::data_expression_list& param(const pbes_expression& t)
{
  assert(is_propositional_variable_instantiation(t));
  return atermpp::down_cast<data::data_expression_list>(t[1]);
}
} // namespace accessors

/// \brief Make a universal quantification. It checks for an empty variable list,
/// which is not allowed.
/// \param l A sequence of data variables
/// \param p A PBES expression
/// \return The value <tt>forall l.p</tt>
inline
pbes_expression make_forall_(const data::variable_list& l, const pbes_expression& p)
{
  if (l.empty())
  {
    return p;
  }
  return pbes_expression(atermpp::aterm_appl(core::detail::function_symbol_PBESForall(), l, p));
}

/// \brief Make an existential quantification. It checks for an empty variable list,
/// which is not allowed.
/// \param l A sequence of data variables
/// \param p A PBES expression
/// \return The value <tt>exists l.p</tt>
inline
pbes_expression make_exists_(const data::variable_list& l, const pbes_expression& p)
{
  if (l.empty())
  {
    return p;
  }
  return pbes_expression(atermpp::aterm_appl(core::detail::function_symbol_PBESExists(), l, p));
}

/// \brief Make a negation
/// \param p A PBES expression
/// \return The value <tt>!p</tt>
inline
void optimized_not(pbes_expression& result, const pbes_expression& p)
{
  data::optimized_not(result, p);
}

/// \brief Make a conjunction
/// \param p A PBES expression
/// \param q A PBES expression
/// \return The value <tt>p && q</tt>
inline
void optimized_and(pbes_expression& result, const pbes_expression& p, const pbes_expression& q)
{
  data::optimized_and(result, p, q);
}

/// \brief Make a disjunction
/// \param p A PBES expression
/// \param q A PBES expression
/// \return The value <tt>p || q</tt>
inline
void optimized_or(pbes_expression& result, const pbes_expression& p, const pbes_expression& q)
{
  data::optimized_or(result, p, q);
}

/// \brief Make an implication
/// \param p A PBES expression
/// \param q A PBES expression
/// \return The value <tt>p => q</tt>
inline
void optimized_imp(pbes_expression& result, const pbes_expression& p, const pbes_expression& q)
{
  data::optimized_imp(result, p, q);
}

/// \brief Make a universal quantification
/// If l is empty, p is returned.
/// \param l A sequence of data variables
/// \param p A PBES expression
/// \return The value <tt>forall l.p</tt>
inline
void optimized_forall(pbes_expression& result, const data::variable_list& l, const pbes_expression& p)
{
  if (l.empty())
  {
    result = p;
    return;
  }
  if (is_false(p))
  {
    // N.B. Here we use the fact that mCRL2 data types are never empty.
    result = data::sort_bool::false_();
    return;
  }
  if (is_true(p))
  {
    result = true_();
    return;
  }
  make_forall(result, l, p);
  return;
}

/// \brief Make an existential quantification
/// If l is empty, p is returned.
/// \param l A sequence of data variables
/// \param p A PBES expression
/// \return The value <tt>exists l.p</tt>
inline
void optimized_exists(pbes_expression& result, const data::variable_list& l, const pbes_expression& p)
{
  if (l.empty())
  {
    result = p;
    return;
  }
  if (is_false(p))
  {
    result = data::sort_bool::false_();
    return;
  }
  if (is_true(p))
  {
    // N.B. Here we use the fact that mCRL2 data types are never empty.
    result = data::sort_bool::true_();
    return;
  }
  make_exists(result, l, p);
  return;
}

inline
bool is_constant(const pbes_expression& x)
{
  return find_free_variables(x).empty();
}

inline
const data::variable_list& quantifier_variables(const pbes_expression& x)
{
  assert(is_exists(x) || is_forall(x));
  if (is_exists(x))
  {
    return atermpp::down_cast<exists>(x).variables();
  }
  else
  {
    return atermpp::down_cast<forall>(x).variables();
  }
}

inline
data::variable_list free_variables(const pbes_expression& x)
{
  std::set<data::variable> v = find_free_variables(x);
  return data::variable_list(v.begin(), v.end());
}

} // namespace pbes_system

} // namespace mcrl2

namespace mcrl2
{

namespace core
{

/// \brief Contains type information for pbes expressions.
template <>
struct term_traits<pbes_system::pbes_expression>
{
  /// \brief The term type
  typedef pbes_system::pbes_expression term_type;

  /// \brief The data term type
  typedef data::data_expression data_term_type;

  /// \brief The data term sequence type
  typedef data::data_expression_list data_term_sequence_type;

  /// \brief The variable type
  typedef data::variable variable_type;

  /// \brief The variable sequence type
  typedef data::variable_list variable_sequence_type;

  /// \brief The propositional variable declaration type
  typedef pbes_system::propositional_variable propositional_variable_decl_type;

  /// \brief The propositional variable instantiation type
  typedef pbes_system::propositional_variable_instantiation propositional_variable_type;

  /// \brief The string type
  typedef core::identifier_string string_type;

  /// \brief Make the value true
  /// \return The value \p true
  static inline
  term_type true_()
  {
    return pbes_system::true_();
  }

  /// \brief Make the value false
  /// \return The value \p false
  static inline
  term_type false_()
  {
    return pbes_system::false_();
  }

  /// \brief Make a negation
  /// \param p A term
  /// \return The value <tt>!p</tt>
  static inline
  term_type not_(const term_type& p)
  {
    return term_type(atermpp::aterm_appl(core::detail::function_symbol_PBESNot(), p));
  }

  /// \brief Make a negation
  /// \param result The value <tt>!p</tt>
  /// \param p A term
  static inline
  void make_not_(term_type& result, const term_type& p)
  {
    pbes_system::make_not_(result, p);
  }

  /// \brief Make a conjunction
  /// \param p A term
  /// \param q A term
  /// \return The value <tt>p && q</tt>
  static inline
  term_type and_(const term_type& p, const term_type& q)
  {
    return term_type(atermpp::aterm_appl(core::detail::function_symbol_PBESAnd(), p,q));
  }

  /// \brief Make a conjunction
  /// \param result The value <tt>p && q</tt>
  /// \param p A term
  /// \param q A term
  static inline
  void make_and_(term_type& result, const term_type& p, const term_type& q)
  {
    pbes_system::make_and_(result, p, q);
  }

  /// \brief Make a disjunction
  /// \param p A term
  /// \param q A term
  /// \return The value <tt>p || q</tt>
  static inline
  term_type or_(const term_type& p, const term_type& q)
  {
    return term_type(atermpp::aterm_appl(core::detail::function_symbol_PBESOr(), p,q));
  }

  /// \brief Make a disjunction
  /// \param result The value <tt>p || q</tt>
  /// \param p A term
  /// \param q A term
  static inline
  void make_or_(term_type& result, const term_type& p, const term_type& q)
  {
    pbes_system::make_or_(result, p,q);
  }

  template <typename FwdIt>
  static inline
  term_type join_or(FwdIt first, FwdIt last)
  {
    return utilities::detail::join(first, last, or_, false_());
  }

  template <typename FwdIt>
  static inline
  term_type join_and(FwdIt first, FwdIt last)
  {
    return utilities::detail::join(first, last, and_, true_());
  }

  /// \brief Make an implication
  /// \param p A term
  /// \param q A term
  /// \return The value <tt>p => q</tt>
  static inline
  term_type imp(const term_type& p, const term_type& q)
  {
    return term_type(atermpp::aterm_appl(core::detail::function_symbol_PBESImp(), p, q));
  }

  /// \brief Make an implication
  /// \param result The value <tt>p => q</tt>
  /// \param p A term
  /// \param q A term
  static inline
  void make_imp(term_type& result, const term_type& p, const term_type& q)
  {
    pbes_system::make_imp(result, p, q);
  }

  /// \brief Make a universal quantification
  /// \param l A sequence of variables
  /// \param p A term
  /// \return The value <tt>forall l.p</tt>
  static inline
  term_type forall(const variable_sequence_type& l, const term_type& p)
  {
    if (l.empty())
    {
      return p;
    }
    return term_type(atermpp::aterm_appl(core::detail::function_symbol_PBESForall(), l, p));
  }

  /// \brief Make a universal quantification
  /// \param result The value <tt>forall l.p</tt>
  /// \param l A sequence of variables
  /// \param p A term
  static inline
  void make_forall(term_type& result, const variable_sequence_type& l, const term_type& p)
  {
    if (l.empty())
    {
      result = p;
      return;
    }
    pbes_system::make_forall(result, l, p);
  }

  /// \brief Make an existential quantification
  /// \param l A sequence of variables
  /// \param p A term
  /// \return The value <tt>exists l.p</tt>
  static inline
  term_type exists(const variable_sequence_type& l, const term_type& p)
  {
    if (l.empty())
    {
      return p;
    }
    return term_type(atermpp::aterm_appl(core::detail::function_symbol_PBESExists(), l, p));
  }

  /// \brief Make an existential quantification
  /// \param result The value <tt>exists l.p</tt>
  /// \param l A sequence of variables
  /// \param p A term
  static inline
  void make_exists(term_type& result, const variable_sequence_type& l, const term_type& p)
  {
    if (l.empty())
    {
      result = p;
      return;
    }
    pbes_system::make_exists(result, l, p);
  }

  /// \brief Test for the value true
  /// \param t A term
  /// \return True if it is the value \p true
  static inline
  bool is_true(const term_type& t)
  {
    return data::sort_bool::is_true_function_symbol(t);
  }

  /// \brief Test for the value false
  /// \param t A term
  /// \return True if it is the value \p false
  static inline
  bool is_false(const term_type& t)
  {
    return data::sort_bool::is_false_function_symbol(t);
  }

  /// \brief Test for a negation
  /// \param t A term
  /// \return True if it is a negation
  static inline
  bool is_not(const term_type& t)
  {
    return pbes_system::is_not(t);
  }

  /// \brief Test for a conjunction
  /// \param t A term
  /// \return True if it is a conjunction
  static inline
  bool is_and(const term_type& t)
  {
    return pbes_system::is_and(t);
  }

  /// \brief Test for a disjunction
  /// \param t A term
  /// \return True if it is a disjunction
  static inline
  bool is_or(const term_type& t)
  {
    return pbes_system::is_or(t);
  }

  /// \brief Test for an implication
  /// \param t A term
  /// \return True if it is an implication
  static inline
  bool is_imp(const term_type& t)
  {
    return pbes_system::is_imp(t);
  }

  /// \brief Test for an universal quantification
  /// \param t A term
  /// \return True if it is a universal quantification
  static inline
  bool is_forall(const term_type& t)
  {
    return pbes_system::is_forall(t);
  }

  /// \brief Test for an existential quantification
  /// \param t A term
  /// \return True if it is an existential quantification
  static inline
  bool is_exists(const term_type& t)
  {
    return pbes_system::is_exists(t);
  }

  /// \brief Test for data term
  /// \param t A term
  /// \return True if the term is a data term
  static inline
  bool is_data(const term_type& t)
  {
    return data::is_data_expression(t);
  }

  /// \brief Test for propositional variable instantiation
  /// \param t A term
  /// \return True if the term is a propositional variable instantiation
  static inline
  bool is_prop_var(const term_type& t)
  {
    return pbes_system::is_propositional_variable_instantiation(t);
  }

  /// \brief Returns the left argument of a term of type and, or or imp
  /// \param t A term
  /// \return The left argument of the term. Also works for data terms
  static inline
  term_type left(const term_type& t)
  {
    return pbes_system::accessors::left(t);
  }

  /// \brief Returns the right argument of a term of type and, or or imp
  /// \param t A term
  /// \return The right argument of the term. Also works for data terms
  static inline
  term_type right(const term_type& t)
  {
    return pbes_system::accessors::right(t);
  }

  /// \brief Returns the argument of a term of type not
  /// \param t A term
  static inline
  const term_type& not_arg(const term_type& t)
  {
    assert(is_pbes_not(t));
    return atermpp::down_cast<term_type>(t[0]);
  }

  /// \brief Returns the quantifier variables of a quantifier expression
  /// \param t A term
  /// \return The requested argument. Doesn't work for data terms
  static inline
  const variable_sequence_type& var(const term_type& t)
  {
    // Forall and exists are not fully supported by the data library
    assert(!data::is_data_expression(t) || (!data::is_abstraction(t)
                                            || (!data::is_forall(data::abstraction(t)) && !data::is_exists(data::abstraction(t)))));
    assert(is_exists(t) || is_forall(t));

    return atermpp::down_cast<variable_sequence_type>(t[0]);
  }

  /// \brief Returns the name of a propositional variable instantiation
  /// \param t A term
  /// \return The name of the propositional variable instantiation
  static inline
  const string_type &name(const term_type& t)
  {
    assert(is_prop_var(t));
    return atermpp::down_cast<string_type>(t[0]);
  }

  /// \brief Returns the parameter list of a propositional variable instantiation
  /// \param t A term
  /// \return The parameter list of the propositional variable instantiation
  static inline
  const data_term_sequence_type &param(const term_type& t)
  {
    assert(is_prop_var(t));
    return atermpp::down_cast<data_term_sequence_type>(t[1]);
  }

  /// \brief Conversion from variable to term
  /// \param v A variable
  /// \return The converted variable
  static inline
  const term_type& variable2term(const variable_type& v)
  {
    return atermpp::down_cast<term_type>(v);
  }

  /// \brief Test if a term is a variable
  /// \param t A term
  /// \return True if the term is a variable
  static inline
  bool is_variable(const term_type& t)
  {
    return data::is_variable(t);
  }

  /// \brief Pretty print function
  /// \param t A term
  /// \return Returns a pretty print representation of the term
  static inline
  std::string pp(const term_type& t)
  {
    return pbes_system::pp(t);
  }
};

} // namespace core

} // namespace mcrl2

namespace std
{

  template <>
  struct hash<mcrl2::pbes_system::pbes_expression>
  {
    std::size_t operator()(const mcrl2::pbes_system::pbes_expression& x) const
    {
      return hash<atermpp::aterm>()(x);
    }
  };

  template <>
  struct hash<mcrl2::pbes_system::propositional_variable_instantiation>
  {
    std::size_t operator()(const mcrl2::pbes_system::propositional_variable_instantiation& x) const
    {
      return hash<atermpp::aterm>()(x);
    }
  };

} // namespace std

#include "mcrl2/pbes/index_traits.h"

#endif // MCRL2_PBES_PBES_EXPRESSION_H
