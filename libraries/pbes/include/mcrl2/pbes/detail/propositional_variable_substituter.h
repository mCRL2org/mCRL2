// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/propositional_variable_substituter.h
/// \brief add your file description here.

// This is to deal with circular header file dependencies
#ifndef MCRL2_PBES_PBES_H
#include "mcrl2/pbes/pbes.h"
#endif

#ifndef MCRL2_PBES_DETAIL_PROPOSITIONAL_VARIABLE_SUBSTITUTER_H
#define MCRL2_PBES_DETAIL_PROPOSITIONAL_VARIABLE_SUBSTITUTER_H

#include <vector>
#include "mcrl2/pbes/pbes_expression_builder.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  /// Visitor that applies a data substitution to a pbes expression.
  template <typename PropositionalVariableSubstitution>
  struct propositional_variable_substitute_builder: public pbes_expression_builder<pbes_expression>
  {
    const PropositionalVariableSubstitution& sigma;
    
    propositional_variable_substitute_builder(const PropositionalVariableSubstitution& sigma_)
      : sigma(sigma_)
    {}

    /// \brief Visit propositional_variable node
    /// \param x A term
    /// \return The result of visiting the node
    pbes_expression visit_propositional_variable(const pbes_expression& x, const propositional_variable_instantiation& v)
    {
      return sigma(v);
    }
  };
  
  template <typename PropositionalVariableSubstitution>
  propositional_variable_substitute_builder<PropositionalVariableSubstitution> make_propositional_variable_substitute_builder(const PropositionalVariableSubstitution& sigma)
  {
    return propositional_variable_substitute_builder<PropositionalVariableSubstitution>(sigma);
  }

  /// \brief Function object for applying a substitution to PBES data types.
  template <typename PropositionalVariableSubstitution>
  struct propositional_variable_substituter
  {
    const PropositionalVariableSubstitution& sigma;
    
    /// If true, propositional variables of are replaced as well. For this it is needed
    /// that the result of the substitution to a propositional variable is a propositional
    /// variable.
    bool replace_propositional_variables;
  
    propositional_variable_substituter(const PropositionalVariableSubstitution& sigma_, bool replace_propositional_variables_ = false)
      : sigma(sigma_),
        replace_propositional_variables(replace_propositional_variables_)
    {}

    /// \brief Applies the substitution to the elements of a term list
    template <typename TermList>
    TermList substitute_list_copy(const TermList& l) const
    {
      // TODO: how to make this function efficient?
      typedef typename TermList::value_type value_type;
      atermpp::vector<value_type> v(l.begin(), l.end());
      for (typename std::vector<value_type>::iterator i = v.begin(); i != v.end(); ++i)
      {
        substitute(*i);
      }
      return TermList(v.begin(), v.end());
    }   

    /// \brief Applies the substitution to the elements of a term list
    template <typename TermList>
    void substitute_list(TermList& l) const
    {
      l = substitute_list_copy(l);
    }

    /// \brief Applies the substitution to the elements of a container
    template <typename Container>
    void substitute_container(Container& c) const
    {
      for (typename Container::iterator i = c.begin(); i != c.end(); ++i)
      {
        substitute(*i);
      }
    }   

    /// \brief Applies the substitution to a pbes expression
    /// \param t A pbes expression
    void substitute(pbes_expression& t) const
    {
      t = make_propositional_variable_substitute_builder(sigma).visit(t);
    } 

    /// \brief Applies the substitution to a propositional variable declaration
    /// \param t A propositional variable declaration
    void substitute(propositional_variable& v) const
    {
      if (replace_propositional_variables)
      {
        propositional_variable_instantiation w = sigma(v);
        data::variable_list params = data::convert<data::variable_list>(w.parameters());
        v = propositional_variable(w.name(), params);
      }
    } 

    /// \brief Applies the substitution to a propositional variable instantiation
    /// \param t A propositional variable instantiation
    void substitute(propositional_variable_instantiation& v) const
    {
      v = sigma(v);
    } 

    /// \brief Applies the substitution to a pbes equation
    /// \param e A pbes equation
    void substitute(pbes_equation& e) const
    {
      substitute(e.variable());
      substitute(e.formula());
    } 
  
    /// \brief Applies the substitution to a PBES
    /// \param p A PBES
    template <typename Container>
    void substitute(pbes<Container>& p) const
    {
      substitute_container(p.equations());
      substitute(p.initial_state());
    }
    
    template <typename Term>
    void operator()(Term& t) const
    {
      substitute(t);
    }
  };

  /// \brief Utility function to create a propositional_variable_substituter.
  template <typename PropositionalVariableSubstitution>    
  propositional_variable_substituter<PropositionalVariableSubstitution> make_propositional_variable_substituter(const PropositionalVariableSubstitution& sigma)        
  {
    return propositional_variable_substituter<PropositionalVariableSubstitution>(sigma);
  } 

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PROPOSITIONAL_VARIABLE_SUBSTITUTER_H
