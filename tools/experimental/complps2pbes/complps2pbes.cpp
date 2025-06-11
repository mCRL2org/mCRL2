// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file complps2pbes.cpp
/// \brief Add your file description here.

#include "mcrl2/lps/linearise.h"
#include "mcrl2/modal_formula/algorithms.h"
#include "mcrl2/pbes/complps2pbes.h"
#include "mcrl2/pbes/pbes_output_tool.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/process/process_specification.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::log;

class complps2pbes_tool : public mcrl2::pbes_system::tools::pbes_output_tool<input_output_tool>
{
    typedef pbes_output_tool<input_output_tool> super;

  protected:
    std::string formfilename;

    std::string synopsis() const
    {
      return "[OPTION]... --formula=FILE [INFILE [OUTFILE]]\n";
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("formula", make_file_argument("FILE"),
                      "use the state formula from FILE", 'f');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      //check for presence of -f
      if (parser.options.count("formula"))
      {
        formfilename = parser.option_argument("formula");
      }
    }

  public:
    complps2pbes_tool() : super(
        "complps2pbes",
        "Wieger Wesselink",
        "generate a PBES from an LPS and a state formula",
        "Convert the state formula in FILE and the LPS in INFILE to a parameterised "
        "boolean equation system (PBES) and save it to OUTFILE. If OUTFILE is not "
        "present, stdout is used. If INFILE is not present, stdin is used."
      )
    {}

    bool run()
    {  
      if (formfilename.empty())
      {
        throw mcrl2::runtime_error("option -f is not specified");
      }

      // load mCRL2 specification
      std::string text;
      if (input_filename().empty())
      {
        mCRL2log(log::verbose) << "reading mCRL2 specification from stdin..." << std::endl;
        text = utilities::read_text(std::cin);
      }
      else
      {
        mCRL2log(log::verbose) << "reading mCRL2 specification from file '" <<  input_filename() << "'..." << std::endl;
        std::ifstream from(input_filename().c_str());
        text = utilities::read_text(from);
      }
      // TODO: check if alpha reduction should be applied
      process::process_specification procspec = process::parse_process_specification(text);
      lps::stochastic_specification spec = lps::linearise(procspec);
      lps::specification temp_spec = remove_stochastic_operators(lps::linearise(procspec)); // Just to check that there are no stochastic operators. 

      // load state formula
      mCRL2log(log::verbose) << "reading formula from file '" <<  formfilename << "'..." << std::endl;
      std::ifstream instream(formfilename.c_str(), std::ifstream::in|std::ifstream::binary);
      if (!instream)
      {
        throw mcrl2::runtime_error("cannot open state formula file: " + formfilename);
      }
      const bool formula_is_quantitative = false;
      state_formulas::state_formula formula = state_formulas::algorithms::parse_state_formula(instream, spec, formula_is_quantitative);
      instream.close();

      pbes result = complps2pbes(procspec, formula);

      // save the result
      if (output_filename().empty())
      {
        mCRL2log(log::verbose) << "writing PBES to stdout..." << std::endl;
      }
      else
      {
        mCRL2log(log::verbose) << "writing PBES to file '" <<  output_filename() << "'..." << std::endl;
      }
      
      save_pbes(result, output_filename(), m_pbes_output_format);
      return true;
    }

};

int main(int argc, char** argv)
{
  return complps2pbes_tool().execute(argc, argv);
}
