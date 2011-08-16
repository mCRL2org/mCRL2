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
#include "mcrl2/data/set.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/exception.h"

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

  struct make_set
  {
    data::sort_expression operator()(const data::sort_expression& s) const
    {
      return data::sort_set::set_(s);
    }
  };

  void add_lifted_mappings(data::data_specification dataspec, const data::function_symbol_vector& user_mappings)
  {
    data::function_symbol_vector result;
    for (data::function_symbol_vector::const_iterator i = user_mappings.begin(); i != user_mappings.end(); ++i)
    {
      std::string name = "Lift" + std::string(i->name());
      data::sort_expression s = i->sort();
      if (data::is_basic_sort(s))
      {
        result.push_back(data::function_symbol(name, make_set()(s)));
      }
      else if (data::is_function_sort(s))
      {
        data::function_sort fs(s);
        result.push_back(
          data::function_symbol(name,
            data::function_sort(atermpp::apply(fs.domain(), make_set()), fs.codomain())
          )
        );
      }
      else if (data::is_container_sort(s))
      {
        result.push_back(data::function_symbol(name, make_set()(s)));
      }
      else
      {
        throw mcrl2::runtime_error("absinthe algorithm: unsupported sort " + data::pp(s) + " detected!");
      }
    }
    for (data::function_symbol_vector::iterator i = result.begin(); i != result.end(); ++i)
    {
//std::cout << "added sort: " << data::pp(*i) << " " << data::pp(i->sort()) << std::endl;
      dataspec.add_mapping(*i);
    }
  }

  void print_mapping(const atermpp::map<data::sort_expression, data::sort_expression>& abstraction)
  {
    for (atermpp::map<data::sort_expression, data::sort_expression>::const_iterator i = abstraction.begin(); i != abstraction.end(); ++i)
    {
      std::cout << data::pp(i->first) << " -> " << data::pp(i->second) << std::endl;
    }
  }

  void run(const pbes<>& p, const std::string& abstraction_mapping_text, const std::string& user_dataspec_text)
  {
    data::data_specification user_dataspec = data::parse_data_specification(user_dataspec_text);
    data::data_specification combined_dataspec = data::parse_data_specification(data::pp(p.data()) + "\n" + user_dataspec_text);

    // parse the abstraction mapping
    atermpp::map<data::sort_expression, data::sort_expression> abstraction = parse_approximation_mapping(abstraction_mapping_text, combined_dataspec);
    print_mapping(abstraction);

    // add lifted versions of the user defined mappings
    add_lifted_mappings(combined_dataspec, user_dataspec.user_defined_mappings());
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ABSINTHE_H
