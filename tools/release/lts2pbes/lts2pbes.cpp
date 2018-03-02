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
#include "mcrl2/bes/pbes_output_tool.h"
#include "mcrl2/lts/detail/lts_load.h"
#include "mcrl2/modal_formula/algorithms.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pbes/lts2pbes.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using bes::tools::pbes_output_tool;
using utilities::tools::input_output_tool;

class lts2pbes_tool : public pbes_output_tool<input_output_tool>
{
  private:
    typedef pbes_output_tool<input_output_tool> super;

  protected:

    std::string formfilename;
    bool preprocess_modal_operators;
    bool generate_counter_example;
    lts::lts_lts_t l;

    void add_options(interface_description& desc)
    {
      super::add_options(desc);

      desc.add_option("formula", make_file_argument("FILE"),
                      "use the state formula from FILE", 'f');

      desc.add_option("preprocess-modal-operators",
                      "insert dummy fixpoints in modal operators, which may lead to smaller PBESs", 'p');

      desc.add_hidden_option("counter-example",
                             "add counter example equations to the generated PBES", 'c');
      lts::detail::add_options(desc);
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      if (parser.options.count("formula"))
      {
        formfilename = parser.option_argument("formula");
      }

      preprocess_modal_operators = parser.options.count("preprocess-modal-operators") > 0;
      generate_counter_example = parser.options.count("counter-example") > 0;
      lts::detail::load_lts(parser, input_filename(), l);
    }

  public:

    lts2pbes_tool() : super(
        TOOLNAME,
        AUTHOR,
        "translates an LTS into an PBES",
        "Translates an LTS in INFILE and writes the resulting PBES to "
        "OUTFILE. If OUTFILE is not present, standard output is used. If INFILE is not "
        "present, standard input is used.")
    {}

  public:
    bool run()
    {
      lps::specification lpsspec = lts::detail::extract_specification(l);
      std::ifstream from(formfilename.c_str());
      if (!from)
      {
        throw mcrl2::runtime_error("Cannot open state formula file: " + formfilename + ".");
      }
      state_formulas::state_formula_specification formspec = state_formulas::parse_state_formula_specification(from, lpsspec);
      from.close();
      lpsspec.data() = data::merge_data_specifications(lpsspec.data(), formspec.data());
      lpsspec.action_labels() = lpsspec.action_labels() + formspec.action_labels();
      pbes_system::pbes result = pbes_system::lts2pbes(l, formspec, preprocess_modal_operators, generate_counter_example);

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
