// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_expression.h
/// \brief The class pbes_expression.

#define MCRL2_SMART_ARGUMENT_SORTING

#ifndef MCRL2_PBES_PBES_EXPRESSION_H
#define MCRL2_PBES_PBES_EXPRESSION_H

#include <iterator>
#include <stdexcept>
#include "mcrl2/core/down_cast.h"
#include "mcrl2/core/index_traits.h"
#include "mcrl2/core/detail/precedence.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/expression_traits.h"
#include "mcrl2/data/hash.h"
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/pbes/detail/free_variable_visitor.h"
#include "mcrl2/pbes/detail/compare_pbes_expression_visitor.h"
#include "mcrl2/utilities/detail/join.h"
#include "mcrl2/utilities/optimized_boolean_operators.h"

namespace mcrl2
{

namespace pbes_system
{

typedef std::pair<core::identifier_string, data::data_expression_list> propositional_variable_key_type;

using namespace core::detail::precedences;

//--- start generated classes ---//
/// \brief A pbes expression
class pbes_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    pbes_expression()
      : atermpp::aterm_appl(core::detail::constructPBExpr())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit pbes_expression(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_PBExpr(*this));
    }

    /// \brief Constructor.
    pbes_expression(const data::data_expression& x)
      : atermpp::aterm_appl(x)
    {}
};

/// \brief list of pbes_expressions
typedef atermpp::term_list<pbes_expression> pbes_expression_list;

/// \brief vector of pbes_expressions
typedef std::vector<pbes_expression>    pbes_expression_vector;

// prototype declaration
std::string pp(const pbes_expression& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const pbes_expression& x)
{
  return out << pbes_system::pp(x);
}

/// \brief swap overload
inline void swap(pbes_expression& t1, pbes_expression& t2)
{
  t1.swap(t2);
}


/// \brief A propositional variable instantiation
class propositional_variable_instantiation: public pbes_expression
{
  public:


    const core::identifier_string& name() const
    {
      return atermpp::aterm_cast<const core::identifier_string>((*this)[0]);
    }

    const data::data_expression_list& parameters() const
    {
      return atermpp::aterm_cast<const data::data_expression_list>((*this)[1]);
    }
//--- start user section propositional_variable_instantiation ---//
    /// \brief Default constructor.
    propositional_variable_instantiation()
      : pbes_expression(core::detail::constructPropVarInst())
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
      : pbes_expression(core::detail::gsMakePropVarInst(
          name,
          parameters,
          atermpp::aterm_int(core::index_traits<propositional_variable_instantiation, propositional_variable_key_type, 2>::insert(std::make_pair(name, parameters)))
       ))
    {}

    /// \brief Constructor.
    propositional_variable_instantiation(const std::string& name, const data::data_expression_list& parameters)
      : pbes_expression(core::detail::gsMakePropVarInst(
          core::identifier_string(name),
          parameters,
          atermpp::aterm_int(core::index_traits<propositional_variable_instantiation, propositional_variable_key_type, 2>::insert(std::make_pair(name, parameters)))
        ))
    {}

    /// \brief Type of the parameters.
    typedef data::data_expression parameter_type;

    /// \brief Constructor.
    /// \param s A string
    propositional_variable_instantiation(std::string const& s)
    {
      std::pair<std::string, data::data_expression_list> p = data::detail::parse_variable(s);
      core::identifier_string name(p.first);
      data::data_expression_list parameters = atermpp::convert<data::data_expression_list>(p.second);
      copy_term(core::detail::gsMakePropVarInst(
        name,
        parameters,
        atermpp::aterm_int(core::index_traits<propositional_variable_instantiation, propositional_variable_key_type, 2>::insert(std::make_pair(name, parameters)))
      ));
    }
//--- end user section propositional_variable_instantiation ---//
};

/// \brief list of propositional_variable_instantiations
typedef atermpp::term_list<propositional_variable_instantiation> propositional_variable_instantiation_list;

/// \brief vector of propositional_variable_instantiations
typedef std::vector<propositional_variable_instantiation>    propositional_variable_instantiation_vector;

// prototype declaration
std::string pp(const propositional_variable_instantiation& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const propositional_variable_instantiation& x)
{
  return out << pbes_system::pp(x);
}

/// \brief swap overload
inline void swap(propositional_variable_instantiation& t1, propositional_variable_instantiation& t2)
{
  t1.swap(t2);
}


/// \brief The value true for pbes expressions
class true_: public pbes_expression
{
  public:
    /// \brief Default constructor.
    true_()
      : pbes_expression(core::detail::constructPBESTrue())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit true_(const atermpp::aterm& term)
      : pbes_expression(term)
    {
      assert(core::detail::check_term_PBESTrue(*this));
    }
};

// prototype declaration
std::string pp(const true_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const true_& x)
{
  return out << pbes_system::pp(x);
}

/// \brief swap overload
inline void swap(true_& t1, true_& t2)
{
  t1.swap(t2);
}


