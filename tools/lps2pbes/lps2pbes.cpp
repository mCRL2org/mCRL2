// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2pbes.cpp
/// \brief Add your file description here.

#include "boost.hpp" // precompiled headers

#include "mcrl2/core/text_utility.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/modal_formula/mucalculus.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

using mcrl2::core::gsVerboseMsg;

class lps2pbes_tool : public input_output_tool
{
  typedef input_output_tool super;

  protected:
    bool timed;
    std::string formfilename;

    std::string synopsis() const
    {
      return "[OPTION]... --formula=FILE [INFILE [OUTFILE]]\n";
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("formula", make_mandatory_argument("FILE"),
          "use the state formula from FILE", 'f');
      desc.add_option("timed",
          "use the timed version of the algorithm, even for untimed LPS's", 't');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      timed     = 0 < parser.options.count("timed");

      //check for presence of -f
      if (parser.options.count("formula")) {
        formfilename = parser.option_argument("formula");
      }
    }

  public:
    lps2pbes_tool() : super(
      "lps2pbes",
      "Wieger Wesselink; Tim Willemse",
      "generate a PBES from an LPS and a state formula",
      "Convert the state formula in FILE and the LPS in INFILE to a parameterised "
      "boolean equation system (PBES) and save it to OUTFILE. If OUTFILE is not "
      "present, stdout is used. If INFILE is not present, stdin is used.\n"
      "\n"
      "The concrete syntax of state formulas can be found at <http://www.mcrl2.org/mcrl2/wiki/index.php/Language_reference/mu-calculus_syntax>."
      )
    {}

    bool run()
    {
      if (formfilename.empty())
      {
        throw mcrl2::runtime_error("option -f is not specified");
      }
     
      //load LPS
      if (input_filename().empty()) {
        gsVerboseMsg("reading LPS from stdin...\n");
      } else {
        gsVerboseMsg("reading LPS from file '%s'...\n", input_filename().c_str());
      }
      lps::specification spec;
      spec.load(input_filename());
      //load formula file
      gsVerboseMsg("reading input from file '%s'...\n", formfilename.c_str());
      std::ifstream instream(formfilename.c_str(), std::ifstream::in|std::ifstream::binary);
      if (!instream.is_open()) {
        throw mcrl2::runtime_error("cannot open state formula file: " + formfilename);
      }
      state_formulas::state_formula formula = state_formulas::parse_state_formula(instream, spec);
      instream.close();
      //convert formula and LPS to a PBES
      gsVerboseMsg("converting state formula and LPS to a PBES...\n");
      pbes_system::pbes<> result = pbes_system::lps2pbes(spec, formula, timed);
      //save the result
      if (output_filename().empty()) {
        gsVerboseMsg("writing PBES to stdout...\n");
      } else {
        gsVerboseMsg("writing PBES to file '%s'...\n", output_filename().c_str());
      }
      result.save(output_filename());
      return true;
    }

};

class lps2pbes_gui_tool: public mcrl2_gui_tool<lps2pbes_tool>
{
  public:
	lps2pbes_gui_tool()
    {
      m_gui_options["timed"] = create_checkbox_widget();
      m_gui_options["formula"] = create_filepicker_widget("modal mu-calculus files (*.mcf)|*.mcf|Text files(*.txt)|*.txt|All Files (*.*)|*.*");
    }
};


int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return lps2pbes_gui_tool().execute(argc, argv);
}
