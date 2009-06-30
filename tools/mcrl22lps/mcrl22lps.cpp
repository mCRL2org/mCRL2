// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl22lps.cpp
/// \brief This tool linearises mcrl2 specifications into linear
///         form.

#include "boost.hpp" // precompiled headers

#define TOOLNAME "mcrl22lps"
#define AUTHOR "Jan Friso Groote"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "mcrl2/core/messaging.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/lin_types.h"
#include "mcrl2/lps/lin_std.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/process/parse.h"

// #include "gc.h"  Required for ad hoc garbage collection. This is possible with ATcollect,
// useful to find garbage collection problems.

using namespace mcrl2::utilities;
using namespace mcrl2::data::detail;
using namespace mcrl2::process;

using mcrl2::core::gsVerbose;
using mcrl2::utilities::tools::input_output_tool;
using mcrl2::utilities::tools::rewriter_tool;
using mcrl2::utilities::tools::squadt_tool;

class mcrl22lps_tool : public squadt_tool< rewriter_tool< input_output_tool > >
{
  typedef squadt_tool< rewriter_tool< input_output_tool > > super;

  private:
    t_lin_options m_linearisation_options;
    bool noalpha;   // indicates whether alpa reduction is needed.
    bool opt_check_only;
    bool pretty;


  protected:

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("lin-method", make_mandatory_argument("NAME"),
          "use linearisation method NAME:\n"
          "  'regular' for generating an LPS in regular form\n"
          "  (specification should be regular, default),\n"
          "  'regular2' for a variant of 'regular' that uses more data variables\n"
          "  (useful when 'regular' does not work), or\n"
          "  'stack' for using stack data types\n"
          "  (useful when 'regular' and 'regular2' do not work)"
          , 'l');
      desc.add_option("cluster",
          "all actions in the final LPS are clustered", 'c');
      desc.add_option("no-cluster",
          "the actions in intermediate LPSs are not clustered "
          "(default behaviour is that intermediate LPSs are "
          "clustered and the final LPS is not clustered)", 'n');
      desc.add_option("no-alpha",
          "alphabet reductions are not applied", 'z');
      desc.add_option("newstate",
          "state variables are encoded using enumerated types "
          "(requires linearisation method 'regular' or 'regular2'); without this option numbers are used", 'w');
      desc.add_option("binary",
          "when clustering use binary case functions instead of "
          "n-ary; in the presence of -w/--newstate, state variables are "
          "encoded by a vector of boolean variables", 'b');
      desc.add_option("statenames",
          "the names of state variables are derived from the specification", 'a');
      desc.add_option("no-rewrite",
          "do not rewrite data terms while linearising; useful when the rewrite "
          "system does not terminate", 'o');
      desc.add_option("no-globvars",
          "instantiate don't care values with arbitrary constants, "
          "instead of modelling them by global variables. This has no effect"
          "on global variable that are declared in the specification.", 'f');
      desc.add_option("no-sumelm",
          "avoid applying sum elimination in parallel composition", 'm');
      desc.add_option("no-deltaelm",
          "avoid removing spurious delta summands", 'g');
      desc.add_option("delta",
          "add a true->delta summands to each state in each process; "
          "these delta's subsume all other conditional timed delta's, "
          "effectively reducing the number of delta summands drastically "
          "in the resulting linear process; speeds up linearisation ", 'D');
      desc.add_option("check-only",
          "check syntax and static semantics; do not linearise", 'e');
      desc.add_option("pretty",
          "return a pretty printed version of the output", 'P');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      m_linearisation_options.final_cluster           = 0 < parser.options.count("cluster");
      m_linearisation_options.no_intermediate_cluster = 0 < parser.options.count("no-cluster");
      // m_linearisation_options.noalpha                 = 0 < parser.options.count("no-alpha");
      noalpha                                         = 0 < parser.options.count("no-alpha");
      m_linearisation_options.newstate                = 0 < parser.options.count("newstate");
      m_linearisation_options.binary                  = 0 < parser.options.count("binary");
      m_linearisation_options.statenames              = 0 < parser.options.count("statenames");
      m_linearisation_options.norewrite               = 0 < parser.options.count("no-rewrite");
      m_linearisation_options.noglobalvars              = 0 < parser.options.count("no-globvars");
      m_linearisation_options.nosumelm                = 0 < parser.options.count("no-sumelm");
      m_linearisation_options.nodeltaelimination      = 0 < parser.options.count("no-deltaelm");
      m_linearisation_options.add_delta               = 0 < parser.options.count("delta");
      pretty                                          = 0 < parser.options.count("pretty");
      // m_linearisation_options.pretty                  = 0 < parser.options.count("pretty");
      m_linearisation_options.rewrite_strategy        = parser.option_argument_as< mcrl2::data::rewriter::strategy >("rewriter");
      m_linearisation_options.lin_method = lmRegular;

