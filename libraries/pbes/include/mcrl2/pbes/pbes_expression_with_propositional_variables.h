// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes_expression_with_propositional_variables.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBES_EXPRESSION_WITH_PROPOSITIONAL_VARIABLES_H
#define MCRL2_PBES_PBES_EXPRESSION_WITH_PROPOSITIONAL_VARIABLES_H

#include "mcrl2/pbes/pbes_expression_with_variables.h"
#include "mcrl2/atermpp/make_list.h"

namespace mcrl2 {

namespace pbes_system {

  /// \brief A pbes expression that stores a sequence of data variables and a sequence of propositional
  /// variable instantiations.
  class pbes_expression_with_propositional_variables: public pbes_expression_with_variables
  {
    protected:
      propositional_variable_instantiation_list m_propositional_variables;

    public:
      /// \brief Constructor.
      pbes_expression_with_propositional_variables()
      {}

      /// \brief Constructor. Creates a pbes expression with empty sequences of data and propositional variables.
      /// \param term A term
      pbes_expression_with_propositional_variables(atermpp::aterm_appl term)
        : pbes_expression_with_variables(term)
      {}

      /// \brief Constructor. Creates a pbes expression with empty sequences of data and propositional variables.
      /// \param term A term
      pbes_expression_with_propositional_variables(ATermAppl term)
        : pbes_expression_with_variables(term)
      {}

      /// \brief Constructor.
      pbes_expression_with_propositional_variables(pbes_expression expression,
                                                   data::data_variable_list variables,
                                                   propositional_variable_instantiation_list propositional_variables = propositional_variable_instantiation_list())
        : pbes_expression_with_variables(expression, variables), m_propositional_variables(propositional_variables)
      {}

      /// \brief Constructor. Creates a pbes expression with an empty sequences of propositional variables.
      pbes_expression_with_propositional_variables(pbes_expression_with_variables expression,
                                                   propositional_variable_instantiation_list propositional_variables)
        : pbes_expression_with_variables(expression), m_propositional_variables(propositional_variables)
      {}

      /// \brief Returns the propositional variables.
      /// \return The propositional variables.
      propositional_variable_instantiation_list propositional_variables() const
      {
        return m_propositional_variables;
      }

      /// \brief Returns the propositional variables.
      /// \return The propositional variables.
      propositional_variable_instantiation_list& propositional_variables()
      {
        return m_propositional_variables;
      }
  };

} // namespace pbes_system

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp {
template<>
struct aterm_traits<mcrl2::pbes_system::pbes_expression_with_propositional_variables >
{
  typedef ATermAppl aterm_type;
  static void protect(mcrl2::pbes_system::pbes_expression_with_propositional_variables t)   { t.protect(); t.variables().protect(); t.propositional_variables().protect(); }
  static void unprotect(mcrl2::pbes_system::pbes_expression_with_propositional_variables t) { t.unprotect(); t.variables().unprotect(); t.propositional_variables().unprotect(); }
  static void mark(mcrl2::pbes_system::pbes_expression_with_propositional_variables t)      { t.mark(); t.variables().mark(); t.propositional_variables().mark(); }
  static ATerm term(mcrl2::pbes_system::pbes_expression_with_propositional_variables t)     { return t.term(); }
  static ATerm* ptr(mcrl2::pbes_system::pbes_expression_with_propositional_variables& t)    { return &t.term(); }
};
}
/// \endcond

namespace mcrl2 {

namespace core {

  /// \brief Contains type information for pbes expressions with propositional variables.
  template <>
  struct term_traits<pbes_system::pbes_expression_with_propositional_variables>
  {
    /// \brief The term type
    typedef pbes_system::pbes_expression_with_propositional_variables term_type;

    /// \brief The data term type
    typedef data::data_expression_with_variables data_term_type;

    /// \brief The data term sequence type
    typedef data::data_expression_list data_term_sequence_type;

    /// \brief The variable type
    typedef data::data_variable variable_type;

    /// \brief The variable sequence type
    typedef data::data_variable_list variable_sequence_type;

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
    term_type true_() { return tr::true_(); }

    /// \brief The value false
    /// \return The value false
    static inline
    term_type false_() { return tr::false_(); }

    /// \brief Operator not
    /// \param p A term
    /// \return Operator not applied to p
    static inline
    term_type not_(term_type p) { return term_type(tr::not_(p), p.variables(), p.propositional_variables()); }

    /// \brief Make a conjunction
    /// \param p A term
    /// \param q A term
    /// \return The value <tt>p && q</tt>
    static inline
    term_type and_(term_type p, term_type q)
    {
      return term_type(tr::and_(p, q),
                       atermpp::term_list_union(p.variables(), q.variables()),
                       atermpp::term_list_union(p.propositional_variables(), q.propositional_variables())
                      );
    }

    /// \brief Make a disjunction
    /// \param p A term
    /// \param q A term
    /// \return The value <tt>p || q</tt>
    static inline
    term_type or_(term_type p, term_type q)
    {
      return term_type(tr::or_(p, q),
                       atermpp::term_list_union(p.variables(), q.variables()),
                       atermpp::term_list_union(p.propositional_variables(), q.propositional_variables())
                      );
    }

    /// \brief Make an implication
    /// \param p A term
    /// \param q A term
    /// \return The value <tt>p => q</tt>
    static inline
    term_type imp(term_type p, term_type q)
    {
      return term_type(tr::imp(p, q),
                       atermpp::term_list_union(p.variables(), q.variables()),
                       atermpp::term_list_union(p.propositional_variables(), q.propositional_variables())
                      );
    }

