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

#include <sstream>

#include <boost/algorithm/string/trim.hpp>

#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/atermpp/map.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/exists.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/substitutions.h"
#include "mcrl2/data/detail/data_construction.h"
#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/utilities/identifier_generator.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/exception.h"

namespace mcrl2 {

namespace pbes_system {

  template <typename Term>
  std::string print_term(const Term& x)
  {
    return data::pp(x) + " " + x.to_string();
  }

  template <typename Term>
  std::string print_symbol(const Term& x)
  {
    return data::pp(x) + ": " + data::pp(x.sort());
  }

namespace detail {

  // parse a string like 'tail: List(D) -> List(D)'
  //
  // TODO: replace this by a proper parse function once the current parser has been replaced
  inline
  data::function_symbol parse_function_symbol(std::string text, const std::string& dataspec_text)
  {
    const std::string prefix = "UNIQUE_FUNCTION_SYMBOL_PREFIX";
    boost::algorithm::trim(text);
    std::string::size_type pos = text.find_first_of(':');
    std::string name = boost::algorithm::trim_copy(text.substr(0, pos));
    std::string type = prefix + text.substr(pos);
    std::string spec_text = dataspec_text + "\nmap " + prefix + type + ";\n";
    data::data_specification dataspec = data::parse_data_specification(spec_text);
    data::function_symbol f = dataspec.user_defined_mappings().back();
    data::function_symbol result = data::function_symbol(name, f.sort());
    return result;
  }

  inline
  void print_used_function_symbols(const pbes<>& p)
  {
    std::cout << "--- used function symbols ---" << std::endl;
    std::set<data::function_symbol> find_function_symbols = pbes_system::find_function_symbols(p);
    for (std::set<data::function_symbol>::iterator i = find_function_symbols.begin(); i != find_function_symbols.end(); ++i)
    {
      std::cout << print_symbol(*i) << std::endl;
    }
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

    const sort_expression_substitution_map& sigmaS;
    const function_symbol_substitution_map& sigmaF;

    absinthe_sort_expression_builder(const sort_expression_substitution_map& sigmaS_, const function_symbol_substitution_map& sigmaF_)
      : sigmaS(sigmaS_),
        sigmaF(sigmaF_)
    {}

    data::sort_expression operator()(const data::sort_expression& x)
    {
      sort_expression_substitution_map::const_iterator i = sigmaS.find(x);
      if (i != sigmaS.end())
      {
        return i->second;
      }
      return super::operator()(x);
    }

    data::data_expression operator()(const data::function_symbol& x)
    {
      function_symbol_substitution_map::const_iterator i = sigmaF.find(x);
      if (i != sigmaF.end())
      {
        return i->second;
      }
      throw mcrl2::runtime_error("function symbol " + print_symbol(x) + " not present in the function symbol mapping!");
      return data::data_expression();
    }

    data::data_expression operator()(const data::data_expression& x)
    {
      // first apply the sort and function symbol transformations
      data::data_expression result = super::operator()(x);

      // if it is a variable (in the context of a data expression), wrap it in a set
      if (data::is_variable(x))
      {
        result = data::detail::create_finite_set(result);
      }
      return result;
    }
  };

  struct absinthe_data_expression_builder: public pbes_expression_builder<absinthe_data_expression_builder>
  {
    typedef pbes_expression_builder<absinthe_data_expression_builder> super;
    using super::enter;
    using super::leave;
    using super::operator();

    data::variable_list make_variables(const data::data_expression_list& x, const std::string& hint) const
    {
      atermpp::vector<data::variable> result;
      unsigned int i = 0;
      for (data::data_expression_list::const_iterator j = x.begin(); j != x.end(); ++i, ++j)
      {
        result.push_back(data::variable(hint + boost::lexical_cast<std::string>(i), apply_sigmaS(sigmaS)(j->sort())));
      }
      return data::variable_list(result.begin(), result.end());
    }

    const sort_expression_substitution_map& sigmaS;
    const function_symbol_substitution_map& sigmaF;
    bool m_is_over_approximation;

    data::data_expression lift(const data::data_expression& x)
    {
      return absinthe_sort_expression_builder(sigmaS, sigmaF)(x);
    }

    data::data_expression_list lift(const data::data_expression_list& x)
    {
      return absinthe_sort_expression_builder(sigmaS, sigmaF)(x);
    }

