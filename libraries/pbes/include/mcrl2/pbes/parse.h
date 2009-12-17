// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/parse.h
/// \brief Parser for pbes expressions.

#ifndef MCRL2_PBES_PARSE_H
#define MCRL2_PBES_PARSE_H

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>
#include "mcrl2/core/parse.h"
#include "mcrl2/core/text_utility.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/typecheck.h"

//#define MCRL2_LOG_PARSER_OUTPUT
#ifdef MCRL2_LOG_PARSER_OUTPUT
#include <fstream>
#include "mcrl2/core/file_utility.h"
#endif

namespace mcrl2 {

namespace pbes_system {

  template <typename Container>
  void apply_internal_format_conversion(pbes<Container>& p)
  {
    // TODO: make proper internal_format_conversion function for pbes
    ATermAppl t = pbes_to_aterm(p);
    t = data::detail::internal_format_conversion(t);
    p = pbes<Container>(t);
  }

  /// \brief Reads a PBES from an input stream.
  /// \param from An input stream
  /// \param p A PBES
  /// \return The input stream
  template <typename Container>
  std::istream& operator>>(std::istream& from, pbes<Container>& p)
  {
    ATermAppl result = core::parse_pbes_spec(from);
    if (result == NULL) {
      throw mcrl2::runtime_error("parsing failed");
    }

#ifdef MCRL2_LOG_PARSER_OUTPUT
    std::string filename = core::create_filename("pbes", ".txt");
    std::ofstream out(filename.c_str());
    out << "CHECK PBES " << core::detail::check_rule_PBES(result) << std::endl;
    out << core::pp(result) << std::endl << std::endl;
    out << atermpp::aterm_appl(result).to_string() << std::endl;
#endif

#ifdef MCRL2_NEW_TYPECHECKER
    std::cout << "CHECK PBES " << core::detail::check_rule_PBES(result) << std::endl;
    p = pbes<Container>(result);
    type_check(p);
    apply_internal_format_conversion(p);
#else
    result = core::type_check_pbes_spec(result);
    if (result == NULL) {
      throw mcrl2::runtime_error("type checking failed");
    }
    result = data::detail::internal_format_conversion(result);

    p = pbes<Container>(result);
#endif
    complete_data_specification(p);
    return from;
  }

  /// \brief Parses a sequence of pbes expressions. The format of the text is as
  /// follows:
  /// <ul>
  /// <li><tt>"datavar"</tt>, followed by a sequence of data variable declarations</li>
  /// <li><tt>"predvar"</tt>, followed by a sequence of predicate variable declarations</li>
  /// <li><tt>"expressions"</tt>, followed by a sequence of pbes expressions separated by newlines</li>
  /// </ul>
  /// An example of this is:
  /// \code
  /// datavar
  ///   n: Nat;
  /// predvar
  ///   X: Pos;
  ///   Y: Nat, Bool;
  /// \endcode
  /// \param text A string
  /// \param data_spec A string
  /// N.B. A side effect of the data specification is that it determines whether rewrite rules
  /// for types like Pos and Nat are generated or not.
  /// \return The parsed expression and the data specification that was used.
  inline
  std::pair<atermpp::vector<pbes_expression>, data::data_specification> parse_pbes_expressions(std::string text, std::string data_spec = "")
  {
    std::string unique_prefix("UNIQUE_PREFIX");
    int unique_prefix_index = 0;

    text = core::remove_comments(text);
    const std::string separator1 = "datavar";
    const std::string separator2 = "predvar";
    const std::string separator3 = "expressions";

    std::string::size_type i = text.find(separator1);
    std::string::size_type j = text.find(separator2);
    std::string::size_type k = text.find(separator3);
    if (i == std::string::npos) {
      throw std::runtime_error("Error in parse_pbes_expressions: could not find keyword " + separator1);
    }
    if (j == std::string::npos) {
      throw std::runtime_error("Error in parse_pbes_expressions: could not find keyword " + separator2);
    }
    if (j == std::string::npos) {
      throw std::runtime_error("Error in parse_pbes_expressions: could not find keyword " + separator3);
    }

    std::string datavar_text     = text.substr(i + separator1.size(), j - i - separator1.size());
    std::string predvar_text     = text.substr(j + separator2.size(), k - j - separator2.size());
    std::string expressions_text = text.substr(k + separator3.size());

    // the generated pbes specification
    std::string pbesspec = "pbes";

    std::vector<std::string> pwords = core::split(predvar_text, ";");
    for (std::vector<std::string>::iterator i = pwords.begin(); i != pwords.end(); ++i)
    {
      if (boost::trim_copy(*i).empty())
      {
        continue;
      }
      std::vector<std::string> args;
      std::vector<std::string> words = core::split(*i, ":");
      std::string var = boost::trim_copy(words[0]);
      if (words.size() >= 2 && !boost::trim_copy(words[1]).empty())
      {
        args = core::split(boost::trim_copy(words[1]), "#");
      }
      for (std::vector<std::string>::iterator j = args.begin(); j != args.end(); ++j)
      {
        std::vector<std::string> w = core::split(*j, ",");
        for (std::vector<std::string>::iterator k = w.begin(); k != w.end(); ++k)
        {
          *k = unique_prefix + boost::lexical_cast<std::string>(unique_prefix_index++) + ": " + *k;
        }
        *j = boost::algorithm::join(w, ", ");
      }
      std::string arg;
      if (!args.empty())
      {
        arg = "(" + boost::algorithm::join(args, ", ") + ")";
      }
      pbesspec = pbesspec + "\nmu " + var + arg + " = true;";
    }

    datavar_text = core::remove_whitespace(datavar_text);
    if (!datavar_text.empty() && datavar_text[datavar_text.size() - 1] == ';')
    {
      datavar_text = datavar_text.erase(datavar_text.size() - 1);
    }
    datavar_text = core::regex_replace(";", ", ", datavar_text);

    // add a dummy propositional variable to the pbes, that is used for the initialization
    pbesspec = pbesspec + "\nmu dummy1 = true;";

    // for each expression add an equation to the pbes
    std::vector<std::string> expressions = core::split(expressions_text, ";");
    if (!expressions.empty() && boost::trim_copy(expressions.back()).empty())
    {
      expressions.pop_back();
    }
    for (std::vector<std::string>::iterator i = expressions.begin(); i != expressions.end(); ++i)
    {
      pbesspec = pbesspec
        + "\nmu "
        + unique_prefix
        + boost::lexical_cast<std::string>(unique_prefix_index++)
        + (datavar_text.empty() ? "" : "(")
        + datavar_text
        + (datavar_text.empty() ? "" : ")")
        + " = "
        + boost::trim_copy(*i) + ";";
    }

    // add an initialization section to the pbes
    pbesspec = data_spec + (data_spec.empty() ? "" : "\n") + pbesspec + "\ninit dummy1;";

    pbes<> p;
    std::stringstream in(pbesspec);
    try
    {
      in >> p;
    }
    catch (std::runtime_error e)
    {
      std::cerr << "parse_pbes_expression: parse error detected in the generated specification\n"
                << pbesspec
                << std::endl;
      throw e;
    }

    atermpp::vector<pbes_expression> result;
    for (pbes<>::container_type::iterator i = p.equations().end() - expressions.size(); i != p.equations().end(); ++i)
    {
      result.push_back(i->formula());
    }

    return std::make_pair(result, p.data());
  }

