// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_expression_with_variables.h
/// \brief A pbes expression with an associated sequence of data variables.

#ifndef MCRL2_PBES_PBES_EXPRESSION_WITH_VARIABLES_H
#define MCRL2_PBES_PBES_EXPRESSION_WITH_VARIABLES_H

#include <algorithm>
#include <set>
#include "mcrl2/atermpp/set_operations.h"
#include "mcrl2/utilities/detail/join.h"
#include "mcrl2/data/detail/data_expression_with_variables.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/pbes_expression_with_variables.h"

namespace mcrl2
{

namespace pbes_system
{

/// \brief A pbes expression that stores a list of variables. The
/// intended use case is to store the free variables of the
/// expression.
class pbes_expression_with_variables: public pbes_expression
{
  protected:
    /// \brief The sequence of variables.
    data::variable_list m_variables;

  public:
    /// \brief Constructor.
    pbes_expression_with_variables()
    {}

    /// \brief Constructor. Creates a data expression with an empty sequence of variables.
    /// \param term A term
    pbes_expression_with_variables(const atermpp::aterm_appl& term)
      : pbes_expression(term)
    {}

    /// \brief Constructor.
    /// \param expression A PBES expression
    /// \param variables A sequence of data variables
    pbes_expression_with_variables(const pbes_expression& expression, const data::variable_list& variables)
      : pbes_expression(expression), m_variables(variables)
    {}

    /// \brief Returns the variables
    /// \return The variables
    const data::variable_list& variables() const
    {
      return m_variables;
    }

    /// \brief Returns the variables
    /// \return The variables
    data::variable_list& variables()
    {
      return m_variables;
    }

    /// \brief Returns true if the expression has no free variables.
    /// \return True if the expression has no free variables.
    bool is_closed() const
    {
      return m_variables.empty();
    }
};

} // namespace pbes_system

} // namespace mcrl2

namespace mcrl2
{

namespace core
{

/// \brief Contains type information for pbes expressions with variables.
template <>
struct term_traits<pbes_system::pbes_expression_with_variables>
{
  /// \brief The term type
  typedef pbes_system::pbes_expression_with_variables term_type;

  /// \brief The data term type
  typedef data::data_expression_with_variables data_term_type;

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

  /// \cond INTERNAL_DOCS
  typedef core::term_traits<pbes_system::pbes_expression> tr;
  /// \endcond

  /// \brief The value true
  /// \return The value true
  static inline
  term_type true_()
  {
    return tr::true_();
  }

  /// \brief The value false
  /// \return The value false
  static inline
  term_type false_()
  {
    return tr::false_();
  }

  /// \brief Operator not
  /// \param p A term
  /// \return Operator not applied to p
  static inline
  term_type not_(term_type p)
  {
    return term_type(tr::not_(p), p.variables());
  }

  /// \brief Make a conjunction
  /// \param p A term
  /// \param q A term
  /// \return The value <tt>p && q</tt>
  static inline
  term_type and_(term_type p, term_type q)
  {
    return term_type(tr::and_(p, q), atermpp::term_list_union(p.variables(), q.variables()));
  }

  /// \brief Make a disjunction
  /// \param p A term
  /// \param q A term
  /// \return The value <tt>p || q</tt>
  static inline
  term_type or_(term_type p, term_type q)
  {
    return term_type(tr::or_(p, q), atermpp::term_list_union(p.variables(), q.variables()));
  }

  /// \brief Make a sorted conjunction
  /// \param p A term
  /// \param q A term
  /// \return The value <tt>p && q</tt>
  static inline
  term_type sorted_and(term_type p, term_type q)
  {
    return term_type(tr::sorted_and(p, q), atermpp::term_list_union(p.variables(), q.variables()));
  }

  /// \brief Make a sorted disjunction
  /// \param p A term
  /// \param q A term
  /// \return The value <tt>p || q</tt>
  static inline
  term_type sorted_or(term_type p, term_type q)
  {
    return term_type(tr::sorted_or(p, q), atermpp::term_list_union(p.variables(), q.variables()));
  }

  /// \brief Make an implication
  /// \param p A term
  /// \param q A term
  /// \return The value <tt>p => q</tt>
  static inline
  term_type imp(term_type p, term_type q)
  {
    return term_type(tr::imp(p, q), atermpp::term_list_union(p.variables(), q.variables()));
  }

  /// \brief Make a universal quantification
  /// \param l A sequence of variables
  /// \param p A term
  /// \return The value <tt>forall l.p</tt>
  static inline
  term_type forall(variable_sequence_type const& l, term_type p)
  {
    return term_type(tr::forall(l, p), atermpp::term_list_difference(p.variables(), l));
  }

  /// \brief Make an existential quantification
  /// \param l A sequence of variables
  /// \param p A term
  /// \return The value <tt>exists l.p</tt>
  static inline
  term_type exists(variable_sequence_type const& l, term_type p)
  {
    return term_type(tr::exists(l, p), atermpp::term_list_difference(p.variables(), l));
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
    std::set<data_term_type> v;
    for (Iter i = first; i != last; ++i)
    {
      v.insert(i->variables().begin(), i->variables().end());
    }
    return term_type(tr::prop_var(name, first, last), variable_sequence_type(v.begin(), v.end()));
  }

  /// \brief Test for value true
  /// \param t A term
  /// \return True if the term has the value true. Also works for data terms
  static inline
  bool is_true(const term_type& t)
  {
    return tr::is_true(t);
  }

