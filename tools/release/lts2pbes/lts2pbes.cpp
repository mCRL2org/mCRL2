// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts2pbes.cpp
/// \brief This tool reads an LTS and translates it into a PBES.

constexpr const char* TOOLNAME = "lts2pbes";
constexpr const char* AUTHOR = "Wieger Wesselink";

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/lts/detail/lts_load.h"
#include "mcrl2/lts/lts_utilities.h"
#include "mcrl2/modal_formula/check_formula_actions.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pbes/lts2pbes.h"
#include "mcrl2/pbes/pbes_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using pbes_system::tools::pbes_output_tool;
using utilities::tools::input_output_tool;

class lts2pbes_tool : public pbes_output_tool<input_output_tool>
{
  private:
    using super = pbes_output_tool<input_output_tool>;

  protected:

    std::string formfilename;
    bool preprocess_modal_operators = false;
    bool generate_counter_example = false;
    lts::lts_lts_t ltsspec;

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);

      desc.add_option("formula", make_file_argument("FILE"),
                      "use the state formula from FILE", 'f');

      desc.add_option("preprocess-modal-operators",
                      "insert dummy fixpoints in modal operators, which may lead to smaller PBESs", 'p');

      desc.add_option("counter-example",
                      "add counter example equations to the generated PBES", 'c');
      lts::detail::add_options(desc);
    }

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);

      if (parser.options.count("formula"))
      {
        formfilename = parser.option_argument("formula");
      }
      else 
      {
        throw mcrl2::runtime_error("The option --formula=FILE must be provided to pass the modal formula that is used in the translation.");      
      }

      preprocess_modal_operators = parser.options.count("preprocess-modal-operators") > 0;
      generate_counter_example = parser.options.count("counter-example") > 0;
      lts::detail::load_lts(parser, input_filename(), ltsspec);
      lts::check_lts(ltsspec);
    }

  public:

    lts2pbes_tool() : super(
        TOOLNAME,
        AUTHOR,
        "translates an LTS into an PBES",
        "Translates an LTS in INFILE and the given modal formula FILE,"
        "and writes the resulting PBES to OUTFILE. If OUTFILE is not present, standard"
        "output is used. If INFILE is not present, standard input is used.")
    {}

  public:
    bool run() override
    {
      lps::stochastic_specification lpsspec = lts::detail::extract_specification(ltsspec);
      std::ifstream from(formfilename.c_str());
      if (!from)
      {
        throw mcrl2::runtime_error("Cannot open state formula file: " + formfilename + ".");
      }
      const bool formula_is_quantitative = false;
      state_formulas::state_formula_specification formspec = state_formulas::parse_state_formula_specification(from, lpsspec, formula_is_quantitative);
      state_formulas::check_formula_actions(formspec.formula(), lts::find_action_labels(ltsspec), "LTS");
      from.close();
      ltsspec.set_data(data::merge_data_specifications(ltsspec.data(), formspec.data()));
      if (!formspec.action_labels().empty())
      {
        mCRL2log(log::log_level_t::warning) << "The modal formula contains action declarations. These are ignored.\n";
      }
      lpsspec.action_labels() = lpsspec.action_labels() + formspec.action_labels();
      pbes_system::pbes result = pbes_system::lts2pbes(ltsspec, formspec, preprocess_modal_operators, generate_counter_example);

      //save the result
      if (output_filename().empty())
      {
        mCRL2log(log::log_level_t::verbose) << "Writing PBES to stdout..." << std::endl;
      }
      else
      {
        mCRL2log(log::log_level_t::verbose) << "Writing PBES to file '" <<  output_filename() << "'..." << std::endl;
      }
      save_pbes(result, output_filename(), pbes_output_format());
      return true;
    }
};

int main(int argc, char** argv)
{
  return lts2pbes_tool().execute(argc, argv);
}
