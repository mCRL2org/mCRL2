// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/lps2pbes_utility.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_LPS2PBES_UTILITY_H
#define MCRL2_PBES_DETAIL_LPS2PBES_UTILITY_H

#include "mcrl2/pbes/pbes.h"

namespace mcrl2 {

namespace pbes_system {

/// \cond INTERNAL_DOCS
//
/// \brief Concatenates two sequences of PBES equations
/// \param p A sequence of PBES equations
/// \param q A sequence of PBES equations
/// \return The concatenation result
inline
atermpp::vector<pbes_equation> operator+(const atermpp::vector<pbes_equation>& p, const atermpp::vector<pbes_equation>& q)
{
  atermpp::vector<pbes_equation> result(p);
  result.insert(result.end(), q.begin(), q.end());
  return result;
}
/// \endcond

/// \cond INTERNAL_DOCS
//
/// \brief Appends a PBES equation to a sequence of PBES equations
/// \param p A sequence of PBES equations
/// \param e A PBES equation
/// \return The append result
inline
atermpp::vector<pbes_equation> operator+(const atermpp::vector<pbes_equation>& p, const pbes_equation& e)
{
  atermpp::vector<pbes_equation> result(p);
  result.push_back(e);
  return result;
}
/// \endcond

namespace detail {

inline
std::string myprint(const atermpp::vector<pbes_equation>& v)
{
  std::ostringstream out;
  out << "[";
  for (atermpp::vector<pbes_equation>::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    out << "\n  " << pbes_system::pp(i->symbol()) << " " << pbes_system::pp(i->variable()) << " = " << pbes_system::pp(i->formula());
  }
  out << "\n]";
  return out.str();
}

/// \brief Generates fresh variables with names that do not appear in the given context.
/// Caveat: the implementation is very inefficient.
/// \param update_context If true, then generated names are added to the context
inline
data::variable_list make_fresh_variables(const data::variable_list& variables, data::set_identifier_generator& id_generator, bool add_to_context = true)
{
  data::variable_vector result;
  for (data::variable_list::const_iterator i = variables.begin(); i != variables.end(); ++i)
  {
    core::identifier_string name =  id_generator(std::string(i->name()));
    result.push_back(data::variable(name, i->sort()));
    if (!add_to_context)
    {
      id_generator.remove_identifier(name);
    }
  }
  return atermpp::convert<data::variable_list>(result);
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_LPS2PBES_UTILITY_H
