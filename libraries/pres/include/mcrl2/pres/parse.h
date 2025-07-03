// Author(s): Jan Friso Groote. Based on pbes/parse.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/parse.h
/// \brief Parser for pres expressions.

#ifndef MCRL2_PRES_PARSE_H
#define MCRL2_PRES_PARSE_H

#include "mcrl2/data/detail/parse_substitution.h"
#include "mcrl2/pres/typecheck.h"
#include "mcrl2/pres/untyped_pres.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>



namespace mcrl2::pres_system
{

namespace detail
{

pres_expression parse_pres_expression_new(const std::string& text);
untyped_pres parse_pres_new(const std::string& text);
void complete_pres(pres& x);
propositional_variable parse_propositional_variable(const std::string& text);
pres_expression parse_pres_expression(const std::string& text);

} // namespace detail

inline
pres parse_pres(std::istream& in)
{
  std::string text = utilities::read_text(in);
  pres result = detail::parse_pres_new(text).construct_pres();
  detail::complete_pres(result);
  return result;
}

/// \brief Reads a PRES from an input stream.
/// \param from An input stream
/// \param result A PRES
/// \return The input stream
inline
std::istream& operator>>(std::istream& from, pres& result)
{
  result = parse_pres(from);
  return from;
}

inline
pres parse_pres(const std::string& text)
{
  std::istringstream in(text);
  return parse_pres(in);
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

/** \brief     Parse a pres expression.
 *  Throws an exception if something went wrong.
 *  \param[in] text A string containing a pres expression.
 *  \param[in] variables A sequence of data variables that may appear in x.
 *  \param[in] propositional_variables A sequence of propositional variables that may appear in x.
 *  \param[in] dataspec A data specification.
 *  \param[in] type_check If true the parsed input is also typechecked.
 *  \return    The parsed PRES expression.
 **/
template <typename VariableContainer, typename PropositionalVariableContainer>
pres_expression parse_pres_expression(const std::string& text,
                                      const data::data_specification& dataspec,
                                      const VariableContainer& variables,
                                      const PropositionalVariableContainer& propositional_variables,
                                      bool type_check = true,
                                      bool translate_user_notation = true,
                                      bool normalize_sorts = true
                                     )
{
  pres_expression x = detail::parse_pres_expression(text);
  if (type_check)
  {
    x = pres_system::typecheck_pres_expression(x, variables, propositional_variables, dataspec);
  }
  if (translate_user_notation)
  {
    x = pres_system::translate_user_notation(x);
  }
  if (normalize_sorts)
  {
    x = pres_system::normalize_sorts(x, dataspec);
  }
  return x;
}

/** \brief     Parse a pres expression.
 *  Throws an exception if something went wrong.
 *  \param[in] text A string containing a pres expression.
 *  \param[in] presspec A PRES used as context.
 *  \param[in] variables A sequence of data variables that may appear in x.
 *  \param[in] type_check If true the parsed input is also typechecked.
 *  \return    The parsed PRES expression.
 **/
template <typename VariableContainer>
pres_expression parse_pres_expression(const std::string& text,
                                      const pres& presspec,
                                      const VariableContainer& variables,
                                      bool type_check = true,
                                      bool translate_user_notation = true,
                                      bool normalize_sorts = true
                                     )
{
  std::vector<propositional_variable> propositional_variables;
  for (const pres_equation& eqn: presspec.equations())
  {
    propositional_variables.push_back(eqn.variable());
  }
  return parse_pres_expression(text, presspec.data(), variables, propositional_variables, type_check, translate_user_notation, normalize_sorts);
}

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_PARSE_H
