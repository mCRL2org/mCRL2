// Author(s): Wieger Wesselink, Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/parameter_selection.h
/// \brief

#ifndef MCRL2_LPS_DETAIL_PARAMETER_SELECTION_H
#define MCRL2_LPS_DETAIL_PARAMETER_SELECTION_H

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/parse.h"
#include <regex>

namespace mcrl2::lps::detail
{

/// \brief Returns true if the selection name:type matches with the variable v.
inline
bool match_selection(const data::variable& v, const std::string& name, const std::string& type, const data::data_specification& data_spec)
{
  if (name != "*" && core::identifier_string(name) != v.name())
  {
    return false;
  }
  return type == "*" || data::parse_sort_expression(type, data_spec) == v.sort();
}

/// \brief Find parameter declarations that match a given string.
template <typename OutputIterator>
inline
std::vector<data::variable> find_matching_parameters(
  const data::variable_list& params,
  const data::data_specification& dataspec,
  const std::vector<std::pair<std::string, std::string>>& selections,
  OutputIterator o
  )
{
  std::set<data::variable> result;
  for (const data::variable& v: params)
  {
    for (const auto& [name, type]: selections)
    {
      if (match_selection(v, name, type, dataspec))
      {
        *o = v;
      }
    }
  }
  return std::vector<data::variable>(result.begin(), result.end());
}

/// \brief Parses parameter selection for finite pbesinst algorithm
inline
std::vector<data::variable> parse_lps_parameter_selection(const data::variable_list& params, const data::data_specification& dataspec, const std::string& text)
{
  std::vector<data::variable> result;

  std::string line = utilities::trim_copy(text);
  if (line.empty())
  {
    throw mcrl2::runtime_error("The parameter selection argument is empty.");
  }
  std::regex sre(R"(\s*(\w[\w']*|\*)\s*:\s*(\w[\w']*|\*)\s*)", std::regex::icase);
  std::vector<std::pair<std::string, std::string>> selections;
  for (const std::string& var_sort: utilities::split(text, ","))
  {
    std::match_results<std::string::const_iterator> what;
    if (!std::regex_match(var_sort, what, sre))
    {
      mCRL2log(log::warning) << "Ignoring " << var_sort << " since it does not follow the necessary format (NAME:SORT)." << std::endl;
      continue;
    }

    selections.emplace_back(what[1], what[2]);
  }

  find_matching_parameters(params, dataspec, selections, std::inserter(result, result.end()));

  // sort the parameters in result according to their position in params
  std::map<data::variable, std::size_t> m;
  std::size_t index = 0;
  for (const data::variable& v: params)
  {
    m[v] = index++;
  }
  std::sort(result.begin(), result.end(), [&](const data::variable& x, const data::variable& y) { return m[x] < m[y]; });

  return result;
}

} // namespace mcrl2::lps::detail

#endif // MCRL2_LPS_DETAIL_PARAMETER_SELECTION_H
