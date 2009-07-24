// Author(s): Tom Haenen, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsactionrename.cpp
/// \brief The tool lpsactionrename renames actions in an LPS

#include "boost.hpp" // precompiled headers

#define TOOLNAME "lpsactionrename"
#define AUTHOR "Jan Friso Groote and Tom Haenen"

#include <fstream>
#include <iostream>
#include <algorithm>
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/rewrite.h"
#include "mcrl2/lps/sumelm.h"
#include "mcrl2/lps/action_rename.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::lps;

using mcrl2::core::gsVerboseMsg;
using mcrl2::utilities::tools::input_output_tool;
using mcrl2::utilities::tools::rewriter_tool;
using mcrl2::utilities::tools::squadt_tool;

class action_rename_tool: public squadt_tool< rewriter_tool<input_output_tool> >
{
  //Type definitions
  //----------------
  typedef squadt_tool< rewriter_tool<input_output_tool> > super;

  protected:
    //t_phase represents the phases at which the program should be able to stop
   typedef enum { PH_NONE, PH_PARSE, PH_TYPE_CHECK, PH_DATA_IMPL} t_phase;

    //t_tool_options represents the options of the tool

    bool            m_pretty;
    bool            m_rewrite;
    bool            m_sumelm;
    t_phase         m_end_phase;
    std::string     m_action_rename_filename;

    std::string synopsis() const
    {
      return "[OPTION]... --renamefile=NAME [INFILE [OUTFILE]]\n";
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("renamefile", make_mandatory_argument("NAME"),
              "use the rename rules from NAME", 'f');
      desc.add_option("no-rewrite",
              "do not rewrite data expressions while renaming; use when the rewrite system "
              "does not terminate", 'o');
      desc.add_option("no-sumelm",
                "do not apply sum elimination to the final result", 'm');
      desc.add_option("end-phase", make_mandatory_argument("PHASE"),
                "stop conversion and output the action rename specification after phase PHASE: "
                "'pa' (parsing), "
                "'tc' (type checking), or "
                , 'p');
      desc.add_option("pretty",
                "return a pretty printed version of the output", 'P');

    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      m_rewrite = (parser.options.count("rewrite")==0);
      m_sumelm  = (parser.options.count("sumelm")==0);
      m_pretty = (parser.options.count("pretty"));

      if (parser.options.count("end-phase")>0)
      {
        std::string phase = parser.option_argument("end-phase");

        if (std::strncmp(phase.c_str(), "pa", 3) == 0)
        { m_end_phase = PH_PARSE;
        } else if (std::strncmp(phase.c_str(), "tc", 3) == 0)
        { m_end_phase = PH_TYPE_CHECK;
        } else
        { parser.error("option -p has illegal argument '" + phase + "'");
        }
      }

      m_action_rename_filename = parser.option_argument("renamefile");
    }

  public:
    action_rename_tool()
      : super(
          TOOLNAME,
          AUTHOR,
          "rename actions of an LPS",
          "Apply the action rename specification in FILE to the LPS in INFILE and save it to OUTFILE. "
          "If OUTFILE is not present, stdout is used. If INFILE is not present, stdin is used."
        ),
        m_pretty(false),
        m_rewrite(true),
        m_sumelm(true),
        m_end_phase(PH_NONE)
    {}

