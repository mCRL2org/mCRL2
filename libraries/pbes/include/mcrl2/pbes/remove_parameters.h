// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/remove_parameters.h
/// \brief Functions for removing insignificant parameters from pbes types.

#ifndef MCRL2_PBES_REMOVE_PARAMETERS_H
#define MCRL2_PBES_REMOVE_PARAMETERS_H

#include <algorithm>
#include <map>
#include <vector>
#include <boost/bind.hpp>
#include "mcrl2/data/data.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression_builder.h"

namespace mcrl2 {

namespace pbes_system {

/// \cond INTERNAL_DOCS
namespace detail {

  template <typename Term>
  atermpp::term_list<Term> remove_elements(atermpp::term_list<Term> l, const std::vector<int>& to_be_removed)
  {
    int index = 0;
    std::vector<Term> result;
    std::vector<int>::const_iterator j = to_be_removed.begin();
    for (typename atermpp::term_list<Term>::iterator i = l.begin(); i != l.end(); ++i, index++)
    {
      if (j != to_be_removed.end() && index == *j)
      {
        j++;
      }
      else
      {
        result.push_back(*i);
      }
    }
    return atermpp::term_list<Term>(result.begin(), result.end());
  }

} // namespace detail
/// \endcond

/// Removes parameters from a propositional variable
/// \param v A propositional variable
/// \param to_be_removed A set of indices
/// \return The variable \p v without the parameters that have an index in \p to_be_removed
inline
propositional_variable remove_parameters(propositional_variable v, const std::vector<int>& to_be_removed)
{
  return propositional_variable(v.name(), detail::remove_elements(v.parameters(), to_be_removed));
}

/// Removes parameters from a propositional variable
/// \param v A propositional variable
/// \param to_be_removed A mapping that maps propositional variable names to indices of parameters that are removed
/// \return The variable \p v without the parameters that have an index in \code to_be_removed(v.name()) \endcode
inline
propositional_variable remove_parameters(propositional_variable v, const std::map<core::identifier_string, std::vector<int> >& to_be_removed)
{
  std::map<core::identifier_string, std::vector<int> >::const_iterator i = to_be_removed.find(v.name());
  if (i == to_be_removed.end())
  {
    return v;
  }
  return remove_parameters(v, i->second);
}

/// Removes parameters from a propositional variable instantiation
/// \param v A propositional variable instantiation
/// \param to_be_removed A set of indices
/// \return The variable \p v without the parameters that have an index in \p to_be_removed
inline
propositional_variable_instantiation remove_parameters(propositional_variable_instantiation v, const std::vector<int>& to_be_removed)
{
  return propositional_variable_instantiation(v.name(), detail::remove_elements(v.parameters(), to_be_removed));
}

/// Removes parameters from a propositional variable instantiation
/// \param v A propositional variable
/// \param to_be_removed A mapping that maps propositional variable names to indices of parameters that are removed
/// \return The variable \p v without the parameters that have an index in \code to_be_removed(v.name()) \endcode
inline
propositional_variable_instantiation remove_parameters(propositional_variable_instantiation v, const std::map<core::identifier_string, std::vector<int> >& to_be_removed)
{
  std::map<core::identifier_string, std::vector<int> >::const_iterator i = to_be_removed.find(v.name());
  if (i == to_be_removed.end())
  {
    return v;
  }
  return remove_parameters(v, i->second);
}

/// \cond INTERNAL_DOCS
namespace detail {
struct pbes_remove_parameters_builder: public pbes_expression_builder<pbes_expression>
{
  const std::map<core::identifier_string, std::vector<int> >& to_be_removed_;

  pbes_remove_parameters_builder(const std::map<core::identifier_string, std::vector<int> >& to_be_removed)
    : to_be_removed_(to_be_removed)
  {}

  /// \brief Visit propositional variable node.
  ///
  pbes_expression visit_propositional_variable(const pbes_expression& x, const propositional_variable_instantiation& v)
  {
    std::map<core::identifier_string, std::vector<int> >::const_iterator i = to_be_removed_.find(v.name());
    if (i == to_be_removed_.end())
    {
      return x;
    }
    else
    {
      return remove_parameters(v, i->second);
    }
  }
};
} // namespace detail
/// \endcond

/// Removes parameters from propositional variable instantiations in a pbes expression
/// \param p A pbes expression
/// \param to_be_removed A mapping that maps propositional variable names to indices of parameters that are removed
/// \return The expression \p p with parameters removed according to the mapping \p to_be_removed
inline
pbes_expression remove_parameters(pbes_expression p, const std::map<core::identifier_string, std::vector<int> >& to_be_removed)
{
  return detail::pbes_remove_parameters_builder(to_be_removed).visit(p);
}

/// Removes parameters from propositional variables in a pbes equation
/// \param e A pbes equation
/// \param to_be_removed A mapping that maps propositional variable names to indices of parameters that are removed
/// \return The equation \p e with parameters removed according to the mapping \p to_be_removed
inline
pbes_equation remove_parameters(pbes_equation e, const std::map<core::identifier_string, std::vector<int> >& to_be_removed)
{
  return pbes_equation(e.symbol(),
                       remove_parameters(e.variable(), to_be_removed),
                       remove_parameters(e.formula(), to_be_removed)
                      );
}

/// Removes parameters from propositional variables in a pbes
/// \param p A pbes
/// \param to_be_removed A mapping that maps propositional variable names to indices of parameters that are removed
/// \return The pbes \p p with parameters removed according to the mapping \p to_be_removed
template <typename Container>
void remove_parameters(pbes<Container>& p, const std::map<core::identifier_string, std::vector<int> >& to_be_removed)
{
  typedef pbes_equation (*f)(pbes_equation, const std::map<core::identifier_string, std::vector<int> >&);
  std::transform(p.equations().begin(),
                 p.equations().end(),
                 p.equations().begin(),
                 boost::bind(static_cast<f>(remove_parameters), _1, to_be_removed)
                );
  p.initial_state() = remove_parameters(p.initial_state(), to_be_removed);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REMOVE_PARAMETERS_H
