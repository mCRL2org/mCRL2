// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/parse.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PARSE_H
#define MCRL2_PBES_DETAIL_PARSE_H

#include "mcrl2/pbes/parse.h"

namespace mcrl2::pbes_system {

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
std::pair<std::vector<pbes_expression>, data::data_specification> parse_pbes_expressions(std::string text, const std::string& data_spec = "")
{
  std::string unique_prefix("UNIQUE_PREFIX");
  std::size_t unique_prefix_index = 0;

  text = utilities::remove_comments(text);
  const std::string separator1 = "datavar";
  const std::string separator2 = "predvar";
  const std::string separator3 = "expressions";

  std::string::size_type i = text.find(separator1);
  std::string::size_type j = text.find(separator2);
  std::string::size_type k = text.find(separator3);
  if (i == std::string::npos)
  {
    throw std::runtime_error("Error in parse_pbes_expressions: could not find keyword " + separator1);
  }
  if (j == std::string::npos)
  {
    throw std::runtime_error("Error in parse_pbes_expressions: could not find keyword " + separator2);
  }
  if (k == std::string::npos)
  {
    throw std::runtime_error("Error in parse_pbes_expressions: could not find keyword " + separator3);
  }

  std::string datavar_text     = text.substr(i + separator1.size(), j - i - separator1.size());
  std::string predvar_text     = text.substr(j + separator2.size(), k - j - separator2.size());
  std::string expressions_text = text.substr(k + separator3.size());

  // the generated pbes specification
  std::string pbesspec = "pbes";

  std::vector<std::string> pwords = utilities::split(predvar_text, ";");
  for (const std::string& pword: pwords)
  {
    if (boost::trim_copy(pword).empty())
    {
      continue;
    }
    std::vector<std::string> args;
    std::vector<std::string> words = utilities::split(pword, ":");
    std::string var = boost::trim_copy(words[0]);
    if (words.size() >= 2 && !boost::trim_copy(words[1]).empty())
    {
      args = utilities::split(boost::trim_copy(words[1]), "#");
    }
    for (std::string& arg: args)
    {
      std::vector<std::string> w = utilities::split(arg, ",");
      for (std::string& k: w)
      {
        k = unique_prefix + utilities::number2string(unique_prefix_index++) + ": " + k;
      }
      arg = boost::algorithm::join(w, ", ");
    }
    std::string arg;
    if (!args.empty())
    {
      arg = "(" + boost::algorithm::join(args, ", ") + ")";
    }
    pbesspec = pbesspec + "\nmu " + var + arg + " = true;";
  }

  datavar_text = utilities::remove_whitespace(datavar_text);
  if (!datavar_text.empty() && datavar_text[datavar_text.size() - 1] == ';')
  {
    datavar_text = datavar_text.erase(datavar_text.size() - 1);
  }
  datavar_text = utilities::regex_replace(";", ", ", datavar_text);

  // add a dummy propositional variable to the pbes, that is used for the initialization
  pbesspec = pbesspec + "\nmu dummy1 = true;";

  // for each expression add an equation to the pbes
  std::vector<std::string> expressions = utilities::split(expressions_text, ";");
  if (!expressions.empty() && boost::trim_copy(expressions.back()).empty())
  {
    expressions.pop_back();
  }
  for (std::string& expression: expressions)
  {
    pbesspec = pbesspec
               + "\nmu "
               + unique_prefix
               + utilities::number2string(unique_prefix_index++)
               + (datavar_text.empty() ? "" : "(")
               + datavar_text
               + (datavar_text.empty() ? "" : ")")
               + " = "
               + boost::trim_copy(expression) + ";";
  }

  // add an initialization section to the pbes
  pbesspec = data_spec + (data_spec.empty() ? "" : "\n") + pbesspec + "\ninit dummy1;";

  pbes p;
  std::stringstream in(pbesspec);
  try
  {
    in >> p;
  }
  catch (const std::runtime_error& e)
  {
    mCRL2log(log::error) << "parse_pbes_expression: parse error detected in the generated specification\n"
                         << pbesspec
                         << std::endl;
    throw e;
  }

  std::vector<pbes_expression> result;
  for (auto i = p.equations().end() - expressions.size(); i != p.equations().end(); ++i)
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
pbes_expression parse_pbes_expression(const std::string& text, const std::string& var_decl = "datavar\npredvar\n", const std::string& data_spec = "")
{
  return parse_pbes_expressions(var_decl + "\nexpressions\n" + text, data_spec).first.front();
}

/// \brief Parses a pbes expression.
/// \param expr A string
/// \param subst A string
/// \param p A PBES
/// \param sigma A substitution function
/// \return The parsed expression
template <typename SubstitutionFunction>
pbes_expression parse_pbes_expression(const std::string& expr, const std::string& subst, const pbes& p, SubstitutionFunction& sigma)
{
  data::detail::parse_substitution(subst, sigma, p.data());

  std::string datavar_text;
  for (auto i = sigma.begin(); i != sigma.end(); ++i)
  {
    data::variable v = i->first;
    datavar_text = datavar_text + (i == sigma.begin() ? "" : ", ") + data::pp(v) + ": " + data::pp(v.sort());
  }

  pbes q = p;
  q.initial_state() = atermpp::down_cast<propositional_variable_instantiation>(true_());
  std::string pbesspec = pbes_system::pp(q);
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

  std::stringstream in(pbesspec);
  try
  {
    in >> q;
  }
  catch (const std::runtime_error& e)
  {
    mCRL2log(log::error) << "parse_pbes_expression: parse error detected in the generated specification\n"
                         << pbesspec
                         << std::endl;
    throw e;
  }

  pbes_expression result = q.equations()[q.equations().size() - 2].formula();
  return result;
}

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_DETAIL_PARSE_H
