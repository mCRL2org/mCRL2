// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes_parameter_map.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBES_PARAMETER_MAP_H
#define MCRL2_PBES_DETAIL_PBES_PARAMETER_MAP_H

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <boost/algorithm/string.hpp>
#include <boost/xpressive/xpressive.hpp>
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/data/parse.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

/// \brief Data structure for storing the variables that should be expanded by the finite pbesinst algorithm.
typedef atermpp::map<core::identifier_string, std::vector<data::variable> > pbes_parameter_map;

/// \brief Returns true if the declaration text matches with the variable d.
inline
bool match_declaration(const std::string& text, const data::variable& d, const data::data_specification& data_spec)
{
  std::vector<std::string> words = utilities::split(text, ":");
  if (words.size() != 2)
  {
    throw mcrl2::runtime_error("invalid parameter declaration: '" + text + "'");
  }
  std::string name = boost::trim_copy(words[0]);
  std::string type = boost::trim_copy(words[1]);
  if (name != "*" && core::identifier_string(name) != d.name())
  {
    return false;
  }
  if (type != "*" && data::parse_sort_expression(type, data_spec) != d.sort())
  {
    return false;
  }
  return true;
}

/// \brief Find parameter declarations that match a given string.
inline
std::vector<data::variable> find_matching_parameters(const pbes<>& p, const std::string& name, const std::set<std::string>& declarations)
{
  std::set<data::variable> result;
  for (atermpp::vector<pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
  {
    propositional_variable X = i->variable();
    if (name == "*" || (name == std::string(X.name())))
    {
      data::variable_list v = X.parameters();
      for (data::variable_list::const_iterator j = v.begin(); j != v.end(); ++j)
      {
        // find a declaration *k that accepts the variable *j
        for (std::set<std::string>::const_iterator k = declarations.begin(); k != declarations.end(); ++k)
        {
          if (match_declaration(*k, *j, p.data()))
          {
            result.insert(*j);
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
pbes_parameter_map parse_pbes_parameter_map(const pbes<>& p, const std::string& text)
{
  using namespace boost::xpressive;
  pbes_parameter_map result;

  // maps propositional variable name to the corresponding variable declarations, for example:
  // X(b:Bool,c:C) X(d:*) Y(*:Bool) results in the mapping
  //
  // X -> { "b:Bool", "c:C", "d:*" }
  // Y -> { "*:Bool" }
  typedef std::map<std::string, std::set<std::string> > name_map;
  name_map parameter_declarations;

  std::vector<std::string> lines = utilities::split(text, ";");
  for (std::vector<std::string>::iterator i = lines.begin(); i != lines.end(); ++i)
  {
    std::string line = boost::trim_copy(*i);
    if (line.empty())
    {
      continue;
    }
    sregex sre = sregex::compile("(\\*|\\w*)\\(([:,#*\\s\\w>-]*)\\)\\s*", regex_constants::icase);
    match_results<std::string::const_iterator> what;
    if (!regex_match(line, what, sre))
    {
      std::cerr << "warning: ignoring selection '" << line << "'" << std::endl;
      continue;
    }
    std::string X = what[1];
    boost::trim(X);
    std::string word = what[2];
    boost::trim(word);
    std::vector<std::string> parameters = utilities::regex_split(word, "\\s*,\\s*");
    for (std::vector<std::string>::iterator j = parameters.begin(); j != parameters.end(); ++j)
    {
      parameter_declarations[X].insert(*j);
    }
  }

  // expand the name "*"
  name_map::iterator q = parameter_declarations.find("*");
  if (q != parameter_declarations.end())
  {
    std::set<std::string> v = q->second;
    parameter_declarations.erase(q);
    for (atermpp::vector<pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
    {
      std::string name = i->variable().name();
      parameter_declarations[name].insert(v.begin(), v.end());
    }
  }

  for (name_map::const_iterator k = parameter_declarations.begin(); k != parameter_declarations.end(); ++k)
  {
    std::vector<data::variable> variables = find_matching_parameters(p, k->first, k->second);
    core::identifier_string name(k->first);
    result[name] = variables;
  }
  return result;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_PARAMETER_MAP_H
