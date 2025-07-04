// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/parse.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_PARSE_H
#define MCRL2_LPS_PARSE_H

#include "mcrl2/lps/detail/linear_process_conversion_traverser.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/process/parse.h"

namespace mcrl2::lps
{

namespace detail {

process::untyped_multi_action parse_multi_action_new(const std::string& text);
multi_action complete_multi_action(process::untyped_multi_action& x, multi_action_type_checker& typechecker, const data::data_specification& data_spec = data::detail::default_specification());
multi_action complete_multi_action(process::untyped_multi_action& x, const process::action_label_list& action_decls, const data::data_specification& data_spec = data::detail::default_specification());
action_rename_specification parse_action_rename_specification_new(const std::string& text);
void complete_action_rename_specification(action_rename_specification& x, const lps::stochastic_specification& spec);

} // namespace detail

/// \brief Parses a multi_action from an input stream
/// \param in An input stream containing a multi_action
/// \param[in] action_decls A list of allowed action labels that is used for type checking.
/// \param[in] data_spec The data specification that is used for type checking.
/// \return The parsed multi_action
/// \exception mcrl2::runtime_error when the input does not match the syntax of a multi action.
inline
multi_action parse_multi_action(std::stringstream& in, const process::action_label_list& action_decls, const data::data_specification& data_spec = data::detail::default_specification())
{
  std::string text = utilities::read_text(in);
  process::untyped_multi_action u = detail::parse_multi_action_new(text);
  return detail::complete_multi_action(u, action_decls, data_spec);
}

/// \brief Parses a multi_action from an input stream
/// \param in An input stream containing a multi_action
/// \param[in] typechecker Typechecker used to check the action.
/// \param[in] data_spec The data specification that is used for type checking.
/// \return The parsed multi_action
/// \exception mcrl2::runtime_error when the input does not match the syntax of a multi action.
inline
multi_action parse_multi_action(std::stringstream& in, multi_action_type_checker& typechecker, const data::data_specification& data_spec = data::detail::default_specification())
{
  std::string text = utilities::read_text(in);
  process::untyped_multi_action u = detail::parse_multi_action_new(text);
  return detail::complete_multi_action(u, typechecker, data_spec);
}

/// \brief Parses a multi_action from a string
/// \param text A string containing a multi_action
/// \param[in] action_decls A list of allowed action labels that is used for type checking.
/// \param[in] data_spec The data specification that is used for type checking.
/// \return The parsed multi_action
/// \exception mcrl2::runtime_error when the input does not match the syntax of a multi action.
inline
multi_action parse_multi_action(const std::string& text, const process::action_label_list& action_decls, const data::data_specification& data_spec = data::detail::default_specification())
{
  std::stringstream ma_stream(text);
  return parse_multi_action(ma_stream, action_decls, data_spec);
}

/// \brief Parses a multi_action from a string
/// \param text A string containing a multi_action
/// \param[in] typechecker Typechecker used to check the action.
/// \param[in] data_spec The data specification that is used for type checking.
/// \return The parsed multi_action
/// \exception mcrl2::runtime_error when the input does not match the syntax of a multi action.
inline
multi_action parse_multi_action(const std::string& text, multi_action_type_checker& typechecker, const data::data_specification& data_spec = data::detail::default_specification())
{
  std::stringstream ma_stream(text);
  return parse_multi_action(ma_stream, typechecker, data_spec);
}

/// \brief Parses a process specification from an input stream
/// \param in An input stream
/// \param spec A linear process specification.
/// \return The parse result
inline
action_rename_specification parse_action_rename_specification(std::istream& in, const lps::stochastic_specification& spec)
{
  std::string text = utilities::read_text(in);
  action_rename_specification result = detail::parse_action_rename_specification_new(text);
  detail::complete_action_rename_specification(result, spec);
  return result;
}

/// \brief Parses an action rename specification.
/// Parses an acion rename specification.
/// If the action rename specification contains data types that are not
/// present in the data specification of \p spec they are added to it.
/// \param spec_string A string containing an action rename specification.
/// \param spec A linear process specification
/// \return An action rename specification
inline
action_rename_specification parse_action_rename_specification(
           const std::string& spec_string,
           const lps::stochastic_specification& spec)
{
  std::istringstream in(spec_string);
  return parse_action_rename_specification(in, spec);
}

/// \brief Parses a linear process specification from an input stream
/// \param spec_stream An input stream containing a linear process specification
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
  process::process_specification pspec =
      mcrl2::process::parse_process_specification(spec_stream);
  if (!process::is_linear(pspec, true))
  {
    throw mcrl2::runtime_error("the process specification is not linear!");
  }
  process::detail::linear_process_conversion_traverser visitor;
  specification result = visitor.convert(pspec);
  complete_data_specification(result);
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

template <typename Specification>
void parse_lps(std::istream&, Specification&)
{
  throw mcrl2::runtime_error("parse_lps not implemented yet!");
}

template <>
inline
void parse_lps<specification>(std::istream& from, specification& result)
{
  result = parse_linear_process_specification(from);
}

/// \brief Parses a stochastic linear process specification from an input stream.
/// \param from An input stream containing a linear process specification.
/// \param result An output parameter in which the parsed stochastic process is put.
/// \return The parsed specification.
/// \exception non_linear_process if a non-linear sub-expression is encountered.
/// \exception mcrl2::runtime_error in the following cases:
/// \li The number of equations is not equal to one.
/// \li The initial process is not a process instance, or it does not match with the equation.
/// \li A sequential process is found with a right hand side that is not a process instance,
/// or it doesn't match the equation.
template <>
inline
void parse_lps<stochastic_specification>(std::istream& from, stochastic_specification& result)
{
  process::process_specification pspec =
      mcrl2::process::parse_process_specification(from);
  if (!process::is_linear(pspec, true))
  {
    throw mcrl2::runtime_error("the process specification is not linear!");
  }
  process::detail::stochastic_linear_process_conversion_traverser visitor;
  result = visitor.convert(pspec);
  complete_data_specification(result);
}

template <typename Specification>
void parse_lps(const std::string& text, Specification& result)
{
  std::istringstream stream(text);
  parse_lps(stream, result);
}

/// \brief Parses an action from a string
/// \param text A string containing an action
/// \param action_decls An action declaration
/// \param[in] data_spec A data specification used for sort normalization
/// \return An action
/// \exception mcrl2::runtime_error when the input does not match the syntax of an action.
// TODO: implement this function in the Process Library
inline
process::action parse_action(const std::string& text, const process::action_label_list& action_decls, const data::data_specification& data_spec = data::detail::default_specification())
{
  multi_action result = parse_multi_action(text, action_decls, data_spec);
  if (result.actions().size() != 1)
  {
    throw mcrl2::runtime_error("cannot parse '" + text + " as an action!");
  }
  return result.actions().front();
}

} // namespace mcrl2::lps



#endif // MCRL2_LPS_PARSE_H
