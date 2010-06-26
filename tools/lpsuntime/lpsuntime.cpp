// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsuntime.cpp

#include "boost.hpp" // precompiled headers

#include "mcrl2/lps/untime.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::core;

class untime_tool: public squadt_tool< input_output_tool >
{
  protected:

    typedef squadt_tool< input_output_tool > super;

  public:

    untime_tool()
      : super(
          "lpsuntime",
          "Jeroen Keiren",
          "remove time from an LPS",
          "Remove time from the linear process specification (LPS) in INFILE and write the "
          "result to OUTFILE. If INFILE is not present, stdin is used. If OUTFILE is not "
          "present, stdout is used.")
    {}

    bool run()
    {
      lps::specification spec;
      spec.load(m_input_filename);

      lps::untime_algorithm untime(spec, core::gsVerbose);
      untime.run();

      spec.save(m_output_filename);

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

class untime_gui_tool: public mcrl2_gui_tool<untime_tool> {
public:
	untime_gui_tool() {
	}
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return untime_gui_tool().execute(argc, argv);
}
