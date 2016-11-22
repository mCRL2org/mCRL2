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
#include "mcrl2/data/parse.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lts/detail/lts_convert.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/modal_formula/algorithms.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/lts2pbes.h"
#include "mcrl2/process/parse.h"
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
    typedef enum { none_e, data_e, lps_e, mcrl2_e } data_file_type_t;

    std::string infilename;
    std::string outfilename;
    std::string formfilename;
    lts::lts_type input_type;
    data_file_type_t data_file_type;
    std::string data_file;
    data::data_specification data;
    process::action_label_list action_labels;
    data::variable_list process_parameters;
    bool extra_data_is_defined;

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
        data_file_type = data_e;
        data_file = parser.option_argument("data");
      }

      if (parser.options.count("lps"))
      {
        if (1 < parser.options.count("lps") || data_file_type != none_e)
        {
          mCRL2log(log::warning) << "multiple data specification files are specified; can only use one.\n";
        }

        data_file_type = lps_e;
        data_file = parser.option_argument("lps");
      }

      if (parser.options.count("mcrl2"))
      {
        if (1 < parser.options.count("mcrl2") || data_file_type != none_e)
        {
          mCRL2log(log::warning) << "multiple data specification files are specified; can only use one.\n";
        }

        data_file_type = mcrl2_e;
        data_file = parser.option_argument("mcrl2");
      }

      if (parser.options.count("in"))
      {
        if (1 < parser.options.count("in"))
        {
          throw parser.error("multiple input formats specified; can only use one");
        }

        input_type = lts::detail::parse_format(parser.option_argument("in"));
        if (input_type == lts::lts_none || input_type == lts::lts_dot)
        {
          throw parser.error("option -i/--in has illegal argument '" +
                       parser.option_argument("in") + "'");
        }
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
      input_type(lts::lts_none), data_file_type(none_e)
    {}

  protected:

    void read_lps_context()
    {
      lps::specification spec;
      load_lps(spec, data_file);
      data = spec.data();
      action_labels = spec.action_labels();
      process_parameters = spec.process().process_parameters();
      extra_data_is_defined = true;
    }

    void read_data_context()
    {
      data = data::parse_data_specification(utilities::read_text(data_file));
      extra_data_is_defined = true;
    }

    void read_mcrl2_context()
    {
      process::process_specification procspec = process::parse_process_specification(utilities::read_text(data_file), false);
      data = procspec.data();
      action_labels = procspec.action_labels();
      extra_data_is_defined = true;
    }

    // converts an arbitrary lts to lts_lts_t
    template <class LTS_TYPE>
    void convert_to_lts_lts(LTS_TYPE& src, lts::lts_lts_t& dest)
    {
      switch (data_file_type)
      {
        case data_e:  read_data_context(); break;
        case lps_e:   read_lps_context(); break;
        case mcrl2_e: read_mcrl2_context(); break;
        default:
        {
          extra_data_is_defined = false;
          mCRL2log(log::info) << "No data and action label specification is provided. Only the standard data types and no action labels can be used." << std::endl; break;
        }
      }
      lts::detail::lts_convert(src, dest, data, action_labels, process_parameters, extra_data_is_defined);
    }

    // loads an lts of type lts_lts_t
    void load_lts(lts::lts_lts_t& result, lts::lts_type type)
    {
      switch (type)
      {
        case lts::lts_lts:
        {
          if (data_file_type != none_e)
          {
            mCRL2log(log::warning) << "The lts file comes with a data specification. Ignoring the extra data and action label specification provided." << std::endl;
          }
          result.load(infilename);
          break;
        }
        case lts::lts_none:
          mCRL2log(log::warning) << "Cannot determine type of input. Assuming .aut.\n";
        case lts::lts_aut:
        {
          lts::lts_aut_t l;
          l.load(infilename);
          convert_to_lts_lts(l, result);
          break;
        }
        case lts::lts_fsm:
        {
          lts::lts_fsm_t l;
          l.load(infilename);
          convert_to_lts_lts(l, result);
          break;
        }
        case lts::lts_dot:
        {
          throw mcrl2::runtime_error("Reading of .dot files is not supported anymore.");
        }
      }
    }

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
      if (input_type == lts::lts_none)
      {
        input_type = lts::detail::guess_format(infilename);
      }

      lts::lts_lts_t l;
      load_lts(l, input_type);

      //load formula file
      lps::specification spec = extract_specification(l);
      std::ifstream from(formfilename.c_str());
      if (!from)
      {
        throw mcrl2::runtime_error("cannot open state formula file: " + formfilename);
      }
      state_formulas::state_formula formula = state_formulas::algorithms::parse_state_formula(from, spec);
      from.close();

      pbes_system::pbes result = pbes_system::lts2pbes(l, formula);
      //save the result
      if (output_filename().empty())
      {
        mCRL2log(log::verbose) << "writing PBES to stdout..." << std::endl;
      }
      else
      {
        mCRL2log(log::verbose) << "writing PBES to file '" <<  output_filename() << "'..." << std::endl;
      }
      save_pbes(result, output_filename(), pbes_output_format());
      return true;
    }
};

int main(int argc, char** argv)
{
  return lts2pbes_tool().execute(argc, argv);
}
