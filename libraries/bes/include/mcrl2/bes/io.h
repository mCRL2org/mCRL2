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
#include "mcrl2/bes/bes2pbes.h"
#include "mcrl2/bes/print.h"
#include "mcrl2/bes/pg_parse.h"
#include "mcrl2/pbes/file_formats.h"
#include "mcrl2/pbes/is_bes.h"
#include "mcrl2/pbes/pbesinstconversion.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/utilities/exception.h"

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
    result = "(" + left + "&" + right + ")";
  }
  else if (tr::is_or(p))
  {
    std::string left = bes_expression2cwi(tr::left(p), variables);
    std::string right = bes_expression2cwi(tr::right(p), variables);
    result = "(" + left + "|" + right + ")";
  }
  else if (tr::is_prop_var(p))
  {
    typename VariableMap::const_iterator i = variables.find(tr::name(p));
    if (i == variables.end())
    {
      throw mcrl2::runtime_error("Found undeclared variable in bes_expression2cwi: " + tr::pp(p));
    }
    std::stringstream out;
    out << "X" << i->second;
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
  int index = 1;
  for (Iter i = first; i != last; ++i)
  {
    variables[i->variable().name()] = index++;
  }

  for (Iter i = first; i != last; ++i)
  {
    out << (i->symbol().is_mu() ? "min" : "max")
        << " "
        << "X" << variables[i->variable().name()]
        << "="
        << bes_expression2cwi(i->formula(), variables)
        << "\n";
  }
}

/// \brief Save a sequence of BES equations in CWI format to a file.
template <typename Iter>
void bes2cwi(Iter first, Iter last, const std::string& outfilename)
{
  if (outfilename == "-" || outfilename.empty())
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
    std::set<Expression> expressions = split_and(p);
    result = boolean_variables2pgsolver(expressions.begin(), expressions.end(), variables);
  }
  else if (tr::is_or(p))
  {
    std::set<Expression> expressions = split_or(p);
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
void bes2pgsolver(Iter first, Iter last, std::ostream& out, bool maxpg = true)
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

    if(maxpg && block % 2 == 1)
    {
      ++block;
    }

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
          << (maxpg?(block-priority):priority)
          << " "
          << (tr::is_and(i->formula()) ? 1 : (tr::is_or(i->formula())?0:block_to_player[priority]))
          << " "
          << bes_expression2pgsolver(i->formula(), variables)
#ifdef MCRL2_BES2PGSOLVER_PRINT_VARIABLE_NAMES
          << " "
          << "\""
          << tr::pp(i->variable())
          << "\""
#endif
        << ";\n";
  }
}

