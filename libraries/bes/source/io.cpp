// Author(s): anonymous, Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file io.cpp
/// \brief

#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_io_binary.h"
#include "mcrl2/bes/bes2pbes.h"
#include "mcrl2/bes/detail/io.h"
#include "mcrl2/bes/io.h"
#include "mcrl2/bes/parse.h"
#include "mcrl2/bes/pbesinst_conversion.h"
#include "mcrl2/bes/pg_parse.h"
// #include "mcrl2/core/detail/default_values.h"
#include "mcrl2/core/load_aterm.h"
#include "mcrl2/pbes/io.h"

namespace mcrl2
{

namespace bes
{

const std::vector<utilities::file_format>& bes_file_formats()
{
  static std::vector<utilities::file_format> result;
  if (result.empty())
  {
    result.push_back(utilities::file_format("bes", "BES in internal format", false));
    result.back().add_extension("bes");
    result.push_back(utilities::file_format("pgsolver", "BES in PGSolver format", true));
    result.back().add_extension("gm");
    result.back().add_extension("pg");
  }

  return result;
}

/// \brief Save a BES in the format specified.
/// \param bes The bes to be stored
/// \param stream The name of the file to which the output is stored.
/// \param format Determines the format in which the result is written.
void save_bes(const boolean_equation_system& bes,
              std::ostream& stream,
              utilities::file_format format)
{
  if (format == utilities::file_format())
  {
    format = bes_format_internal();
  }
  mCRL2log(log::verbose) << "Saving result in " << format.shortname() << " format..." << std::endl;
  if (format == bes_format_internal())
  {
    atermpp::binary_aterm_ostream(stream) << bes;
  }
  else
  if (format == bes_format_pgsolver())
  {
    save_bes_pgsolver(bes, stream);
  }
  else
  if (format == pbes_system::pbes_format_text())
  {
    stream << bes;
  }
  else
  if (pbes_system::is_pbes_file_format(format))
  {
    bes::save_pbes(bes2pbes(bes), stream, format);
  }
  else
  {
    throw mcrl2::runtime_error("Trying to save BES in non-BES format (" + format.shortname() + ")");
  }
}

/// \brief Load bes from a stream.
/// \param bes The bes to which the result is loaded.
/// \param stream The file from which to load the BES.
/// \param format The format that should be assumed for the stream.
/// \param source The source from which the stream originates. Used for error messages.
void load_bes(boolean_equation_system& bes, std::istream& stream, utilities::file_format format, const std::string& source)
{
  if (format == utilities::file_format())
  {
    format = bes_format_internal();
  }
  mCRL2log(log::verbose) << "Loading BES in " << format.shortname() << " format..." << std::endl;
  if (format == bes_format_internal())
  {
    atermpp::binary_aterm_istream(stream) >> bes;
  }
  else
  if (format == bes_format_pgsolver())
  {
    parse_pgsolver(stream, bes);
  }
  else
  if (format == pbes_system::pbes_format_text())
  {
    stream >> bes;
  }
  else
  if (pbes_system::is_pbes_file_format(format))
  {
    pbes_system::pbes pbes;
    load_pbes(pbes, stream, format, source);
    if(!pbes_system::algorithms::is_bes(pbes))
    {
      throw mcrl2::runtime_error("The PBES that was loaded is not a BES");
    }
    bes = bes::pbesinst_conversion(pbes);
  }
  else
  {
    throw mcrl2::runtime_error("Trying to load BES from non-BES format (" + format.shortname() + ")");
  }
}

/// \brief save_bes Saves a BES to a file.
/// \param bes The BES to save.
/// \param filename The file to save the BES in.
/// \param format The format in which to save the BES.
void save_bes(const boolean_equation_system& bes,
              const std::string& filename,
              utilities::file_format format)
{
  if (format == utilities::file_format())
  {
    format = guess_format(filename);
  }

 if (filename.empty())
  {
    save_bes(bes, std::cout, format);
  }
  else
  {
    std::ofstream filestream(filename,(format.text_format()?std::ios_base::out: std::ios_base::binary));
    if (!filestream.good())
    {
      throw mcrl2::runtime_error("Could not open file " + filename);
    }
    save_bes(bes, filestream, format);
  }
}

/// \brief Loads a BES from a file.
/// \param bes The object in which the result is stored.
/// \param filename The file from which to load the BES.
/// \param format An indication of the file format. If this is file_format() the
///        format of the file in infilename is guessed.
void load_bes(boolean_equation_system& bes, const std::string& filename, utilities::file_format format)
{
  if (format == utilities::file_format())
  {
    format = guess_format(filename);
  }
  if (filename.empty())
  {
    load_bes(bes, std::cin, format);
  }
  else
  {
    std::ifstream filestream(filename,(format.text_format()?std::ios_base::in: std::ios_base::binary));
    if (!filestream.good())
    {
      throw mcrl2::runtime_error("Could not open file " + filename);
    }
    load_bes(bes, filestream, format, core::detail::file_source(filename));
  }
}

/// \brief Loads a PBES from a file. If the file stores a BES, then it is converted to a PBES.
/// \param pbes The object in which the result is stored.
/// \param filename The file from which to load the PBES.
/// \param format An indication of the file format. If this is file_format() the
///        format of the file in infilename is guessed.
void load_pbes(pbes_system::pbes& pbes,
               const std::string& filename,
               utilities::file_format format)
{
  if (format == utilities::file_format())
  {
    format = pbes_system::guess_format(filename);
    if (format == utilities::file_format())
    {
      format = guess_format(filename);
    }
  }
  if (pbes_system::is_pbes_file_format(format))
  {
    pbes_system::load_pbes(pbes, filename, format);
    return;
  }
  boolean_equation_system bes;
  load_bes(bes, filename, format);
  pbes = bes2pbes(bes);
}

/// \brief Saves a PBES to a stream. If the PBES is not a BES and a BES file format is requested, an
///        exception is thrown.
/// \param pbes The object in which the PBES is stored.
/// \param stream The stream which to save the PBES to.
/// \param format The file format to store the PBES in.
///
/// This function converts the pbes_system::pbes to a boolean_equation_system if the requested file
/// format does not provide a save routine for pbes_system::pbes structures.
void save_pbes(const pbes_system::pbes& pbes, std::ostream& stream,
               const utilities::file_format& format)
{
  if (pbes_system::is_pbes_file_format(format) || format == utilities::file_format())
  {
    pbes_system::save_pbes(pbes, stream, format);
  }
  else
  {
    if (pbes_system::algorithms::is_bes(pbes))
    {
      save_bes(pbesinst_conversion(pbes), stream, format);
    }
    else
    {
      throw mcrl2::runtime_error("Trying to save a PBES with data parameters as a BES.");
    }
  }
}

/// \brief Saves a PBES to a file. If the PBES is not a BES and a BES file format is requested, an
///        exception is thrown.
/// \param pbes The object in which the PBES is stored.
/// \param filename The file which to save the PBES to.
/// \param format The file format to store the PBES in.
///
/// The format of the file in infilename is guessed.
void save_pbes(const pbes_system::pbes& pbes,
               const std::string& filename,
               utilities::file_format format)
{
  if (format == utilities::file_format())
  {
    format = guess_format(filename);
  }
  if (filename.empty())
  {
    bes::save_pbes(pbes, std::cout, format);
  }
  else
  {
    std::ofstream filestream(filename,(format.text_format()?std::ios_base::out: std::ios_base::binary));
    if (!filestream.good())
    {
      throw mcrl2::runtime_error("Could not open file " + filename);
    }
    bes::save_pbes(pbes, filestream, format);
  }
}


/// \brief Conversion to aterm_appl.
/// \return The boolean equation system converted to aterm format.
inline
atermpp::aterm_appl boolean_equation_system_to_aterm(const boolean_equation_system& p)
{
  atermpp::aterm_list eqn_list;
  const std::vector<boolean_equation>& eqn = p.equations();

  for (auto i = eqn.rbegin(); i != eqn.rend(); ++i)
  {
    atermpp::aterm a = boolean_equation_to_aterm(*i);
    eqn_list.push_front(a);
  }

  return atermpp::aterm_appl(core::detail::function_symbol_BES(), eqn_list, p.initial_state());
}

// transforms BooleanVariable to BooleanVariableNoIndex
static atermpp::aterm_appl remove_index_impl(const atermpp::aterm_appl& x)
{
  if (x.function() == core::detail::function_symbol_BooleanVariable())
  {
    return atermpp::aterm_appl(core::detail::function_symbol_BooleanVariableNoIndex(), x.begin(), --x.end());
  }
  return x;
}

// transforms BooleanVariableNoIndex to BooleanVariable
static atermpp::aterm_appl add_index_impl(const atermpp::aterm_appl& x)
{
  if (x.function() == core::detail::function_symbol_BooleanVariableNoIndex())
  {
    const bes::boolean_variable& y = atermpp::down_cast<const bes::boolean_variable>(x);
    std::size_t index = core::index_traits<bes::boolean_variable, bes::boolean_variable_key_type, 1>::insert(y.name());
    return atermpp::aterm_appl(core::detail::function_symbol_BooleanVariable(), x[0], atermpp::aterm_int(index));
  }
  return x;
}

atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const boolean_equation_system& bes)
{
  atermpp::aterm_stream_state state(stream);
  stream << remove_index_impl;

  stream << bes.initial_state();
  stream << bes.equations();

  return stream;
}

atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const boolean_equation& equation)
{
  atermpp::aterm_stream_state state(stream);
  stream << remove_index_impl;

  stream << equation.symbol();
  stream << equation.variable();
  stream << equation.formula();

  return stream;
}

atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, boolean_equation_system& bes)
{
  atermpp::aterm_stream_state state(stream);
  stream >> add_index_impl;

  boolean_expression initial_state;
  std::vector<boolean_equation> equations;

  stream >> initial_state;
  stream >> equations;

  bes = boolean_equation_system(equations, initial_state);
  return stream;
}

atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, boolean_equation& equation)
{
  atermpp::aterm_stream_state state(stream);
  stream >> add_index_impl;

  fixpoint_symbol symbol;
  boolean_variable variable;
  boolean_expression formula;

  stream >> symbol;
  stream >> variable;
  stream >> formula;

  equation = boolean_equation(symbol, variable, formula);

  return stream;
}

} // namespace bes

} // namespace mcrl2
