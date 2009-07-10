// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes_substituter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBES_SUBSTITUTER_H
#define MCRL2_PBES_DETAIL_PBES_SUBSTITUTER_H

#include <vector>
#include "mcrl2/data/replace.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/detail/substitute_builder.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  /// \brief Function object for applying a substitution to PBES data types.
  template <typename Substitution>
  struct pbes_substituter
  {
    const Substitution& sigma;
    
    /// If true, the process parameters, global variables and the left hand
    /// sides of assignments are replaced as well. For this it is needed
    /// that the result of the substitution is a variable.
    bool replace_parameters;
  
    pbes_substituter(const Substitution& sigma_, bool replace_parameters_ = false)
      : sigma(sigma_),
        replace_parameters(replace_parameters_)
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

    /// \brief Applies the substitution to a data variable
    /// \param d A data variable
    data::variable substitute_copy(const data::variable& d) const
    {
      return sigma(d);
    } 

    /// \brief Applies the substitution to a data variable
    /// \param d A data variable
    void substitute(data::variable& d) const
    {                                         
      d = substitute_copy(d);
    } 
  
    /// \brief Applies the substitution to a data expression
    /// \param d A data expression
    data::data_expression substitute_copy(const data::data_expression& d) const    
    {                                         
      return data::replace_free_variables(d, sigma);
    } 

    /// \brief Applies the substitution to a data expression
    /// \param d A data expression
    void substitute(data::data_expression& d) const    
    {                                         
      d = substitute_copy(d);
    } 
  
    /// \brief Applies the substitution to an assignment
    /// \param a An assignment
    void substitute(data::assignment& a) const
    {
      if (replace_parameters)
      {
        data::variable v = substitute_copy(a.lhs());
        a = data::assignment(v, substitute_copy(a.rhs()));
      }
      else
      {
        a = data::assignment(a.lhs(), substitute_copy(a.rhs()));
      }
    } 

    /// \brief Applies the substitution to a pbes expression
    /// \param t A pbes expression
    void substitute(pbes_expression& t) const
    {
      t = detail::make_substitute_builder(sigma).visit(t);
    } 

    /// \brief Applies the substitution to a propositional variable declaration
    /// \param t A propositional variable declaration
    void substitute(propositional_variable& v) const
    {
      if (replace_parameters)
      {
        v = propositional_variable(v.name(), substitute_list_copy(v.parameters()));
      }
    } 

    /// \brief Applies the substitution to a propositional variable instantiation
    /// \param t A propositional variable instantiation
    void substitute(propositional_variable_instantiation& v) const
    {
      v = propositional_variable_instantiation(v.name(), substitute_list_copy(v.parameters()));
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
      if (replace_parameters)
      {
        atermpp::set<data::variable> glob;
        for (atermpp::set<data::variable>::iterator i = p.global_variables().begin(); i != p.global_variables().end(); ++i)
        {
          glob.insert(substitute_copy(*i));
        }
        p.global_variables() = glob;
      }
      substitute_container(p.equations());
      substitute(p.initial_state());
    }
    
    template <typename Term>
    void operator()(Term& t) const
    {
      substitute(t);
    }
  };

  /// \brief Utility function to create an pbes_substituter.
  template <typename Substitution>    
  pbes_substituter<Substitution> make_pbes_substituter(const Substitution& sigma)        
  {
    return pbes_substituter<Substitution>(sigma);
  } 

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_SUBSTITUTER_H
