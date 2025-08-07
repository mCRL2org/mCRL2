// Author(s): Jan Friso Groote. Based on pbes/source/io.cpp by a.o. Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/atermpp/aterm_io_binary.h"
#include "mcrl2/core/load_aterm.h"
#include "mcrl2/data/data_io.h"
#include "mcrl2/pres/algorithms.h"
#include "mcrl2/pres/detail/pres_io.h"
#include "mcrl2/pres/io.h"
#include "mcrl2/pres/parse.h"



namespace mcrl2::pres_system
{

const std::vector<utilities::file_format>& pres_file_formats()
{
  static std::vector<utilities::file_format> result;
  if (result.empty())
  {
    result.emplace_back("pres", "PRES in internal format", false);
    result.back().add_extension("pres");
    result.emplace_back("text", "PRES in textual (mCRL2) format", true);
    result.back().add_extension("txt");
  }
  return result;
}


/// \brief Save a PRES in the format specified.
/// \param pres The PRES to be stored
/// \param stream The stream to which the output is saved.
/// \param format Determines the format in which the result is written. If unspecified, or
///        pres_file_unknown is specified, then a default format is chosen.
void save_pres(const pres& pres,
               std::ostream& stream,
               utilities::file_format format)
{
  if (format == utilities::file_format())
  {
    format = pres_format_internal();
  }
  mCRL2log(log::verbose) << "Saving result in " << format.shortname() << " format..." << std::endl;
  if (format == pres_format_internal())
  {
    atermpp::binary_aterm_ostream(stream) << pres;
  }
  else
  if (format == pres_format_text())
  {
    stream << pp(pres);
  }
  else
  {
    throw mcrl2::runtime_error("Trying to save PRES in non-PRES format (" + format.shortname() + ")");
  }
}

/// \brief Load a PRES from file.
/// \param pres The PRES to which the result is loaded.
/// \param stream The stream from which to load the PRES.
/// \param format The format that should be assumed for the file in infilename. If unspecified, or
///        pres_file_unknown is specified, then a default format is chosen.
/// \param source The source from which the stream originates. Used for error messages.
void load_pres(pres& pres, std::istream& stream, utilities::file_format format, const std::string& /*source*/)
{
  if (format == utilities::file_format())
  {
    format = pres_format_internal();
  }
  mCRL2log(log::verbose) << "Loading PRES in " << format.shortname() << " format..." << std::endl;
  if (format == pres_format_internal())
  {
    atermpp::binary_aterm_istream(stream) >> pres;
  }
  else
  if (format == pres_format_text())
  {
    stream >> pres;
  }
  else
  {
    throw mcrl2::runtime_error("Trying to load PRES from non-PRES format (" + format.shortname() + ")");
  }
}

/// \brief save_pres Saves a PRES to a file.
/// \param pres The PRES to save.
/// \param filename The file to save the PRES in.
/// \param format The format in which to save the PRES.
/// \param welltypedness_check If set to false, skips checking whether pres is well typed before
///                            saving it to file.
///
/// The format of the file in infilename is guessed if format is not given or if it is equal to
/// utilities::file_format().
void save_pres(const pres& pres, const std::string& filename,
               utilities::file_format format,
               bool welltypedness_check)
{
  if (welltypedness_check)
  {
    assert(pres.is_well_typed());
  }
  if (format == utilities::file_format())
  {
    format = guess_format(filename);
  }

  if (filename.empty() || filename == "-")
  {
    save_pres(pres, std::cout, format);
  }
  else
  {
    std::ofstream filestream(filename,(format.text_format()?std::ios_base::out: std::ios_base::binary));
    if (!filestream.good())
    {
      throw mcrl2::runtime_error("Could not open file " + filename);
    }
    save_pres(pres, filestream, format);
  }
}

/// \brief Load pres from file.
/// \param pres The pres to which the result is loaded.
/// \param filename The file from which to load the PRES.
/// \param format The format in which the PRES is stored in the file.
///
/// The format of the file in infilename is guessed if format is not given or if it is equal to
/// utilities::file_format().
void load_pres(pres& pres,
               const std::string& filename,
               utilities::file_format format)
{
  if (format == utilities::file_format())
  {
    format = guess_format(filename);
  }
  if (filename.empty() || filename == "-")
  {
    load_pres(pres, std::cin, format);
  }
  else
  {
    std::ifstream filestream(filename,(format.text_format()?std::ios_base::in: std::ios_base::binary));
    if (!filestream.good())
    {
      throw mcrl2::runtime_error("Could not open file " + filename);
    }
    load_pres(pres, filestream, format, core::detail::file_source(filename));
  }
}

// transforms DataVarId to DataVarIdNoIndex
// transforms OpId to OpIdNoIndex
// transforms PropVarInst to PropVarInstNoIndex
static atermpp::aterm remove_index_impl(const atermpp::aterm& x)
{
  if (x.function() == core::detail::function_symbol_OpId())
  {
    return atermpp::aterm(core::detail::function_symbol_OpIdNoIndex(), x.begin(), --x.end());
  }
  return x;
}

// transforms DataVarIdNoIndex to DataVarId (obsolete)
// transforms OpIdNoIndex to OpId
// transforms PropVarInstNoIndex to PropVarInst (obsolete)
static atermpp::aterm add_index_impl(const atermpp::aterm& x)
{
  if (x.function() == core::detail::function_symbol_DataVarIdNoIndex())
  {
    const data::variable& y = reinterpret_cast<const data::variable&>(x);
    return data::variable(y.name(), y.sort());
  }
  else if (x.function() == core::detail::function_symbol_OpIdNoIndex())    // This should be removed in due time. Say 2025.
  {
    const data::function_symbol& y = reinterpret_cast<const data::function_symbol&>(x);
    return data::function_symbol(y.name(), y.sort());
  }
  else if (x.function() == core::detail::function_symbol_PropVarInstNoIndex())    // This should be removed in due time. Say 2025. 
  {
    const pres_system::propositional_variable_instantiation& y = reinterpret_cast<const pres_system::propositional_variable_instantiation&>(x);
    return pres_system::propositional_variable_instantiation(y.name(), y.parameters());
  }
  return x;
}

inline atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const pres_equation& equation)
{
  stream << equation.symbol();
  stream << equation.variable();
  stream << equation.formula();
  return stream;
}

