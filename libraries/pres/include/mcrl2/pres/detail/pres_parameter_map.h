// Author(s): Jan Friso Groote. Based on mcrl2/pres/detail/pres_parameter_map.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/detail/pres_parameter_map.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_DETAIL_PRES_PARAMETER_MAP_H
#define MCRL2_PRES_DETAIL_PRES_PARAMETER_MAP_H

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/pres/pres.h"
#include <regex>





namespace mcrl2::pres_system::detail
{

/// \brief Data structure for storing the variables that should be expanded by the finite presinst algorithm.
using pres_parameter_map = std::map<core::identifier_string, std::vector<data::variable>>;

/// \brief Returns true if the declaration text matches with the variable d.
inline
bool match_declaration(const std::string& text, const data::variable& d, const data::data_specification& data_spec)
{
  std::vector<std::string> words = utilities::split(text, ":");
  if (words.size() != 2)
  {
    throw mcrl2::runtime_error("invalid parameter declaration: '" + text + "'");
  }
  std::string name = utilities::trim_copy(words[0]);
  std::string type = utilities::trim_copy(words[1]);
  if (name != "*" && core::identifier_string(name) != d.name())
  {
    return false;
  }
  return type == "*" || data::parse_sort_expression(type, data_spec) == d.sort();
}

/// \brief Find parameter declarations that match a given string.
inline
std::vector<data::variable> find_matching_parameters(const pres& p, const std::string& name, const std::set<std::string>& declarations)
{
  std::set<data::variable> result;
  for (const pres_equation& eqn: p.equations())
  {
    const propositional_variable& X = eqn.variable();
    if (name == "*" || name == std::string(X.name()))
    {
      for (const data::variable& v: X.parameters())
      {
        // find a declaration *k that accepts the variable *j
        for (const std::string& declaration: declarations)
        {
          if (match_declaration(declaration, v, p.data()))
          {
            result.insert(v);
            break;
          }
        }
      }
    }
  }
  return std::vector<data::variable>(result.begin(), result.end());
}

/// \brief Parses parameter selection for finite presinst algorithm
inline
pres_parameter_map parse_pres_parameter_map(const pres& p, const std::string& text)
{
  pres_parameter_map result;

  // maps propositional variable name to the corresponding variable declarations, for example:
  // X(b:Bool,c:C) X(d:*) Y(*:Bool) results in the mapping
  //
  // X -> { "b:Bool", "c:C", "d:*" }
  // Y -> { "*:Bool" }
  std::map<std::string, std::set<std::string>> parameter_declarations;

  for (const std::string& s: utilities::split(text, ";"))
  {
    std::string line = utilities::trim_copy(s);
    if (line.empty())
    {
      continue;
    }
    std::regex sre(R"((\*|\w*)\(([:,#*\s\w>-]*)\)\s*)", std::regex::icase);
    std::match_results<std::string::const_iterator> what;
    if (!regex_match(line, what, sre))
    {
      mCRL2log(log::warning) << "ignoring selection '" << line << "'" << std::endl;
      continue;
    }
    std::string X = what[1];
    utilities::trim(X);
    std::string word = what[2];
    utilities::trim(word);
    for (const std::string& parameter: utilities::regex_split(word, "\\s*,\\s*"))
    {
      parameter_declarations[X].insert(parameter);
    }
  }

  // expand the name "*"
  auto q = parameter_declarations.find("*");
  if (q != parameter_declarations.end())
  {
    std::set<std::string> v = q->second;
    parameter_declarations.erase(q);
    for (const pres_equation& eqn: p.equations())
    {
      std::string name = eqn.variable().name();
      parameter_declarations[name].insert(v.begin(), v.end());
    }
  }

  // create a mapping from PRES variable names to the corresponding parameters
  std::map<core::identifier_string, data::variable_list> pres_index;
  for (const pres_equation& eqn: p.equations())
  {
    const propositional_variable& X = eqn.variable();
    pres_index[X.name()] = X.parameters();
  }

  for (const auto& decl: parameter_declarations)
  {
    core::identifier_string name(decl.first);
    std::vector<data::variable> variables = find_matching_parameters(p, decl.first, decl.second);

    // sort variables according to their position in the PRES variable
    std::map<data::variable, std::size_t> m;
    std::size_t index = 0;
    for (const data::variable& v: pres_index[name])
    {
      m[v] = index++;
    }
    std::sort(variables.begin(), variables.end(), [&](const data::variable& x, const data::variable& y) { return m[x] < m[y]; });

    result[name] = variables;
  }

  return result;
}

/// \brief Print a parameter map.
inline
std::ostream& print_pres_parameter_map(std::ostream& out, const pres_parameter_map& m)
{
  for (const auto& p: m)
  {
    out << p.first << " -> " << core::detail::print_list(p.second) << std::endl;
  }
  return out;
}

} // namespace mcrl2::pres_system::detail





#endif // MCRL2_PRES_DETAIL_PRES_PARAMETER_MAP_H
