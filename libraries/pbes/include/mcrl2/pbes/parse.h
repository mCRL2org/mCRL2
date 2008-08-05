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

#ifndef MCRL2_PBES_PARSER_H
#define MCRL2_PBES_PARSER_H

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
#include "mcrl2/data/parser.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"

namespace mcrl2 {

namespace pbes_system {

  /// Parses a sequence of pbes expressions. The format of the text is as
  /// follows:
  /// <ul>
  /// <li>an mCRL2 data specification</li>
  /// <li><tt>"variables"</tt>, followed by a comma-separated list of variable declarations</li>
  /// <li><tt>"expressions"</tt>, followed by a sequence of modal formulas separated by newlines</li>
  /// </ul>
  inline
  std::pair<std::vector<pbes_expression>, data::data_specification> parse_pbes_expressions(std::string text)
  {
    text = core::remove_comments(text);
    std::vector<pbes_expression> result;
    const std::string separator1 = "variables";
    const std::string separator2 = "expressions";

    std::string::size_type i = text.find(separator1);
    std::string::size_type j = text.find(separator2);

    if (i == std::string::npos) {
      std::cout << "Error in parse_pbes_expressions: could not find keyword '" << separator1 << "'." << std::endl;
      return std::make_pair(result, data::data_specification());
    }
    if (j == std::string::npos) {
      std::cout << "Error in parse_pbes_expressions: could not find keyword '" << separator2 << "'." << std::endl;
      return std::make_pair(result, data::data_specification());
    }

    std::string data_text = text.substr(0, i);
    std::string::size_type k = i + separator1.size();
    std::string variables_text = text.substr(k, j - k);
    std::string expressions_text = text.substr(j + separator2.size());

    std::string spec_text = data_text + "\ninit delta;\n";
    data::data_specification data_spec = data::parse_data_specification(data_text);

    boost::trim(variables_text);
    if (variables_text.size() > 0)
    {
      variables_text = "(" + variables_text + ")";
    }

    std::vector<std::string> paragraphs = core::split_paragraphs(expressions_text);
    for (std::vector<std::string>::iterator i = paragraphs.begin(); i != paragraphs.end(); ++i)
    {
      std::string formula_text = "nu X" + variables_text + "." + *i;
      pbes<> p = lps2pbes(spec_text, formula_text, false);
      pbes_expression e = p.equations().front().formula();
      result.push_back(e);
    }

    return std::make_pair(result, data_spec);
  }

  /// Stream operator for pbes.
  template <typename Container>
  std::istream& operator>>(std::istream& from, pbes<Container>& p)
  {
    ATermAppl result = core::parse_pbes_spec(from);
    if (result == NULL) {
      throw mcrl2::runtime_error("parsing failed");
    }

    result = core::type_check_pbes_spec(result);
    if (result == NULL) {
      throw mcrl2::runtime_error("type checking failed");
    }

    result = core::implement_data_pbes_spec(result);
    if (result == NULL) {
      throw mcrl2::runtime_error("data implementation failed");
    }

    p = pbes<Container>(result);
    return from;
  }

  /// Parses a pbes expression.
  /// \param[in] text The text that is parsed.
  /// \param[in] var_spec An optional specification of data variables and predicate variables
  /// with their types.<br>
  /// An example of this is:
  /// \code
  /// datavar
  ///   n: Nat;
  ///
  /// predvar
  ///   X: Pos;
  ///   Y: Nat, Bool;
  ///
  /// \endcode
  /// \result the parsed expression
  inline
  pbes_expression parse_pbes_expression(std::string text, std::string var_spec)
  {
    std::string unique_prefix("UNIQUE_PREFIX_4123478");
    int index = 0;
    
    var_spec = core::remove_comments(var_spec);
    const std::string separator1 = "datavar";
    const std::string separator2 = "predvar";

    std::string::size_type i = var_spec.find(separator1);
    std::string::size_type j = var_spec.find(separator2);
    if (i == std::string::npos) {
      throw std::runtime_error("Error in parse_pbes_expression: could not find keyword " + separator1);
    }
    if (j == std::string::npos) {
      throw std::runtime_error("Error in parse_pbes_expression: could not find keyword " + separator2);
    }

    std::string::size_type k = i + separator1.size();
    std::string datavar_text = var_spec.substr(k, j - k);
    std::string predvar_text = var_spec.substr(j + separator2.size());

    std::string pbesspec = "pbes";

    std::vector<std::string> pwords = core::split(predvar_text, ";");
    for (std::vector<std::string>::iterator i = pwords.begin(); i != pwords.end(); ++i)
    {
      std::vector<std::string> words = core::split(*i, ":");
      if (words.size() != 2)
      {
        // throw std::runtime_error("Error in parse_pbes_expression: '" + *i + "' has wrong number of : characters");
        continue;
      }
      std::string var  = boost::trim_copy(words[0]);
      std::vector<std::string> args = core::split(boost::trim_copy(words[1]), "#");
      for (std::vector<std::string>::iterator j = args.begin(); j != args.end(); ++j)
      {
        std::vector<std::string> w = core::split(*j, ",");
        for (std::vector<std::string>::iterator k = w.begin(); k != w.end(); ++k)
        {
          *k = unique_prefix + boost::lexical_cast<std::string>(index++) + ": " + *k;
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
    if (datavar_text[datavar_text.size() - 1] == ';')
    {
      datavar_text = datavar_text.erase(datavar_text.size() - 1);
    }
    else
    {
      throw std::runtime_error("Error in parse_pbes_expression: '" + datavar_text + "' has no trailing semicolon");
    }
    datavar_text = core::regex_replace(";", ", ", datavar_text);
    pbesspec = pbesspec + "\nmu dummy1 = true;";
    pbesspec = pbesspec + "\nmu dummy2(" + datavar_text + ") = " + text + ";";
    pbesspec = pbesspec + "\ninit dummy1;";

    pbes<> p;
    std::stringstream in(pbesspec);
    in >> p;
    return p.equations().back().formula();    
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PARSER_H
