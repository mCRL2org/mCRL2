// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/parse_substitutions.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DETAIL_PARSE_SUBSTITUTIONS_H
#define MCRL2_DATA_DETAIL_PARSE_SUBSTITUTIONS_H

#include <string>
#include <vector>
#include "mcrl2/core/text_utility.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/parser.h"

namespace mcrl2 {

namespace data {

namespace detail {

  /// \brief Parses a string of the form "b: Bool := true, n: Nat := 0", and adds
  /// the substitutions to the substition function sigma.
  template <typename SubstitutionFunction>
  void parse_substitutions(std::string text, SubstitutionFunction& sigma)
  {
    std::vector<std::string> substitutions = core::split(text, ";");
    for (std::vector<std::string>::iterator i = substitutions.begin(); i != substitutions.end(); ++i)
    {
      std::vector<std::string> words = core::regex_split(*i, ":=");
      if (words.size() != 2)
      {
        continue;
      }
      data::variable v = data::parse_variable(words[0]);
      data::data_expression e = data::parse_data_expression(words[1]);
      sigma[v] = e;
    }
  }

  /// \brief Parses a string of the form "b: Bool := true, n: Nat := 0", and adds
  /// the substitutions to the substition function sigma.
  template <typename SubstitutionFunction>
  void parse_substitutions(std::string text, const data_specification &data_spec, SubstitutionFunction& sigma)
  {
    std::vector<std::string> substitutions = core::split(text, ";");
    for (std::vector<std::string>::iterator i = substitutions.begin(); i != substitutions.end(); ++i)
    {
      std::vector<std::string> words = core::regex_split(*i, ":=");
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

#endif // MCRL2_DATA_DETAIL_PARSE_SUBSTITUTIONS_H