/// \brief The value false for pbes expressions
class false_: public pbes_expression
{
  public:
    /// \brief Default constructor.
    false_()
      : pbes_expression(core::detail::constructPBESFalse())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit false_(const atermpp::aterm& term)
      : pbes_expression(term)
    {
      assert(core::detail::check_term_PBESFalse(*this));
    }
};

// prototype declaration
std::string pp(const false_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const false_& x)
{
  return out << pbes_system::pp(x);
}

/// \brief swap overload
inline void swap(false_& t1, false_& t2)
{
  t1.swap(t2);
}


/// \brief The not operator for pbes expressions
class not_: public pbes_expression
{
  public:
    /// \brief Default constructor.
    not_()
      : pbes_expression(core::detail::constructPBESNot())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit not_(const atermpp::aterm& term)
      : pbes_expression(term)
    {
      assert(core::detail::check_term_PBESNot(*this));
    }

    /// \brief Constructor.
    not_(const pbes_expression& operand)
      : pbes_expression(core::detail::gsMakePBESNot(operand))
    {}

    const pbes_expression& operand() const
    {
      return atermpp::aterm_cast<const pbes_expression>((*this)[0]);
    }
};

// prototype declaration
std::string pp(const not_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const not_& x)
{
  return out << pbes_system::pp(x);
}

/// \brief swap overload
inline void swap(not_& t1, not_& t2)
{
  t1.swap(t2);
}


/// \brief The and operator for pbes expressions
class and_: public pbes_expression
{
  public:
    /// \brief Default constructor.
    and_()
      : pbes_expression(core::detail::constructPBESAnd())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit and_(const atermpp::aterm& term)
      : pbes_expression(term)
    {
      assert(core::detail::check_term_PBESAnd(*this));
    }

    /// \brief Constructor.
    and_(const pbes_expression& left, const pbes_expression& right)
      : pbes_expression(core::detail::gsMakePBESAnd(left, right))
    {}

    const pbes_expression& left() const
    {
      return atermpp::aterm_cast<const pbes_expression>((*this)[0]);
    }

    const pbes_expression& right() const
    {
      return atermpp::aterm_cast<const pbes_expression>((*this)[1]);
    }
};

// prototype declaration
std::string pp(const and_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const and_& x)
{
  return out << pbes_system::pp(x);
}

/// \brief swap overload
inline void swap(and_& t1, and_& t2)
{
  t1.swap(t2);
}


/// \brief The or operator for pbes expressions
class or_: public pbes_expression
{
  public:
    /// \brief Default constructor.
    or_()
      : pbes_expression(core::detail::constructPBESOr())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit or_(const atermpp::aterm& term)
      : pbes_expression(term)
    {
      assert(core::detail::check_term_PBESOr(*this));
    }

    /// \brief Constructor.
    or_(const pbes_expression& left, const pbes_expression& right)
      : pbes_expression(core::detail::gsMakePBESOr(left, right))
    {}

    const pbes_expression& left() const
    {
      return atermpp::aterm_cast<const pbes_expression>((*this)[0]);
    }

    const pbes_expression& right() const
    {
      return atermpp::aterm_cast<const pbes_expression>((*this)[1]);
    }
};

// prototype declaration
std::string pp(const or_& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const or_& x)
{
  return out << pbes_system::pp(x);
}

/// \brief swap overload
inline void swap(or_& t1, or_& t2)
{
  t1.swap(t2);
}


/// \brief The implication operator for pbes expressions
class imp: public pbes_expression
{
  public:
    /// \brief Default constructor.
    imp()
      : pbes_expression(core::detail::constructPBESImp())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit imp(const atermpp::aterm& term)
      : pbes_expression(term)
    {
      assert(core::detail::check_term_PBESImp(*this));
    }

    /// \brief Constructor.
    imp(const pbes_expression& left, const pbes_expression& right)
      : pbes_expression(core::detail::gsMakePBESImp(left, right))
    {}

    const pbes_expression& left() const
    {
      return atermpp::aterm_cast<const pbes_expression>((*this)[0]);
    }

    const pbes_expression& right() const
    {
      return atermpp::aterm_cast<const pbes_expression>((*this)[1]);
    }
};

// prototype declaration
std::string pp(const imp& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const imp& x)
{
  return out << pbes_system::pp(x);
}

/// \brief swap overload
inline void swap(imp& t1, imp& t2)
{
  t1.swap(t2);
}


/// \brief The universal quantification operator for pbes expressions
class forall: public pbes_expression
{
  public:
    /// \brief Default constructor.
    forall()
      : pbes_expression(core::detail::constructPBESForall())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit forall(const atermpp::aterm& term)
      : pbes_expression(term)
    {
      assert(core::detail::check_term_PBESForall(*this));
    }

    /// \brief Constructor.
    forall(const data::variable_list& variables, const pbes_expression& body)
      : pbes_expression(core::detail::gsMakePBESForall(variables, body))
    {}

    const data::variable_list& variables() const
    {
      return atermpp::aterm_cast<const data::variable_list>((*this)[0]);
    }

