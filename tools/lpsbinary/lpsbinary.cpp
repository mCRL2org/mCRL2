// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsbinary.cpp
/// \brief The binary tool, this runs the binary algorithm.

#include "boost.hpp" // precompiled headers

#include "mcrl2/lps/binary.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/atermpp/aterm_init.h"


using namespace std;
using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class binary_tool: public squadt_tool< rewriter_tool<input_output_tool> >
{
  protected:

    typedef squadt_tool< rewriter_tool<input_output_tool> > super;

  public:

    binary_tool()
      : super(
          "lpsbinary",
          "Jeroen Keiren",
          "replace finite sort variables by vectors of boolean variables in an LPS",
          "Replace finite sort variables by vectors of boolean variables in the LPS in "
          "INFILE and write the result to OUTFILE. If INFILE is not present, stdin is used. "
          "If OUTFILE is not present, stdout is used"
        )
    {}

    bool run()
    {
      lps::specification spec;
      spec.load(m_input_filename);
      data::rewriter r(create_rewriter(spec.data()));

      lps::binary_algorithm<data::rewriter>(spec, r).run();
      spec.save(m_output_filename);

      return true;
    }

#ifdef ENABLE_SQUADT_CONNECTIVITY
    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& c) const
    {
      c.add_input_configuration("main-input",
          tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration& c)
    {
      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;

      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(c);

      /* Create display */
      tipi::tool_display d;

      layout::vertical_box& m = d.create< vertical_box >();

      add_rewrite_option(d, m);

      button& okay_button = d.create< button >().set_label("OK");

      m.append(d.create< label >().set_text(" ")).
        append(okay_button, layout::right);

      send_display_layout(d.manager(m));

      okay_button.await_change();

      if (!c.output_exists("main-output")) { 
        c.add_output("main-output", 
               tipi::mime_type("lps", tipi::mime_type::application), c.get_output_name(".lps")); 
      } 

      // let squadt_tool update configuration for rewriter and input/output files
      update_configuration(c);
    }

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& c) const
    {
      return c.input_exists("main-input") ||
             c.input_exists("main-output") ||
             c.option_exists("rewrite-strategy");
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration& c)
    {
      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;

      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(c);

      /* Create display */
      tipi::tool_display d;

      send_display_layout(d.manager(d.create< vertical_box >().
                    append(d.create< label >().set_text("Binary in progress"), layout::left)));

      //Perform binary
      bool result = run();

      send_display_layout(d.manager(d.create< vertical_box >().
                    append(d.create< label >().set_text(std::string("Binary ") + ((result) ? "succeeded" : "failed")), layout::left)));

      return result;
    }
#endif //ENABLE_SQUADT_CONNECTIVITY
};

class binary_gui_tool: public mcrl2_gui_tool<binary_tool>
{
  public:
	binary_gui_tool()
    {
      std::vector<std::string> values;

      values.clear();
      values.push_back("jitty");
      values.push_back("jittyp");
      values.push_back("jittyc");
      values.push_back("inner");
      values.push_back("innerp");
      values.push_back("innerc");
      m_gui_options["rewriter"] = create_radiobox_widget(values);
    }
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return binary_gui_tool().execute(argc, argv);
}
