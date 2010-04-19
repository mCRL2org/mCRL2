// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file txt2lps.cpp
/// \brief This tool reads a mcrl2 specification of a linear process,
/// and translates it directly into LPS format.

#include "boost.hpp" // precompiled headers

#define TOOLNAME "txt2lps"
#define AUTHOR "Wieger Wesselink"

#include <fstream>
#include <iostream>
#include <string>
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::utilities::tools;

class txt2lps_tool : public squadt_tool< input_output_tool >
{
  typedef squadt_tool< input_output_tool > super;

  public:

    txt2lps_tool() : super(
             TOOLNAME,
             AUTHOR,
             "translates an mCRL2 specification of a linear process into an LPS",
             "Translates the mCRL2 specification in INFILE and writes the resulting LPS to "
             "OUTFILE. If OUTFILE is not present, standard output is used. If INFILE is not "
             "present, standard input is used.")
    {}

    bool run() 
    {     
      lps::specification spec;

      if (input_filename().empty()) 
      {
        spec = lps::parse_linear_process_specification(std::cin);
      } 
      else
      {
        std::ifstream from(input_filename().c_str());
        if (!from)
        {
          throw mcrl2::runtime_error("Could not read from input file: " + input_filename());
        }
        spec = lps::parse_linear_process_specification(from);       
      }
      spec.save(output_filename());
      return true;
    }

//Squadt connectivity
#ifdef ENABLE_SQUADT_CONNECTIVITY
  protected:

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& capabilities) const
    {
      // The tool accepts .txt and .mcrl2 input
      capabilities.add_input_configuration("main-input",
                 tipi::mime_type("txt", tipi::mime_type::text), tipi::tool::category::transformation);
      capabilities.add_input_configuration("main-input",
                 tipi::mime_type("mcrl2", tipi::mime_type::text), tipi::tool::category::transformation);
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration& configuration)
    {
      if (!configuration.output_exists("main-output")) {
        configuration.add_output("main-output",
                 tipi::mime_type("lps", tipi::mime_type::application), configuration.get_output_name(".lps"));
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

      return run();
    }
#endif //ENABLE_SQUADT_CONNECTIVITY

};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)
  
  return txt2lps_tool().execute(argc, argv);
}
