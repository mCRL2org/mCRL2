// Author(s): anonymous, Thomas Neele
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
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/detail/pbes_io.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/parse.h"

namespace mcrl2
{

namespace pbes_system
{

const std::vector<utilities::file_format>& pbes_file_formats()
{
  static std::vector<utilities::file_format> result;
  if (result.empty())
  {
    result.push_back(utilities::file_format("pbes", "PBES in internal format", false));
    result.back().add_extension("pbes");
    result.push_back(utilities::file_format("text", "PBES in textual (mCRL2) format", true));
    result.back().add_extension("txt");
  }
  return result;
}


/// \brief Save a PBES in the format specified.
/// \param pbes The PBES to be stored
/// \param stream The stream to which the output is saved.
/// \param format Determines the format in which the result is written. If unspecified, or
///        pbes_file_unknown is specified, then a default format is chosen.
void save_pbes(const pbes& pbes,
               std::ostream& stream,
               utilities::file_format format)
{
  if (format == utilities::file_format())
  {
    format = pbes_format_internal();
  }
  mCRL2log(log::verbose) << "Saving result in " << format.shortname() << " format..." << std::endl;
  if (format == pbes_format_internal())
  {
    atermpp::binary_aterm_ostream(stream) << pbes;
  }
  else
  if (format == pbes_format_text())
  {
    stream << pp(pbes);
  }
  else
  {
    throw mcrl2::runtime_error("Trying to save PBES in non-PBES format (" + format.shortname() + ")");
  }
}

/// \brief Load a PBES from file.
/// \param pbes The PBES to which the result is loaded.
/// \param stream The stream from which to load the PBES.
/// \param format The format that should be assumed for the file in infilename. If unspecified, or
///        pbes_file_unknown is specified, then a default format is chosen.
/// \param source The source from which the stream originates. Used for error messages.
void load_pbes(pbes& pbes, std::istream& stream, utilities::file_format format, const std::string& source)
{
  if (format == utilities::file_format())
  {
    format = pbes_format_internal();
  }
  mCRL2log(log::verbose) << "Loading PBES in " << format.shortname() << " format..." << std::endl;
  if (format == pbes_format_internal())
  {
    atermpp::binary_aterm_istream(stream) >> pbes;
  }
  else
  if (format == pbes_format_text())
  {
    stream >> pbes;
  }
  else
  {
    throw mcrl2::runtime_error("Trying to load PBES from non-PBES format (" + format.shortname() + ")");
  }
}

/// \brief save_pbes Saves a PBES to a file.
/// \param pbes The PBES to save.
/// \param filename The file to save the PBES in.
/// \param format The format in which to save the PBES.
/// \param welltypedness_check If set to false, skips checking whether pbes is well typed before
///                            saving it to file.
///
/// The format of the file in infilename is guessed if format is not given or if it is equal to
/// utilities::file_format().
void save_pbes(const pbes& pbes, const std::string& filename,
               utilities::file_format format,
               bool welltypedness_check)
{
  if (welltypedness_check)
  {
    assert(pbes.is_well_typed());
  }
  if (format == utilities::file_format())
  {
    format = guess_format(filename);
  }

  if (filename.empty())
  {
    save_pbes(pbes, std::cout, format);
  }
  else
  {
    std::ofstream filestream(filename,(format.text_format()?std::ios_base::out: std::ios_base::binary));
    if (!filestream.good())
    {
      throw mcrl2::runtime_error("Could not open file " + filename);
    }
    save_pbes(pbes, filestream, format);
  }
}

/// \brief Load pbes from file.
/// \param pbes The pbes to which the result is loaded.
/// \param filename The file from which to load the PBES.
/// \param format The format in which the PBES is stored in the file.
///
/// The format of the file in infilename is guessed if format is not given or if it is equal to
/// utilities::file_format().
void load_pbes(pbes& pbes,
               const std::string& filename,
               utilities::file_format format)
{
  if (format == utilities::file_format())
  {
    format = guess_format(filename);
  }
 if (filename.empty())
  {
    load_pbes(pbes, std::cin, format);
  }
  else
  {
    std::ifstream filestream(filename,(format.text_format()?std::ios_base::in: std::ios_base::binary));
    if (!filestream.good())
    {
      throw mcrl2::runtime_error("Could not open file " + filename);
    }
    load_pbes(pbes, filestream, format, core::detail::file_source(filename));
  }
}

// transforms DataVarId to DataVarIdNoIndex
// transforms OpId to OpIdNoIndex
// transforms PropVarInst to PropVarInstNoIndex
static atermpp::aterm_appl remove_index_impl(const atermpp::aterm_appl& x)
{
  if (x.function() == core::detail::function_symbol_DataVarId())
  {
    return atermpp::aterm_appl(core::detail::function_symbol_DataVarIdNoIndex(), x.begin(), --x.end());
  }
  else if (x.function() == core::detail::function_symbol_OpId())
  {
    return atermpp::aterm_appl(core::detail::function_symbol_OpIdNoIndex(), x.begin(), --x.end());
  }
  else if (x.function() == core::detail::function_symbol_PropVarInst())
  {
    return atermpp::aterm_appl(core::detail::function_symbol_PropVarInstNoIndex(), x.begin(), --x.end());
  }
  return x;
}

// transforms DataVarIdNoIndex to DataVarId
// transforms OpIdNoIndex to OpId
// transforms PropVarInstNoIndex to PropVarInst
static atermpp::aterm_appl add_index_impl(const atermpp::aterm_appl& x)
{
  if (x.function() == core::detail::function_symbol_DataVarIdNoIndex())
  {
    const data::variable& y = atermpp::down_cast<const data::variable>(x);
    std::size_t index = core::index_traits<data::variable, data::variable_key_type, 2>::insert(std::make_pair(y.name(), y.sort()));
    return atermpp::aterm_appl(core::detail::function_symbol_DataVarId(), x[0], x[1], atermpp::aterm_int(index));
  }
  else if (x.function() == core::detail::function_symbol_OpIdNoIndex())
  {
    const data::function_symbol& y = atermpp::down_cast<const data::function_symbol>(x);
    std::size_t index = core::index_traits<data::function_symbol, data::function_symbol_key_type, 2>::insert(std::make_pair(y.name(), y.sort()));
    return atermpp::aterm_appl(core::detail::function_symbol_OpId(), x[0], x[1], atermpp::aterm_int(index));
  }
  else if (x.function() == core::detail::function_symbol_PropVarInstNoIndex())
  {
    const pbes_system::propositional_variable_instantiation& y = atermpp::down_cast<const pbes_system::propositional_variable_instantiation>(x);
    std::size_t index = core::index_traits<propositional_variable_instantiation, propositional_variable_key_type, 2>::insert(std::make_pair(y.name(), y.parameters()));
    return atermpp::aterm_appl(core::detail::function_symbol_PropVarInst(), x[0], x[1], atermpp::aterm_int(index));
  }
  return x;
}

inline atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const pbes_equation& equation)
{
  stream << equation.symbol();
  stream << equation.variable();
  stream << equation.formula();
  return stream;
}

inline atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, pbes_equation& equation)
{
  fixpoint_symbol symbol;
  propositional_variable var;
  pbes_expression expression;

  stream >> symbol;
  stream >> var;
  stream >> expression;

  equation = pbes_equation(symbol, var, expression);

  return stream;
}

