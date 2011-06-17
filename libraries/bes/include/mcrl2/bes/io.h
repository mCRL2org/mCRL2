// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/io.h
/// \brief add your file description here.

#ifndef MCRL2_BES_IO_H
#define MCRL2_BES_IO_H

#include <fstream>
#include <iostream>
#include <map>
#include <utility>
#include <iostream>
#include <string>
#include <algorithm>
#include <functional>
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/normal_forms.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/exception.h"

namespace mcrl2
{

namespace bes
{

/// \brief Convert a BES expression to cwi format.
template <typename Expression, typename VariableMap>
std::string bes_expression2cwi(const Expression& p, const VariableMap& variables)
{
  typedef typename core::term_traits<Expression> tr;

  std::string result;
  if (tr::is_true(p))
  {
    result = "T";
  }
  else if (tr::is_false(p))
  {
    result = "F";
  }
  else if (tr::is_and(p))
  {
    std::string left = bes_expression2cwi(tr::left(p), variables);
    std::string right = bes_expression2cwi(tr::right(p), variables);
    result = "(" + left + " & " + right + ")";
  }
  else if (tr::is_or(p))
  {
    std::string left = bes_expression2cwi(tr::left(p), variables);
    std::string right = bes_expression2cwi(tr::right(p), variables);
    result = "(" + left + " | " + right + ")";
  }
  else if (tr::is_prop_var(p))
  {
    typename VariableMap::const_iterator i = variables.find(tr::name(p));
    if (i == variables.end())
    {
      throw mcrl2::runtime_error("Found undeclared variable in bes_expression2cwi: " + tr::pp(p));
    }
    std::stringstream out;
    out << i->second;
    result = out.str();
  }
  else
  {
    throw mcrl2::runtime_error("Unknown expression encountered in bes_expression2cwi: " + tr::pp(p));
  }
  return result;
}

/// \brief Save a sequence of BES equations in CWI format to a stream.
template <typename Iter>
void bes2cwi(Iter first, Iter last, std::ostream& out)
{
  typedef typename std::iterator_traits<Iter>::value_type equation_type;
  typedef typename equation_type::term_type term_type;
  typedef typename core::term_traits<term_type> tr;
  typedef typename tr::string_type string_type;

  // Number the variables of the equations 0, 1, ... and put them in the map variables.
  std::map<string_type, int> variables;
  int index = 0;
  for (Iter i = first; i != last; ++i)
  {
    variables[i->variable().name()] = index++;
  }

  for (Iter i = first; i != last; ++i)
  {
    out << (i->symbol().is_mu() ? "min" : "max")
        << " "
        << variables[i->variable().name()]
        << " = "
        << bes_expression2cwi(i->formula(), variables)
        << "\n";
  }
}

/// \brief Save a sequence of BES equations in CWI format to a file.
template <typename Iter>
void bes2cwi(Iter first, Iter last, const std::string& outfilename)
{
  if (outfilename == "-")
  {
    bes2cwi(first, last, std::cout);
  }
  else
  {
    std::ofstream out(outfilename.c_str());
    if (out)
    {
      bes2cwi(first, last, out);
    }
    else
    {
      throw mcrl2::runtime_error("could not open file " + outfilename);
    }
  }
}

/// \brief Convert a sequence of Boolean variables to PGSolver format.
template <typename Iter, typename VariableMap>
std::string boolean_variables2pgsolver(Iter first, Iter last, const VariableMap& variables)
{
  typedef typename Iter::value_type expression_type;
  typedef typename core::term_traits<expression_type> tr;

  std::set<int> variables_int;
  for (Iter i = first; i != last; ++i)
  {
    assert(tr::is_variable(*i));
    typename VariableMap::const_iterator j = variables.find(tr::name(*i));
    if (j == variables.end())
    {
      throw mcrl2::runtime_error("Found undeclared variable in boolean_variables2pgsolver: " + tr::pp(*i));
    }
    variables_int.insert(j->second);
  }
  return utilities::string_join(variables_int, ", ");
}

/// \brief Convert a BES expression to PGSolver format.
template <typename Expression, typename VariableMap>
std::string bes_expression2pgsolver(const Expression& p, const VariableMap& variables)
{
  typedef typename core::term_traits<Expression> tr;

  std::string result;
  if (tr::is_and(p))
  {
    atermpp::set<Expression> expressions = split_and(p);
    result = boolean_variables2pgsolver(expressions.begin(), expressions.end(), variables);
  }
  else if (tr::is_or(p))
  {
    atermpp::set<Expression> expressions = split_or(p);
    result = boolean_variables2pgsolver(expressions.begin(), expressions.end(), variables);
  }
  else if (tr::is_prop_var(p))
  {
    typename VariableMap::const_iterator i = variables.find(tr::name(p));
    if (i == variables.end())
    {
      throw mcrl2::runtime_error("Found undeclared variable in bes_expression2cwi: " + tr::pp(p));
    }
    std::stringstream out;
    out << i->second;
    result = out.str();
  }
  else
  {
    throw mcrl2::runtime_error("Unknown or unsupported expression encountered in bes_expression2cwi: " + tr::pp(p));
  }
  return result;
}

/// \brief Save a sequence of BES equations in CWI format to a stream.
template <typename Iter>
void bes2pgsolver(Iter first, Iter last, std::ostream& out)
{
  typedef typename std::iterator_traits<Iter>::value_type equation_type;
  typedef typename equation_type::term_type term_type;
  typedef typename core::term_traits<term_type> tr;
  typedef typename tr::string_type string_type;

    // Number the variables of the equations 0, 1, ... and put them in the map variables.
    // Also store player to which variables without operand are assigned, per
    // block, in block_to_player.
    std::map<string_type, int> variables;
    int index = 0;
    std::map<int, int> block_to_player;

    bool and_in_block = false;
    int block = 0;
    fixpoint_symbol sigma = fixpoint_symbol::nu();
    for (Iter i = first; i != last; ++i)
    {
      if(i->symbol() != sigma)
      {
        block_to_player[block++] = (and_in_block)?1:0;
        and_in_block = false;
        sigma = i->symbol();
      }
      variables[i->variable().name()] = index++;
      and_in_block = and_in_block || tr::is_and(i->formula());
    }
    block_to_player[block] = (and_in_block)?1:0;

    out << "parity " << index -1 << ";\n";

    int priority = 0;
    sigma = fixpoint_symbol::nu();
    for (Iter i = first; i != last; ++i)
    {
      if(i->symbol() != sigma)
      {
        ++priority;
        sigma = i->symbol();
      }

      out << variables[i->variable().name()]
          << " "
          << priority
          << " "
          << (tr::is_and(i->formula()) ? 1 : (tr::is_or(i->formula())?0:block_to_player[priority]))
          << " "
          << bes_expression2pgsolver(i->formula(), variables)
// The following is optional, print variable name for traceability.
//          << " "
//          << "\""
//          << tr::pp(i->variable())
//          << "\""
        << ";\n";
  }
}

/// \brief Save a sequence of BES equations in PGSolver format to a file.
template <typename Iter>
void bes2pgsolver(Iter first, Iter last, const std::string& outfilename)
{
  if (outfilename == "-")
  {
    bes2pgsolver(first, last, std::cout);
  }
  else
  {
    std::ofstream out(outfilename.c_str());
    if (out)
    {
      bes2pgsolver(first, last, out);
    }
    else
    {
      throw mcrl2::runtime_error("could not open file " + outfilename);
    }
  }
}

/// \brief Supported BES output formats.
enum bes_output_format
{
  bes_output_bes,
  bes_output_cwi,
  bes_output_pgsolver
};

/// \brief Save a PBES in the format specified.
inline
void save_bes(const boolean_equation_system<>& bes_spec, std::string outfilename, bes_output_format output_format, bool aterm_ascii = false)
{
  switch (output_format)
  {
    case bes_output_bes:
    {
      if (aterm_ascii)
      {
        mCRL2log(verbose) << "Saving result in ATerm ascii format..." << std::endl;
        bes_spec.save(outfilename, false);
      }
      else
      {
        mCRL2log(verbose) << "Saving result in ATerm binary format..." << std::endl;
        bes_spec.save(outfilename, true);
      }
      break;
    }
    case bes_output_cwi:
    {
      mCRL2log(verbose) << "Saving result in CWI format..." << std::endl;
      bes::bes2cwi(bes_spec.equations().begin(), bes_spec.equations().end(), outfilename);
      break;
    }
    case bes_output_pgsolver:
    {
      mCRL2log(verbose) << "Saving result in PGSolver format..." << std::endl;
      boolean_equation_system<> bes_spec_standard_form(bes_spec);
      make_standard_form(bes_spec_standard_form, true);
      bes::bes2pgsolver(bes_spec_standard_form.equations().begin(), bes_spec_standard_form.equations().end(), outfilename);
      break;
    }
    default:
    {
      throw mcrl2::runtime_error("unknown output format encountered in save_bes");
    }
  }
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_IO_H
