// Author(s): Wieger Wesselink, Simon Janssen, Tim Willemse
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesparelm.cpp

#include "boost.hpp" // precompiled headers

// #define MCRL2_PBES_CONSTELM_DEBUG
// #define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/pbes/parelm.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

//[pbes_parelm_tool
class pbes_parelm_tool: public squadt_tool<input_output_tool>
{
  typedef squadt_tool<input_output_tool> super;

  public:
    pbes_parelm_tool()
      : super(
          "pbesparelm",
          "Wieger Wesselink; Simon Janssen and Tim Willemse",
          "remove unused parameters from a PBES",
          "Reads a file containing a PBES, and applies parameter elimination to it. If OUTFILE "
          "is not present, standard output is used. If INFILE is not present, standard input is used."
        )
    {}

    bool run() /*< The virtual function `run` executes the tool.
                   The user has to override this function to add behavior. >*/
    {
      if (core::gsVerbose) /*< The flag `core::gsVerbose` is a global boolean value that is set
                               to true if the user has set the verbose flag on the command line. >*/
      {
        std::clog << "pbesparelm parameters:" << std::endl;
        std::clog << "  input file:         " << m_input_filename << std::endl;
        std::clog << "  output file:        " << m_output_filename << std::endl;
      }

      // load the pbes
      pbes_system::pbes<> p;
      p.load(input_filename()); /*< The functions `input_filename()` and `output_filename()`
                                    return the corresponding values that the user has entered
                                    on the command line. >*/

      // apply the algorithm
      pbes_system::pbes_parelm_algorithm algorithm;
      algorithm.run(p);

      // save the result
      p.save(output_filename());

      return true;
    }

// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& c) const {
      c.add_input_configuration("main-input",
                 tipi::mime_type("pbes", tipi::mime_type::application),
                                         tipi::tool::category::transformation);
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration& c) {

      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;

      synchronise_with_configuration(c);

      if (!c.output_exists("main-output")) {
        c.add_output("main-output",
                 tipi::mime_type("pbes", tipi::mime_type::application),
                                                 c.get_output_name(".pbes"));
      }

      update_configuration(c);
    }

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& c) const {
      return c.input_exists("main-input") && c.output_exists("main-output");
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration& c) {
      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;

      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(c);

      // Create display
      tipi::tool_display d;

      send_display_layout(d.manager(d.create< vertical_box >().
        append(d.create< label >().set_text("Parameter elimination in progress"), layout::left)));

      // Run
      bool result = run();

      send_display_layout(d.manager(d.create<vertical_box>().
                                    append(d.create< label >().set_text(std::string("Parameter elimination ") + ((result) ? "succeeded" : "failed")), layout::left)));

      return result;
    }
#endif

};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  pbes_parelm_tool tool;
  return tool.execute(argc, argv); /*< The function `execute` first parses the command line
                                       arguments, and then calls the function `run`. >*/
}
//]