    const pbes_expression& body() const
    {
      return atermpp::aterm_cast<const pbes_expression>((*this)[1]);
    }
};

// prototype declaration
std::string pp(const forall& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const forall& x)
{
  return out << pbes_system::pp(x);
}

/// \brief swap overload
inline void swap(forall& t1, forall& t2)
{
  t1.swap(t2);
}


/// \brief The existential quantification operator for pbes expressions
class exists: public pbes_expression
{
  public:
    /// \brief Default constructor.
    exists()
      : pbes_expression(core::detail::constructPBESExists())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit exists(const atermpp::aterm& term)
      : pbes_expression(term)
    {
      assert(core::detail::check_term_PBESExists(*this));
    }

    /// \brief Constructor.
    exists(const data::variable_list& variables, const pbes_expression& body)
      : pbes_expression(core::detail::gsMakePBESExists(variables, body))
    {}

    const data::variable_list& variables() const
    {
      return atermpp::aterm_cast<const data::variable_list>((*this)[0]);
    }

    const pbes_expression& body() const
    {
      return atermpp::aterm_cast<const pbes_expression>((*this)[1]);
    }
};

// prototype declaration
std::string pp(const exists& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const exists& x)
{
  return out << pbes_system::pp(x);
}

/// \brief swap overload
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

// TODO: These should be removed when the aterm code has been replaced.
std::string pp(const atermpp::aterm& x);
std::string pp(const atermpp::aterm_appl& x);

/// \brief Returns true if the term t is equal to true
/// \param t A PBES expression
/// \return True if the term t is equal to true
inline bool is_pbes_true(const pbes_expression& t)
{
  return core::detail::gsIsPBESTrue(t);
}

/// \brief Returns true if the term t is equal to false
/// \param t A PBES expression
/// \return True if the term t is equal to false
inline bool is_pbes_false(const pbes_expression& t)
{
  return core::detail::gsIsPBESFalse(t);
}

/// \brief Returns true if the term t is a not expression
/// \param t A PBES expression
/// \return True if the term t is a not expression
inline bool is_pbes_not(const pbes_expression& t)
{
  return core::detail::gsIsPBESNot(t);
}


/// \brief Returns true if the term t is an and expression
/// \param t A PBES expression
/// \return True if the term t is an and expression
inline bool is_pbes_and(const pbes_expression& t)
{
  return core::detail::gsIsPBESAnd(t);
}

/// \brief Returns true if the term t is an or expression
/// \param t A PBES expression
/// \return True if the term t is an or expression
inline bool is_pbes_or(const pbes_expression& t)
{
  return core::detail::gsIsPBESOr(t);
}

/// \brief Returns true if the term t is an imp expression
/// \param t A PBES expression
/// \return True if the term t is an imp expression
inline bool is_pbes_imp(const pbes_expression& t)
{
  return core::detail::gsIsPBESImp(t);
}

/// \brief Returns true if the term t is a universal quantification
/// \param t A PBES expression
/// \return True if the term t is a universal quantification
inline bool is_pbes_forall(const pbes_expression& t)
{
  return core::detail::gsIsPBESForall(t);
}

/// \brief Returns true if the term t is an existential quantification
/// \param t A PBES expression
/// \return True if the term t is an existential quantification
inline bool is_pbes_exists(const pbes_expression& t)
{
  return core::detail::gsIsPBESExists(t);
}

/// \brief Test for the value true
/// \param t A PBES expression
/// \return True if it is the value \p true
inline bool is_true(const pbes_expression& t)
{
  return is_pbes_true(t) || data::sort_bool::is_true_function_symbol(t);
}

/// \brief Test for the value false
/// \param t A PBES expression
/// \return True if it is the value \p false
inline bool is_false(const pbes_expression& t)
{
  return is_pbes_false(t) || data::sort_bool::is_false_function_symbol(t);
}

/// \brief Test for a negation
/// \param t A PBES expression
/// \return True if it is a negation
inline bool is_not(const pbes_expression& t)
{
  return is_pbes_not(t);
}

/// \brief Test for a conjunction
/// \param t A PBES expression
/// \return True if it is a conjunction
inline bool is_and(const pbes_expression& t)
{
  return is_pbes_and(t);
}


/// \brief Test for a conjunction
/// \param t A PBES expression or a data expression
/// \return True if it is a conjunction
inline bool data_is_and(const pbes_expression& t)
{
  return is_pbes_and(t) || data::sort_bool::is_and_application(t);
}

/// \brief Test for a disjunction
/// \param t A PBES expression
/// \return True if it is a disjunction
inline bool is_or(const pbes_expression& t)
{
  return is_pbes_or(t);
}

/// \brief Test for a disjunction
/// \param t A PBES expression or a data expression
/// \return True if it is a disjunction
inline bool data_is_or(const pbes_expression& t)
{
  return is_pbes_or(t) || data::sort_bool::is_or_application(t);
}