atermpp::aterm pbes_marker()
{
  return atermpp::aterm_appl(atermpp::function_symbol("parameterised_boolean_equation_system", 0));
}

atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const pbes& pbes)
{
  atermpp::aterm_stream_state state(stream);
  stream << remove_index_impl;

  stream << pbes_marker();
  stream << pbes.data();
  stream << pbes.global_variables();
  stream << pbes.equations();
  stream << pbes.initial_state();
  return stream;
}

atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, pbes& pbes)
{
  atermpp::aterm_stream_state state(stream);
  stream >> add_index_impl;

  try
  {
    atermpp::aterm marker;
    stream >> marker;

    if (marker != pbes_marker())
    {
      throw mcrl2::runtime_error("Stream does not contain a parameterised boolean equation system (PBES).");
    }

    data::data_specification data;
    std::set<data::variable> global_variables;
    std::vector<pbes_equation> equations;
    propositional_variable_instantiation initial_state;

    stream >> data;
    stream >> global_variables;
    stream >> equations;
    stream >> initial_state;

    pbes = pbes_system::pbes(data, equations, global_variables, initial_state);

    // Add all the sorts that are used in the specification
    // to the data specification. This is important for those
    // sorts that are built in, because these are not explicitly
    // declared.
    complete_data_specification(pbes);
  }
  catch (std::exception& ex)
  {
    mCRL2log(log::error) << ex.what() << "\n";
    throw mcrl2::runtime_error(std::string("Error reading parameterised boolean equation system (PBES)."));
  }

  return stream;
}

namespace detail
{

pbes load_pbes(const std::string& filename)
{
  pbes result;
  if (filename.empty())
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

void save_pbes(const pbes& pbesspec, const std::string& filename)
{
  if (filename.empty())
  {
    atermpp::binary_aterm_ostream(std::cout) << pbesspec;
  }
  else
  {
    std::ofstream to(filename, std::ofstream::out | std::ofstream::binary);
    if (!to.good())
    {
      throw mcrl2::runtime_error("Could not write to filename " + filename);
    }
    atermpp::binary_aterm_ostream(to) << pbesspec;
  }
}

} // namespace detail

/// \brief Conversion to atermappl.
/// \return The PBES converted to aterm format.
atermpp::aterm_appl pbes_to_aterm(const pbes& p)
{
  atermpp::aterm_appl global_variables = atermpp::aterm_appl(core::detail::function_symbol_GlobVarSpec(),
                                                             data::variable_list(p.global_variables().begin(),
                                                                                 p.global_variables().end()));

  atermpp::aterm_list eqn_list;
  const std::vector<pbes_equation>& eqn = p.equations();
  for (auto i = eqn.rbegin(); i != eqn.rend(); ++i)
  {
    atermpp::aterm a = pbes_equation_to_aterm(*i);
    eqn_list.push_front(a);
  }
  atermpp::aterm_appl equations = atermpp::aterm_appl(core::detail::function_symbol_PBEqnSpec(), eqn_list);
  atermpp::aterm_appl initial_state = atermpp::aterm_appl(core::detail::function_symbol_PBInit(), p.initial_state());
  atermpp::aterm_appl result;

  result = atermpp::aterm_appl(core::detail::function_symbol_PBES(),
             data::detail::data_specification_to_aterm(p.data()),
             global_variables,
             equations,
             initial_state);

  return result;
}

} // namespace pbes_system

} // namespace mcrl2