  /// \brief Test for value false
  /// \param t A term
  /// \return True if the term has the value false. Also works for data terms
  static inline
  bool is_false(const term_type& t)
  {
    return tr::is_false(t);
  }

  /// \brief Test for operator not
  /// \param t A term
  /// \return True if the term is of type and. Also works for data terms
  static inline
  bool is_not(const term_type& t)
  {
    return tr::is_not(t);
  }

  /// \brief Test for operator and
  /// \param t A term
  /// \return True if the term is of type and. Also works for data terms
  static inline
  bool is_and(const term_type& t)
  {
    return tr::is_and(t);
  }

  /// \brief Test for operator or
  /// \param t A term
  /// \return True if the term is of type or. Also works for data terms
  static inline
  bool is_or(const term_type& t)
  {
    return tr::is_or(t);
  }

  /// \brief Test for implication
  /// \param t A term
  /// \return True if the term is an implication. Also works for data terms
  static inline
  bool is_imp(const term_type& t)
  {
    return tr::is_imp(t);
  }

  /// \brief Test for universal quantification
  /// \param t A term
  /// \return True if the term is an universal quantification. Also works for data terms
  static inline
  bool is_forall(const term_type& t)
  {
    return tr::is_forall(t);
  }

  /// \brief Test for existential quantification
  /// \param t A term
  /// \return True if the term is an existential quantification. Also works for data terms
  static inline
  bool is_exists(const term_type& t)
  {
    return tr::is_exists(t);
  }

  /// \brief Test for data term
  /// \param t A term
  /// \return True if the term is a data term
  static inline
  bool is_data(const term_type& t)
  {
    return tr::is_data(t);
  }

  /// \brief Test for propositional variable instantiation
  /// \param t A term
  /// \return True if the term is a propositional variable instantiation
  static inline
  bool is_prop_var(const term_type& t)
  {
    return tr::is_prop_var(t);
  }

  /// \brief Returns the argument of a term of type not, exists or forall
  /// \param t A term
  /// \return The requested argument. Partially works for data terms
  static inline
  term_type arg(const term_type& t)
  {
    return tr::arg(t);
  }

  /// \brief Returns the right argument of a term of type and, or or imp
  /// \param t A term
  /// \return The right argument of the term. Also works for data terms
  static inline
  term_type left(const term_type& t)
  {
    return tr::left(t);
  }

  /// \brief Returns the right argument of a term of type and, or or imp
  /// \param t A term
  /// \return The right argument of the term. Also works for data terms
  static inline
  term_type right(const term_type& t)
  {
    return tr::right(t);
  }

  /// \brief Returns the argument of a term of type not
  /// \param t A term
  static inline
  term_type not_arg(const term_type& t)
  {
    return tr::not_arg(t);
  }

  /// \brief Returns the quantifier variables of a quantifier expression
  /// \param t A term
  /// \return The requested argument. Doesn't work for data terms
  static inline
  variable_sequence_type var(const term_type& t)
  {
    return tr::var(t);
  }

  /// \brief Returns the name of a propositional variable instantiation
  /// \param t A term
  /// \return The name of the propositional variable instantiation
  static inline
  string_type name(const term_type& t)
  {
    return tr::name(t);
  }

  /// \brief Returns the parameter list of a propositional variable instantiation
  /// \param t A term
  /// \return The parameter list of the propositional variable instantiation
  static inline
  data_term_sequence_type param(const term_type& t)
  {
    return tr::param(t);
  }

  /// \brief Conversion from variable to term
  /// \param v A variable
  /// \return The converted variable
  static inline
  term_type variable2term(variable_type v)
  {
    return tr::variable2term(v);
  }

  /// \brief Test if a term is a variable
  /// \param t A term
  /// \return True if the term is a variable
  static inline
  bool is_variable(const term_type& t)
  {
    return tr::is_variable(t);
  }

  /// \brief Returns the free variables of a term
  /// \param t A term
  /// \return The free variables of a term
  static inline
  variable_sequence_type free_variables(const term_type& t)
  {
    return t.variables();
  }

  /// \brief Conversion from data term to term
  /// \param t A data term
  /// \return The converted term
  static inline
  term_type dataterm2term(const data_term_type& t)
  {
    return term_type(t, t.variables());
  }

  /// \brief Conversion from term to data term
  /// \param t A term
  /// \returns The converted term
  static inline
  data_term_type term2dataterm(const term_type& t)
  {
    return tr::term2dataterm(t);
  }

  /// \brief Conversion from term to propositional variable instantiation
  /// \param t A term
  /// \return The converted term
  static inline
  const propositional_variable_type& term2propvar(const term_type& t)
  {
    return core::static_down_cast<const propositional_variable_type&>(static_cast<const pbes_system::pbes_expression&>(t));
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
  /// \param t A term
  /// \return True if the term is constant
  static inline
  bool is_constant(const term_type& t)
  {
    return t.variables().empty();
  }

  /// \brief Pretty print function
  /// \param t A term
  /// \return A pretty print representation of the term
  static inline
  std::string pp(const term_type& t)
  {
    return pbes_system::pp(t) + " variables: " + data::pp(t.variables());
  }
};

} // namespace core

} // namespace mcrl2

#endif // MCRL2_PBES_PBES_EXPRESSION_WITH_VARIABLES_H