    bool run()
    {
      //load LPS
      if (input_filename().empty()) {
        gsVerboseMsg("reading LPS from stdin...\n");
      } else {
        gsVerboseMsg("reading LPS from file '%s'...\n", input_filename().c_str());
      }
      specification lps_old_spec;
      lps_old_spec.load(input_filename());

      //load action rename file
      gsVerboseMsg("reading input from file '%s'...\n", m_action_rename_filename.c_str());
      std::ifstream rename_stream(m_action_rename_filename.c_str());
      if (!rename_stream.is_open())
      {
        mcrl2::runtime_error("cannot open rename file " + m_action_rename_filename);
      }

      // Parse the rename spec in rename_stream.
      // Note that all parsed data and action declarations in rename_stream are
      // added to lps_old_spec.
      action_rename_specification action_rename_spec =
                      parse_action_rename_specification(rename_stream,lps_old_spec);
      rename_stream.close();

      //rename all assigned actions
      gsVerboseMsg("renaming actions in LPS...\n");
      specification lps_new_spec = action_rename(action_rename_spec, lps_old_spec);
      data::rewriter datar;
      if (m_rewrite) {
        gsVerboseMsg("rewriting data expressions in LPS...\n");
        datar = create_rewriter(lps_new_spec.data());
        lps::rewrite(lps_new_spec, datar);
      }
      if(m_sumelm) {
        gsVerboseMsg("applying sum elimination...\n");
        sumelm_algorithm(lps_new_spec, core::gsVerbose||core::gsDebug).run();
        if(m_rewrite) {
          gsVerboseMsg("rewriting data expressions in LPS...\n");
          lps::rewrite(lps_new_spec, datar);
        }
      }
      //save the result
      if (output_filename().empty()) {
        gsVerboseMsg("writing LPS to stdout...\n");
      } else {
        gsVerboseMsg("writing LPS to file '%s'...\n", output_filename().c_str());
      }
      lps_new_spec.save(output_filename());

      return true;
    }

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
// Strings containing tags used for communication between lpsactionrename and squadt
# define rename_file           "renamefile_in"
# define option_rewrite        "rewrites"
# define option_sumelm         "sumelm"

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

      if (!c.option_exists(option_rewrite)) {
        c.add_option(option_rewrite).set_argument_value< 0 >(true);
      }
      if (!c.option_exists(option_sumelm)) {
        c.add_option(option_sumelm).set_argument_value< 0 >(true);
      }

      /* Create display */
      tipi::tool_display d;

      layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0,5,0,5));

      /* Create and add the top layout manager */
      text_field& rename_file_field = d.create< text_field >();
      checkbox&   rewrite           = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_rewrite));
      checkbox&   sumelm            = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_sumelm));
      button&     okay_button       = d.create< button >().set_label("OK");

      m.append(d.create< horizontal_box >().
            append(d.create< label >().set_text("Rename file name : ")).
            append(rename_file_field)).
            append(d.create< vertical_box >().set_default_alignment(layout::left).
                append(rewrite.set_label("Use rewriting")).
                append(sumelm.set_label("Apply sum elimination"))).
         append(d.create< label >().set_text(" ")).
         append(okay_button, layout::right);

      // Set default values if the configuration specifies them
      if (c.input_exists(rename_file)) {
        rename_file_field.set_text(c.get_input(rename_file).location());
      }

      send_display_layout(d.manager(m));

      /* Wait until the ok button was pressed */
      okay_button.await_change();

      c.add_input(rename_file, tipi::mime_type("mf", tipi::mime_type::text), rename_file_field.get_text());

      /* Add output file to the configuration */
      if (c.output_exists("main-output")) {
        tipi::configuration::object& output_file = c.get_output("main-output");

        output_file.location(c.get_output_name(".lps"));
      }
      else {
        c.add_output("main-output", tipi::mime_type("lps", tipi::mime_type::application), c.get_output_name(".lps"));
      }

      c.get_option(option_rewrite).set_argument_value< 0, boolean >(!rewrite.get_status());
      c.get_option(option_sumelm).set_argument_value< 0, boolean >(!sumelm.get_status());

      send_clear_display();

      update_configuration(c);
    }

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& c) const
    {
      return c.input_exists("main-input") &&
             c.input_exists(rename_file) &&
             c.output_exists("main-output");
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration& c)
    {
      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(c);

      m_action_rename_filename = c.get_input(rename_file).location();
      m_sumelm = c.get_option_argument< bool >(option_sumelm);
      m_rewrite = c.get_option_argument< bool >(option_rewrite);

      bool result = run();

      if (result) {
        send_clear_display();
      }

      return result;
    }
#endif //ENABLE_SQUADT_CONNECTIVITY
};

int main(int argc, char* argv[])
{
   MCRL2_ATERMPP_INIT(argc, argv)

   return action_rename_tool().execute(argc, argv);
}
