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

//#define MCRL2_DEBUG_DATA_CONSTRUCTION
#define MCRL2_ABSINTHE_CHECK_EXPRESSIONS

#include <algorithm>
#include <sstream>
#include <utility>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/exists.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/detail/data_construction.h"
#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/detail/separate_keyword_section.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/detail/container_utility.h"

#ifdef MCRL2_ABSINTHE_CHECK_EXPRESSIONS
#include "mcrl2/data/detail/print_parse_check.h"
#endif

namespace mcrl2 {

namespace pbes_system {

  template <typename Term>
  std::string print_term(const Term& x)
  {
    return data::pp(x) + " " + to_string(x);
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

  inline
  data::data_specification& absinthe_data_specification()
  {
    static data::data_specification dataspec;
    return dataspec;
  }

#ifdef MCRL2_ABSINTHE_CHECK_EXPRESSIONS
  template <typename T>
  inline void absinthe_check_expression(const T& x)
  {
    data::detail::print_parse_check(x, absinthe_data_specification());
  }
#else
  template <typename T>
  inline void absinthe_check_expression(const T&)
  {}
#endif

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
        const data::structured_sort& ss = atermpp::down_cast<data::structured_sort>(s);
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
  void print_used_function_symbols(const pbes& p)
  {
    mCRL2log(log::debug, "absinthe") << "--- used function symbols ---" << std::endl;
    std::set<data::function_symbol> find_function_symbols = pbes_system::find_function_symbols(p);
    for (std::set<data::function_symbol>::iterator i = find_function_symbols.begin(); i != find_function_symbols.end(); ++i)
    {
      mCRL2log(log::debug, "absinthe") << print_symbol(*i) << std::endl;
    }
  }

  // TODO: Is this correct if s has the shape A -> (B -> C)? Should the result be (B -> C) or C?
  inline
  data::sort_expression target_sort(const data::sort_expression& s)
  {
    if (data::is_basic_sort(s))
    {
      return s;
    }
    else if (data::is_function_sort(s))
    {
      const data::function_sort& fs = atermpp::down_cast<data::function_sort>(s);
      return fs.codomain();
    }
    else if (data::is_container_sort(s))
    {
      return s;
    }
    throw mcrl2::runtime_error("target_sort: unsupported sort " + print_term(s) + " detected!");
    return data::sort_expression();
  }

} // namespace detail

struct absinthe_algorithm
{
  typedef std::map<data::sort_expression, data::sort_expression> sort_expression_substitution_map;
  typedef std::map<data::function_symbol, data::function_symbol> function_symbol_substitution_map;
  typedef std::map<data::sort_expression, data::function_symbol> abstraction_map;

  struct absinthe_sort_expression_builder: public sort_expression_builder<absinthe_sort_expression_builder>
  {
    typedef sort_expression_builder<absinthe_sort_expression_builder> super;
    using super::enter;
    using super::leave;
    using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif

    const abstraction_map& sigmaH;
    const sort_expression_substitution_map& sigmaS;
    const function_symbol_substitution_map& sigmaF;

    absinthe_sort_expression_builder(const abstraction_map& sigmaA_,
                                     const sort_expression_substitution_map& sigmaS_,
                                     const function_symbol_substitution_map& sigmaF_)
      : sigmaH(sigmaA_),
        sigmaS(sigmaS_),
        sigmaF(sigmaF_)
    {}

    data::sort_expression operator()(const data::sort_expression& x)
    {
      data::sort_expression result;
      sort_expression_substitution_map::const_iterator i = sigmaS.find(x);
      if (i == sigmaS.end())
      {
        result = super::operator()(x);
        //pbes_system::detail::absinthe_check_expression(result);
      }
      else
      {
        result = i->second;
        //pbes_system::detail::absinthe_check_expression(result);
      }
      return result;
    }