/// \brief Test for an implication
/// \param t A PBES expression
/// \return True if it is an implication
inline bool is_imp(const pbes_expression& t)
{
  return is_pbes_imp(t);
}

/// \brief Test for an universal quantification
/// \param t A PBES expression
/// \return True if it is a universal quantification
inline bool is_forall(const pbes_expression& t)
{
  return is_pbes_forall(t);
}

/// \brief Test for an existential quantification
/// \param t A PBES expression
/// \return True if it is an existential quantification
inline bool is_exists(const pbes_expression& t)
{
  return is_pbes_exists(t);
}

/// \brief Returns true if the term t is a data expression
/// \param t A PBES expression
/// \return True if the term t is a data expression
inline bool is_data(const pbes_expression& t)
{
  return data::is_data_expression(t);
}

/// \brief Returns true if the term t is a propositional variable expression
/// \param t A PBES expression
/// \return True if the term t is a propositional variable expression
inline bool is_propositional_variable_instantiation(const pbes_expression& t)
{
  return core::detail::gsIsPropVarInst(t);
}

// From the documentation:
// The "!" operator has the highest priority, followed by "&&" and "||", followed by "=>", followed by "forall" and "exists".
// The infix operators "&&", "||" and "=>" associate to the right.
/// \brief Returns the precedence of pbes expressions
inline
int precedence(const pbes_expression& x)
{
  if (is_forall(x) || is_exists(x))
  {
    return 0;
  }
  else if (is_imp(x))
  {
    return 2;
  }
  else if (is_or(x))
  {
    return 3;
  }
  else if (is_and(x))
  {
    return 4;
  }
  else if (is_not(x))
  {
    return 5;
  }
  return core::detail::precedences::max_precedence;
}

// TODO: is there a cleaner way to make the precedence function work for derived classes like and_ ?
inline int precedence(const forall & x)  { return precedence(static_cast<const pbes_expression&>(x)); }
inline int precedence(const exists & x)  { return precedence(static_cast<const pbes_expression&>(x)); }
inline int precedence(const imp& x)      { return precedence(static_cast<const pbes_expression&>(x)); }
inline int precedence(const and_& x)     { return precedence(static_cast<const pbes_expression&>(x)); }
inline int precedence(const or_& x)      { return precedence(static_cast<const pbes_expression&>(x)); }
inline int precedence(const not_& x)     { return precedence(static_cast<const pbes_expression&>(x)); }
inline int precedence(const data::data_expression & x)               { return precedence(static_cast<const pbes_expression&>(x)); }
inline int precedence(const propositional_variable_instantiation& x) { return precedence(static_cast<const pbes_expression&>(x)); }

inline const pbes_expression& unary_operand(const not_& x) { return x.operand(); }
inline const pbes_expression& binary_left(const and_& x)   { return x.left(); }
inline const pbes_expression& binary_right(const and_& x)  { return x.right(); }
inline const pbes_expression& binary_left(const or_& x)    { return x.left(); }
inline const pbes_expression& binary_right(const or_& x)   { return x.right(); }
inline const pbes_expression& binary_left(const imp& x)    { return x.left(); }
inline const pbes_expression& binary_right(const imp& x)   { return x.right(); }

/// \brief Returns true if the operations have the same precedence, but are different
template <typename T1, typename T2>
bool is_same_different_precedence(const T1&, const T2&)
{
  return false;
}

/// \brief Returns true if the operations have the same precedence, but are different
inline
bool is_same_different_precedence(const and_&, const pbes_expression& x)
{
  return is_or(x);
}

/// \brief Returns true if the operations have the same precedence, but are different
inline
bool is_same_different_precedence(const or_&, const pbes_expression& x)
{
  return is_and(x);
}

