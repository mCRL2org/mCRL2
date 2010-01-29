// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpssumelm.cpp

#include "boost.hpp" // precompiled headers

//LPS Framework
#include "mcrl2/lps/sumelm.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::core;

class sumelm_tool: public squadt_tool< input_output_tool >
{
  protected:

    typedef squadt_tool< input_output_tool > super;

  public:

    sumelm_tool()
      : super(
          "lpssumelm",
          "Jeroen Keiren",
          "remove superfluous summations from an LPS",
          "Remove superfluous summations from the linear process specification (LPS) in "
          "INFILE and write the result to OUTFILE. If INFILE is not present, stdin is used. "
          "If OUTFILE is not present, stdout is used.")
    {}

    ///Reads a specification from input_file,
    ///applies sum elimination to it and writes the result to output_file.
    bool run()
    {
      lps::specification lps_specification;

      lps_specification.load(m_input_filename);

      // apply sum elimination to lps_specification and save the output to a binary file
      lps::sumelm_algorithm(lps_specification, core::gsVerbose||core::gsDebug).run();

      gsDebugMsg("Sum elimination completed, saving to %s\n", m_output_filename.c_str());
      lps_specification.save(m_output_filename);

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
          tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration& configuration)
    {
      if (configuration.fresh()) {
        if (!configuration.output_exists("main-output")) {
          configuration.add_output("main-output", tipi::mime_type("lps", tipi::mime_type::application), configuration.get_output_name(".lps"));
        }
      }
    }

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& configuration) const
    {
      // Check if everything present
      return configuration.input_exists("main-input") &&
             configuration.output_exists("main-output");
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

  return sumelm_tool().execute(argc, argv);
}
