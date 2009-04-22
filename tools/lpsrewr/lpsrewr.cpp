// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsrewr.cpp
/// \brief A tool to rewrite linear process specifications

#include "boost.hpp" // precompiled headers

#define TOOLNAME "lpsrewr"
#define AUTHOR "Aad Mathijssen"

#include "mcrl2/lps/lps_rewrite.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/squadt_tool.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;

using mcrl2::utilities::tools::input_output_tool;
using mcrl2::utilities::tools::rewriter_tool;
using mcrl2::utilities::tools::squadt_tool;

class lps_rewriter_tool : public squadt_tool< rewriter_tool< input_output_tool > >
{
  protected:
    typedef squadt_tool< rewriter_tool< input_output_tool > > super;

    bool                      m_benchmark;
    unsigned long             m_bench_times;

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("benchmark", make_mandatory_argument("NUM"),
              "rewrite data expressions NUM times; do not save output", 'b');
    }

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      m_benchmark = (parser.options.count("benchmark")>0);

      if (m_benchmark)
      { m_bench_times = parser.option_argument_as< unsigned long >("benchmark");
      }
    }

  public:

    lps_rewriter_tool()
      : super(
          TOOLNAME,
          AUTHOR,
          "rewrite data expressions in an LPS",
          "Rewrite data expressions of the LPS in INFILE and save the result to OUTFILE."
          "If OUTFILE is not present, stdout is used. If INFILE is not present, stdin is"
          "used."
        ),
        m_benchmark(false),
        m_bench_times(1)
    {}

    bool run()
    {
      if (gsVerbose)
      {
        std::clog << "lpsrewr parameters:" << std::endl;
        std::clog << "  input file:         " << m_input_filename << std::endl;
        std::clog << "  output file:        " << m_output_filename << std::endl;
        std::clog << "  benchmark:          " << (m_benchmark?"YES":"NO") << std::endl;
        std::clog << "  number of times:    " << m_bench_times << std::endl;
      }

      lps::specification specification;

      specification.load(m_input_filename);

      mcrl2::data::rewriter rewriter = create_rewriter(specification.data());

      if (m_benchmark)
      {
        std::clog << "rewriting LPS " << m_bench_times << " times...\n";
      }
      for (unsigned long i=0; i < m_bench_times; i++)
      {
        specification = rewrite_lps(specification, rewriter);
      }
      specification.save(m_output_filename);

      return true;
    }

#ifdef ENABLE_SQUADT_CONNECTIVITY
    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& c) const {
      c.add_input_configuration("main-input", tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration& c) {
      /* Add output file to the configuration */
      if (!c.output_exists("main-output")) {
        c.add_output("main-output", tipi::mime_type("lps", tipi::mime_type::application), c.get_output_name(".lps"));
      }
    }

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const&) const {
      return true;
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration& c) {
      synchronise_with_configuration(c);

      bool result = run();

      if (result) {
        send_clear_display();
      }

      return (result);
    }
#endif //ENABLE_SQUADT_CONNECTIVITY
};

//Main program
//------------

int main(int argc, char **argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return lps_rewriter_tool().execute(argc, argv);
}