/// \brief The namespace for accessor functions on pbes expressions.
namespace accessors
{

/// \brief Conversion of a pbes expression to a data expression.
/// \pre The pbes expression must be of the form val(d) for some data variable d.
/// \param t A PBES expression
/// \return The converted expression
inline
data::data_expression val(const pbes_expression& t)
{
  assert(data::is_data_expression(t));
  return data::data_expression(atermpp::aterm_appl(t));
}

/// \brief Returns the pbes expression argument of expressions of type not, exists and forall.
/// \param t A PBES expression
/// \return The pbes expression argument of expressions of type not, exists and forall.
inline
const pbes_expression& arg(const pbes_expression& t)
{
  if (is_pbes_not(t))
  {
    return atermpp::aterm_cast<const pbes_expression>(t[0]);
  }
  else
  {
    assert(is_forall(t) || is_exists(t));
    return atermpp::aterm_cast<const pbes_expression>(t[1]);
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
    const data::application &a=atermpp::aterm_cast<const data::application>(t);
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
  return core::static_down_cast<const pbes_expression&>(t[0]);
}

/// \brief Returns the left hand side of an expression of type and, or or imp.
/// \param x A PBES expression or a data expression
/// \return The left hand side of an expression of type and, or or imp.
inline
pbes_expression data_left(const pbes_expression& x)
{
  if (data::is_data_expression(x))
  {
    return data::binary_left(atermpp::aterm_cast<data::application>(x));
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
  return core::static_down_cast<const pbes_expression&>(t[1]);
}

/// \brief Returns the left hand side of an expression of type and, or or imp.
/// \param x A PBES expression or a data expression
/// \return The left hand side of an expression of type and, or or imp.
inline
pbes_expression data_right(const pbes_expression& x)
{
  if (data::is_data_expression(x))
  {
    return data::binary_right(atermpp::aterm_cast<data::application>(x));
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
  return atermpp::aterm_cast<data::variable_list>(t[0]);
}

/// \brief Returns the name of a propositional variable expression
/// \param t A PBES expression
/// \return The name of a propositional variable expression
inline
const core::identifier_string& name(const pbes_expression& t)
{
  assert(is_propositional_variable_instantiation(t));
  return atermpp::aterm_cast<core::identifier_string>(t[0]);
}

/// \brief Returns the parameters of a propositional variable instantiation.
/// \param t A PBES expression
/// \return The parameters of a propositional variable instantiation.
inline
const data::data_expression_list& param(const pbes_expression& t)
{
  assert(is_propositional_variable_instantiation(t));
  return core::static_down_cast<const data::data_expression_list&>(t[1]);
}
} // accessors

/// Accessor functions and predicates for pbes expressions.
namespace pbes_expr
{

/// \brief Make the value true
/// \return The value \p true
inline
pbes_expression true_()
{
  return pbes_expression(core::detail::gsMakePBESTrue());
}

/// \brief Make the value false
/// \return The value \p false
inline
pbes_expression false_()
{
  return pbes_expression(core::detail::gsMakePBESFalse());
}

/// \brief Make a negation
/// \param p A PBES expression
/// \return The value <tt>!p</tt>
inline
pbes_expression not_(const pbes_expression& p)
{
  return pbes_expression(core::detail::gsMakePBESNot(p));
}

/// \brief Make a conjunction
/// \param p A PBES expression
/// \param q A PBES expression
/// \return The value <tt>p && q</tt>
inline
pbes_expression and_(const pbes_expression& p, const pbes_expression& q)
{
  return pbes_expression(core::detail::gsMakePBESAnd(p,q));
}

/// \brief Make a disjunction
/// \param p A PBES expression
/// \param q A PBES expression
/// \return The value <tt>p || q</tt>
inline
pbes_expression or_(const pbes_expression& p, const pbes_expression& q)
{
  return pbes_expression(core::detail::gsMakePBESOr(p,q));
}

/// \brief Make an implication
/// \param p A PBES expression
/// \param q A PBES expression
/// \return The value <tt>p => q</tt>
inline
pbes_expression imp(const pbes_expression& p, const pbes_expression& q)
{
  return pbes_expression(core::detail::gsMakePBESImp(p,q));
}

/// \brief Make a universal quantification
/// \param l A sequence of data variables
/// \param p A PBES expression
/// \return The value <tt>forall l.p</tt>
inline
pbes_expression forall(const data::variable_list& l, const pbes_expression& p)
{
  if (l.empty())
  {
    return p;
  }
  return pbes_expression(core::detail::gsMakePBESForall(l, p));
}

/// \brief Make an existential quantification
/// \param l A sequence of data variables
/// \param p A PBES expression
/// \return The value <tt>exists l.p</tt>
inline
pbes_expression exists(const data::variable_list& l, const pbes_expression& p)
{
  if (l.empty())
  {
    return p;
  }
  return pbes_expression(core::detail::gsMakePBESExists(l, p));
}

/// \brief Returns or applied to the sequence of pbes expressions [first, last)
/// \param first Start of a sequence of pbes expressions
/// \param last End of a sequence of of pbes expressions
/// \return Or applied to the sequence of pbes expressions [first, last)
template <typename FwdIt>
pbes_expression join_or(FwdIt first, FwdIt last)
{
  return utilities::detail::join(first, last, or_, false_());
}

/// \brief Returns and applied to the sequence of pbes expressions [first, last)
/// \param first Start of a sequence of pbes expressions
/// \param last End of a sequence of of pbes expressions
/// \return And applied to the sequence of pbes expressions [first, last)
template <typename FwdIt>
pbes_expression join_and(FwdIt first, FwdIt last)
{
  return utilities::detail::join(first, last, and_, true_());
}

/// \brief Splits a disjunction into a sequence of operands
/// Given a pbes expression of the form p1 || p2 || .... || pn, this will yield a
/// set of the form { p1, p2, ..., pn }, assuming that pi does not have a || as main
/// function symbol.
/// \param expr A PBES expression
/// \param split_data_expressions if true, both data and pbes disjunctions are
///        split, otherwise only pbes disjunctions are split.
/// \return A sequence of operands
inline
std::set<pbes_expression> split_or(const pbes_expression& expr, bool split_data_expressions = false)
{
  using namespace accessors;
  std::set<pbes_expression> result;

  if (split_data_expressions)
  {
    utilities::detail::split(expr, std::insert_iterator<std::set<pbes_expression> >(result, result.begin()), data_is_or, data_left, data_right);
  }
  else
  {
    utilities::detail::split(expr, std::insert_iterator<std::set<pbes_expression> >(result, result.begin()), is_or, left, right);
  }

  return result;
}

/// \brief Splits a conjunction into a sequence of operands
/// Given a pbes expression of the form p1 && p2 && .... && pn, this will yield a
/// set of the form { p1, p2, ..., pn }, assuming that pi does not have a && as main
/// function symbol.
/// \param expr A PBES expression
/// \param split_data_expressions if true, both data and pbes conjunctions are
///        split, otherwise only pbes conjunctions are split.
/// \return A sequence of operands
inline
std::set<pbes_expression> split_and(const pbes_expression& expr, bool split_data_expressions = false)
{
  using namespace accessors;
  std::set<pbes_expression> result;

  if (split_data_expressions)
  {
    utilities::detail::split(expr, std::insert_iterator<std::set<pbes_expression> >(result, result.begin()), data_is_and, data_left, data_right);
  }
  else
  {
    utilities::detail::split(expr, std::insert_iterator<std::set<pbes_expression> >(result, result.begin()), is_and, left, right);
  }

  return result;
}
} // namespace pbes_expr

namespace pbes_expr_optimized
{
using pbes_expr::true_;
using pbes_expr::false_;
using pbes_expr::split_and;
using pbes_expr::split_or;

/// \brief Make a negation
/// \param p A PBES expression
/// \return The value <tt>!p</tt>
inline
pbes_expression not_(const pbes_expression& p)
{
  return utilities::optimized_not(p);
}

/// \brief Make a conjunction
/// \param p A PBES expression
/// \param q A PBES expression
/// \return The value <tt>p && q</tt>
inline
pbes_expression and_(const pbes_expression& p, const pbes_expression& q)
{
  return utilities::optimized_and(p, q);
}

/// \brief Make a disjunction
/// \param p A PBES expression
/// \param q A PBES expression
/// \return The value <tt>p || q</tt>
inline
pbes_expression or_(const pbes_expression& p, const pbes_expression& q)
{
  return utilities::optimized_or(p, q);
}

/// \brief Make an implication
/// \param p A PBES expression
/// \param q A PBES expression
/// \return The value <tt>p => q</tt>
inline
pbes_expression imp(const pbes_expression& p, const pbes_expression& q)
{
  return utilities::optimized_imp(p, q);
}

/// \brief Returns or applied to the sequence of pbes expressions [first, last)
/// \param first Start of a sequence of pbes expressions
/// \param last End of a sequence of pbes expressions
/// \return Or applied to the sequence of pbes expressions [first, last)
template <typename FwdIt>
inline pbes_expression join_or(FwdIt first, FwdIt last)
{
  return utilities::detail::join(first, last, or_, false_());
}

/// \brief Returns and applied to the sequence of pbes expressions [first, last)
/// \param first Start of a sequence of pbes expressions
/// \param last End of a sequence of pbes expressions
/// \return And applied to the sequence of pbes expressions [first, last)
template <typename FwdIt>
inline pbes_expression join_and(FwdIt first, FwdIt last)
{
  return utilities::detail::join(first, last, and_, true_());
}

/// \brief Make a universal quantification
/// If l is empty, p is returned.
/// \param l A sequence of data variables
/// \param p A PBES expression
/// \return The value <tt>forall l.p</tt>
inline
pbes_expression forall(const data::variable_list& l, const pbes_expression& p)
{
  if (l.empty())
  {
    return p;
  }
  if (is_false(p))
  {
    // N.B. Here we use the fact that mCRL2 data types are never empty.
    return data::sort_bool::false_();
  }
  if (is_true(p))
  {
    return true_();
  }
  return pbes_expr::forall(l, p);
}

/// \brief Make an existential quantification
/// If l is empty, p is returned.
/// \param l A sequence of data variables
/// \param p A PBES expression
/// \return The value <tt>exists l.p</tt>
inline
pbes_expression exists(const data::variable_list& l, const pbes_expression& p)
{
  if (l.empty())
  {
    return p;
  }
  if (is_false(p))
  {
    return data::sort_bool::false_();
  }
  if (is_true(p))
  {
    // N.B. Here we use the fact that mCRL2 data types are never empty.
    return data::sort_bool::true_();
  }
  return pbes_expr::exists(l, p);
}

} // namespace pbes_expr_optimized

/// \brief The namespace for access functions that operate on both pbes and data expressions
// TODO: unfinished!
namespace combined_access
{
/// \brief Test for the value true
/// \param t A PBES expression
/// \return True if it is the value \p true
inline bool is_true(const pbes_expression& t)
{
  return is_pbes_true(t) || data::sort_bool::is_true_function_symbol(t);
}

/// \brief Test for the value false
/// \param t A PBES expression
/// \return True if it is the value \p false
inline bool is_false(const pbes_expression& t)
{
  return is_pbes_false(t) || data::sort_bool::is_false_function_symbol(t);
}

/// \brief Test for a negation
/// \param t A PBES expression
/// \return True if it is a negation
inline bool is_not(const pbes_expression& t)
{
  return is_pbes_not(t) || data::sort_bool::is_not_application(t);
}

/// \brief Test for a conjunction
/// \param t A PBES expression
/// \return True if it is a conjunction
inline bool is_and(const pbes_expression& t)
{
  return is_pbes_and(t) || data::sort_bool::is_and_application(t);
}

/// \brief Test for a disjunction
/// \param t A PBES expression
/// \return True if it is a disjunction
inline bool is_or(const pbes_expression& t)
{
  return is_pbes_or(t) || data::sort_bool::is_or_application(t);
}

/// \brief Test for an implication
/// \param t A PBES expression
/// \return True if it is an implication
inline bool is_imp(const pbes_expression& t)
{
  return is_pbes_imp(t);
}

/// \brief Test for an universal quantification
/// \param t A PBES expression
/// \return True if it is a universal quantification
inline bool is_forall(const pbes_expression& t)
{
  return is_pbes_forall(t);
}

/// \brief Test for an existential quantification
/// \param t A PBES expression
/// \return True if it is an existential quantification
inline bool is_exists(const pbes_expression& t)
{
  return is_pbes_exists(t);
}

/// \brief Returns true if the term t is a propositional variable expression
/// \param t A PBES expression
/// \return True if the term t is a propositional variable expression
inline bool is_propositional_variable_instantiation(const pbes_expression& t)
{
  return core::detail::gsIsPropVarInst(t);
}

/// \brief Returns the left hand side of an expression of type and, or or imp.
/// \param t A PBES expression or a data expression
/// \return The left hand side of an expression of type and, or or imp.
inline
pbes_expression left(const pbes_expression& t)
{
  return accessors::data_left(t);
}

/// \brief Returns the left hand side of an expression of type and, or or imp.
/// \param t A PBES expression or a data expression
/// \return The left hand side of an expression of type and, or or imp.
inline
pbes_expression right(const pbes_expression& t)
{
  return accessors::data_right(t);
}

}; // namespace pbes_data

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
    return term_type(core::detail::gsMakePBESTrue());
  }

  /// \brief Make the value false
  /// \return The value \p false
  static inline
  term_type false_()
  {
    return term_type(core::detail::gsMakePBESFalse());
  }

  /// \brief Make a negation
  /// \param p A term
  /// \return The value <tt>!p</tt>
  static inline
  term_type not_(const term_type& p)
  {
    return term_type(core::detail::gsMakePBESNot(p));
  }

  static inline
  bool is_sorted(const term_type& p, const term_type& q)
  {
    pbes_system::detail::compare_pbes_expression_visitor<term_type> pvisitor;
    pvisitor.visit(p);
    pbes_system::detail::compare_pbes_expression_visitor<term_type> qvisitor;
    qvisitor.visit(q);
    if (pvisitor.has_predicate_variables != qvisitor.has_predicate_variables)
    {
      return qvisitor.has_predicate_variables;
    }
    if (pvisitor.has_quantifiers != qvisitor.has_quantifiers)
    {
      return qvisitor.has_quantifiers;
    }
    if (pvisitor.result.size() != qvisitor.result.size())
    {
      return pvisitor.result.size() < qvisitor.result.size();
    }
    return p < q;
  }

  /// \brief Make a conjunction
  /// \param p A term
  /// \param q A term
  /// \return The value <tt>p && q</tt>
  static inline
  term_type and_(const term_type& p, const term_type& q)
  {
    return term_type(core::detail::gsMakePBESAnd(p,q));
  }

  /// \brief Make a disjunction
  /// \param p A term
  /// \param q A term
  /// \return The value <tt>p || q</tt>
  static inline
  term_type or_(const term_type& p, const term_type& q)
  {
    return term_type(core::detail::gsMakePBESOr(p,q));
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

  /// \brief Make a sorted conjunction
  /// \param p A term
  /// \param q A term
  /// \return The value <tt>p && q</tt>, or <tt>q && p</tt>
  static inline
  term_type sorted_and(const term_type& p, const term_type& q)
  {
#ifdef MCRL2_SMART_ARGUMENT_SORTING
    bool sorted = is_sorted(p, q);
#else
    bool sorted = p < q;
#endif
    return sorted ? term_type(core::detail::gsMakePBESAnd(p,q)) : term_type(core::detail::gsMakePBESAnd(q,p));
  }

  /// \brief Make a sorted disjunction
  /// \param p A term
  /// \param q A term
  /// \return The value <tt>p || q</tt>
  static inline
  term_type sorted_or(const term_type& p, const term_type& q)
  {
#ifdef MCRL2_SMART_ARGUMENT_SORTING
    bool sorted = is_sorted(p, q);
#else
    bool sorted = p < q;
#endif
    return sorted ? term_type(core::detail::gsMakePBESOr(p,q)) : term_type(core::detail::gsMakePBESOr(q,p));
  }

  /// \brief Make an implication
  /// \param p A term
  /// \param q A term
  /// \return The value <tt>p => q</tt>
  static inline
  term_type imp(const term_type& p, const term_type& q)
  {
    return term_type(core::detail::gsMakePBESImp(p,q));
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
    return term_type(core::detail::gsMakePBESForall(l, p));
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
    return term_type(core::detail::gsMakePBESExists(l, p));
  }

  /// \brief Propositional variable instantiation
  /// \param name A string
  /// \param first Start of a sequence of data terms
  /// \param last End of a sequence of data terms
  /// \return Propositional variable instantiation with the given name, and the range [first, last) as data parameters
  template <typename Iter>
  static
  term_type prop_var(const string_type& name, Iter first, Iter last)
  {
    return propositional_variable_type(name, data_term_sequence_type(first, last));
  }

  /// \brief Test for the value true
  /// \param t A term
  /// \return True if it is the value \p true
  static inline
  bool is_true(const term_type& t)
  {
    return core::detail::gsIsPBESTrue(t) || data::sort_bool::is_true_function_symbol(t);
  }

  /// \brief Test for the value false
  /// \param t A term
  /// \return True if it is the value \p false
  static inline
  bool is_false(const term_type& t)
  {
    return core::detail::gsIsPBESFalse(t) || data::sort_bool::is_false_function_symbol(t);
  }

  /// \brief Test for a negation
  /// \param t A term
  /// \return True if it is a negation
  static inline
  bool is_not(const term_type& t)
  {
    return core::detail::gsIsPBESNot(t);
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
    return core::detail::gsIsPropVarInst(t);
  }

  /// \brief Returns the argument of a term of type not, exists or forall
  /// \param t A term
  /// \return The requested argument. Partially works for data terms
  static inline
  term_type arg(const term_type& t)
  {
    return pbes_system::accessors::arg(t);
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
    return core::static_down_cast<const term_type&>(t[0]);
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

    return atermpp::aterm_cast<variable_sequence_type>(t[0]);
  }

  /// \brief Returns the name of a propositional variable instantiation
  /// \param t A term
  /// \return The name of the propositional variable instantiation
  static inline
  const string_type &name(const term_type& t)
  {
    assert(is_prop_var(t));
    return atermpp::aterm_cast<string_type>(t[0]);
  }

  /// \brief Returns the parameter list of a propositional variable instantiation
  /// \param t A term
  /// \return The parameter list of the propositional variable instantiation
  static inline
  const data_term_sequence_type &param(const term_type& t)
  {
    assert(is_prop_var(t));
    return atermpp::aterm_cast<data_term_sequence_type>(t[1]);
  }

  /// \brief Conversion from variable to term
  /// \param v A variable
  /// \return The converted variable
  static inline
  const term_type &variable2term(const variable_type& v)
  {
    return atermpp::aterm_cast<term_type>(v);
  }

  /// \brief Test if a term is a variable
  /// \param t A term
  /// \return True if the term is a variable
  static inline
  bool is_variable(const term_type& t)
  {
    return data::is_variable(t);
  }

  /// \brief Returns the free variables of a term
  /// \param t A term
  /// \return The free variables of a term
  static inline
  variable_sequence_type free_variables(const term_type& t)
  {
    pbes_system::detail::free_variable_visitor<term_type> visitor;
    visitor.visit(t);
    return variable_sequence_type(visitor.result.begin(), visitor.result.end());
  }

  /// \brief Conversion from data term to term
  /// \param t A data term
  /// \return The converted term
  static inline
  term_type dataterm2term(const data_term_type& t)
  {
    return t;
  }

  /// \brief Conversion from term to data term
  /// \param t A term
  /// \return The converted term
  static inline
  data_term_type term2dataterm(const term_type& t)
  {
    return data_term_type(t);
  }

  /// \brief Conversion from term to propositional variable instantiation
  /// \param t A term
  /// \return The converted term
  static inline
  const propositional_variable_type& term2propvar(const term_type& t)
  {
    return core::static_down_cast<const propositional_variable_type&>(t);
  }

  /// \brief Returns the difference of two unordered sets of variables
  /// \param v A sequence of data variables
  /// \param w A sequence of data variables
  /// \return The difference of two sets.
  static inline
  variable_sequence_type set_intersection(const variable_sequence_type& v, const variable_sequence_type& w)
  {
    return term_traits<data::data_expression>::set_intersection(v, w);
  }

  /// \brief Test if a term is constant
  /// \return True if the term is constant
  static inline
  bool is_constant(const term_type& /* t */)
  {
    return false;
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

#ifdef MCRL2_USE_INDEX_TRAITS
#include "mcrl2/pbes/index_traits.h"
#endif // MCRL2_USE_INDEX_TRAITS

#endif // MCRL2_PBES_PBES_EXPRESSION_H
