// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/parse.h
/// \brief Parse function for process specifications

#ifndef MCRL2_PROCESS_PARSE_H
#define MCRL2_PROCESS_PARSE_H

#include "mcrl2/data/parse.h"
#include "mcrl2/process/typecheck.h"
#include "mcrl2/utilities/detail/separate_keyword_section.h"

namespace mcrl2::process
{

namespace detail {

process_expression parse_process_expression_new(const std::string& text);
process_specification parse_process_specification_new(const std::string& text);
void complete_process_specification(process_specification& x, bool alpha_reduce = false);

} // namespace detail

/// \brief Parses an action declaration from a string
/// \param text A string containing an action declaration
/// \param[in] data_spec A data specification used for sort normalization
/// \return A list of action labels
/// \exception mcrl2::runtime_error when the input does not match the syntax of an action declaration.
process::action_label_list parse_action_declaration(const std::string& text, const data::data_specification& data_spec = data::detail::default_specification());

/// \brief Parses a process specification from an input stream
/// \param in An input stream
/// \return The parse result
inline
process_specification
parse_process_specification(std::istream& in)
{
  std::string text = utilities::read_text(in);
  process_specification result = detail::parse_process_specification_new(text);
  detail::complete_process_specification(result);
  return result;
}

/// \brief Parses a process specification from a string
/// \param spec_string A string
/// \return The parse result
inline
process_specification
parse_process_specification(const std::string& spec_string)
{
  std::istringstream in(spec_string);
  return parse_process_specification(in);
}

/// \brief Parses a process identifier.
inline
process_identifier parse_process_identifier(std::string text, const data::data_specification& dataspec)
{
  text = utilities::trim_copy(text);

  // unfortunately there is no grammar element for a process identifier, so parsing has to be done in an ad hoc manner
  auto pos = text.find('(');
  if (pos == std::string::npos)
  {
    return process_identifier(core::identifier_string(text), {});
  }
  std::string name    = text.substr(0, pos);
  std::string vardecl = utilities::trim_copy(text.substr(pos + 1));
  vardecl.pop_back();

  core::identifier_string id(name);
  data::variable_list variables = data::parse_variable_declaration_list(vardecl, dataspec);

  return process_identifier(id, variables);
}

/// \brief Parses and type checks a process expression.
/// \param[in] text The input text containing a process expression.
/// \param[in] data_decl A declaration of data and actions ("glob m:Nat; act a:Nat;").
/// \param[in] proc_decl A process declaration ("proc P(n: Nat);").
inline
process_expression parse_process_expression(const std::string& text,
                                            const std::string& data_decl,
                                            const std::string& proc_decl
                                           )
{
  std::string proc_text = utilities::regex_replace(";", " = delta;", proc_decl);
  std::string init_text = "init\n     " + text + ";\n";
  std::string spec_text = data_decl + "\n" + proc_text + "\n" + init_text;
  process_specification spec = parse_process_specification(spec_text);
  return spec.init();
}

/// \brief Parses and type checks a process expression.
/// \param[in] text The input text containing a process expression.
/// \param[in] procspec_text A textual version of a process specification used as context
inline
process_expression parse_process_expression(const std::string& text, const std::string& procspec_text)
{
  std::vector<std::string> keywords{"sort", "var", "eqn", "map", "cons", "act", "glob", "proc", "init"};
  std::pair<std::string, std::string> result = utilities::detail::separate_keyword_section(procspec_text, "init", keywords);
  std::string init_text = "init\n     " + text + ";\n";
  std::string ptext = result.second + init_text;
  process_specification spec = parse_process_specification(ptext);
  return spec.init();
}

/// \brief Parses and type checks a process expression. N.B. Very inefficient!
template <typename VariableContainer>
process_expression parse_process_expression(const std::string& text, const VariableContainer& variables, const process_specification& procspec)
{
  process_specification procspec1 = procspec;
  auto& globvars = procspec1.global_variables();
  globvars.insert(variables.begin(), variables.end());
  std::string ptext = process::pp(procspec1);
  ptext = utilities::regex_replace("\\binit.*;", "init " + text + ";", ptext);
  process_specification procspec2 =
      parse_process_specification(ptext);
  return procspec2.init();
}

template <typename VariableContainer, typename ActionLabelContainer, typename ProcessIdentifierContainer>
process_expression parse_process_expression(const std::string& text,
                                            const VariableContainer& variables = VariableContainer(),
                                            const data::data_specification& dataspec = data::data_specification(),
                                            const ActionLabelContainer& action_labels = std::vector<action_label>(),
                                            const ProcessIdentifierContainer& process_identifiers = ProcessIdentifierContainer(),
                                            const process_identifier* current_equation = nullptr
                                           )
{
  process_expression x = detail::parse_process_expression_new(text);
  x = typecheck_process_expression(x, variables, dataspec, action_labels, process_identifiers, current_equation);
  x = translate_user_notation(x);
  return x;
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_PARSE_H