    /// \brief Make a universal quantification
    /// \param l A sequence of variables
    /// \param p A term
    /// \return The value <tt>forall l.p</tt>
    static inline
    term_type forall(variable_sequence_type l, term_type p)
    {
      return term_type(tr::forall(l, p),
                       atermpp::term_list_difference(p.variables(), l),
                       p.propositional_variables()
                      );
    }

    /// \brief Make an existential quantification
    /// \param l A sequence of variables
    /// \param p A term
    /// \return The value <tt>exists l.p</tt>
    static inline
    term_type exists(variable_sequence_type l, term_type p)
    {
      return term_type(tr::exists(l, p),
                       atermpp::term_list_difference(p.variables(), l),
                       p.propositional_variables()
                      );
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
      pbes_system::pbes_expression_with_variables tmp = core::term_traits<pbes_system::pbes_expression_with_variables>::prop_var(name, first, last);
      pbes_system::propositional_variable_instantiation elem = tmp;
      return term_type(tmp, tmp.variables(), atermpp::make_list(elem));
    }

    /// \brief Test for value true
    /// \param t A term
    /// \return True if the term has the value true. Also works for data terms
    static inline
    bool is_true(term_type t) { return tr::is_true(t); }

    /// \brief Test for value false
    /// \param t A term
    /// \return True if the term has the value false. Also works for data terms
    static inline
    bool is_false(term_type t) { return tr::is_false(t); }

    /// \brief Test for operator not
    /// \param t A term
    /// \return True if the term is of type and. Also works for data terms
    static inline
    bool is_not(term_type t) { return tr::is_not(t); }

    /// \brief Test for operator and
    /// \param t A term
    /// \return True if the term is of type and. Also works for data terms
    static inline
    bool is_and(term_type t) { return tr::is_and(t); }

    /// \brief Test for operator or
    /// \param t A term
    /// \return True if the term is of type or. Also works for data terms
    static inline
    bool is_or(term_type t) { return tr::is_or(t); }

    /// \brief Test for implication
    /// \param t A term
    /// \return True if the term is an implication. Also works for data terms
    static inline
    bool is_imp(term_type t) { return tr::is_imp(t); }

    /// \brief Test for universal quantification
    /// \param t A term
    /// \return True if the term is an universal quantification. Also works for data terms
    static inline
    bool is_forall(term_type t) { return tr::is_forall(t); }

    /// \brief Test for existential quantification
    /// \param t A term
    /// \return True if the term is an existential quantification. Also works for data terms
    static inline
    bool is_exists(term_type t) { return tr::is_exists(t); }

    /// \brief Test for data term
    /// \param t A term
    /// \return True if the term is a data term
    static inline
    bool is_data(term_type t) { return tr::is_data(t); }

    /// \brief Test for propositional variable instantiation
    /// \param t A term
    /// \return True if the term is a propositional variable instantiation
    static inline
    bool is_prop_var(term_type t) { return tr::is_prop_var(t); }

    /// \brief Returns the argument of a term of type not, exists or forall
    /// \param t A term
    /// \return The requested argument. Partially works for data terms
    static inline
    term_type arg(term_type t) { return tr::arg(t); }

    /// \brief Returns the right argument of a term of type and, or or imp
    /// \param t A term
    /// \return The right argument of the term. Also works for data terms
    static inline
    term_type left(term_type t) { return tr::left(t); }

    /// \brief Returns the quantifier variables of a quantifier expression
    /// \param t A term
    /// \return The requested argument. Doesn't work for data terms
    static inline
    term_type right(term_type t) { return tr::right(t); }

    /// \brief Returns the quantifier variables of a quantifier expression
    /// \param t A term
    /// \return The requested argument. Doesn't work for data terms
    static inline
    variable_sequence_type var(term_type t) { return tr::var(t); }

    /// \brief Returns the name of a propositional variable instantiation
    /// \param t A term
    /// \return The name of the propositional variable instantiation
    static inline
    string_type name(term_type t) { return tr::name(t); }

    /// \brief Returns the parameter list of a propositional variable instantiation
    /// \param t A term
    /// \return The parameter list of the propositional variable instantiation
    static inline
    data_term_sequence_type param(term_type t) { return tr::param(t); }

    /// \brief Conversion from variable to term
    /// \param v A variable
    /// \return The converted variable
    static inline
    term_type variable2term(variable_type v)
    {
      return tr::variable2term(v);
    }

    /// \brief Conversion from data term to term
    /// \param t A data term
    /// \return The converted term
    static inline
    term_type dataterm2term(data_term_type t)
    {
      return term_type(t, t.variables());
    }

    /// \brief Conversion from term to data term
    /// \param t A term
    /// \return The converted term
    static inline
    data_term_type term2dataterm(term_type t)
    { return tr::term2dataterm(t); }

    /// \brief Test if a term is constant
    /// \param t A term
    /// \return True if the term is constant
    static inline
    bool is_constant(const term_type& t)
    { return t.variables().empty(); }

    /// \brief Pretty print function
    /// \param t A term
    /// \return A pretty print representation of the term
    static inline
    std::string pp(term_type t)
    {
      return core::pp(t) + " " + core::pp(t.variables()) + " " + core::pp(t.propositional_variables());
    }
  };

} // namespace core

} // namespace mcrl2

#endif // MCRL2_PBES_PBES_EXPRESSION_WITH_PROPOSITIONAL_VARIABLES_H