    data::variable_list lift(const data::variable_list& x)
    {
      return absinthe_sort_expression_builder(sigmaS, sigmaF)(x);
    }

    pbes_system::propositional_variable lift(const pbes_system::propositional_variable& x)
    {
      return absinthe_sort_expression_builder(sigmaS, sigmaF)(x);
    }

    absinthe_data_expression_builder(const sort_expression_substitution_map& sigmaS_,
                                     const function_symbol_substitution_map& sigmaF_,
                                     bool is_over_approximation)
      : sigmaS(sigmaS_),
        sigmaF(sigmaF_),
        m_is_over_approximation(is_over_approximation)
    {}

    pbes_expression operator()(const data::data_expression& x)
    {
      data::data_expression x1 = lift(x);
      data::variable var("y", apply_sigmaS(sigmaS)(x.sort()));
      data::variable_list variables = atermpp::make_list(var);
      if (m_is_over_approximation)
      {
        pbes_expression body = and_(data::detail::create_set_in(var, x1), var);
        return exists(variables, body);
      }
      else
      {
        pbes_expression body = imp(data::detail::create_set_in(var, x1), var);
        return forall(variables, body);
      }
    }

    pbes_expression operator()(const propositional_variable_instantiation& x)
    {
      data::data_expression_list e = lift(x.parameters());
      data::variable_list variables = make_variables(x.parameters(), "x");
      data::data_expression_list::iterator i = e.begin();
      data::variable_list::iterator j = variables.begin();
      data::data_expression_vector z;
      for (; i != e.end(); ++i, ++j)
      {
        z.push_back(data::detail::create_set_in(*j, *i));
      }
      data::data_expression q = data::lazy::join_and(z.begin(), z.end());
      if (m_is_over_approximation)
      {
        return exists(variables, and_(q, propositional_variable_instantiation(x.name(), variables)));
      }
      else
      {
        return forall(variables, imp(q, propositional_variable_instantiation(x.name(), variables)));
      }
    }

    pbes_system::pbes_expression operator()(const pbes_system::forall& x)
    {
      return pbes_system::forall(lift(x.variables()), super::operator()(x.body()));
    }

    pbes_system::pbes_expression operator()(const pbes_system::exists& x)
    {
      return pbes_system::exists(lift(x.variables()), super::operator()(x.body()));
    }