    data::data_expression operator()(const data::function_symbol& x)
    {
      function_symbol_substitution_map::const_iterator i = sigmaF.find(x);
      if (i != sigmaF.end())
      {
        //pbes_system::detail::absinthe_check_expression(i->second);
        return i->second;
      }
      throw mcrl2::runtime_error("function symbol " + print_symbol(x) + " not present in the function symbol mapping!");
      return data::undefined_data_expression();
    }

    //data::data_expression operator()(const data::variable& x)
    //{
    //  data::data_expression result = super::operator()(x);
    //  result = data::detail::create_finite_set(result);
    //  pbes_system::detail::absinthe_check_expression(result);
    //  return result;
    //}

    data::data_expression operator()(const data::application& x)
    {
      if (data::is_variable(x.head()))
      {
        data::variable v = atermpp::down_cast<data::variable>(x.head());
        v = data::variable(v.name(), super::operator()(v.sort()));
        return data::detail::create_finite_set(data::application(v, x.begin(), x.end()));
      }
      else if (data::is_function_symbol(x.head()))
      {
        return super::operator()(x);
      }
      else
      {
        throw mcrl2::runtime_error("don't know how to handle arbitrary expression as head: " + data::pp(x));
      }
      return data::undefined_data_expression();
    }

    data::data_expression operator()(const data::lambda& x)
    {
      data::data_expression body = super::operator()(x);
      //pbes_system::detail::absinthe_check_expression(body);
      data::sort_expression s = body.sort();
      data::set_identifier_generator generator;
      std::set<core::identifier_string> ids = data::find_identifiers(x);
      generator.add_identifiers(ids);
      data::variable v(generator("v"), s);
      data::data_expression result = data::detail::create_set_comprehension(v, data::equal_to(v, body));
      //pbes_system::detail::absinthe_check_expression(result);
      return result;
    }

    data::data_expression operator()(const data::data_expression& x)
    {
      data::data_expression result;

      if (data::is_variable(x))
      {
        result = super::operator()(x);
        result = data::detail::create_finite_set(result);
        //pbes_system::detail::absinthe_check_expression(result);
      }
      else
      {
        // check if it is a "ground term", i.e. it does not contain any variables
        abstraction_map::const_iterator i = sigmaH.find(x.sort());
        if (i != sigmaH.end() && data::find_all_variables(x).empty())
        {
          data::data_expression_list args = atermpp::make_list(x);
          result = data::detail::create_finite_set(data::application(i->second, args));
          //pbes_system::detail::absinthe_check_expression(result);
        }
        else
        {
          // first apply the sort and function symbol transformations
          result = super::operator()(x);
          //pbes_system::detail::absinthe_check_expression(result);
        }
      }
      return result;
    }
  };

  // Applies sigmaS to a sort expression
  struct sort_function: public std::unary_function<data::sort_expression, data::sort_expression>
  {
    absinthe_sort_expression_builder f;

    sort_function(const abstraction_map& sigmaH,
                  const sort_expression_substitution_map& sigmaS,
                  const function_symbol_substitution_map& sigmaF
                 )
      : f(sigmaH, sigmaS, sigmaF)
    {}

    data::sort_expression operator()(const data::sort_expression& x)
    {
      return f(x);
    }
  };

  struct absinthe_data_expression_builder: public pbes_expression_builder<absinthe_data_expression_builder>
  {
    typedef pbes_expression_builder<absinthe_data_expression_builder> super;
    using super::enter;
    using super::leave;
    using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif

    data::variable_list make_variables(const data::data_expression_list& x, const std::string& hint, sort_function sigma) const
    {
      std::vector<data::variable> result;
      std::size_t i = 0;
      for (auto j = x.begin(); j != x.end(); ++i, ++j)
      {
        result.push_back(data::variable(hint + utilities::number2string(i), sigma(j->sort())));
      }
      return data::variable_list(result.begin(), result.end());
    }

    const abstraction_map& sigmaH;
    const sort_expression_substitution_map& sigmaS;
    const function_symbol_substitution_map& sigmaF;
    bool m_is_over_approximation;

