// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/replace.h
/// \brief Contains a function for replacing data variables in a term.

#ifndef MCRL2_PBES_REPLACE_H
#define MCRL2_PBES_REPLACE_H

#include <utility>
#include "mcrl2/pbes/substitute.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/builder.h"

namespace mcrl2
{

namespace pbes_system
{

struct prop_var_substitution: public std::unary_function<propositional_variable_instantiation, pbes_expression>
{
  const propositional_variable& X;
  const pbes_expression& phi;

  prop_var_substitution(const propositional_variable& X_, const pbes_expression& phi_)
    : X(X_), phi(phi_)
  {}

  pbes_expression operator()(const propositional_variable_instantiation& x)
  {
    if (x.name() != X.name())
    {
      return x;
    }
    return pbes_system::substitute_free_variables(phi, data::make_sequence_sequence_substitution(X.parameters(), x.parameters()));
  }
};

struct prop_var_map_substitution: public data::mutable_associative_container_substitution<atermpp::map<propositional_variable_instantiation, pbes_expression> >
{
  // override the substitution
  pbes_expression operator()(const propositional_variable_instantiation& x) const
  {
    for (atermpp::map<propositional_variable_instantiation, pbes_expression>::const_iterator i = m_map.begin(); i != m_map.end(); ++i)
    {
      if (i->first.name() == x.name())
      {
        return pbes_system::substitute_free_variables(i->second, data::make_sequence_sequence_substitution(x.parameters(), i->first.parameters()));        
      }
    }
    return x;
  }
};

/// \brief Applies the substitution \p X := \p phi to the pbes expression \p t.
/// \param t A pbes expression
/// \param X A propositional variable
/// \param phi A pbes expression
/// \return The result of the substitution.
inline
pbes_expression substitute_propositional_variable(const pbes_expression& x,
    const propositional_variable& X,
    const pbes_expression& phi)
{
  return core::make_update_apply_builder<pbes_expression_builder>(prop_var_substitution(X, phi))(x);
}

/// \brief Applies a propositional variable substitution.
template <typename T, typename Substitution>
void substitute_propositional_variables(T& x,
                                        Substitution sigma,
                                        typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                                       )
{
  core::make_update_apply_builder<pbes_expression_builder>(sigma)(x);
}

/// \brief Applies a propositional variable substitution.
template <typename T, typename Substitution>
T substitute_propositional_variables(const T& x,
                                     Substitution sigma,
                                     typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                                    )
{
  return core::make_update_apply_builder<pbes_expression_builder>(sigma)(x);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REPLACE_H
