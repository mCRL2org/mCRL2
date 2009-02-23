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

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <aterm2.h>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/data_implementation.h"
#include "mcrl2/core/data_reconstruct.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/lps_rewrite.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/lps/rename.h"
#include "mcrl2/lps/sumelm.h"
#include "mcrl2/lps/action_rename.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/data/sort_identifier.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/data/data_expression.h"
#include <mcrl2/utilities/input_output_tool.h>
#include <mcrl2/utilities/rewriter_tool.h>

using namespace atermpp;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::data::data_expr;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace std;

using namespace mcrl2;
using mcrl2::utilities::tools::input_output_tool;
using mcrl2::utilities::tools::rewriter_tool;

class action_rename_tool: public rewriter_tool<input_output_tool>
{
  //Type definitions
  //----------------

  protected:
    //t_phase represents the phases at which the program should be able to stop
   typedef enum { PH_NONE, PH_PARSE, PH_TYPE_CHECK, PH_DATA_IMPL} t_phase;

    //t_tool_options represents the options of the tool

    bool            m_pretty;
    bool            m_rewrite;
    bool            m_sumelm;
    t_phase         m_end_phase;
    string          m_action_rename_filename;

    std::string synopsis() const
    {
      return "[OPTION]... --renamefile=NAME [INFILE [OUTFILE]]\n";
    }

    void add_options(interface_description& desc)
    {
      rewriter_tool<input_output_tool>::add_options(desc);
      desc.add_option("renamefile", make_mandatory_argument("NAME"),
              "use the rename rules from NAME", 'f');
      desc.add_option("no-rewrite",
              "do not rewrite data terms while renaming; use when the rewrite system "
              "does not terminate", 'o');
      desc.add_option("no-sumelm",
                "do not apply sum elimination to the final result", 'm');
      desc.add_option("end-phase", make_mandatory_argument("PHASE"),
                "stop conversion and output the action rename specification after phase PHASE: "
                "'pa' (parsing), "
                "'tc' (type checking), or "
                "'di' (data implementation)"
                , 'p');
      desc.add_option("pretty",
                "return a pretty printed version of the output", 'P');

    }

    void parse_options(const command_line_parser& parser)
    {
      rewriter_tool<input_output_tool>::parse_options(parser);

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
        } else if (std::strncmp(phase.c_str(), "di", 3) == 0)
        { m_end_phase = PH_DATA_IMPL;
        } else
        { parser.error("option -p has illegal argument '" + phase + "'");
        }
      }

      m_action_rename_filename = parser.option_argument("renamefile");
    }

  public:
    action_rename_tool()
      : rewriter_tool<input_output_tool>(
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

  //Functions used by the main program
  //----------------------------------

  // static bool parse_command_line(int argc, char **argv, t_tool_options&);
  //Post: The command line options are parsed.
  //      The program has aborted with a suitable error code, if:
  //      - errors were encountered
  //      - non-standard behaviour was requested (help or version)
  //Ret:  the parsed command line options

  // static specification rename_actions(t_tool_options tool_options); XXXXXXXXXXXXXXXXXXXXXXX
  //Pre:  tool_options.action_rename_filename contains a action rename
  //      specification
  //      tool_options.infilename contains an LPS ("" indicates stdin)
  //      tool_options.end_phase indicates at which phase conversion stops
  //Ret:  if end_phase == PH_NONE, the new LPS generated from the action rename
  //      file and the old LPS
  //      if end_phase != PH_NONE, the state formula after phase end_phase
  //      NULL, if something went wrong

  bool run()
  {
    //process action rename specfication

    if (core::gsVerbose)
    {
      std::cerr << "lpsactionrename parameters:" << std::endl;
      std::cerr << "  input file:         " << m_input_filename << std::endl;
      std::cerr << "  output file:        " << m_output_filename << std::endl;
      std::cerr << "  rename file:        " << m_action_rename_filename << std::endl;
      std::cerr << "  rewrite:            " << (m_rewrite?"YES":"NO") << std::endl;
      std::cerr << "  sumelm:             " << (m_sumelm?"YES":"NO") << std::endl;
    }

    //open infilename
    specification lps_old_spec;
    lps_old_spec.load(m_input_filename);

    //parse the action rename file
    ifstream rename_stream(m_action_rename_filename.c_str());
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


    /* if (action_rename_spec == NULL) {
      gsErrorMsg("parsing failed\n");
      return NULL;
    }
    gsDebugMsg("parsing succeeded\n");
    if (end_phase == PH_PARSE) {
      return action_rename_spec;
    }

    //type check formula against a reconstructed lps specification
    ATermAppl reconstructed_lps_old_spec = reconstruct_spec(lps_old_spec);

    gsVerboseMsg("type checking...\n");
    action_rename_spec = type_check_action_rename_spec(action_rename_spec, reconstructed_lps_old_spec);
    if (action_rename_spec == NULL) {
      gsErrorMsg("type checking failed\n");
      return NULL;
    }
    if (end_phase == PH_TYPE_CHECK) {
      return action_rename_spec;
    }

    //implement standard data types and type constructors on the result
    gsVerboseMsg("implementing standard data types and type constructors...\n");
    action_rename_spec = implement_data_action_rename_spec(action_rename_spec, reconstructed_lps_old_spec);
    if (action_rename_spec == NULL) {
      gsErrorMsg("data implementation failed\n");
      return NULL;
    }
    if (end_phase == PH_DATA_IMPL) {
      return action_rename_spec;
    }
    lps_old_spec = specification(reconstructed_lps_old_spec);
    */

    //rename all assigned actions
    gsVerboseMsg("renaming actions...\n");
    specification lps_new_spec = action_rename(action_rename_spec, lps_old_spec);
    data::rewriter datar;
    if (m_rewrite)
    { datar = create_rewriter(lps_new_spec.data());
      lps_new_spec = rewrite_lps(lps_new_spec,datar);
      // lps_new_spec = lpsrewr(lps_new_spec, datar);
      // lps_new_spec = rewrite_lps(lps_new_spec, tool_options.rewrite_strategy);
    }
    if(m_sumelm)
    { lps_new_spec = sumelm(lps_new_spec);
      if(m_rewrite)
      {
        lps_new_spec = rewrite_lps(lps_new_spec,datar);
      }
    }
    // return lps_new_spec;

    lps_new_spec.save(m_output_filename);
    return true;

    //store the result
    /* string outfilename = tool_options.outfilename;
    if (outfilename.empty())
    {
      gsVerboseMsg("saving result to stdout...\n");
    } else {
      gsVerboseMsg("saving result to '%s'...\n", outfilename.c_str());
    }
    if (tool_options.end_phase == PH_NONE && !tool_options.pretty) {
      lps_new_spec.save(outfilename);
    } else
    {
      if (outfilename.empty()) {
        PrintPart_CXX(cout, (ATerm) lps_new_spec, (tool_options.pretty)?ppDefault:ppInternal);
        cout << endl;
      } else {
        ofstream outstream(outfilename.c_str(), ofstream::out|ofstream::binary);
        if (!outstream.is_open()) {
          throw mcrl2::runtime_error("could not open output file '" + outfilename + "' for writing");
        }
        PrintPart_CXX(outstream, (ATerm) lps_new_spec, (tool_options.pretty)?ppDefault:ppInternal);
        outstream.close();
      }
    }
    return EXIT_SUCCESS; */
  }
};


// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include "mcrl2/utilities/mcrl2_squadt_interface.h"

// Strings containing tags used for communication between lpsactionrename and squadt
const char* lps_file_for_input    = "lps_in";
const char* rename_file           = "renamefile_in";
const char* lps_file_for_output   = "lps_out";
const char* option_rewrite        = "rewrites";
const char* option_sumelm         = "sumelm";

class squadt_interactor
              : public mcrl2::utilities::squadt::mcrl2_tool_interface,
                public action_rename_tool
{
  private:
    static bool initialise_types()
    {
      /* tipi::datatype::enumeration< pbes_rewriter_type > storage_types;

      storage_types.
          add(simplify, "simplify").
          add(quantifier_all, "quantifier_all").
          add(quantifier_finite, "quantifier_finite"); */

      return true;
    }

  public:

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration&);

    int execute(int argc, char** argv)
    { if (utilities::squadt::free_activation(*this, argc, argv))
      { return EXIT_SUCCESS;
      }
      return action_rename_tool::execute(argc,argv);
    }
};

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(lps_file_for_input, tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

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
  if (c.output_exists(lps_file_for_output)) {
    tipi::configuration::object& output_file = c.get_output(lps_file_for_output);

    output_file.location(c.get_output_name(".lps"));
  }
  else {
    c.add_output(lps_file_for_output, tipi::mime_type("lps", tipi::mime_type::application), c.get_output_name(".lps"));
  }

  c.get_option(option_rewrite).set_argument_value< 0, boolean >(!rewrite.get_status());
  c.get_option(option_sumelm).set_argument_value< 0, boolean >(!sumelm.get_status());

  send_clear_display();
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const
{
  bool result = true;

  result &= c.input_exists(lps_file_for_input);
  result &= c.input_exists(rename_file);
  result &= c.output_exists(lps_file_for_output);

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {

  m_action_rename_filename = c.get_input(rename_file).location();
  input_filename() = c.get_input(lps_file_for_input).location();
  output_filename() = c.get_output(lps_file_for_output).location();
  m_sumelm = c.get_option_argument< bool >(option_sumelm);
  m_rewrite = c.get_option_argument< bool >(option_rewrite);

  bool result = run();

  if (result) {
    send_clear_display();
  }

  return (result);
}
#endif //ENABLE_SQUADT_CONNECTIVITY


//Main program
//------------

int main(int argc, char **argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  try
  {
#ifdef ENABLE_SQUADT_CONNECTIVITY
     squadt_interactor tool;
#else
     action_rename_tool tool;
#endif
    return tool.execute(argc, argv);
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
  return EXIT_FAILURE;
}


