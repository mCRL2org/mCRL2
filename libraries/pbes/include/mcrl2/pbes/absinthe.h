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

#include <boost/algorithm/string/trim.hpp>

#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/atermpp/map.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/exists.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/substitutions.h"
#include "mcrl2/data/detail/data_construction.h"
#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/utilities/identifier_generator.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/exception.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  // parse a string like 'tail: List(D) -> List(D)'
  //
  // TODO: replace this by a proper parse function once the current parser has been replaced
  inline
  data::function_symbol parse_function_symbol(const std::string& text, const std::string& dataspec_text)
  {
    const std::string prefix = "UNIQUE_FUNCTION_SYMBOL_PREFIX";
    std::string spec_text = dataspec_text + "\nmap " + prefix + boost::algorithm::trim_copy(text) + ";\n";
    data::data_specification dataspec = data::parse_data_specification(spec_text);
    data::function_symbol f = dataspec.user_defined_mappings().back();
    std::string name = std::string(f.name());
    name = name.substr(prefix.size());
    data::function_symbol result = data::function_symbol(name, f.sort());
    return result;
  }

} // namespace detail

struct absinthe_algorithm
{
  typedef atermpp::map<data::sort_expression, data::sort_expression> sort_expression_substitution_map;
  typedef atermpp::map<data::function_symbol, data::function_symbol> function_symbol_substitution_map;

  struct absinthe_sort_expression_builder: public sort_expression_builder<absinthe_sort_expression_builder>
  {
    typedef sort_expression_builder<absinthe_sort_expression_builder> super;
    using super::enter;
    using super::leave;
    using super::operator();

    const sort_expression_substitution_map& m_sort_expression_substitutions;
    const function_symbol_substitution_map& m_function_symbol_substitutions;

    absinthe_sort_expression_builder(const sort_expression_substitution_map& sigmaS, const function_symbol_substitution_map& sigmaF)
      : m_sort_expression_substitutions(sigmaS),
        m_function_symbol_substitutions(sigmaF)
    {}

    data::sort_expression operator()(const data::sort_expression& x)
    {
      sort_expression_substitution_map::const_iterator i = m_sort_expression_substitutions.find(x);
      if (i != m_sort_expression_substitutions.end())
      {
        return i->second;
      }
      return super::operator()(x);
    }

    data::data_expression operator()(const data::function_symbol& x)
    {
      function_symbol_substitution_map::const_iterator i = m_function_symbol_substitutions.find(x);
      if (i != m_function_symbol_substitutions.end())
      {
        return i->second;
      }
      return super::operator()(x);
    }
  };

  struct absinthe_data_expression_builder: public pbes_expression_builder<absinthe_data_expression_builder>
  {
    typedef pbes_expression_builder<absinthe_data_expression_builder> super;
    using super::enter;
    using super::leave;
    using super::operator();

    const function_symbol_substitution_map& m_function_symbol_substitutions;
    bool m_is_over_approximation;

    absinthe_data_expression_builder(const function_symbol_substitution_map& sigmaF,
                                     bool is_over_approximation)
      : m_function_symbol_substitutions(sigmaF),
        m_is_over_approximation(is_over_approximation)
    {}

    pbes_expression operator()(const data::data_expression& x)
    {
      // apply function symbol substitutions to x
      data::data_expression x1 = super::operator()(x);

      data::variable var("y", data::sort_bool::bool_());
      data::variable_list variables = atermpp::make_list(var);
      pbes_expression body = imp(data::detail::create_set_in(var, x1), var);
      return forall(variables, body);
    }

    pbes_expression operator()(const propositional_variable_instantiation& x)
    {
      return x;
    }
  };

