// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts2pbes.cpp
/// \brief This tool reads an LTS and translates it into a PBES.

#define TOOLNAME "lts2pbes"
#define AUTHOR "Wieger Wesselink"

#include <fstream>
#include <iostream>
#include <string>
#include "mcrl2/bes/pbes_output_tool.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/modal_formula/algorithms.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/lts2pbes.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/logger.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using bes::tools::pbes_output_tool;
using utilities::tools::input_output_tool;

class lts2pbes_tool : public pbes_output_tool<input_output_tool>
{
  private:
    typedef pbes_output_tool<input_output_tool> super;

  protected:

    std::string infilename;
    std::string outfilename;
    std::string formfilename;
    mcrl2::lts::data_file_type_t data_file_type;
    std::string data_file;

    void add_options(interface_description& desc)
    {
      super::add_options(desc);

      desc.add_option("formula", make_file_argument("FILE"),
                      "use the state formula from FILE", 'f');

      desc.add_option("data", make_file_argument("FILE"),
                      "use FILE as the data and action specification. "
                      "FILE must be a .mcrl2 file which does not contain an init clause. ", 'D');

      desc.add_option("lps", make_file_argument("FILE"),
                      "use FILE for the data and action specification. "
                      "FILE must be a .lps file. ", 'l');

      desc.add_option("mcrl2", make_file_argument("FILE"),
                      "use FILE as the data and action specification for the LTS. "
                      "FILE must be a .mcrl2 file. ", 'm');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      if (parser.options.count("data"))
      {
        if (1 < parser.options.count("data"))
        {
          mCRL2log(log::warning) << "multiple data specification files are specified; can only use one.\n";
        }
        data_file_type = mcrl2::lts::data_file_type_t::data_e;
        data_file = parser.option_argument("data");
      }

      if (parser.options.count("lps"))
      {
        if (1 < parser.options.count("lps") || data_file_type != mcrl2::lts::data_file_type_t::none_e)
        {
          mCRL2log(log::warning) << "multiple data specification files are specified; can only use one.\n";
        }

        data_file_type = mcrl2::lts::data_file_type_t::lps_e;
        data_file = parser.option_argument("lps");
      }

      if (parser.options.count("mcrl2"))
      {
        if (1 < parser.options.count("mcrl2") || data_file_type != mcrl2::lts::data_file_type_t::none_e)
        {
          mCRL2log(log::warning) << "multiple data specification files are specified; can only use one.\n";
        }

        data_file_type = mcrl2::lts::data_file_type_t::mcrl2_e;
        data_file = parser.option_argument("mcrl2");
      }

      if (parser.options.count("formula"))
      {
        formfilename = parser.option_argument("formula");
      }

      infilename  = input_filename();
      outfilename = output_filename();
    }

  public:

    lts2pbes_tool() : super(
        TOOLNAME,
        AUTHOR,
        "translates an LTS into an PBES",
        "Translates an LTS in INFILE and writes the resulting PBES to "
        "OUTFILE. If OUTFILE is not present, standard output is used. If INFILE is not "
        "present, standard input is used."),
      data_file_type(mcrl2::lts::data_file_type_t::none_e)
    {}

  protected:

    // extracts a specification from an LTS
    lps::specification extract_specification(const lts::lts_lts_t& l)
    {
      lps::action_summand_vector action_summands;
      data::variable process_parameter("x", data::sort_pos::pos());
      data::variable_list process_parameters({ process_parameter });
      std::set<data::variable> global_variables;
      // Add a single delta.
      lps::deadlock_summand_vector deadlock_summands(1, lps::deadlock_summand(data::variable_list(), data::sort_bool::true_(), lps::deadlock()));
      lps::linear_process lps(process_parameters, deadlock_summands, lps::action_summand_vector());
      lps::process_initializer initial_process(data::assignment_list({ data::assignment(process_parameter, data::sort_pos::pos(l.initial_state() + 1)) }));
      return lps::specification(l.data(), l.action_labels(), global_variables, lps, initial_process);
    }

  public:
    bool run()
    {
      const lts::lts_type input_type = lts::detail::guess_format(infilename);
      lts::lts_lts_t l;
      load_lts(l, infilename, input_type, data_file_type, data_file);

      //load formula file
      lps::specification lpsspec = extract_specification(l);
      std::ifstream from(formfilename.c_str());
      if (!from)
      {
        throw mcrl2::runtime_error("Cannot open state formula file: " + formfilename + ".");
      }
      state_formulas::state_formula_specification formspec = state_formulas::parse_state_formula_specification(from, lpsspec);
      from.close();
      lpsspec.data() = data::merge_data_specifications(lpsspec.data(), formspec.data());
      lpsspec.action_labels() = lpsspec.action_labels() + formspec.action_labels();

      pbes_system::pbes result = pbes_system::lts2pbes(l, formspec);
      //save the result
      if (output_filename().empty())
      {
        mCRL2log(log::verbose) << "Writing PBES to stdout..." << std::endl;
      }
      else
      {
        mCRL2log(log::verbose) << "Writing PBES to file '" <<  output_filename() << "'..." << std::endl;
      }
      save_pbes(result, output_filename(), pbes_output_format());
      return true;
    }
};

int main(int argc, char** argv)
{
  return lts2pbes_tool().execute(argc, argv);
}
