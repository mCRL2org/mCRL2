// Author(s): Jan Friso Groote, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsrealelm.cpp

#define TOOLNAME "lpsrealelm"
#define AUTHORS "Jan Friso Groote and Jeroen Keiren"

#define DEFAULT_MAX_ITERATIONS 10

//C++
#include <exception>
#include <cstdio>

//Aterms
#include <mcrl2/atermpp/aterm.h>
#include <mcrl2/atermpp/aterm_list.h>
#include <mcrl2/atermpp/table.h>
#include <mcrl2/atermpp/algorithm.h>
#include "mcrl2/utilities/squadt_tool.h"

//Tool framework
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"

#include "realelm.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using utilities::tools::squadt_tool;
using utilities::tools::rewriter_tool;
using utilities::tools::input_output_tool;

//Squadt connectivity

class lpsrealelm_tool: public squadt_tool < rewriter_tool<input_output_tool> >
{
  protected:
    typedef squadt_tool< rewriter_tool<input_output_tool> > super;

    unsigned int max_iterations;

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      if (parser.options.count("max")>0)
      { max_iterations = parser.option_argument_as< unsigned int > ("max");
      } 
    }
  
    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.
        add_option("max", 
                   make_mandatory_argument("NUM"),
                   "perform at most NUM iterations");
    }

  public:
    lpsrealelm_tool()
      : super(
          TOOLNAME,
          AUTHORS,
          "remove real numbers from an LPS",
          "Remove Real numbers from the linear process specification (LPS) in "
          "INFILE and write the result to OUTFILE. If INFILE is not present, stdin is used. "),
        max_iterations(DEFAULT_MAX_ITERATIONS)
    {}

    /// Runs the algorithm.
    /// Reads a specification from input_file,
    ///i applies real time abstraction to it and writes the result to output_file.
    bool run()
    {
      using namespace mcrl2::pbes_system;

      if (core::gsVerbose)
      {
        std::cerr << "Parameters of lpsrealelm:" << std::endl;
        std::cerr << "  input file:         " << m_input_filename << std::endl;
        std::cerr << "  output file:        " << m_output_filename << std::endl;
        std::cerr << "  data rewriter       " << m_rewrite_strategy << std::endl;
        std::cerr << "  max_iterations:     " << max_iterations << std::endl;
      }

      specification lps_specification;
      lps_specification.load(m_input_filename);

      // Untime lps_specification and save the output to a binary file
      rewriter r=create_rewriter(lps_specification.data());
      specification new_spec = realelm(lps_specification, max_iterations, r);

      if (core::gsVerbose)
      { std::cerr << "Real time abstraction completed, saving to " << m_output_filename << "\n";
      }
      new_spec.save(m_output_filename);

      return true;
    }

#ifdef ENABLE_SQUADT_CONNECTIVITY
  
    void set_capabilities(tipi::tool::capabilities& capabilities) const
    {
      // The tool has only one main input combination
      capabilities.add_input_configuration("main-input", tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
    }
    
    void user_interactive_configuration(tipi::configuration& configuration)
    {
      if (configuration.fresh()) {
        if (!configuration.output_exists("main-output")) {
          configuration.add_output("main-output", tipi::mime_type("lps", tipi::mime_type::application), configuration.get_output_name(".lps"));
        }
      }
    }
    
    bool check_configuration(tipi::configuration const& configuration) const
    {
      return (configuration.input_exists("main-input") &&
              configuration.output_exists("main-output")
             );
    }
    
    bool perform_task(tipi::configuration& configuration)
    {
      synchronise_with_configuration(configuration);
      max_iterations = DEFAULT_MAX_ITERATIONS;
       
      const bool result = run();
      send_clear_display();
      return result;
    }

#endif //ENABLE_SQUADT_CONNECTIVITY
};

int main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)
  return lpsrealelm_tool().execute(argc, argv);
}
