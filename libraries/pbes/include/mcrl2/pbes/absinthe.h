// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/absinthe.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_ABSINTHE_H
#define MCRL2_PBES_ABSINTHE_H

#include "mcrl2/atermpp/map.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2 {

namespace pbes_system {

struct absinthe_algorithm
{
  atermpp::map<data::sort_expression, data::sort_expression> parse_approximation_mapping(const std::string& text, const data::data_specification& dataspec)
  {
    atermpp::map<data::sort_expression, data::sort_expression> result;

    std::vector<std::string> lines = utilities::regex_split(text, "\\n");
    for (std::vector<std::string>::iterator i = lines.begin(); i != lines.end(); ++i)
    {
      std::vector<std::string> words = utilities::regex_split(*i, ":=");
      if (words.size() == 2)
      {
        data::sort_expression lhs = data::parse_sort_expression(words[0], dataspec);
        data::sort_expression rhs = data::parse_sort_expression(words[1], dataspec);
        result[lhs] = rhs;
      }
    }
    return result;
  }

  void print_mapping(const atermpp::map<data::sort_expression, data::sort_expression>& abstraction)
  {
    for (atermpp::map<data::sort_expression, data::sort_expression>::const_iterator i = abstraction.begin(); i != abstraction.end(); ++i)
    {
      std::cout << data::pp(i->first) << " -> " << data::pp(i->second) << std::endl;
    }
  }

  void run(const pbes<>& p, const std::string& abstraction_mapping_text, const std::string& dataspec_text)
  {
    // merge the data specifications
    std::string text = data::pp(p.data()) + "\n" + dataspec_text;
    data::data_specification dataspec = data::parse_data_specification(text);

    // parse the abstraction mapping
    atermpp::map<data::sort_expression, data::sort_expression> abstraction = parse_approximation_mapping(abstraction_mapping_text, dataspec);
    print_mapping(abstraction);
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ABSINTHE_H
