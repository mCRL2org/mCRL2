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

#define MCRL2_DEBUG_DATA_CONSTRUCTION

#include <algorithm>
#include <sstream>
#include <utility>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/atermpp/map.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/print.h"
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

  inline
  pbes_expression make_exists(const data::variable_list& variables, const pbes_expression& body)
  {
    if (variables.empty())
    {
      return body;
    }
    return pbes_system::exists(variables, body);
  }

  inline
  pbes_expression make_forall(const data::variable_list& variables, const pbes_expression& body)
  {
    if (variables.empty())
    {
      return body;
    }
    return pbes_system::forall(variables, body);
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

  // Returns true if f appears as a structured sort constructor in dataspec.
  inline
  bool is_structured_sort_constructor(const data::data_specification& dataspec, const data::function_symbol& f)
  {
    for (data::alias_vector::const_iterator i = dataspec.user_defined_aliases().begin(); i != dataspec.user_defined_aliases().end(); ++i)
    {
      if (f.sort() != i->name())
      {
        continue;
      }
      data::sort_expression s = i->reference();
      if (data::is_structured_sort(s))
      {
        data::structured_sort ss = s;
        data::function_symbol_vector v = ss.constructor_functions();
        for (data::function_symbol_vector::iterator j = v.begin(); j != v.end(); ++j)
        {
          data::function_symbol g = *j;
          if (f.name() == g.name())
          {
            return true;
          }
        }
      }
    }
    return false;
  }

  inline
  void print_used_function_symbols(const pbes<>& p)
  {
    mCRL2log(log::verbose) << "--- used function symbols ---" << std::endl;
    std::set<data::function_symbol> find_function_symbols = pbes_system::find_function_symbols(p);
    for (std::set<data::function_symbol>::iterator i = find_function_symbols.begin(); i != find_function_symbols.end(); ++i)
    {
      mCRL2log(log::verbose) << print_symbol(*i) << std::endl;
    }
  }

  // Separates all sections with a keyword from the other keyword sections
  // Returns a pair containing consisiting of the keyword section and the other keyword sections
  inline
  std::pair<std::string, std::string> separate_keyword_section(const std::string& text, const std::string& keyword, const std::vector<std::string>& all_keywords)
  {
    std::ostringstream out1; // will contain the keyword sections
    std::ostringstream out2; // will contain the other keyword sections

    std::string regex_keyword = "\\b" + keyword + "\\b";
    // create a regex that looks like this: "(\\beqn\\b)|(\\bcons\\b)|(\\bmap\\b)|(\\bvar\\b)"
    std::vector<std::string> v = all_keywords;
    v.erase(std::remove(v.begin(), v.end(), keyword), v.end()); // erase keyword from v
    for (std::vector<std::string>::iterator i = v.begin(); i != v.end(); ++i)
    {
      *i = "(\\b" + *i + "\\b)";
    }
    std::string regex_other_keywords = boost::algorithm::join(v, "|");

    std::vector<std::string> specs = utilities::regex_split(text, regex_keyword);
    if (text.find(keyword) != 0 && !specs.empty())
    {
      out2 << specs.front() << std::endl;
      specs.erase(specs.begin());
    }
    for (std::vector<std::string>::iterator i = specs.begin(); i != specs.end(); ++i)
    {
      // strip trailing map/cons/var/eqn declarations
      std::vector<std::string> v = utilities::regex_split(*i, regex_other_keywords);
      if (!v.empty())
      {
        out1 << "  " << v.front();
        out2 << i->substr(v.front().size());
      }
    }
    std::string s1 = out1.str();
    if (!s1.empty())
    {
      s1 = keyword + "\n" + s1;
    }
    return std::make_pair(s1 + "\n", out2.str() + "\n");
  }

  // Separates the sort declarations from the map/cons/var/eqn declarations
  // Returns a pair containing consisiting of the combined sort spec and the combined map/eqn/var/cons declarations
  inline
  std::pair<std::string, std::string> separate_sort_declarations(const std::string& text)
  {
    std::string keyword = "sort";
    std::vector<std::string> all_keywords;
    all_keywords.push_back("sort");
    all_keywords.push_back("var");
    all_keywords.push_back("eqn");
    all_keywords.push_back("map");
    all_keywords.push_back("cons");
    return separate_keyword_section(text, keyword, all_keywords);
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
        // pbes_expression body = and_(data::detail::create_set_in(var, x1), var);
        // return make_exists(variables, body);
        return data::sort_bool::not_(data::detail::create_set_in(data::sort_bool::false_(), x1));
      }
      else
      {
        // pbes_expression body = imp(data::detail::create_set_in(var, x1), var);
        // return make_forall(variables, body);
        return data::detail::create_set_in(data::sort_bool::true_(), x1);
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
        return make_exists(variables, and_(q, propositional_variable_instantiation(x.name(), variables)));
      }
      else
      {
        return make_forall(variables, imp(q, propositional_variable_instantiation(x.name(), variables)));
      }
    }

    pbes_system::pbes_expression operator()(const pbes_system::forall& x)
    {
      return make_forall(lift(x.variables()), super::operator()(x.body()));
    }

    pbes_system::pbes_expression operator()(const pbes_system::exists& x)
    {
      return make_exists(lift(x.variables()), super::operator()(x.body()));
    }

    void operator()(pbes_system::pbes_equation& x)
    {
      x.variable() = lift(x.variable());
      x.formula() = super::operator()(x.formula());
    }

    template <typename Container>
    void operator()(pbes_system::pbes<Container>& x)
    {
      super::operator()(x.equations());
      pbes_expression kappa = (*this)(x.initial_state());
      core::identifier_string name("GeneratedZ");
      propositional_variable Z(name, data::variable_list());
      x.equations().push_back(pbes_equation(fixpoint_symbol::mu(), Z, kappa));
      x.initial_state() = propositional_variable_instantiation(name, data::data_expression_list());
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

  // Parse the right hand sides of the function symbol mapping in text, and add them to dataspec if needed
  void parse_right_hand_sides(const std::string& text, data::data_specification& dataspec)
  {
    std::string dataspec_text = data::pp(dataspec);
    std::vector<std::string> lines = utilities::regex_split(text, "\\n");
    for (std::vector<std::string>::iterator i = lines.begin(); i != lines.end(); ++i)
    {
      std::vector<std::string> words = utilities::regex_split(*i, ":=");
      if (words.size() == 2)
      {
        data::function_symbol f = pbes_system::detail::parse_function_symbol(words[1], dataspec_text);
        if (!pbes_system::detail::is_structured_sort_constructor(dataspec, f))
        {
          dataspec.add_mapping(f);
        }
      }
    }
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
  struct lift_function_symbol_1_2
  {
    const sort_expression_substitution_map& sigmaS;
    std::map<std::string, std::string> unprintable;

    lift_function_symbol_1_2(const sort_expression_substitution_map& sigmaS_)
      : sigmaS(sigmaS_)
    {
      unprintable["&&"] = "and";
      unprintable["!"] = "not";
      unprintable["#"] = "len";
      unprintable["+"] = "plus";
      unprintable["-"] = "minus";
      unprintable[">"] = "greater";
      unprintable["<"] = "less";
      unprintable[">="] = "ge";
      unprintable["<="] = "le";
      unprintable["=="] = "eq";
      unprintable["!="] = "neq";
      unprintable["[]"] = "emptylist";
      unprintable["++"] = "concat";
      unprintable["<|"] = "snoc";
      unprintable["|>"] = "cons";
      unprintable["@cNat"] = "cNat";
      unprintable["@cDub"] = "cDub";
      unprintable["@c0"] = "c0";
      unprintable["@c1"] = "c1";
    }

    void check_sort(const data::function_symbol& f, const data::sort_expression& s) const
    {
      if (sigmaS.find(s) != sigmaS.end())
      {
        throw mcrl2::runtime_error("could not lift function symbol " + print_symbol(f) + " because there is a user defined abstraction for sort " + data::pp(s));
      }
    }

    data::function_symbol operator()(const data::function_symbol& f) const
    {
      //mCRL2log(log::verbose) << "lift_function_symbol_1_2 f = " << print_symbol(f) << std::endl;
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
        check_sort(f, s);
        return data::function_symbol(name, s);
      }
      else if (data::is_function_sort(s))
      {
        data::function_sort fs(s);

        // check the sorts
        data::sort_expression_list domain = fs.domain();
        for (data::sort_expression_list::iterator i = domain.begin(); i != domain.end(); ++i)
        {
          check_sort(f, *i);
        }
        check_sort(f, fs.codomain());

        return data::function_symbol(name, data::function_sort(fs.domain(), make_set()(fs.codomain())));
      }
      else if (data::is_container_sort(s))
      {
        return data::function_symbol(name, s);
      }
      throw mcrl2::runtime_error("absinthe algorithm: unsupported sort " + print_term(s) + " detected!");
      return data::function_symbol();
    }
  };

  // function that lifts a function symbol
  struct lift_function_symbol_2_3
  {
    data::function_symbol operator()(const data::function_symbol& f) const
    {
      //mCRL2log(log::verbose) << "lift_function_symbol_2_3 f = " << print_symbol(f) << std::endl;
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

  // function that generates an equation from a function symbol and it's corresponding 'generated' version
  struct lift_equation_1_2
  {
    const sort_expression_substitution_map& sigmaS;

    lift_equation_1_2(const sort_expression_substitution_map& sigmaS_)
      : sigmaS(sigmaS_)
    {}

    atermpp::vector<data::variable> make_variables(const data::sort_expression_list& sorts, const std::string& hint) const
    {
      atermpp::vector<data::variable> result;
      unsigned int i = 0;
      for (data::sort_expression_list::const_iterator j = sorts.begin(); j != sorts.end(); ++i, ++j)
      {
        result.push_back(data::variable(hint + boost::lexical_cast<std::string>(i), apply_sigmaS(sigmaS)(*j)));
      }
      return result;
    }

    data::data_equation operator()(const data::function_symbol& f1, const data::function_symbol& f2) const
    {
      //mCRL2log(log::verbose) << "lift_equation_1_2 f1 = " << print_symbol(f1) << " f2 = " << print_symbol(f2) << std::endl;
      data::variable_list variables;
      data::data_expression condition = data::sort_bool::true_();
      data::data_expression lhs;
      data::data_expression rhs;

      data::sort_expression s1 = f1.sort();

      if (data::is_basic_sort(s1))
      {
        lhs = f2;
        rhs = f1;
      }
      else if (data::is_function_sort(s1))
      {
        data::function_sort fs1(f1.sort());
        data::function_sort fs2(f2.sort());
        // TODO: generate these variables in a proper way
        atermpp::vector<data::variable> x = make_variables(fs2.domain(), "x");
        variables = data::variable_list(x.begin(), x.end());
        lhs = data::application(f2, data::data_expression_list(x.begin(), x.end()));
        rhs = data::detail::create_finite_set(data::application(f1, data::data_expression_list(x.begin(), x.end())));
      }
//      else if (data::is_container_sort(s1))
//      {
//        sort_expression_substitution_map::const_iterator i = sigmaS.find(s1);
//        if (i != sigmaS.end())
//        {
//        }
//      }
      else
      {
        throw mcrl2::runtime_error("absinthe algorithm (lift_equation_1_2): unsupported sort " + print_term(s1) + " detected!");
      }
      return data::data_equation(variables, condition, lhs, rhs);
    }
  };

  // function that generates an equation from a function symbol and it's corresponding lifted version
  struct lift_equation_2_3
  {
    const sort_expression_substitution_map& sigmaS;

    lift_equation_2_3(const sort_expression_substitution_map& sigmaS_)
      : sigmaS(sigmaS_)
    {}

    atermpp::vector<data::variable> make_variables(const data::sort_expression_list& sorts, const std::string& hint) const
    {
      atermpp::vector<data::variable> result;
      unsigned int i = 0;
      for (data::sort_expression_list::const_iterator j = sorts.begin(); j != sorts.end(); ++i, ++j)
      {
        result.push_back(data::variable(hint + boost::lexical_cast<std::string>(i), apply_sigmaS(sigmaS)(*j)));
      }
      return result;
    }

    // Let x = [x1:D1, ..., xn:Dn] and X = [X1:Set(D1), ..., Xn:Set(Dn)]. Returns the expression
    //
    // (x1 in X1 /\ ... /\ xn in Xn)
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
      return body;
    }

    data::data_equation operator()(const data::function_symbol& f2, const data::function_symbol& f3) const
    {
      //mCRL2log(log::verbose) << "lift_equation_2_3 f2 = " << print_symbol(f2) << " f3 = " << print_symbol(f3) << std::endl;
      data::variable_list variables;
      data::data_expression condition = data::sort_bool::true_();
      data::data_expression lhs;
      data::data_expression rhs;

      data::sort_expression s2 = f2.sort();

      if (data::is_basic_sort(s2))
      {
        lhs = f3;
        rhs = data::detail::create_finite_set(f2);
      }
      else if (data::is_function_sort(s2))
      {
        data::function_sort fs2(f2.sort());
        data::function_sort fs3(f3.sort());

        // TODO: generate these variables in a proper way
        atermpp::vector<data::variable> x = make_variables(fs2.domain(), "x");
        atermpp::vector<data::variable> X = make_variables(fs3.domain(), "X");

        variables = data::variable_list(X.begin(), X.end());
        lhs = data::application(f3, data::data_expression_list(X.begin(), X.end()));
        data::variable y("y", data::detail::get_set_sort(fs2.codomain()));
        data::data_expression Y = data::application(f2, data::data_expression_list(x.begin(), x.end()));
        data::data_expression body = data::sort_bool::and_(enumerate_domain(x, X), data::detail::create_set_in(y, Y));
        rhs = data::detail::create_set_comprehension(y, data::exists(x, body));
      }
//      else if (data::is_container_sort(s2))
//      {
//      }
      else
      {
        throw mcrl2::runtime_error("absinthe algorithm (lift_equation_2_3): unsupported sort " + print_term(s2) + " detected!");
      }
      return data::data_equation(variables, condition, lhs, rhs);
    }
  };

  template <typename Map>
  std::string print_mapping(const Map& m)
  {
    std::ostringstream out;
    for (typename Map::const_iterator i = m.begin(); i != m.end(); ++i)
    {
      out << data::pp(i->first) << " -> " << data::pp(i->second) << std::endl;
    }
    return out.str();
  }

  void check_consistency(const data::sort_expression& s, const data::sort_expression& t, const data::function_symbol& f, sort_expression_substitution_map& sigmaS) const
  {
    sort_expression_substitution_map::const_iterator i = sigmaS.find(s);
    if (i != sigmaS.end() && i->second != t)
    {
      throw mcrl2::runtime_error("inconsistent abstraction " + data::pp(s) + " := " + data::pp(t) + " detected in the abstraction of " + print_symbol(f) + " (elsewhere it is abstracted as " + data::pp(s) + " := " + data::pp(i->second) + ").");
    }
    else
    {
      sigmaS[s] = t;
    }
  }

  // Let f1: s1 x ... sn -> s and f2: t1 x ... tn -> t
  // This function checks if the correspondence si -> ti conflicts with sigmaS.
  void check_consistency(const data::function_symbol& f1, const data::function_symbol& f2, sort_expression_substitution_map& sigmaS) const
  {
    data::sort_expression s1 = f1.sort();
    data::sort_expression s2 = f2.sort();

    if (data::is_basic_sort(s1))
    {
      check_consistency(s1, s2, f1, sigmaS);
    }
    else if (data::is_function_sort(s1))
    {
      data::function_sort fs1(s1);
      data::function_sort fs2(s2);

      data::sort_expression_list domain1 = fs1.domain();
      data::sort_expression_list domain2 = fs2.domain();

      data::sort_expression_list::iterator i1 = domain1.begin();
      data::sort_expression_list::iterator i2 = domain2.begin();

      for (; i1 != domain1.end(); ++i1, ++i2)
      {
        check_consistency(*i1, *i2, f1, sigmaS);
      }
      // check_consistency(fs1.codomain(), fs2.codomain(), f1, sigmaS);
    }
//    else if (data::is_container_sort(s1))
//    {
//    }
  }

  // add lifted mappings and equations to the data specification
  void lift_data_specification(const pbes<>& p, const sort_expression_substitution_map& sigmaS, function_symbol_substitution_map& sigmaF, data::data_specification& dataspec)
  {
    sort_expression_substitution_map sigmaS_consistency = sigmaS; // is only used for consistency checking

#ifdef MCRL2_DEBUG_DATA_CONSTRUCTION
  data::detail::set_data_specification(dataspec);
#endif

    // add lifted versions of used function symbols that are not specified by the user to sigmaF, and adds them to the data specification as well
    std::set<data::function_symbol> used_function_symbols = pbes_system::find_function_symbols(p);
    for (std::set<data::function_symbol>::iterator i = used_function_symbols.begin(); i != used_function_symbols.end(); ++i)
    {
      data::function_symbol f1 = *i;
      if (sigmaF.find(f1) == sigmaF.end())
      {
        data::function_symbol f2 = lift_function_symbol_1_2(sigmaS)(f1);
        check_consistency(f1, f2, sigmaS_consistency);
        sigmaF[f1] = f2;
        dataspec.add_mapping(f2);

        data::data_equation eq = lift_equation_1_2(sigmaS)(f1, f2);
        mCRL2log(log::verbose) << "adding equation: " << eq << std::endl;
        mCRL2log(log::verbose) << "adding equation: " << data::pp(eq) << std::endl;
        dataspec.add_equation(eq);
      }
    }

    for (function_symbol_substitution_map::iterator i = sigmaF.begin(); i != sigmaF.end(); ++i)
    {
      data::function_symbol f1 = i->first;
      data::function_symbol f2 = i->second;
      data::function_symbol f3 = lift_function_symbol_2_3()(f2);

      mCRL2log(log::verbose) << "adding mapping: " << core::pp(f3) << " " << core::pp(f3.sort()) << std::endl;
      dataspec.add_mapping(f3);

      // update sigmaF
      i->second = f3;

      // make an equation for the lifted function symbol f
      data::data_equation eq = lift_equation_2_3(sigmaS)(f2, f3);
      mCRL2log(log::verbose) << "adding equation: " << core::pp(eq) << std::endl;
      dataspec.add_equation(eq);
    }
  }

  void print_fsvec(const data::function_symbol_vector& v, const std::string& msg) const
  {
    mCRL2log(log::verbose) << "--- " << msg << std::endl;
    for (data::function_symbol_vector::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      mCRL2log(log::verbose) << print_symbol(*i) << std::endl;
    }
  }

  void print_fsmap(const function_symbol_substitution_map& v, const std::string& msg) const
  {
    mCRL2log(log::verbose) << "--- " << msg << std::endl;
    for (function_symbol_substitution_map::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      mCRL2log(log::verbose) << print_symbol(i->first) << "  -->  " << print_symbol(i->second) << std::endl;
    }
  }

  void run(pbes<>& p, const std::string& abstraction_text, bool is_over_approximation)
  {
    // split the string abstraction_text into four different parts
    std::string sort_expression_mapping_text;
    std::string function_symbol_mapping_text;
    std::string user_sorts_text;
    std::string user_equations_text;

    std::string text = abstraction_text;
    std::vector<std::string> all_keywords;
    all_keywords.push_back("sort");
    all_keywords.push_back("var");
    all_keywords.push_back("eqn");
    all_keywords.push_back("map");
    all_keywords.push_back("cons");
    all_keywords.push_back("absfunc");
    all_keywords.push_back("abssort");
    std::pair<std::string, std::string> q;

    q = pbes_system::detail::separate_keyword_section(text, "sort", all_keywords);
    user_sorts_text = q.first;
    text = q.second;

    q = pbes_system::detail::separate_keyword_section(text, "abssort", all_keywords);
    sort_expression_mapping_text = q.first;
    text = q.second;

    q = pbes_system::detail::separate_keyword_section(text, "absfunc", all_keywords);
    function_symbol_mapping_text = q.first;
    user_equations_text = q.second;

    // 0) split user_dataspec_text into user_sorts_text and user_equations_text
    mCRL2log(log::verbose) << "--- user sorts ---\n" << user_sorts_text << std::endl;
    mCRL2log(log::verbose) << "--- user equations ---\n" << user_equations_text << std::endl;
    mCRL2log(log::verbose) << "--- function mapping ---\n" << function_symbol_mapping_text << std::endl;
    mCRL2log(log::verbose) << "--- sort mapping ---\n" << sort_expression_mapping_text << std::endl;

    // 1) create the data specification data_spec, which consists of user_sorts_text and p.data()
    data::data_specification data_spec = data::parse_data_specification(data::pp(p.data()) + "\n" + user_sorts_text);
    mCRL2log(log::verbose) << "--- data specification 1) ---\n" << data::pp(data_spec) << std::endl;

    // 2) parse the right hand sides of the function symbol mapping, and add them to data_spec
    parse_right_hand_sides(function_symbol_mapping_text, data_spec);
    mCRL2log(log::verbose) << "--- data specification 2) ---\n" << data::pp(data_spec) << std::endl;

    // 3) add user_equations_text to data_spec
    data_spec = data::parse_data_specification(data::pp(data_spec) + "\n" + user_equations_text);
    mCRL2log(log::verbose) << "--- data specification 3) ---\n" << data::pp(data_spec) << std::endl;

    // sort expressions replacements (specified by the user)
    sort_expression_substitution_map sigmaS = parse_sort_expression_mapping(sort_expression_mapping_text, data_spec);
    mCRL2log(log::verbose) << "\n--- sort expression mapping ---\n" << print_mapping(sigmaS) << std::endl;

    // function symbol replacements (specified by the user)
    function_symbol_substitution_map sigmaF = parse_function_symbol_mapping(function_symbol_mapping_text, data_spec);
    mCRL2log(log::verbose) << "\n--- function symbol mapping ---\n" << print_mapping(sigmaF) << std::endl;

    // 4) add lifted sorts, mappings and equations to data_spec
    // before: the mapping sigmaF is f1 -> f2
    // after: the mapping sigmaF is f1 -> f3
    // after: f2 and f3 have been added to data_spec
    // after: equations for f3 have been added to data_spec
    // generate mapping f1 -> f2 for missing function symbols
    lift_data_specification(p, sigmaS, sigmaF, data_spec);
    mCRL2log(log::verbose) << "--- data specification 4) ---\n" << data::pp(data_spec) << std::endl;

    mCRL2log(log::verbose) << "\n--- function symbol mapping after lifting ---\n" << print_mapping(sigmaF) << std::endl;

    mCRL2log(log::verbose) << "--- pbes before ---\n" << pbes_system::pp(p) << std::endl;

    p.data() = data_spec;

    // then transform the data expressions and the propositional variable instantiations
    absinthe_data_expression_builder(sigmaS, sigmaF, is_over_approximation)(p);

    mCRL2log(log::verbose) << "--- pbes after ---\n" << pbes_system::pp(p) << std::endl;
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ABSINTHE_H