  sort_expression_substitution_map parse_sort_expression_mapping(const std::string& text, const data::data_specification& dataspec)
  {
    sort_expression_substitution_map result;

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

  function_symbol_substitution_map parse_function_symbol_mapping(const std::string& text, const data::data_specification& dataspec)
  {
    function_symbol_substitution_map result;
    std::string dataspec_text = data::pp(dataspec);

    std::vector<std::string> lines = utilities::regex_split(text, "\\n");
    for (std::vector<std::string>::iterator i = lines.begin(); i != lines.end(); ++i)
    {
      std::vector<std::string> words = utilities::regex_split(*i, ":=");
      if (words.size() == 2)
      {
        data::function_symbol lhs = detail::parse_function_symbol(words[0], dataspec_text);
        data::function_symbol rhs = detail::parse_function_symbol(words[1], dataspec_text);
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

  // function that lifts a function symbol
  struct lift_function_symbol
  {
    const data::function_symbol_vector& user_mappings;

    lift_function_symbol(const data::function_symbol_vector& user_mappings_)
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

  // function that generates an equation from a function symbol and it's corresponding lifted version
  struct generate_lifted_equation
  {
    const data::function_symbol_vector& user_mappings;
    const sort_expression_substitution_map& abstraction;

    generate_lifted_equation(const data::function_symbol_vector& user_mappings_, const sort_expression_substitution_map& abstraction_)
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
  void lift_data_specification(data::data_specification& dataspec, const data::function_symbol_vector& user_mappings, const sort_expression_substitution_map& sigmaS, const function_symbol_substitution_map& sigmaF)
  {
    for (data::function_symbol_vector::const_iterator i = user_mappings.begin(); i != user_mappings.end(); ++i)
    {
      // lift the function symbol *i
      data::function_symbol f = lift_function_symbol(user_mappings)(*i);
      dataspec.add_mapping(f);
      mCRL2log(log::debug1) << "added function symbol: " << core::pp(f) << " " << core::pp(f.sort()) << std::endl;

      // make an equation for the lifted function symbol f
      data::data_equation eq = generate_lifted_equation(user_mappings, sigmaS)(*i, f);
      dataspec.add_equation(eq);
      mCRL2log(log::debug1) << "added equation: " << core::pp(eq) << std::endl;
    }
  }

  template <typename Map>
  void print_mapping(const Map& m, const std::string& message = "")
  {
    std::cout << message << std::endl;
    for (typename Map::const_iterator i = m.begin(); i != m.end(); ++i)
    {
      std::cout << data::pp(i->first) << " -> " << data::pp(i->second) << std::endl;
    }
  }

  void run(pbes<>& p, const std::string& sort_expression_mapping_text, const std::string& function_symbol_mapping_text, const std::string& user_dataspec_text, bool is_over_approximation)
  {
    data::data_specification user_dataspec = data::parse_data_specification(user_dataspec_text);
    data::data_specification combined_dataspec = data::parse_data_specification(data::pp(p.data()) + "\n" + user_dataspec_text);

    // sort expressions replacements (specified by the user)
    sort_expression_substitution_map sigmaS = parse_sort_expression_mapping(sort_expression_mapping_text, combined_dataspec);
    print_mapping(sigmaS, "--- sigmaS ---");

    // function symbol replacements (specified by the user)
    function_symbol_substitution_map sigmaF = parse_function_symbol_mapping(function_symbol_mapping_text, combined_dataspec);
    print_mapping(sigmaF, "--- sigmaF ---");
    for (function_symbol_substitution_map::iterator i = sigmaF.begin(); i != sigmaF.end(); ++i)
    {
      i->second = lift_function_symbol(user_dataspec.user_defined_mappings())(i->second);
    }
    print_mapping(sigmaF, "--- sigmaF ---");

    // add lifted versions of the user defined mappings and equations
    lift_data_specification(combined_dataspec, user_dataspec.user_defined_mappings(), sigmaS, sigmaF);

    std::cout << "--- data specification extension ---" << std::endl;
    std::cout << user_dataspec_text << std::endl;

    std::cout << "--- pbes before ---" << std::endl;
    std::cout << pbes_system::pp(p) << std::endl;

    p.data() = combined_dataspec;

    // first transform the sort expressions
    absinthe_sort_expression_builder builder(sigmaS, sigmaF);
    builder(p);

    // then transform the data expressions and the propositional variable instantiations
    absinthe_data_expression_builder(sigmaF, is_over_approximation)(p);

    std::cout << "--- pbes after ---" << std::endl;
    std::cout << pbes_system::pp(p) << std::endl;
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ABSINTHE_H