inline atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, pres_equation& equation)
{
  fixpoint_symbol symbol;
  propositional_variable var;
  pres_expression expression;

  stream >> symbol;
  stream >> var;
  stream >> expression;

  equation = pres_equation(symbol, var, expression);

  return stream;
}

atermpp::aterm pres_marker()
{
  return atermpp::aterm(atermpp::function_symbol("parameterised_boolean_equation_system", 0));
}

atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const pres& pres)
{
  atermpp::aterm_stream_state state(stream);
  stream << remove_index_impl;

  stream << pres_marker();
  stream << pres.data();
  stream << pres.global_variables();
  stream << pres.equations();
  stream << pres.initial_state();
  return stream;
}

atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, pres& pres)
{
  atermpp::aterm_stream_state state(stream);
  stream >> add_index_impl;

  try
  {
    atermpp::aterm marker;
    stream >> marker;

    if (marker != pres_marker())
    {
      throw mcrl2::runtime_error("Stream does not contain a parameterised boolean equation system (PRES).");
    }

    data::data_specification data;
    std::set<data::variable> global_variables;
    std::vector<pres_equation> equations;
    propositional_variable_instantiation initial_state;

    stream >> data;
    stream >> global_variables;
    stream >> equations;
    stream >> initial_state;

    pres = pres_system::pres(data, global_variables, equations, initial_state);

    // Add all the sorts that are used in the specification
    // to the data specification. This is important for those
    // sorts that are built in, because these are not explicitly
    // declared.
    complete_data_specification(pres);
  }
  catch (std::exception& ex)
  {
    mCRL2log(log::error) << ex.what() << "\n";
    throw mcrl2::runtime_error(std::string("Error reading parameterised boolean equation system (PRES)."));
  }

  return stream;
}

namespace detail
{

pres load_pres(const std::string& filename)
{
  pres result;
  if (filename.empty() || filename == "-")
  {
    atermpp::binary_aterm_istream(std::cin) >> result;
  }
  else
  {
    std::ifstream from(filename, std::ifstream::in | std::ifstream::binary);
    atermpp::binary_aterm_istream(from) >> result;
  }
  return result;
}

void save_pres(const pres& presspec, const std::string& filename)
{
  if (filename.empty() || filename == "-")
  {
    atermpp::binary_aterm_ostream(std::cout) << presspec;
  }
  else
  {
    std::ofstream to(filename, std::ofstream::out | std::ofstream::binary);
    if (!to.good())
    {
      throw mcrl2::runtime_error("Could not write to filename " + filename);
    }
    atermpp::binary_aterm_ostream(to) << presspec;
  }
}

} // namespace detail

/// \brief Conversion to atermappl.
/// \return The PRES converted to aterm format.
atermpp::aterm pres_to_aterm(const pres& p)
{
  atermpp::aterm global_variables = atermpp::aterm(core::detail::function_symbol_GlobVarSpec(),
                                                             data::variable_list(p.global_variables().begin(),
                                                                                 p.global_variables().end()));

  atermpp::aterm_list eqn_list;
  const std::vector<pres_equation>& eqn = p.equations();
  for (std::vector<pres_equation>::const_reverse_iterator i = eqn.rbegin(); i != eqn.rend(); ++i)
  {
    atermpp::aterm a = pres_equation_to_aterm(*i);
    eqn_list.push_front(a);
  }
  atermpp::aterm equations = atermpp::aterm(core::detail::function_symbol_PREqnSpec(), eqn_list);
  atermpp::aterm initial_state = atermpp::aterm(core::detail::function_symbol_PRInit(), p.initial_state());
  atermpp::aterm result;

  result = atermpp::aterm(core::detail::function_symbol_PRES(),
             data::detail::data_specification_to_aterm(p.data()),
             global_variables,
             equations,
             initial_state);
  return result;
}

} // namespace mcrl2::pres_system