    void operator()(pbes_system::pbes_equation& x)
    {
      x.variable() = lift(x.variable());
      x.formula() = super::operator()(x.formula());
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

  std::string parse_right_hand_sides(const std::string& text)
  {
    std::ostringstream out;
    out << "map" << std::endl;
    std::vector<std::string> lines = utilities::regex_split(text, "\\n");
    for (std::vector<std::string>::iterator i = lines.begin(); i != lines.end(); ++i)
    {
      std::vector<std::string> words = utilities::regex_split(*i, ":=");
      if (words.size() == 2)
      {
        out << "  " << words[1] << ";" << std::endl;
      }
    }
    return out.str();
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

  struct apply_sigmaS
  {
    const sort_expression_substitution_map& sigmaS;

    apply_sigmaS(const sort_expression_substitution_map& sigmaS_)
      : sigmaS(sigmaS_)
    {}

    data::sort_expression operator()(const data::sort_expression& s) const
    {
      sort_expression_substitution_map::const_iterator i = sigmaS.find(s);
      if (i != sigmaS.end())
      {
        return i->second;
      }
      return s;
    }
  };

  // function that transforms a function symbol
  struct transform_function_symbol
  {
    const sort_expression_substitution_map& sigmaS;
    std::map<std::string, std::string> unprintable;

    transform_function_symbol(const sort_expression_substitution_map& sigmaS_)
      : sigmaS(sigmaS_)
    {
      unprintable["&&"] = "and";
      unprintable["!"] = "not";
      unprintable["#"] = "len";
      unprintable[">"] = "greater";
      unprintable["<"] = "less";
      unprintable[">="] = "ge";
      unprintable["<="] = "le";
      unprintable["=="] = "eq";
      unprintable["!="] = "neq";
    }

    data::function_symbol operator()(const data::function_symbol& f) const
    {
      std::string name = std::string(f.name());
      std::map<std::string, std::string>::const_iterator i = unprintable.find(name);
      if (i != unprintable.end())
      {
        name = i->second;
      }
      name = "Generated_" + name;
      data::sort_expression s = f.sort();
      if (data::is_basic_sort(s))
      {
        return data::function_symbol(name, apply_sigmaS(sigmaS)(s));
      }
      else if (data::is_function_sort(s))
      {
        data::function_sort fs(s);
        return data::function_symbol(name, data::function_sort(atermpp::apply(fs.domain(), apply_sigmaS(sigmaS)), make_set()(fs.codomain())));
      }
      else if (data::is_container_sort(s))
      {
        return data::function_symbol(name, make_set()(s));
      }
      throw mcrl2::runtime_error("absinthe algorithm: unsupported sort " + print_term(s) + " detected!");
      return data::function_symbol();
    }
  };

  // function that lifts a function symbol
  struct lift_function_symbol
  {
    data::function_symbol operator()(const data::function_symbol& f) const
    {
      std::string name = "Lift" + boost::algorithm::trim_copy(std::string(f.name()));
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
      throw mcrl2::runtime_error("absinthe algorithm (lift): unsupported sort " + print_term(s) + " detected!");
      return data::function_symbol();
    }
  };

  // function that generates an equation from a function symbol and it's corresponding lifted version
  struct generate_lifted_equation
  {
    const sort_expression_substitution_map& abstraction;

    generate_lifted_equation(const sort_expression_substitution_map& abstraction_)
      : abstraction(abstraction_)
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

    data::data_equation operator()(const data::function_symbol& f2, const data::function_symbol& f3) const
    {
std::cout << "lifting equation " << print_symbol(f2) << " " << print_symbol(f3) << std::endl;

      data::variable_list variables;
      data::data_expression condition = data::sort_bool::true_();
      data::data_expression lhs;
      data::data_expression rhs;

      data::sort_expression s2 = f2.sort();

      if (data::is_basic_sort(s2))
      {
        lhs = f3;
        rhs = data::detail::create_finite_set(f2);
        return data::data_equation(variables, condition, lhs, rhs);
      }
      else if (data::is_function_sort(s2))
      {
        data::function_sort fs2(f2.sort());
        data::function_sort fs3(f3.sort());

        // TODO: generate these variables in a proper way
        atermpp::vector<data::variable> x = make_variables(fs2.domain(), "x");
        atermpp::vector<data::variable> X = make_variables(fs3.domain(), "X");

        lhs = data::application(f3, data::data_expression_list(X.begin(), X.end()));
        data::variable y("y", data::detail::get_set_sort(fs2.codomain()));
        data::data_expression Y = data::application(f2, data::data_expression_list(x.begin(), x.end()));
        rhs = data::detail::create_set_comprehension(y, data::sort_bool::and_(enumerate_domain(x, X), data::detail::create_set_in(y, Y)));

        return data::data_equation(variables, condition, lhs, rhs);
      }
//      else if (data::is_container_sort(s2))
//      {
//        return data::data_equation(variables, condition, lhs, rhs);
//      }
      throw mcrl2::runtime_error("absinthe algorithm (eq): unsupported sort " + print_term(s2) + " detected!");
      return data::data_equation(variables, condition, lhs, rhs);
    }
  };

  // add lifted mappings and equations to the data specification
  void lift_data_specification(data::data_specification& dataspec, const data::function_symbol_vector& user_mappings, const sort_expression_substitution_map& sigmaS, function_symbol_substitution_map& sigmaF)
  {
    for (function_symbol_substitution_map::iterator i = sigmaF.begin(); i != sigmaF.end(); ++i)
    {
      data::function_symbol f1 = i->first;
      data::function_symbol f2 = i->second;
      data::function_symbol f3 = lift_function_symbol()(f2);

      // TODO: is this needed?
      dataspec.add_mapping(f2);
      dataspec.add_mapping(f3);

      mCRL2log(log::debug1) << "added function symbol: " << core::pp(f2) << " " << core::pp(f2.sort()) << std::endl;
      mCRL2log(log::debug1) << "added function symbol: " << core::pp(f3) << " " << core::pp(f3.sort()) << std::endl;

      // update sigmaF
      i->second = f3;

      // make an equation for the lifted function symbol f
      data::data_equation eq = generate_lifted_equation(sigmaS)(f2, f3);
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

  // adds lifted versions of used function symbols that are not specified by the user to sigmaF, and adds them to the data specification as well
  void complete_function_symbol_mapping(const pbes<>& p, const sort_expression_substitution_map& sigmaS, function_symbol_substitution_map& sigmaF, data::data_specification& dataspec)
  {
    std::set<data::function_symbol> used_function_symbols = pbes_system::find_function_symbols(p);
    for (std::set<data::function_symbol>::iterator i = used_function_symbols.begin(); i != used_function_symbols.end(); ++i)
    {
      data::function_symbol f1 = *i;
      if (sigmaF.find(f1) == sigmaF.end())
      {
        data::function_symbol f2 = transform_function_symbol(sigmaS)(f1);
        sigmaF[f1] = f2;
        dataspec.add_mapping(f2);
      }
    }
  }

  void print_fsvec(const data::function_symbol_vector& v, const std::string& msg) const
  {
    std::cout << "--- " << msg << std::endl;
    for (data::function_symbol_vector::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      std::cout << print_symbol(*i) << std::endl;
    }
  }

  void print_fsmap(const function_symbol_substitution_map& v, const std::string& msg) const
  {
    std::cout << "--- " << msg << std::endl;
    for (function_symbol_substitution_map::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      std::cout << print_symbol(i->first) << "  -->  " << print_symbol(i->second) << std::endl;
    }
  }

  void run(pbes<>& p, const std::string& sort_expression_mapping_text, const std::string& function_symbol_mapping_text, std::string user_dataspec_text, bool is_over_approximation)
  {
    // first we have to add right hand sides of the function symbol mappings to the user dataspec
    user_dataspec_text = user_dataspec_text + "\n" + parse_right_hand_sides(function_symbol_mapping_text) + "\n";

    std::string extra = "\nsort\n";
    for (data::sort_expression_vector::const_iterator i = p.data().user_defined_sorts().begin(); i != p.data().user_defined_sorts().end(); ++i)
    {
      extra = extra + "  " + data::pp(*i) + ";\n";
    }
    for (data::alias_vector::const_iterator i = p.data().user_defined_aliases().begin(); i != p.data().user_defined_aliases().end(); ++i)
    {
      extra = extra + "  " + data::pp(*i) + ";\n";
    }
    std::cout << "--- user spec " << std::endl;
    std::cout << user_dataspec_text + extra << std::endl;
    data::data_specification user_dataspec = data::parse_data_specification(user_dataspec_text + extra);
    data::data_specification combined_dataspec = data::parse_data_specification(data::pp(p.data()) + "\n" + user_dataspec_text);

    // sort expressions replacements (specified by the user)
    sort_expression_substitution_map sigmaS = parse_sort_expression_mapping(sort_expression_mapping_text, combined_dataspec);
    print_mapping(sigmaS, "\n--- sigmaS ---");

    // function symbol replacements (specified by the user)
    function_symbol_substitution_map sigmaF = parse_function_symbol_mapping(function_symbol_mapping_text, combined_dataspec);

print_fsmap(sigmaF, "--- sigmaF before completion ---");

    // generate mapping f1 -> f2 for missing function symbols
    complete_function_symbol_mapping(p, sigmaS, sigmaF, combined_dataspec);

print_fsmap(sigmaF, "--- sigmaF after completion ---");

    // add lifted versions of the user defined mappings and equations
    print_fsvec(user_dataspec.user_defined_mappings(), "user_dataspec.user_defined_mappings()");
    print_fsmap(sigmaF, "sigmaF");

    // before: the mapping sigmaF is f1 -> f2
    // after: the mapping sigmaF is f1 -> f3
    // after: f2 and f3 have been added to combined_dataspec
    // after: equations for f3 have been added to combined_dataspec
    lift_data_specification(combined_dataspec, user_dataspec.user_defined_mappings(), sigmaS, sigmaF);

    std::cout << "--- pbes before ---" << std::endl;
    std::cout << pbes_system::pp(p) << std::endl;

    p.data() = combined_dataspec;

    // then transform the data expressions and the propositional variable instantiations
    absinthe_data_expression_builder(sigmaS, sigmaF, is_over_approximation)(p);

    std::cout << "--- pbes after ---" << std::endl;
    std::cout << pbes_system::pp(p) << std::endl;
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ABSINTHE_H