      if (0 < parser.options.count("check-only")) 
      { opt_check_only = true;
      }

      if (0 < parser.options.count("lin-method")) {
        if (1 < parser.options.count("lin-method")) {
          parser.error("multiple use of option -l/--lin-method; only one occurrence is allowed");
        }
        std::string lin_method_str(parser.option_argument("lin-method"));
        if (lin_method_str == "stack") {
          m_linearisation_options.lin_method = lmStack;
        } else if (lin_method_str == "regular") {
          m_linearisation_options.lin_method = lmRegular;
        } else if (lin_method_str == "regular2") {
          m_linearisation_options.lin_method = lmRegular2;
        } else {
          parser.error("option -l/--lin-method has illegal argument '" + lin_method_str + "'");
        }
      }

      //check for dangerous and illegal option combinations
      if (m_linearisation_options.newstate && m_linearisation_options.lin_method == lmStack) {
        parser.error("option -w/--newstate cannot be used with -lstack/--lin-method=stack");
      }

      m_linearisation_options.infilename       = input_filename();
      m_linearisation_options.outfilename      = output_filename();
      m_linearisation_options.rewrite_strategy = rewrite_strategy();
    }

    char const* lin_method_to_string(t_lin_method lin_method)
    {
      static const char* method[] = {"stack","regular","regular2"};

      return method[lin_method];
    }

  public:

    mcrl22lps_tool() : super(
             TOOLNAME,
             AUTHOR,
             "translate an mCRL2 specification to an LPS",
             "Linearises the mCRL2 specification in INFILE and writes the resulting LPS to "
             "OUTFILE. If OUTFILE is not present, stdout is used. If INFILE is not present, "
             "stdin is used."),noalpha(false),opt_check_only(false),pretty(false)
    {}

    bool run() 
    { //linearise infilename with options
      
      process_specification spec;
      if (m_linearisation_options.infilename.empty()) 
      { //parse specification from stdin
        if (gsVerbose)
        { std::cerr << "Parsing input from stdin...\n";
        }
        std::stringbuf buf;
        std::cin.get(buf,'\0');
        const std::string input_string=buf.str();
        spec = parse_process_specification(input_string,!noalpha);
      } 
      else 
      { //parse specification from infilename
        std::ifstream instream(m_linearisation_options.infilename.c_str(), std::ifstream::in|std::ifstream::binary);
        if (!instream.is_open()) 
        { throw mcrl2::runtime_error("Cannot open input file: " + m_linearisation_options.infilename);
        }
        if (gsVerbose) 
        { std::cerr << "parsing input file '"  + m_linearisation_options.infilename + "'\n";
        }
        std::stringbuf buf;
        instream.get(buf,'\0');
        const std::string input_string=buf.str();
        spec = parse_process_specification(input_string,!noalpha);
      }
      //report on well-formedness (if needed)
      if (opt_check_only) {
        if (m_linearisation_options.infilename.empty()) {
          std::cerr << "stdin";
        } else {
          std::cerr << "The file '" << m_linearisation_options.infilename << "'";
        }
        std::cerr << " contains a well-formed mCRL2 specification." << std::endl;
        return true;
      }
      //store the result
      if (!pretty) 
      { mcrl2::lps::specification linear_spec(linearise_std(spec,m_linearisation_options));
        if (gsVerbose)
        { if (m_linearisation_options.outfilename.empty())
          { std::cerr << "saving result to stdout...\n";
          } 
          else
          { std::cerr << "saving result to '" << m_linearisation_options.outfilename << "'\n";
          }
        }
        linear_spec.save(m_linearisation_options.outfilename);
      } 
      else
      { if (m_linearisation_options.outfilename.empty()) 
        { std::cout << pp(spec) << std::endl;
          //PrintPart_CXX(std::cout, (ATermAppl) spec, (pretty)?ppDefault:ppInternal);
        } 
        else 
        { std::ofstream outstream(m_linearisation_options.outfilename.c_str(), std::ofstream::out|std::ofstream::binary);
          if (!outstream.is_open()) 
          { throw mcrl2::runtime_error("could not open output file '" + m_linearisation_options.outfilename + "' for writing");
          }
          outstream << pp(spec); // PrintPart_CXX(outstream, (ATermAppl) spec, pretty?ppDefault:ppInternal);
          outstream.close();
        }
      }
      return true;
    }

