// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes_parameter_map.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBES_PARAMETER_MAP_H
#define MCRL2_PBES_DETAIL_PBES_PARAMETER_MAP_H

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/text_utility.h"
#include <iostream>
#include <regex>
#include <set>
#include <string>
#include <vector>

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

/// \brief Data structure for storing the variables that should be expanded by the finite pbesinst algorithm.
typedef std::map<core::identifier_string, std::vector<data::variable> > pbes_parameter_map;

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
std::vector<data::variable> find_matching_parameters(const pbes& p, const std::string& name, const std::set<std::string>& declarations)
{
  std::set<data::variable> result;
  for (const pbes_equation& eqn: p.equations())
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

/// \brief Parses parameter selection for finite pbesinst algorithm
inline
pbes_parameter_map parse_pbes_parameter_map(const pbes& p, const std::string& text)
{
  pbes_parameter_map result;

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
    for (const pbes_equation& eqn: p.equations())
    {
      std::string name = eqn.variable().name();
      parameter_declarations[name].insert(v.begin(), v.end());
    }
  }

  for (const auto& decl: parameter_declarations)
  {
    std::vector<data::variable> variables = find_matching_parameters(p, decl.first, decl.second);
    core::identifier_string name(decl.first);
    result[name] = variables;
  }
  return result;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_PARAMETER_MAP_H
