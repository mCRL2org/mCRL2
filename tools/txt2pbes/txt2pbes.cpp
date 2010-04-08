// Author(s): Aad Mathijssen, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./txt2pbes.cpp
/// \brief Parse a textual description of a PBES.

#include "boost.hpp" // precompiled headers

#define NAME "txt2pbes"
#define AUTHOR "Aad Mathijssen"

//C++
#include <cstdio>
#include <fstream>
#include <string>

//mCRL2 specific
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/text_utility.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

using mcrl2::core::gsVerboseMsg;

class txt2pbes_tool: public squadt_tool<input_output_tool>
{
  typedef squadt_tool<input_output_tool> super;

  public:
    txt2pbes_tool()
      : super(NAME, AUTHOR,
          "parse a textual description of a PBES",
          "Parse the textual description of a PBES from INFILE and write it to OUTFILE. "
          "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used.\n\n"
          "The textual description should adhere to the BNF syntax description at "
          "<http://www.mcrl2.org/mcrl2/wiki/index.php/Language_reference/PBES_syntax>."
        )
    {}

    bool run()
    {
      pbes_system::pbes<> p;
      if (input_filename().empty()) 
      { //parse specification from stdin
        gsVerboseMsg("reading input from stdin...\n");
        p = pbes_system::txt2pbes(std::cin);
      } 
      else {
        //parse specification from input filename
        gsVerboseMsg("reading input from file '%s'...\n", input_filename().c_str());
        std::ifstream instream(input_filename().c_str(), std::ifstream::in|std::ifstream::binary);
        if (!instream.is_open()) {
          throw mcrl2::runtime_error("cannot open input file: " + input_filename());
        }
        p = pbes_system::txt2pbes(instream);
        instream.close();
      }
      if (output_filename().empty()) {
        gsVerboseMsg("writing PBES to stdout...\n");
      } else {
        gsVerboseMsg("writing PBES to file '%s'...\n", output_filename().c_str());
      }
      p.save(output_filename());
      return true;
    }

//Squadt connectivity
#ifdef ENABLE_SQUADT_CONNECTIVITY
  protected:

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& capabilities) const
    {
      // The tool has only one main input combination
      capabilities.add_input_configuration("main-input",
                 tipi::mime_type("txt", tipi::mime_type::application), tipi::tool::category::transformation);
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration& configuration)
    {
      if (!configuration.output_exists("main-output")) {
        configuration.add_output("main-output",
                 tipi::mime_type("pbes", tipi::mime_type::application), configuration.get_output_name(".pbes"));
      }
    }

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& configuration) const
    {
      // Check if everything present
      return (configuration.input_exists("main-input") &&
              configuration.output_exists("main-output"));
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration& configuration)
    {
      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(configuration);

      return run() == 0;
    }
#endif //ENABLE_SQUADT_CONNECTIVITY

};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return txt2pbes_tool().execute(argc, argv);
}
