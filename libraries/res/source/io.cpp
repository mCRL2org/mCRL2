// Author(s): Jan Friso Groote. Based a.o. on bes/source/io.cpp by a.o. Thomas Neele
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
#include "mcrl2/res/res2pres.h"
#include "mcrl2/res/detail/io.h"
#include "mcrl2/res/io.h"
#include "mcrl2/res/parse.h"
#include "mcrl2/res/presinst_conversion.h"
#include "mcrl2/core/load_aterm.h"
#include "mcrl2/pres/io.h"

namespace mcrl2
{

namespace res
{

const std::vector<utilities::file_format>& res_file_formats()
{
  static std::vector<utilities::file_format> result;
  if (result.empty())
  {
    result.push_back(utilities::file_format("res", "RES in internal format", false));
    result.back().add_extension("res");
  }

  return result;
}

/// \brief Save a RES in the format specified.
/// \param res The res to re stored
/// \param stream The name of the file to which the output is stored.
/// \param format Determines the format in which the result is written.
void save_res(const res_equation_system& res,
              std::ostream& stream,
              utilities::file_format format)
{
  if (format == utilities::file_format())
  {
    format = res_format_internal();
  }
  mCRL2log(log::verbose) << "Saving result in " << format.shortname() << " format..." << std::endl;
  if (format == res_format_internal())
  {
    atermpp::binary_aterm_ostream(stream) << res;
  }
  else
  if (format == pres_system::pres_format_text())
  {
    stream << res;
  }
  else
  if (pres_system::is_pres_file_format(format))
  {
    res::save_pres(res2pres(res), stream, format);
  }
  else
  {
    throw mcrl2::runtime_error("Trying to save RES in non-RES format (" + format.shortname() + ")");
  }
}

/// \brief Load res from a stream.
/// \param res The res to which the result is loaded.
/// \param stream The file from which to load the RES.
/// \param format The format that should re assumed for the stream.
/// \param source The source from which the stream originates. Used for error messages.
void load_res(res_equation_system& res, std::istream& stream, utilities::file_format format, const std::string& source)
{
  if (format == utilities::file_format())
  {
    format = res_format_internal();
  }
  mCRL2log(log::verbose) << "Loading RES in " << format.shortname() << " format..." << std::endl;
  if (format == res_format_internal())
  {
    atermpp::binary_aterm_istream(stream) >> res;
  }
  else
  if (format == pres_system::pres_format_text())
  {
    stream >> res;
  }
  else
  if (pres_system::is_pres_file_format(format))
  {
    pres_system::pres pres;
    load_pres(pres, stream, format, source);
    if(!pres_system::algorithms::is_res(pres))
    {
      throw mcrl2::runtime_error("The PRES that was loaded is not a RES");
    }
    res = res::presinst_conversion(pres);
  }
  else
  {
    throw mcrl2::runtime_error("Trying to load RES from non-RES format (" + format.shortname() + ")");
  }
}

/// \brief save_res Saves a RES to a file.
/// \param res The RES to save.
/// \param filename The file to save the RES in.
/// \param format The format in which to save the RES.
void save_res(const res_equation_system& res,
              const std::string& filename,
              utilities::file_format format)
{
  if (format == utilities::file_format())
  {
    format = guess_format(filename);
  }

 if (filename.empty())
  {
    save_res(res, std::cout, format);
  }
  else
  {
    std::ofstream filestream(filename,(format.text_format()?std::ios_base::out: std::ios_base::binary));
    if (!filestream.good())
    {
      throw mcrl2::runtime_error("Could not open file " + filename);
    }
    save_res(res, filestream, format);
  }
}

/// \brief Loads a RES from a file.
/// \param res The object in which the result is stored.
/// \param filename The file from which to load the RES.
/// \param format An indication of the file format. If this is file_format() the
///        format of the file in infilename is guessed.
void load_res(res_equation_system& res, const std::string& filename, utilities::file_format format)
{
  if (format == utilities::file_format())
  {
    format = guess_format(filename);
  }
  if (filename.empty())
  {
    load_res(res, std::cin, format);
  }
  else
  {
    std::ifstream filestream(filename,(format.text_format()?std::ios_base::in: std::ios_base::binary));
    if (!filestream.good())
    {
      throw mcrl2::runtime_error("Could not open file " + filename);
    }
    load_res(res, filestream, format, core::detail::file_source(filename));
  }
}

/// \brief Loads a PRES from a file. If the file stores a RES, then it is converted to a PRES.
/// \param pres The object in which the result is stored.
/// \param filename The file from which to load the PRES.
/// \param format An indication of the file format. If this is file_format() the
///        format of the file in infilename is guessed.
void load_pres(pres_system::pres& pres,
               const std::string& filename,
               utilities::file_format format)
{
  if (format == utilities::file_format())
  {
    format = pres_system::guess_format(filename);
    if (format == utilities::file_format())
    {
      format = guess_format(filename);
    }
  }
  if (pres_system::is_pres_file_format(format))
  {
    pres_system::load_pres(pres, filename, format);
    return;
  }
  res_equation_system res;
  load_res(res, filename, format);
  pres = res2pres(res);
}

/// \brief Saves a PRES to a stream. If the PRES is not a RES and a RES file format is requested, an
///        exception is thrown.
/// \param pres The object in which the PRES is stored.
/// \param stream The stream which to save the PRES to.
/// \param format The file format to store the PRES in.
///
/// This function converts the pres_system::pres to a res_equation_system if the requested file
/// format does not provide a save routine for pres_system::pres structures.
void save_pres(const pres_system::pres& pres, std::ostream& stream,
               const utilities::file_format& format)
{
  if (pres_system::is_pres_file_format(format) || format == utilities::file_format())
  {
    pres_system::save_pres(pres, stream, format);
  }
  else
  {
    if (pres_system::algorithms::is_res(pres))
    {
      save_res(presinst_conversion(pres), stream, format);
    }
    else
    {
      throw mcrl2::runtime_error("Trying to save a PRES with data parameters as a RES.");
    }
  }
}

/// \brief Saves a PRES to a file. If the PRES is not a RES and a RES file format is requested, an
///        exception is thrown.
/// \param pres The object in which the PRES is stored.
/// \param filename The file which to save the PRES to.
/// \param format The file format to store the PRES in.
///
/// The format of the file in infilename is guessed.
void save_pres(const pres_system::pres& pres,
               const std::string& filename,
               utilities::file_format format)
{
  if (format == utilities::file_format())
  {
    format = guess_format(filename);
  }
  if (filename.empty())
  {
    res::save_pres(pres, std::cout, format);
  }
  else
  {
    std::ofstream filestream(filename,(format.text_format()?std::ios_base::out: std::ios_base::binary));
    if (!filestream.good())
    {
      throw mcrl2::runtime_error("Could not open file " + filename);
    }
    res::save_pres(pres, filestream, format);
  }
}

// transforms BooleanVariableNoIndex to BooleanVariable
static atermpp::aterm_appl add_index_impl(const atermpp::aterm_appl& x)    // This function can re removed in due time, say in the year 2025.
{
  if (x.function() == core::detail::function_symbol_BooleanVariableNoIndex())
  {
    const res::res_variable& y = reinterpret_cast<const res::res_variable&>(x);
    return res::res_variable(y.name()); 
  }
  return x;
}

atermpp::aterm res_equation_system_marker()
{
  return atermpp::aterm_appl(atermpp::function_symbol("res_equation_system", 0));
}

atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const res_equation& equation)
{
  atermpp::aterm_stream_state state(stream);

  stream << equation.symbol();
  stream << equation.variable();
  stream << equation.formula();

  return stream;
}

atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, res_equation& equation)
{
  atermpp::aterm_stream_state state(stream);
  stream >> add_index_impl;

  fixpoint_symbol symbol;
  res_variable variable;
  res_expression formula;

  stream >> symbol;
  stream >> variable;
  stream >> formula;

  equation = res_equation(symbol, variable, formula);

  return stream;
}

atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const res_equation_system& res)
{
  atermpp::aterm_stream_state state(stream);

  stream << res_equation_system_marker();
  stream << res.initial_state();
  stream << res.equations();

  return stream;
}

atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, res_equation_system& res)
{
  atermpp::aterm_stream_state state(stream);
  stream >> add_index_impl;

  try
  {
    atermpp::aterm marker;

    stream >> marker;
    if (marker != res_equation_system_marker())
    {
      throw mcrl2::runtime_error("Stream does not contain a res equation system (RES).");
    }

    res_expression initial_state;
    std::vector<res_equation> equations;

    stream >> initial_state;
    stream >> equations;

    res = res_equation_system(equations, initial_state);
  }
  catch (std::exception& ex)
  {
    mCRL2log(log::error) << ex.what() << "\n";
    throw mcrl2::runtime_error(std::string("Error reading res equation system (RES)."));
  }
  return stream;
}

} // namespace res

} // namespace mcrl2