    data::data_expression lift(const data::data_expression& x)
    {
      return absinthe_sort_expression_builder(sigmaH, sigmaS, sigmaF)(x);
    }

    data::data_expression_list lift(const data::data_expression_list& x)
    {
      return absinthe_sort_expression_builder(sigmaH, sigmaS, sigmaF)(x);
    }

    data::variable_list lift(const data::variable_list& x)
    {
      return absinthe_sort_expression_builder(sigmaH, sigmaS, sigmaF)(x);
    }

    pbes_system::propositional_variable lift(const pbes_system::propositional_variable& x)
    {
      return absinthe_sort_expression_builder(sigmaH, sigmaS, sigmaF)(x);
    }

    absinthe_data_expression_builder(const abstraction_map& sigmaA_,
                                     const sort_expression_substitution_map& sigmaS_,
                                     const function_symbol_substitution_map& sigmaF_,
                                     bool is_over_approximation)
      : sigmaH(sigmaA_),
        sigmaS(sigmaS_),
        sigmaF(sigmaF_),
        m_is_over_approximation(is_over_approximation)
    {}

    pbes_expression operator()(const data::data_expression& x)
    {
      data::data_expression result;
      data::data_expression x1 = lift(x);
      if (m_is_over_approximation)
      {
        result = data::detail::create_set_in(data::sort_bool::true_(), x1);
      }
      else
      {
        result = data::sort_bool::not_(data::detail::create_set_in(data::sort_bool::false_(), x1));
      }
      return result;
    }

    pbes_expression operator()(const propositional_variable_instantiation& x)
    {
      pbes_expression result;
      data::data_expression_list e = lift(x.parameters());
      data::variable_list variables = make_variables(x.parameters(), "x", sort_function(sigmaH, sigmaS, sigmaF));
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
        result = make_exists(variables, and_(q, propositional_variable_instantiation(x.name(), atermpp::container_cast<data::data_expression_list>(variables))));
      }
      else
      {
        result = make_forall(variables, imp(q, propositional_variable_instantiation(x.name(), atermpp::container_cast<data::data_expression_list>(variables))));
      }
      return result;
    }

    pbes_system::pbes_expression operator()(const pbes_system::forall& x)
    {
      pbes_expression result = make_forall(lift(x.variables()), super::operator()(x.body()));
      return result;
    }

    pbes_system::pbes_expression operator()(const pbes_system::exists& x)
    {
      pbes_expression result = make_exists(lift(x.variables()), super::operator()(x.body()));
      return result;
    }

    void operator()(pbes_system::pbes_equation& x)
    {
      x.variable() = lift(x.variable());
      x.formula() = super::operator()(x.formula());
    }

