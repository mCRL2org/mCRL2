// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsrewr.cpp
/// \brief Tool for rewriting a linear process specification.

#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/rewrite.h"
#include "mcrl2/lps/remove.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using mcrl2::utilities::tools::input_output_tool;
using mcrl2::utilities::tools::rewriter_tool;
using mcrl2::utilities::tools::squadt_tool;

class lps_rewriter_tool : public squadt_tool< rewriter_tool< input_output_tool > >
{
  protected:
    typedef squadt_tool< rewriter_tool< input_output_tool > > super;

    bool          m_benchmark;
    unsigned long m_bench_times;

    void add_options(utilities::interface_description& desc)
    {
      super::add_options(desc);
      desc.add_hidden_option("benchmark", utilities::make_mandatory_argument("NUM"),
              "rewrite data expressions NUM times; do not save output", 'b');
    }

    /// Parse the non-default options.
    void parse_options(const utilities::command_line_parser& parser)
    {
      super::parse_options(parser);
      m_benchmark = (parser.options.count("benchmark")>0);
      if (m_benchmark)
      {
        m_bench_times = parser.option_argument_as< unsigned long >("benchmark");
      }
    }

    template <typename DataRewriter>
    void run_bench_mark(const lps::specification& spec, const DataRewriter& R)
    {
      std::clog << "rewriting LPS " << m_bench_times << " times...\n";
      for (unsigned long i=0; i < m_bench_times; i++)
      {
        lps::specification spec1 = spec;
        lps::rewrite(spec1, R);
      }
    }

  public:
    lps_rewriter_tool()
      : super(
          "lpsrewr",
          "Wieger Wesselink and Muck van Weerdenburg",
          "rewrite data expressions in an LPS",
          "Rewrite data expressions of the LPS in INFILE and save the result to OUTFILE."
          "If OUTFILE is not present, standard output is used. If INFILE is not present,"
          "standard input is used"
        ),
        m_benchmark(false),
        m_bench_times(1)
    {}

    bool run()
    {
      lps::specification spec;
      spec.load(input_filename());
      data::rewriter R = create_rewriter(spec.data());
      if (m_benchmark)
      {
        run_bench_mark(spec, R);
      }
      lps::rewrite(spec, R);     
      lps::remove_trivial_summands(spec);
      spec.save(output_filename());
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

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return lps_rewriter_tool().execute(argc, argv);
}

