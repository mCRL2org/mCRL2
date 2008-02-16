// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/parser.h
/// \brief Parser for pbes expressions.

#ifndef MCRL2_PBES_PARSER_H
#define MCRL2_PBES_PARSER_H

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <boost/algorithm/string.hpp>
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
  std::pair<std::vector<pbes_expression>, data::data_specification> parse_pbes_expressions(const std::string& text)
  {
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

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PARSER_H
