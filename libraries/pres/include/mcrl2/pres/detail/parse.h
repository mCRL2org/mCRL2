// Author(s): Jan Friso Groote. Based on pbes/detail/parse.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/detail/parse.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_DETAIL_PARSE_H
#define MCRL2_PRES_DETAIL_PARSE_H

#include "mcrl2/pres/parse.h"



namespace mcrl2::pres_system {

/// \brief Parses a sequence of pres expressions. The format of the text is as
/// follows:
/// <ul>
/// <li><tt>"datavar"</tt>, followed by a sequence of data variable declarations</li>
/// <li><tt>"predvar"</tt>, followed by a sequence of predicate variable declarations</li>
/// <li><tt>"expressions"</tt>, followed by a sequence of pres expressions separated by newlines</li>
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
std::pair<std::vector<pres_expression>, data::data_specification> parse_pres_expressions(std::string text, const std::string& data_spec = "")
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
    throw std::runtime_error("Error in parse_pres_expressions: could not find keyword " + separator1);
  }
  if (j == std::string::npos)
  {
    throw std::runtime_error("Error in parse_pres_expressions: could not find keyword " + separator2);
  }
  if (k == std::string::npos)
  {
    throw std::runtime_error("Error in parse_pres_expressions: could not find keyword " + separator3);
  }

  std::string datavar_text     = text.substr(i + separator1.size(), j - i - separator1.size());
  std::string predvar_text     = text.substr(j + separator2.size(), k - j - separator2.size());
  std::string expressions_text = text.substr(k + separator3.size());

  // the generated pres specification
  std::string presspec = "pres";

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
    presspec = presspec + "\nmu " + var + arg + " = true;";
  }

  datavar_text = utilities::remove_whitespace(datavar_text);
  if (!datavar_text.empty() && datavar_text[datavar_text.size() - 1] == ';')
  {
    datavar_text = datavar_text.erase(datavar_text.size() - 1);
  }
  datavar_text = utilities::regex_replace(";", ", ", datavar_text);

  // add a dummy propositional variable to the pres, that is used for the initialization
  presspec = presspec + "\nmu dummy1 = true;";

  // for each expression add an equation to the pres
  std::vector<std::string> expressions = utilities::split(expressions_text, ";");
  if (!expressions.empty() && boost::trim_copy(expressions.back()).empty())
  {
    expressions.pop_back();
  }
  for (std::string& expression: expressions)
  {
std::cerr << "EXPRESSION " << expression << "\n";
    presspec = presspec
               + "\nmu "
               + unique_prefix
               + utilities::number2string(unique_prefix_index++)
               + (datavar_text.empty() ? "" : "(")
               + datavar_text
               + (datavar_text.empty() ? "" : ")")
               + " = "
               + boost::trim_copy(expression) + ";";
  }

  // add an initialization section to the pres
  presspec = data_spec + (data_spec.empty() ? "" : "\n") + presspec + "\ninit dummy1;";

  pres p;
  std::stringstream in(presspec);
  try
  {
    in >> p;
  }
  catch (const std::runtime_error& e)
  {
    mCRL2log(log::error) << "parse_pres_expression: parse error detected in the generated specification\n"
                         << presspec
                         << std::endl;
    throw e;
  }

  std::vector<pres_expression> result;
  for (auto i = p.equations().end() - expressions.size(); i != p.equations().end(); ++i)
  {
    result.push_back(i->formula());
  }

  return std::make_pair(result, p.data());
}

/// \brief Parses a single pres expression.
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
pres_expression parse_pres_expression(const std::string& text, const std::string& var_decl = "datavar\npredvar\n", const std::string& data_spec = "")
{
  return parse_pres_expressions(var_decl + "\nexpressions\n" + text, data_spec).first.front();
}

/// \brief Parses a pres expression.
/// \param expr A string
/// \param subst A string
/// \param p A PRES
/// \param sigma A substitution function
/// \return The parsed expression
template <typename SubstitutionFunction>
pres_expression parse_pres_expression(const std::string& expr, const std::string& subst, const pres& p, SubstitutionFunction& sigma)
{
  data::detail::parse_substitution(subst, sigma, p.data());

  std::string datavar_text;
  for (auto i = sigma.begin(); i != sigma.end(); ++i)
  {
    data::variable v = i->first;
    datavar_text = datavar_text + (i == sigma.begin() ? "" : ", ") + data::pp(v) + ": " + data::pp(v.sort());
  }

  pres q = p;
  q.initial_state() = atermpp::down_cast<propositional_variable_instantiation>(true_());
  std::string presspec = pres_system::pp(q);
  std::string init("init");
  // remove the init declaration
  presspec = presspec.substr(0, std::find_end(presspec.begin(), presspec.end(), init.begin(), init.end()) - presspec.begin());

  // add an equation mu dummy1(vars) = (expr = expr)
  presspec = presspec
             + "\nmu "
             + "dummy1"
             + (datavar_text.empty() ? "" : "(")
             + datavar_text
             + (datavar_text.empty() ? "" : ")")
             + " = "
             + boost::trim_copy(expr) + ";";

  // add a dummy propositional variable to the pres, that is used for the initialization
  presspec = presspec + "\nmu dummy2 = true;";

  // add an initialization section to the pres
  presspec = presspec + "\ninit dummy2;";

  std::stringstream in(presspec);
  try
  {
    in >> q;
  }
  catch (const std::runtime_error& e)
  {
    mCRL2log(log::error) << "parse_pres_expression: parse error detected in the generated specification\n"
                         << presspec
                         << std::endl;
    throw e;
  }

  pres_expression result = q.equations()[q.equations().size() - 2].formula();
  return result;
}

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_DETAIL_PARSE_H