#ifdef ENABLE_SQUADT_CONNECTIVITY

# define option_linearisation_method     "linearisation_method"
# define option_final_cluster            "final_cluster"
# define option_no_intermediate_cluster  "no_intermediate_cluster"
# define option_no_alpha                 "no_alpha"
# define option_newstate                 "newstate"
# define option_binary                   "binary"
# define option_statenames               "statenames"
# define option_no_rewrite               "no_rewrite"
# define option_no_globalvars              "no_globalvars"
# define option_check_only               "check_only"
# define option_no_sumelm                "no_sumelm"
# define option_no_deltaelm              "no_dataelm"
# define option_add_delta                "add_delta"

  private:

    /** \brief compiles a t_lin_options instance from a configuration */
    bool extract_task_options(tipi::configuration const& c)
    {
      bool result = true;

      if (c.option_exists(option_linearisation_method)) 
      { m_linearisation_options.lin_method = c.get_option_argument< t_lin_method >(option_linearisation_method, 0);
      }
      else 
      { send_error("Configuration does not contain a linearisation method\n");
        result = false;
      }

      m_linearisation_options.final_cluster           = c.get_option_argument< bool >(option_final_cluster);
      m_linearisation_options.no_intermediate_cluster = c.get_option_argument< bool >(option_no_intermediate_cluster);
      noalpha                 = c.get_option_argument< bool >(option_no_alpha);
      m_linearisation_options.newstate                = c.get_option_argument< bool >(option_newstate);
      m_linearisation_options.binary                  = c.get_option_argument< bool >(option_binary);
      m_linearisation_options.statenames              = c.get_option_argument< bool >(option_statenames);
      m_linearisation_options.norewrite               = c.get_option_argument< bool >(option_no_rewrite);
      m_linearisation_options.noglobalvars              = c.get_option_argument< bool >(option_no_globalvars);
      opt_check_only                                  = c.get_option_argument< bool >(option_check_only);
      m_linearisation_options.nosumelm                = c.get_option_argument< bool >(option_no_sumelm);
      m_linearisation_options.nodeltaelimination      = c.get_option_argument< bool >(option_no_deltaelm);
      m_linearisation_options.add_delta               = c.get_option_argument< bool >(option_add_delta);

      m_linearisation_options.infilename       = input_filename();
      m_linearisation_options.outfilename      = output_filename();
      m_linearisation_options.rewrite_strategy = rewrite_strategy();

      return (result);
    }

    static bool initialise_types()
    {
      tipi::datatype::enumeration< t_lin_method > method_enumeration;

      method_enumeration.
        add(lmRegular, "regular").
        add(lmRegular2, "regular2").
        add(lmStack, "expansion");

      return true;
    }

  public:

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& c) const
    {
      bool initialised = initialise_types();

      static_cast< void > (initialised); // harmless, and prevents unused variable warnings

      c.add_input_configuration("main-input", tipi::mime_type("mcrl2", tipi::mime_type::text),
                                                            tipi::tool::category::transformation);
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration& c)
    {
      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::datatype;
      using namespace tipi::layout::elements;

      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(c);

      // Set defaults for options
      if (!c.option_exists(option_final_cluster)) {
        c.add_option(option_final_cluster).set_argument_value< 0 >(false);
      }
      if (!c.option_exists(option_no_intermediate_cluster)) {
        c.add_option(option_no_intermediate_cluster).set_argument_value< 0 >(false);
      }
      if (!c.option_exists(option_no_alpha)) {
        c.add_option(option_no_alpha).set_argument_value< 0 >(false);
      }
      if (!c.option_exists(option_newstate)) {
        c.add_option(option_newstate).set_argument_value< 0 >(false);
      }
      if (!c.option_exists(option_binary)) {
        c.add_option(option_binary).set_argument_value< 0 >(false);
      }
      if (!c.option_exists(option_statenames)) {
        c.add_option(option_statenames).set_argument_value< 0 >(false);
      }
      if (!c.option_exists(option_no_rewrite)) {
        c.add_option(option_no_rewrite).set_argument_value< 0 >(false);
      }
      if (!c.option_exists(option_no_globalvars)) {
        c.add_option(option_no_globalvars).set_argument_value< 0 >(false);
      }
      if (!c.option_exists(option_check_only)) {
        c.add_option(option_check_only).set_argument_value< 0 >(false);
      }
      if (!c.option_exists(option_no_sumelm)) {
        c.add_option(option_no_sumelm).set_argument_value< 0 >(false);
      }
      if (!c.option_exists(option_no_deltaelm)) {
        c.add_option(option_no_deltaelm).set_argument_value< 0 >(false);
      }
      if (!c.option_exists(option_add_delta)) {
        c.add_option(option_add_delta).set_argument_value< 0 >(false);
      }

      /* Create display */
      tipi::tool_display d;

      // Helper for linearisation method selection
      mcrl2::utilities::squadt::radio_button_helper < t_lin_method > method_selector(d);

      layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0,5,0,5));

      m.append(d.create< label >().set_text("Method: ")).
        append(d.create< horizontal_box >().
            append(method_selector.associate(lmStack, "Stack")).
            append(method_selector.associate(lmRegular, "Regular", true)).
            append(method_selector.associate(lmRegular2, "Regular2")));

      checkbox& clusterintermediate = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_no_intermediate_cluster));
      checkbox& clusterfinal        = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_final_cluster));
      checkbox& newstate            = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_newstate));
      checkbox& binary              = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_binary));
      checkbox& statenames          = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_statenames));
      checkbox& add_delta           = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_add_delta));
      checkbox& rewrite             = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_no_rewrite));
      checkbox& alpha               = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_no_alpha));
      checkbox& sumelm              = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_no_sumelm));
      checkbox& deltaelm            = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_no_deltaelm));
      checkbox& globalvars            = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_no_globalvars));
      checkbox& check_only          = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_check_only));

      // two columns to select the linearisation options of the tool
      m.append(d.create< label >().set_text(" ")).
        append(d.create< horizontal_box >().
            append(d.create< vertical_box >().set_default_alignment(layout::right).
                append(clusterintermediate.set_label("Intermediate clustering")).
                append(clusterfinal.set_label("Final clustering")).
                append(newstate.set_label("Use enumerations for state variables")).
                append(binary.set_label("Encode enumerations by booleans")).
                append(statenames.set_label("Derive state names from specification")).
                append(add_delta.set_label("Add delta summands"))).
            append(d.create< vertical_box >().set_default_alignment(layout::left).
                append(rewrite.set_label("Use rewriting")).
                append(alpha.set_label("Apply alphabet axioms")).
                append(sumelm.set_label("Apply sum elimination")).
                append(deltaelm.set_label("Apply delta elimination")).
                append(globalvars.set_label("Generate global variables")).
                append(check_only.set_label("Check input only and do not linearise"))));

      add_rewrite_option(d, m);

      // Add okay button
      button& okay_button = d.create< button >().set_label("OK");

      m.append(d.create< label >().set_text(" ")).
        append(okay_button, layout::right);

      if (c.option_exists(option_linearisation_method)) {
        method_selector.set_selection(c.get_option_argument< t_lin_method >(option_linearisation_method, 0));
      }

      send_display_layout(d.manager(m));

      /* Wait for the OK button to be pressed */
      okay_button.await_change();

      // Update configuration
      if (!c.output_exists("main-output")) {
        c.add_output("main-output", tipi::mime_type("lps", tipi::mime_type::application), c.get_output_name(".lps"));
      }

      c.add_option(option_linearisation_method).
              set_argument_value< 0 >(method_selector.get_selection());

      if (opt_check_only) { // file will not be produced
        c.remove_output("main-output");
      } 

      c.get_option(option_final_cluster).set_argument_value< 0 >(clusterfinal.get_status());
      c.get_option(option_no_intermediate_cluster).set_argument_value< 0 >(!clusterintermediate.get_status());
      c.get_option(option_no_alpha).set_argument_value< 0 >(!alpha.get_status());
      c.get_option(option_newstate).set_argument_value< 0 >(newstate.get_status());
      c.get_option(option_binary).set_argument_value< 0 >(binary.get_status());
      c.get_option(option_statenames).set_argument_value< 0 >(statenames.get_status());
      c.get_option(option_no_rewrite).set_argument_value< 0 >(!rewrite.get_status());
      c.get_option(option_no_globalvars).set_argument_value< 0 >(!globalvars.get_status());
      c.get_option(option_check_only).set_argument_value< 0 >(!check_only.get_status());
      c.get_option(option_no_sumelm).set_argument_value< 0 >(!sumelm.get_status());
      c.get_option(option_no_deltaelm).set_argument_value< 0 >(!deltaelm.get_status());
      c.get_option(option_add_delta).set_argument_value< 0 >(add_delta.get_status());

      send_clear_display();

      // Let squadt_tool update configuration for rewriter and add output file configuration
      update_configuration(c);
    }

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& c) const
    {
      return c.input_exists("main-input") &&
             c.input_exists("main-output") &&
             c.input_exists(option_linearisation_method);
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration& c)
    {
      using namespace boost;
      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::datatype;
      using namespace tipi::layout::elements;

      synchronise_with_configuration(c);

      // Extract configuration
      extract_task_options(c);

      /* Create display */
      tipi::tool_display d;

      label& message = d.create< label >();

      d.manager(d.create< vertical_box >().
                            append(message.set_text("Linearisation in progress"), layout::left));

      send_display_layout(d);

      // Perform linearisation

      std::istringstream instream(m_linearisation_options.infilename.c_str(), std::ifstream::in|std::ifstream::binary);
      const std::string input_string(instream.str());
      process_specification input_result = parse_process_specification(input_string,!noalpha);

      if (opt_check_only) 
      { message.set_text(str(format("%s contains a well-formed mCRL2 specification.") % m_linearisation_options.infilename));
      }
      else 
      { mcrl2::lps::specification specification(linearise_std(input_result, m_linearisation_options));
        //store the result
        specification.save(m_linearisation_options.outfilename);
        message.set_text("Linearisation finished");
      }

      send_display_layout(d);

      return true;
    }
#endif
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)
  
  return mcrl22lps_tool().execute(argc, argv);
}