    void operator()(pbes_system::pbes& x)
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
        data::function_symbol f = data::parse_function_symbol(words[1], dataspec_text);
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
        data::function_symbol lhs = data::parse_function_symbol(words[0], dataspec_text);
        std::string s = words[1];
        s = utilities::regex_replace(";\\s*$", "", s);
        data::function_symbol rhs = data::parse_function_symbol(s, dataspec_text);
        result[lhs] = rhs;
      }
    }
    return result;
  }

  // text is a data_specification; extract the user defined mappings
  abstraction_map parse_abstraction_map(const std::string& text)
  {
    abstraction_map result;
    data::data_specification dataspec = data::parse_data_specification(text);
    const data::function_symbol_vector& m = dataspec.user_defined_mappings();
    for (data::function_symbol_vector::const_iterator i = m.begin(); i != m.end(); ++i)
    {
      const data::function_sort& f = atermpp::down_cast<data::function_sort>(i->sort());
      if (f.domain().size() != 1)
      {
        throw mcrl2::runtime_error("cannot abstract the function " + data::pp(*i) + " since the arity of the domain is not equal to one!");
      }
      result[f.domain().front()] = *i;
    }
    return result;
  }

  // creates a finite set containing one data expression
  struct make_data_expression_set
  {
    data::data_expression operator()(const data::data_expression& x) const
    {
      data::sort_expression s = x.sort();
      data::data_expression result = data::sort_fset::empty(s);
      result = data::sort_fset::cons_(s, x, result);
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

  // function that transforms a function symbol
  struct lift_function_symbol_1_2
  {
    std::map<std::string, std::string> unprintable;
    std::set<std::string> suffix_with_sort;

    lift_function_symbol_1_2()
    {
      unprintable["&&"] = "and";
      unprintable["||"] = "or";
      unprintable["!"] = "not";
      unprintable["#"] = "len";
      unprintable["."] = "element_at";
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
      unprintable["@func_update"] = "func_update";
      unprintable["@cInt"] = "cInt";
      unprintable["@cNeg"] = "cNeg";

      suffix_with_sort.insert("[]");
      suffix_with_sort.insert("|>");
    }

    std::string print_cleaned(const data::sort_expression& s) const
    {
      std::string result = data::pp(s);
      result = utilities::regex_replace("\\(", "_", result);
      result = utilities::regex_replace("\\)", "_", result);
      result = utilities::regex_replace("#", "_", result);
      result = utilities::regex_replace("->", "_", result);
      result = utilities::remove_whitespace(result);
      return result;
    }


    data::function_symbol operator()(const data::function_symbol& f, sort_function sigma) const
    {
    	using utilities::detail::contains;

      //mCRL2log(log::debug, "absinthe") << "lift_function_symbol_1_2 f = " << print_symbol(f) << std::endl;
      std::string name = std::string(f.name());

      bool print_sort = contains(suffix_with_sort, std::string(f.name()));
      auto i = unprintable.find(name);
      if (i != unprintable.end())
      {
        name = i->second;
      }
      name = "Generated_" + name;
      if (print_sort)
      {
        name = name + print_cleaned(f.sort());
      }

      data::sort_expression s = f.sort();
      if (data::is_basic_sort(s))
      {
        return data::function_symbol(name, sigma(s));
      }
      else if (data::is_function_sort(s))
      {
        // Apply sigmaS recursively to s
        //   f:        tail:           List(Nat) -> List(Nat)
        //   result:   generated_tail: List(AbsNat) -> Set(List(AbsNat))
        const data::function_sort& fs = atermpp::down_cast<data::function_sort>(sigma(s));
        return data::function_symbol(name, data::function_sort(fs.domain(), make_set()(fs.codomain())));
      }
      else if (data::is_container_sort(s))
      {
        // Apply sigmaS recursively to s
        // Example: List(Nat) -> List(AbsNat)
        return data::function_symbol(name, sigma(s));
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
      using namespace data;
      //mCRL2log(log::debug, "absinthe") << "lift_function_symbol_2_3 f = " << print_symbol(f) << std::endl;
      std::string name = "Lift" + boost::algorithm::trim_copy(std::string(f.name()));
      const sort_expression &s = f.sort();
      if (is_basic_sort(s))
      {
        return function_symbol(name, make_set()(s));
      }
      else if (is_function_sort(s))
      {
        const function_sort fs(s);
        const sort_expression_list& sl=fs.domain();
        return function_symbol(name,
                 function_sort(sort_expression_list(sl.begin(),sl.end(), make_set()), fs.codomain()));
      }
      else if (is_container_sort(s))
      {
        return data::function_symbol(name, make_set()(s));
      }
      throw mcrl2::runtime_error("absinthe algorithm (lift): unsupported sort " + print_term(s) + " detected!");
    }
  };

  // function that generates an equation from a function symbol and it's corresponding 'generated' version
  struct lift_equation_1_2
  {
    lift_equation_1_2()
    {}

    std::vector<data::variable> make_variables(const data::sort_expression_list& sorts, const std::string& hint, sort_function sigma) const
    {
      std::vector<data::variable> result;
      std::size_t i = 0;
      for (auto j = sorts.begin(); j != sorts.end(); ++i, ++j)
      {
        result.push_back(data::variable(hint + utilities::number2string(i), sigma(*j)));
      }
      return result;
    }

    // sigmaH is used for checking consistency of the user input
    data::data_equation operator()(const data::function_symbol& f1, const data::function_symbol& f2, sort_function sigma, const abstraction_map& sigmaH) const
    {
      mCRL2log(log::debug, "absinthe") << "lift_equation_1_2 f1 = " << print_symbol(f1) << " f2 = " << print_symbol(f2) << std::endl;
      data::variable_list variables;
      data::data_expression condition = data::sort_bool::true_();
      data::data_expression lhs;
      data::data_expression rhs;

      data::sort_expression s1 = f1.sort();
      //data::function_symbol f1a(f1.name(), sigma(s1));

      if (data::is_basic_sort(s1))
      {
        lhs = f2;
        data::function_symbol f1_sigma(f1.name(), sigma(f1.sort()));

        abstraction_map::const_iterator i = sigmaH.find(f1.sort());
        if (i == sigmaH.end())
        {
          rhs = f1_sigma;
        }
        else
        {
          data::function_symbol h = i->second;
          rhs = data::application(h, atermpp::make_list(f1));
          //pbes_system::detail::absinthe_check_expression(rhs);
        }
      }
      else if (data::is_function_sort(s1))
      {
        data::function_sort fs1(f1.sort());
        data::function_sort fs2(f2.sort());

        // check validity
        if (data::is_function_update_application(f1))
        {
          // TODO: add check that the domain of the updated function does not contain abstraction sorts
        }
        else if (fs1.domain() != fs2.domain())
        {
          throw std::runtime_error("can not generalize functions with abstraction sorts in the domain: " + data::pp(f1) + ": " + data::pp(s1));
        }

        data::variable_vector x = make_variables(fs2.domain(), "x", sigma);
        variables = data::variable_list(x.begin(),x.end());
        lhs = data::application(f2, data::data_expression_list(x.begin(), x.end()));
        data::application f_x(f1, data::data_expression_list(x.begin(), x.end()));

        data::function_symbol f1_sigma(f1.name(), sigma(f1.sort()));

        abstraction_map::const_iterator i = sigmaH.find(detail::target_sort(f1.sort()));
        if (i == sigmaH.end())
        {
          data::application f1_sigma_x(f1_sigma, data::data_expression_list(x.begin(), x.end()));
          rhs = data::detail::create_finite_set(f_x);
          //pbes_system::detail::absinthe_check_expression(rhs);
        }
        else
        {
          data::function_symbol h = i->second;
          rhs = data::detail::create_finite_set(data::application(h, atermpp::make_list(f_x)));
          //pbes_system::detail::absinthe_check_expression(rhs);
        }
      }
      else if (data::is_container_sort(s1))
      {
        // Example:
        //   f1:   [] : List(Nat)
        //   f2:   generated_emptylist : List(AbsNat)
        //  eqn:   generated_emptylist = []                    met [] : List(AbsNat)
        // tail: List(AbsNat) -> List(Nat)
        lhs = f2;
        data::function_symbol f1_sigma(f1.name(), sigma(f1.sort()));
        rhs = f1_sigma;
        //pbes_system::detail::absinthe_check_expression(rhs);
      }
      else
      {
        throw mcrl2::runtime_error("absinthe algorithm (lift_equation_1_2): unsupported sort " + print_term(s1) + " detected!");
      }

      if (lhs.sort() != rhs.sort())
      {
        throw mcrl2::runtime_error("absinthe algorithm (lift_equation_1_2): lhs.sort() and rhs.sort are not equal: " + data::pp(lhs.sort()) + " <-> " + data::pp(rhs.sort()));
      }

      return data::data_equation(variables, condition, lhs, rhs);
    }
  };

  // function that generates an equation from a function symbol and it's corresponding lifted version
  struct lift_equation_2_3
  {
    lift_equation_2_3()
    {}

    std::vector<data::variable> make_variables(const data::sort_expression_list& sorts, const std::string& hint, sort_function sigma) const
    {
      std::vector<data::variable> result;
      std::size_t i = 0;
      for (auto j = sorts.begin(); j != sorts.end(); ++i, ++j)
      {
        result.push_back(data::variable(hint + utilities::number2string(i), sigma(*j)));
      }
      return result;
    }

    // Let x = [x1:D1, ..., xn:Dn] and X = [X1:Set(D1), ..., Xn:Set(Dn)]. Returns the expression
    //
    // (x1 in X1 /\ ... /\ xn in Xn)
    data::data_expression enumerate_domain(const std::vector<data::variable>& x, const std::vector<data::variable>& X) const
    {
      std::vector<data::data_expression> a;
      std::vector<data::variable>::const_iterator i = x.begin();
      std::vector<data::variable>::const_iterator j = X.begin();
      for (; i != x.end(); ++i, ++j)
      {
        a.push_back(data::detail::create_set_in(*i, *j));
      }
      data::data_expression body = data::lazy::join_and(a.begin(), a.end());
      return body;
    }

    data::data_equation operator()(const data::function_symbol& f2, const data::function_symbol& f3, sort_function sigma) const
    {
      //mCRL2log(log::debug, "absinthe") << "lift_equation_2_3 f2 = " << print_symbol(f2) << " f3 = " << print_symbol(f3) << std::endl;
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
        std::vector<data::variable> x = make_variables(fs2.domain(), "x", sigma);
        std::vector<data::variable> X = make_variables(fs3.domain(), "X", sigma);

        variables = data::variable_list(X.begin(), X.end());
        lhs = data::application(f3, data::data_expression_list(X.begin(), X.end()));
        data::variable y("y", data::detail::get_set_sort(atermpp::down_cast<data::container_sort>(fs2.codomain())));
        data::data_expression Y = data::application(f2, data::data_expression_list(x.begin(), x.end()));
        data::data_expression body = data::sort_bool::and_(enumerate_domain(x, X), data::detail::create_set_in(y, Y));
        rhs = data::detail::create_set_comprehension(y, data::exists(x, body));
      }
      else if (data::is_container_sort(s2))
      {
        lhs = f3;
        rhs = data::detail::create_finite_set(f2);
      }
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
      out << i->first << " -> " << i->second << std::endl;
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
  void lift_data_specification(const pbes& p, const abstraction_map& sigmaH, const sort_expression_substitution_map& sigmaS, function_symbol_substitution_map& sigmaF, data::data_specification& dataspec)
  {
    using utilities::detail::has_key;

    sort_expression_substitution_map sigmaS_consistency = sigmaS; // is only used for consistency checking
    sort_function sigma(sigmaH, sigmaS, sigmaF);

#ifdef MCRL2_DEBUG_DATA_CONSTRUCTION
  data::detail::set_data_specification(dataspec);
#endif

    // add lifted versions of used function symbols that are not specified by the user to sigmaF, and adds them to the data specification as well
    std::set<data::function_symbol> used_function_symbols = pbes_system::find_function_symbols(p);

    // add List containers for user defined sorts, since they are used in the translation
    const data::sort_expression_vector& sorts = dataspec.user_defined_sorts();
    for (auto i = sorts.begin(); i != sorts.end(); ++i)
    {
      data::sort_expression s = data::container_sort(data::list_container(), *i);
      dataspec.add_context_sort(s);
    }

    // add constructor functions of List containers of abstracted sorts to sigmaF
    for (auto i = sigmaH.begin(); i != sigmaH.end(); ++i)
    {
      data::sort_expression s = data::container_sort(data::list_container(), i->first);
      dataspec.add_context_sort(s);
      data::function_symbol_vector list_constructors = dataspec.constructors(s);
      for (auto j = list_constructors.begin(); j != list_constructors.end(); ++j)
      {
        data::function_symbol f1 = *j;
        data::function_symbol f2 = lift_function_symbol_1_2()(f1, sigma);
        sigmaF[f1] = f2;
        dataspec.add_mapping(f2);
mCRL2log(log::debug, "absinthe") << "adding list constructor " << f1 << " to sigmaF" << std::endl;
      }
    }

    for (auto i = used_function_symbols.begin(); i != used_function_symbols.end(); ++i)
    {
      mCRL2log(log::debug, "absinthe") << "lifting function symbol: " << *i << std::endl;
      data::function_symbol f1 = *i;
      if (!has_key(sigmaF, f1))
      {
        data::function_symbol f2 = lift_function_symbol_1_2()(f1, sigma);
        mCRL2log(log::debug, "absinthe") << "lifted function symbol: " << f1 << " to " << f2 << std::endl;
        check_consistency(f1, f2, sigmaS_consistency);
        sigmaF[f1] = f2;
        dataspec.add_mapping(f2);

        data::data_equation eq = lift_equation_1_2()(f1, f2, sigma, sigmaH);
        mCRL2log(log::debug, "absinthe") << "adding equation: " << eq << std::endl;
        dataspec.add_equation(eq);
      }
    }

    for (function_symbol_substitution_map::iterator i = sigmaF.begin(); i != sigmaF.end(); ++i)
    {
      // data::function_symbol f1 = i->first;
      data::function_symbol f2 = i->second;
      data::function_symbol f3 = lift_function_symbol_2_3()(f2);

      mCRL2log(log::debug, "absinthe") << "adding mapping: " << f3 << " " << f3.sort() << std::endl;
      dataspec.add_mapping(f3);

      // update sigmaF
      i->second = f3;

      // make an equation for the lifted function symbol f
      data::data_equation eq = lift_equation_2_3()(f2, f3, sigma);
      mCRL2log(log::debug, "absinthe") << "adding equation: " << eq << std::endl;
      dataspec.add_equation(eq);
    }
  }

  void print_fsvec(const data::function_symbol_vector& v, const std::string& msg) const
  {
    mCRL2log(log::debug, "absinthe") << "--- " << msg << std::endl;
    for (data::function_symbol_vector::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      mCRL2log(log::debug, "absinthe") << print_symbol(*i) << std::endl;
    }
  }

  void print_fsmap(const function_symbol_substitution_map& v, const std::string& msg) const
  {
    mCRL2log(log::debug, "absinthe") << "--- " << msg << std::endl;
    for (function_symbol_substitution_map::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      mCRL2log(log::debug, "absinthe") << print_symbol(i->first) << "  -->  " << print_symbol(i->second) << std::endl;
    }
  }

  void enable_logging()
  {
    log::mcrl2_logger::set_reporting_level(log::debug, "absinthe");
  }

  void run(pbes& p, const std::string& abstraction_text, bool is_over_approximation)
  {
    // split the string abstraction_text into four different parts
    std::string function_symbol_mapping_text;
    std::string user_sorts_text;
    std::string user_equations_text;
    std::string abstraction_mapping_text;
    std::string pbes_sorts_text;

    std::string text = abstraction_text;
    std::vector<std::string> all_keywords;
    all_keywords.push_back("sort");
    all_keywords.push_back("var");
    all_keywords.push_back("eqn");
    all_keywords.push_back("map");
    all_keywords.push_back("cons");
    all_keywords.push_back("absfunc");
    all_keywords.push_back("absmap");
    std::pair<std::string, std::string> q;

    q = utilities::detail::separate_keyword_section(text, "sort", all_keywords);
    user_sorts_text = q.first;
    text = q.second;

    q = utilities::detail::separate_keyword_section(text, "absmap", all_keywords);

    abstraction_mapping_text = q.first;
    text = q.second;

    // must be the last one!
    q = utilities::detail::separate_keyword_section(text, "absfunc", all_keywords);
    function_symbol_mapping_text = q.first;
    user_equations_text = q.second;

    // extract pbes sorts
    std::string ptext = data::pp(p.data());
    q = utilities::detail::separate_keyword_section(ptext, "sort", all_keywords);
    pbes_sorts_text = q.first;

    // 0) split user_dataspec_text into user_sorts_text and user_equations_text
    mCRL2log(log::debug, "absinthe") << "--- user sorts ---\n" << user_sorts_text << std::endl;
    mCRL2log(log::debug, "absinthe") << "--- user equations ---\n" << user_equations_text << std::endl;
    mCRL2log(log::debug, "absinthe") << "--- function mapping ---\n" << function_symbol_mapping_text << std::endl;
    mCRL2log(log::debug, "absinthe") << "--- abstraction mapping ---\n" << abstraction_mapping_text << std::endl;
    mCRL2log(log::debug, "absinthe") << "--- pbes sorts ---\n" << pbes_sorts_text << std::endl;

    if (abstraction_mapping_text.find("absmap") != 0)
    {
      throw mcrl2::runtime_error("the abstraction mapping may not be empty!");
    }

    // 1) create the data specification dataspec, which consists of user_sorts_text, abstract_mapping_text and p.data()
    data::data_specification dataspec = data::parse_data_specification(data::pp(p.data()) + "\n" + user_sorts_text + "\n" + abstraction_mapping_text.substr(3));
    mCRL2log(log::debug, "absinthe") << "--- data specification 1) ---\n" << dataspec << std::endl;

    // 2) parse the right hand sides of the function symbol mapping, and add them to dataspec
    parse_right_hand_sides(function_symbol_mapping_text, dataspec);
    mCRL2log(log::debug, "absinthe") << "--- data specification 2) ---\n" << dataspec << std::endl;

    // 3) add user_equations_text to dataspec
    dataspec = data::parse_data_specification(data::pp(dataspec) + "\n" + user_equations_text);
    mCRL2log(log::debug, "absinthe") << "--- data specification 3) ---\n" << dataspec << std::endl;

    // abstraction functions (specified by the user)
    abstraction_map sigmaH = parse_abstraction_map(pbes_sorts_text + "\n" + user_sorts_text + "\n" + abstraction_mapping_text.substr(3));

    // sort expressions replacements (extracted from sigmaH)
    sort_expression_substitution_map sigmaS;
    for (abstraction_map::const_iterator i = sigmaH.begin(); i != sigmaH.end(); ++i)
    {
      data::function_symbol f = i->second;
      const data::function_sort& fs = atermpp::down_cast<data::function_sort>(f.sort());
      sigmaS[i->first] = fs.codomain();
    }
    mCRL2log(log::debug, "absinthe") << "\n--- sort expression mapping ---\n" << print_mapping(sigmaS) << std::endl;

    // function symbol replacements (specified by the user)
    function_symbol_substitution_map sigmaF = parse_function_symbol_mapping(function_symbol_mapping_text, dataspec);
    mCRL2log(log::debug, "absinthe") << "\n--- function symbol mapping ---\n" << print_mapping(sigmaF) << std::endl;

    // 4) add lifted sorts, mappings and equations to dataspec
    // before: the mapping sigmaF is f1 -> f2
    // after: the mapping sigmaF is f1 -> f3
    // after: f2 and f3 have been added to dataspec
    // after: equations for f3 have been added to dataspec
    // generate mapping f1 -> f2 for missing function symbols
    lift_data_specification(p, sigmaH, sigmaS, sigmaF, dataspec);
    mCRL2log(log::debug, "absinthe") << "--- data specification 4) ---\n" << dataspec << std::endl;

    mCRL2log(log::debug, "absinthe") << "\n--- function symbol mapping after lifting ---\n" << print_mapping(sigmaF) << std::endl;

    mCRL2log(log::debug, "absinthe") << "--- pbes before ---\n" << p << std::endl;

    p.data() = dataspec;

    // then transform the data expressions and the propositional variable instantiations
    absinthe_data_expression_builder(sigmaH, sigmaS, sigmaF, is_over_approximation)(p);

    mCRL2log(log::debug, "absinthe") << "--- pbes after ---\n" << p << std::endl;
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ABSINTHE_H
