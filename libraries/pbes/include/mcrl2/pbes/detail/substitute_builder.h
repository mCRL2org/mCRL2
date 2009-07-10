// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/substitute_builder.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_SUBSTITUTE_BUILDER_H
#define MCRL2_PBES_DETAIL_SUBSTITUTE_BUILDER_H

#include "mcrl2/data/replace.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression_builder.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// Visitor that applies a data substitution to a pbes expression.
template <typename Substitution>
struct substitute_builder: public pbes_expression_builder<pbes_expression>
{
  const Substitution& sigma;
  
  substitute_builder(const Substitution& sigma_)
    : sigma(sigma_)
  {}

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

  /// \brief Visit data_expression node
  /// \param e A PBES expression
  /// \param d A data expression
  /// \return The result of visiting the node
  pbes_expression visit_data_expression(const pbes_expression& e, const data::data_expression& d)
  {
    return substitute_copy(d);
  }

  /// \brief Visit propositional_variable node
  /// \param x A term
  /// \return The result of visiting the node
  pbes_expression visit_propositional_variable(const pbes_expression& x, const propositional_variable_instantiation& v)
  {
    return propositional_variable_instantiation(v.name(), substitute_list_copy(v.parameters()));
  }
};

template <typename Substitution>
struct substitute_builder<Substitution> make_substitute_builder(const Substitution& sigma)
{
  return substitute_builder<Substitution>(sigma);
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_SUBSTITUTE_BUILDER_H
