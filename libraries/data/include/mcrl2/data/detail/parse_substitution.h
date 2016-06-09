// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/parse_substitution.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DETAIL_PARSE_SUBSTITUTION_H
#define MCRL2_DATA_DETAIL_PARSE_SUBSTITUTION_H

#include <string>
#include <vector>
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2
{

namespace data
{

namespace detail
{

/// \brief Parses a string of the form "b: Bool := true, n: Nat := 0", and adds
/// the substitutions to the substition function sigma. If the string is surrounded
/// by brackets (e.g. "[b: Bool := true, n: Nat := 0]"), these are ignored.
template <typename MutableSubstitution>
void parse_substitution(std::string text, MutableSubstitution& sigma, const data_specification& data_spec = data::data_specification())
{
  text = utilities::regex_replace("^\\s*\\[", "", text);
  text = utilities::regex_replace("\\]\\s*$", "", text);
  std::cout << "text = " << text << std::endl;
  std::vector<std::string> substitutions = utilities::split(text, ";");
  for (const std::string& substitution: substitutions)
  {
    std::vector<std::string> words = utilities::regex_split(substitution, ":=");
    if (words.size() != 2)
    {
      continue;
    }
    data::variable v = data::parse_variable(words[0], data_spec);
    data::data_expression e = data::parse_data_expression(words[1], data_spec);
    sigma[v] = e;
  }
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_PARSE_SUBSTITUTION_H