  /// \brief Parses a single pbes expression.
  /// \param text A string
  /// \param var_decl A string
  /// with their types.<br>
  /// An example of this is:
  /// \code
  /// datavar
  ///   n: Nat;
  /// predvar
  ///   X: Pos;
  ///   Y: Nat, Bool;
  /// \endcode
  /// \param data_spec A string
  /// \return The parsed expression
  inline
  pbes_expression parse_pbes_expression(std::string text, std::string var_decl = "datavar\npredvar\n", std::string data_spec = "")
  {
    return parse_pbes_expressions(var_decl + "\nexpressions\n" + text, data_spec).first.front();
  }

  /// \brief Parses a string with substitutions and adds them to a substition function
  /// \param text A string
  /// \param data_spec A data specification
  /// \param sigma A substitution function
  template <typename SubstitutionFunction>
  void parse_substitutions(std::string text, data::data_specification const& data_spec, SubstitutionFunction& sigma)
  {
    std::cerr << "SSSSWWW" << text << std::endl;
    std::vector<std::string> substitutions = core::split(text, ";");
    for (std::vector<std::string>::iterator i = substitutions.begin(); i != substitutions.end(); ++i)
    {
      std::vector<std::string> words = core::regex_split(*i, ":=");
      if (words.size() != 2)
      {
        continue;
      }
      // std::string spec = core::pp(data_spec);
      std::cerr << "WWWW" << words[0] << "\n";
      data::variable v = data::parse_variable(words[0], data_spec);
      data::data_expression e = data::parse_data_expression(words[1], "", data_spec);
      sigma[v] = e;
    }
  }

  /// \brief Parses a pbes expression.
  /// \param expr A string
  /// \param subst A string
  /// \param p A PBES
  /// \param sigma A substitution function
  /// \return The parsed expression
  template <typename SubstitutionFunction>
  pbes_expression parse_pbes_expression(std::string expr, std::string subst, const pbes<>& p, SubstitutionFunction& sigma)
  {
    typedef core::term_traits<pbes_expression> tr;

    parse_substitutions(subst, p.data(), sigma);

    std::string datavar_text;
    for (typename SubstitutionFunction::iterator i = sigma.begin(); i != sigma.end(); ++i)
    {
      data::variable v = i->first;
      datavar_text = datavar_text + (i == sigma.begin() ? "" : ", ") + core::pp(v) + ": " + core::pp(v.sort());
    }

    pbes<> q = p;
    q.initial_state() == tr::true_();
    std::string pbesspec = core::pp(q);
    std::string init("init");
    // remove the init declaration
    pbesspec = pbesspec.substr(0, std::find_end(pbesspec.begin(), pbesspec.end(), init.begin(), init.end()) - pbesspec.begin());

    // add an equation mu dummy1(vars) = (expr = expr)
    pbesspec = pbesspec
      + "\nmu "
      + "dummy1"
      + (datavar_text.empty() ? "" : "(")
      + datavar_text
      + (datavar_text.empty() ? "" : ")")
      + " = "
      + boost::trim_copy(expr) + ";";

    // add a dummy propositional variable to the pbes, that is used for the initialization
    pbesspec = pbesspec + "\nmu dummy2 = true;";

    // add an initialization section to the pbes
    pbesspec = pbesspec + "\ninit dummy2;";

std::cout << "--------------------------------------------------------------------\n"
          << pbesspec
          << "--------------------------------------------------------------------"
          << std::endl;
    std::stringstream in(pbesspec);
    try
    {
      in >> q;
    }
    catch (std::runtime_error e)
    {
      std::cerr << "parse_pbes_expression: parse error detected in the generated specification\n"
                << pbesspec
                << std::endl;
      throw e;
    }

    pbes_expression result = q.equations()[q.equations().size() - 2].formula();
    return result;
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PARSE_H
