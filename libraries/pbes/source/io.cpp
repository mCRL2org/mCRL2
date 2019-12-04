// Author(s): anonymous, Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/core/load_aterm.h"
#include "mcrl2/data/data_io.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/detail/io.h"
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
    pbes.save(stream, true);
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
    pbes.load(stream, true, source);
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

/// \brief Reads the parameterized boolean equation system from a stream.
/// \param stream The stream to read from.
/// \param binary An indicator whether the stream is binary or textual.
/// \param source The source from which the stream originates. Used for error messages.
void pbes::load(std::istream& stream, bool binary, const std::string& source)
{
  atermpp::aterm t = core::load_aterm(stream, binary, "PBES", source, pbes_system::detail::add_index_impl);

  if (!t.type_is_appl() || !core::detail::check_rule_PBES(atermpp::down_cast<atermpp::aterm_appl>(t)))
  {
    throw mcrl2::runtime_error("The loaded ATerm is not a PBES.");
  }

  init_term(atermpp::down_cast<atermpp::aterm_appl>(t));
  m_data.declare_data_specification_to_be_type_checked();

  // Add all the sorts that are used in the specification
  // to the data specification. This is important for those
  // sorts that are built in, because these are not explicitly
  // declared.
  complete_data_specification(*this);

  // The well typedness check is only done in debug mode, since for large
  // PBESs it takes too much time
  assert(is_well_typed());
}

/// \brief Writes the pbes to a stream.
/// \param stream The stream to which the pbes is written.
/// \param binary If binary is true the pbes is saved in compressed binary format.
/// Otherwise an ascii representation is saved. In general the binary format is
/// much more compact than the ascii representation.
void pbes::save(std::ostream& stream, bool binary) const
{
  if (binary)
  {
    atermpp::binary_aterm_ostream(stream) << pbes_system::detail::remove_index_impl << pbes_to_aterm(*this);
  }
  else
  {
    atermpp::text_aterm_ostream(stream) << pbes_system::detail::remove_index_impl << pbes_to_aterm(*this);
  }
}

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