/// \brief Save a sequence of BES equations in PGSolver format to a file.
template <typename Iter>
void bes2pgsolver(Iter first, Iter last, const std::string& outfilename)
{
  if (outfilename == "-" || outfilename.empty())
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

// Check that the initial equation has, as a left hand side, the variable that
// has been designated as initial.
static
inline bool initial_bes_equation_corresponds_to_initial_state(const boolean_equation_system& bes_spec)
{
  if(is_boolean_variable(bes_spec.initial_state()))
  {
    boolean_variable init(bes_spec.initial_state());
    if(init == bes_spec.equations().begin()->variable())
    {
      return true;
    }
  }
  return false;
}

/// \brief File formats for BES and PBES are the same.
typedef pbes_system::pbes_file_format bes_file_format;
using pbes_system::pbes_file_bes;
using pbes_system::pbes_file_cwi;
using pbes_system::pbes_file_pgsolver;
using pbes_system::pbes_file_pbes;
using pbes_system::pbes_file_unknown;

/// \brief Save a BES in the format specified.
/// \param bes_spec The bes to be stored
/// \param outfilename The name of the file to which the output is stored.
/// \param output_format Determines the format in which the result is written.
/// \param aterm_ascii Determines, if output_format is bes, whether the file
///        is written is ascii format.
inline
void save_bes(const boolean_equation_system& bes_spec,
              const std::string& outfilename,
              const bes_file_format output_format,
              bool aterm_ascii = false)
{
  switch (output_format)
  {
    case pbes_file_bes:
    {
      if (aterm_ascii)
      {
        mCRL2log(log::verbose) << "Saving result in aterm ascii format..." << std::endl;
        bes_spec.save(outfilename, false);
      }
      else
      {
        mCRL2log(log::verbose) << "Saving result in aterm binary format..." << std::endl;
        bes_spec.save(outfilename, true);
      }
      break;
    }
    case pbes_file_cwi:
    {
      mCRL2log(log::verbose) << "Saving result in CWI format..." << std::endl;
      // TODO: clean up the code below.
      if(!initial_bes_equation_corresponds_to_initial_state(bes_spec))
      {
        mCRL2log(log::warning) << "The initial state " + bes::pp(bes_spec.initial_state()) + " and the left hand side of the first equation " + bes::pp(bes_spec.equations().begin()->variable()) + " do not correspond." << std::endl;
        // Determine whether the initial state is in the first block, if so,
        // swap the two equations.
        std::vector<boolean_equation> equations(bes_spec.equations().begin(), bes_spec.equations().end());
        boolean_equation fst = *equations.begin();
        size_t index = 0;
        while(index < equations.size() && fst.symbol() == equations[index].symbol())
        {
          if(equations[index].variable() == boolean_variable(bes_spec.initial_state()))
          {
            break;
          }
          ++index;
        }
        if(fst.symbol() == equations[index].symbol())
        {
          // equation is in the first block, just swap the two
          std::swap(equations[0], equations[index]);
          mCRL2log(log::warning) << "Fixed up by swapping the equations for " << pp(equations[0].variable()) << " and " << pp(equations[index].variable()) << std::endl;
        }
        else
        {
          std::set<boolean_variable> occ = find_boolean_variables(bes_spec);
          std::set<core::identifier_string> occ_ids;
          for(std::set<boolean_variable>::const_iterator i = occ.begin(); i != occ.end(); ++i)
          {
            occ_ids.insert(i->name());
          }

          utilities::number_postfix_generator generator(occ_ids.begin(), occ_ids.end(), "X");
          boolean_variable var = boolean_variable(generator());
          boolean_equation eqn(fst.symbol(), var, bes_spec.initial_state());
          equations.insert(equations.begin(), eqn);
          // bes_spec.initial_state() = var; not used in output

          mCRL2log(log::warning) << "Fixed up by introducing a new initial equation " << pp(eqn) << std::endl;
        }

        bes::bes2cwi(equations.begin(), equations.end(), outfilename);
      }
      else
      {
        bes::bes2cwi(bes_spec.equations().begin(), bes_spec.equations().end(), outfilename);
      }
      break;
    }
    case pbes_file_pgsolver:
    {
      mCRL2log(log::verbose) << "Saving result in PGSolver format..." << std::endl;
      boolean_equation_system bes_spec_standard_form(bes_spec);
      make_standard_form(bes_spec_standard_form, true);
      if(!initial_bes_equation_corresponds_to_initial_state(bes_spec_standard_form))
      {
        throw mcrl2::runtime_error("The initial state " + bes::pp(bes_spec_standard_form.initial_state()) + " and the left hand side of the first equation " + bes::pp(bes_spec_standard_form.equations().begin()->variable()) + " do not correspond. Cannot save BES to PGSolver format.");
      }
      bes::bes2pgsolver(bes_spec_standard_form.equations().begin(), bes_spec_standard_form.equations().end(), outfilename);
      break;
    }
    case pbes_file_pbes:
    {
      mCRL2log(log::verbose) << "Saving result in PGSolver format..." << std::endl;
      bes2pbes(bes_spec).save(outfilename);
      break;
    }
    default:
    {
      throw mcrl2::runtime_error("unknown output format encountered in save_bes");
    }
  }
}

/// \brief Load bes from file.
/// \param b The bes to which the result is loaded.
/// \param infilename The file from which to load the BES.
/// \param f The format that should be assumed for the file in infilename.
inline
void load_bes(boolean_equation_system& b,
              const std::string& infilename,
              const bes_file_format f)
{
  switch (f)
  {
    case pbes_file_bes:
    {
      b.load(infilename);
      break;
    }
    case pbes_file_cwi:
    {
      throw mcrl2::runtime_error("Loading a BES from a CWI file is not implemented");
      break;
    }
    case pbes_file_pgsolver:
    {
      parse_pgsolver(infilename, b);
      break;
    }
    case pbes_file_pbes:
    {
      pbes_system::pbes p;
      p.load(infilename);
      if(!pbes_system::is_bes(p))
      {
        throw mcrl2::runtime_error(infilename + " does not contain a BES");
      }
      b = pbes_system::pbesinstconversion(p);
      break;
    }
    default:
    {
      throw mcrl2::runtime_error("unknown output format encountered in load_bes");
    }
  }
}

/// \brief Load bes from file.
/// \param b The bes to which the result is loaded.
/// \param infilename The file from which to load the BES.
///
/// The format of the file in infilename is guessed.
inline
void load_bes(boolean_equation_system& b,
              const std::string& infilename)
{
  bes_file_format f = pbes_system::guess_format(infilename);
  load_bes(b, infilename, f);
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_IO_H
