// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsrewr.cpp
/// \brief Tool for rewriting a linear process specification.

#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/squadt_tool.h"

using namespace mcrl2;
using mcrl2::utilities::tools::input_output_tool;
using mcrl2::utilities::tools::rewriter_tool;
using mcrl2::utilities::tools::squadt_tool;

class lpsrewr_algorithm: public lps::detail::lps_rewriter_algorithm
{
  public:
    lpsrewr_algorithm(lps::specification& spec, data::rewriter::strategy s)
      : lps_rewriter_algorithm(spec, s)
    {}
    
    void run()
    {
      rewrite();
    }
};

class lps_rewriter_tool : public squadt_tool< rewriter_tool< input_output_tool > >
{
  protected:
    typedef squadt_tool< rewriter_tool< input_output_tool > > super;

  public:
    lps_rewriter_tool()
      : super(
          "lpsrewr",
          "Wieger Wesselink",
          "rewrite data expressions in an LPS",
          "Rewrite data expressions of the LPS in INFILE and save the result to OUTFILE."
          "If OUTFILE is not present, standard output is used. If INFILE is not present,"
          "standard input is used"
        )
    {}

    bool run()
    {
      lps::specification spec;
      spec.load(input_filename());
      lpsrewr_algorithm algorithm(spec, rewrite_strategy());
      algorithm.run();
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

