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
#include "mcrl2/data/exists.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/detail/data_construction.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/utilities/identifier_generator.h"
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

  // creates a finite set containing one data expression
  struct make_data_expression_set
  {
    data::data_expression operator()(const data::data_expression& x) const
    {
      data::sort_expression s = x.sort();
      data::data_expression result = data::sort_fset::fset_empty(s);
      result = data::sort_fset::fset_cons(s, x, result);
      return result;
    }
  };

  // transforms the sort expression s to Set(s)
  struct make_set
  {
    data::sort_expression operator()(const data::sort_expression& s) const
    {
      return data::sort_set::set_(s);
    }
  };

  struct make_mapping
  {
    const data::function_symbol_vector& user_mappings;

    make_mapping(const data::function_symbol_vector& user_mappings_)
      : user_mappings(user_mappings_)
    {}

    data::function_symbol operator()(const data::function_symbol& f) const
    {
      std::string name = "Lift" + std::string(f.name());
      data::sort_expression s = f.sort();
      if (data::is_basic_sort(s))
      {
        return data::function_symbol(name, make_set()(s));
      }
      else if (data::is_function_sort(s))
      {
        data::function_sort fs(s);
        return data::function_symbol(name, data::function_sort(atermpp::apply(fs.domain(), make_set()), fs.codomain()));
      }
      else if (data::is_container_sort(s))
      {
        return data::function_symbol(name, make_set()(s));
      }
      throw mcrl2::runtime_error("absinthe algorithm: unsupported sort " + data::pp(s) + " detected!");
      return data::function_symbol();
    }
  };

  struct make_equation
  {
    const data::function_symbol_vector& user_mappings;
    const atermpp::map<data::sort_expression, data::sort_expression>& abstraction;

    make_equation(const data::function_symbol_vector& user_mappings_, const atermpp::map<data::sort_expression, data::sort_expression>& abstraction_)
      : user_mappings(user_mappings_),
        abstraction(abstraction_)
    {}

    atermpp::vector<data::variable> make_variables(const data::sort_expression_list& sorts, const std::string& hint) const
    {
      atermpp::vector<data::variable> result;
      unsigned int i = 0;
      for (data::sort_expression_list::const_iterator j = sorts.begin(); j != sorts.end(); ++i, ++j)
      {
        result.push_back(data::variable(hint + boost::lexical_cast<std::string>(i), *j));
      }
      return result;
    }

    // Let x = [x1:D1, ..., xn:Dn] and X = [X1:Set(D1), ..., Xn:Set(Dn)]. Returns the expression
    //
    // exists x1:D1, ..., xn:Dn . (x1 in X1 /\ ... /\ xn in Xn)
    data::data_expression enumerate_domain(const atermpp::vector<data::variable>& x, const atermpp::vector<data::variable>& X) const
    {
      atermpp::vector<data::data_expression> a;
      atermpp::vector<data::variable>::const_iterator i = x.begin();
      atermpp::vector<data::variable>::const_iterator j = X.begin();
      for (; i != x.end(); ++i, ++j)
      {
        a.push_back(data::detail::create_set_in(*i, *j));
      }
      data::data_expression body = data::lazy::join_and(a.begin(), a.end());
      return data::exists(x, body);
    }

    data::data_equation operator()(const data::function_symbol& F, const data::function_symbol& LiftF) const
    {
      std::string name = "Lift" + std::string(F.name());

      data::variable_list variables;
      data::data_expression condition = data::sort_bool::true_();
      data::data_expression lhs;
      data::data_expression rhs;

      data::sort_expression s = F.sort();

      if (data::is_basic_sort(s))
      {
        lhs = LiftF;
        rhs = data::detail::create_finite_set(F);
        return data::data_equation(variables, condition, lhs, rhs);
      }
      else if (data::is_function_sort(s))
      {
        data::function_sort Fs(F.sort());
        data::function_sort LiftFs(LiftF.sort());

        // TODO: generate these variables in a proper way
        atermpp::vector<data::variable> x = make_variables(Fs.domain(), "x");
        atermpp::vector<data::variable> X = make_variables(LiftFs.domain(), "X");

        lhs = data::application(LiftF, data::data_expression_list(X.begin(), X.end()));
        data::variable y("y", data::detail::get_set_sort(Fs.codomain()));
        data::data_expression Y = data::application(F, data::data_expression_list(x.begin(), x.end()));
        rhs = data::detail::create_set_comprehension(y, data::sort_bool::and_(enumerate_domain(x, X), data::detail::create_set_in(y, Y)));

        return data::data_equation(variables, condition, lhs, rhs);
      }
// TODO: add container sorts
//      else if (data::is_container_sort(s))
//      {
//        return data::data_equation(variables, condition, lhs, rhs);
//      }
      throw mcrl2::runtime_error("absinthe algorithm: unsupported sort " + data::pp(s) + " detected!");
      return data::data_equation(variables, condition, lhs, rhs);
    }
  };

  // add lifted mappings and equations to the data specification
  void lift_data_specification(data::data_specification& dataspec, const data::function_symbol_vector& user_mappings, const atermpp::map<data::sort_expression, data::sort_expression>& abstraction)
  {
    for (data::function_symbol_vector::const_iterator i = user_mappings.begin(); i != user_mappings.end(); ++i)
    {
      data::function_symbol f = make_mapping(user_mappings)(*i);
      dataspec.add_mapping(f);
      std::cout << "added function symbol: " << core::pp(f) << " " << core::pp(f.sort()) << std::endl;

      data::data_equation eq = make_equation(user_mappings, abstraction)(*i, f);
      dataspec.add_equation(eq);
      std::cout << "added equation: " << core::pp(eq) << std::endl;
    }
  }

  void print_abstraction_mapping(const atermpp::map<data::sort_expression, data::sort_expression>& abstraction)
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

    // add lifted versions of the user defined mappings and equations
    lift_data_specification(combined_dataspec, user_dataspec.user_defined_mappings(), abstraction);
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ABSINTHE_H
