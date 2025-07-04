// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/parse.h
/// \brief Parser for pbes expressions.

#ifndef MCRL2_PBES_PARSE_H
#define MCRL2_PBES_PARSE_H

#include "mcrl2/data/detail/parse_substitution.h"
#include "mcrl2/pbes/typecheck.h"
#include "mcrl2/pbes/untyped_pbes.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>

namespace mcrl2::pbes_system
{

namespace detail
{

pbes_expression parse_pbes_expression_new(const std::string& text);
untyped_pbes parse_pbes_new(const std::string& text);
void complete_pbes(pbes& x);
propositional_variable parse_propositional_variable(const std::string& text);
pbes_expression parse_pbes_expression(const std::string& text);

} // namespace detail

inline
pbes parse_pbes(std::istream& in)
{
  std::string text = utilities::read_text(in);
  pbes result = detail::parse_pbes_new(text).construct_pbes();
  detail::complete_pbes(result);
  return result;
}

/// \brief Reads a PBES from an input stream.
/// \param from An input stream
/// \param result A PBES
/// \return The input stream
inline
std::istream& operator>>(std::istream& from, pbes& result)
{
  result = parse_pbes(from);
  return from;
}

inline
pbes parse_pbes(const std::string& text)
{
  std::istringstream in(text);
  return parse_pbes(in);
}

template <typename VariableContainer>
propositional_variable parse_propositional_variable(const std::string& text,
                                                    const VariableContainer& variables,
                                                    const data::data_specification& dataspec = data::data_specification()
                                                   )
{
  propositional_variable result = detail::parse_propositional_variable(text);
  return typecheck_propositional_variable(result, variables, dataspec);
}

/** \brief     Parse a pbes expression.
 *  Throws an exception if something went wrong.
 *  \param[in] text A string containing a pbes expression.
 *  \param[in] variables A sequence of data variables that may appear in x.
 *  \param[in] propositional_variables A sequence of propositional variables that may appear in x.
 *  \param[in] dataspec A data specification.
 *  \param[in] type_check If true the parsed input is also typechecked.
 *  \return    The parsed PBES expression.
 **/
template <typename VariableContainer, typename PropositionalVariableContainer>
pbes_expression parse_pbes_expression(const std::string& text,
                                      const data::data_specification& dataspec,
                                      const VariableContainer& variables,
                                      const PropositionalVariableContainer& propositional_variables,
                                      bool type_check = true,
                                      bool translate_user_notation = true,
                                      bool normalize_sorts = true
                                     )
{
  pbes_expression x = detail::parse_pbes_expression(text);
  if (type_check)
  {
    x = pbes_system::typecheck_pbes_expression(x, variables, propositional_variables, dataspec);
  }
  if (translate_user_notation)
  {
    x = pbes_system::translate_user_notation(x);
  }
  if (normalize_sorts)
  {
    x = pbes_system::normalize_sorts(x, dataspec);
  }
  return x;
}

/** \brief     Parse a pbes expression.
 *  Throws an exception if something went wrong.
 *  \param[in] text A string containing a pbes expression.
 *  \param[in] pbesspec A PBES used as context.
 *  \param[in] variables A sequence of data variables that may appear in x.
 *  \param[in] type_check If true the parsed input is also typechecked.
 *  \return    The parsed PBES expression.
 **/
template <typename VariableContainer>
pbes_expression parse_pbes_expression(const std::string& text,
                                      const pbes& pbesspec,
                                      const VariableContainer& variables,
                                      bool type_check = true,
                                      bool translate_user_notation = true,
                                      bool normalize_sorts = true
                                     )
{
  std::vector<propositional_variable> propositional_variables;
  for (const pbes_equation& eqn: pbesspec.equations())
  {
    propositional_variables.push_back(eqn.variable());
  }
  return parse_pbes_expression(text, pbesspec.data(), variables, propositional_variables, type_check, translate_user_notation, normalize_sorts);
}

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_PARSE_H
