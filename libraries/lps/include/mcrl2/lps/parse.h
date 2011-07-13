// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/parse.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_PARSE_H
#define MCRL2_LPS_PARSE_H

#include <sstream>
#include "mcrl2/atermpp/convert.h"
#include "mcrl2/exception.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/lps/detail/linear_process_conversion_traverser.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/process/is_linear.h"
#include "mcrl2/process/parse.h"

namespace mcrl2
{

namespace lps
{

/// \brief Parses a linear process specification from an input stream
/// \param text An input stream containing a linear process specification
/// \return The parsed specification
/// \exception non_linear_process if a non-linear sub-expression is encountered.
/// \exception mcrl2::runtime_error in the following cases:
/// \li The number of equations is not equal to one
/// \li The initial process is not a process instance, or it does not match with the equation
/// \li A sequential process is found with a right hand side that is not a process instance,
/// or it doesn't match the equation
inline
specification parse_linear_process_specification(std::istream& spec_stream)
{
  process::process_specification pspec = mcrl2::process::parse_process_specification(spec_stream);
  if (!process::is_linear(pspec, true))
  {
    throw mcrl2::runtime_error("the process specification is not linear!");
  }
  process::detail::linear_process_conversion_traverser visitor;
  specification result = visitor.convert(pspec);
  return result;
}
/// \brief Parses a linear process specification from a string
/// \param text A string containing a linear process specification
/// \return The parsed specification
/// \exception non_linear_process if a non-linear sub-expression is encountered.
/// \exception mcrl2::runtime_error in the following cases:
/// \li The number of equations is not equal to one
/// \li The initial process is not a process instance, or it does not match with the equation
/// \li A sequential process is found with a right hand side that is not a process instance,
/// or it doesn't match the equation
inline
specification parse_linear_process_specification(const std::string& text)
{
  std::istringstream stream(text);
  return parse_linear_process_specification(stream);
}

/// \brief Parses a multi_action from an input stream
/// \param ma_stream An input stream containing a multi_action
/// \param[in] action_decls A list of allowed action labels that is used for type checking.
/// \param[in] data_spec The data specification that is used for type checking.
/// \return The parsed multi_action
/// \exception mcrl2::runtime_error when the input does not match the syntax of a multi action.
inline
multi_action parse_multi_action(std::stringstream& ma_stream, const lps::action_label_list& action_decls, const data::data_specification& data_spec = data::detail::default_specification())
{
  ATermAppl a = mcrl2::core::parse_mult_act(ma_stream);
  if (a == NULL)
  {
    throw mcrl2::runtime_error("Syntax error in multi action " + ma_stream.str());
  }
  lps::multi_action mact = atermpp::aterm_appl(a);
  lps::type_check(mact, data_spec, action_decls);
  return mact;
}
/// \brief Parses a linear process specification from a string
/// \brief Parses a multi_action from a string
/// \param text An input stream containing a multi_action
/// \param[in] action_decls A list of allowed action labels that is used for type checking.
/// \param[in] data_spec The data specification that is used for type checking.
/// \return The parsed multi_action
/// \exception mcrl2::runtime_error when the input does not match the syntax of a multi action.
inline
multi_action parse_multi_action(const std::string& text, const lps::action_label_list& action_decls, const data::data_specification& data_spec = data::detail::default_specification())
{
  std::stringstream ma_stream(text);
  return parse_multi_action(ma_stream, action_decls, data_spec);
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_PARSE_H
