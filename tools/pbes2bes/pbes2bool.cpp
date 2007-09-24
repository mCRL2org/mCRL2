// Author(s): Jan Friso Groote
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes2bool.cpp
/// \brief Add your file description here.

// ======================================================================
//
// file          : pbes2bool
// date          : 15-04-2007
// version       : 0.1.3
//
// author(s)     : Alexander van Dam <avandam@damdonk.nl>
//                 Jan Friso Groote <J.F.Groote@tue.nl>
//
// ======================================================================


#define NAME "pbes2bool"
#define VERSION "0.1.3"
#define AUTHOR "Alexander van Dam and Jan Friso Groote"


//C++
#include <ostream>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>

#include <sstream>

//Boost
#include <boost/program_options.hpp>

//MCRL-specific
#include "print/messaging.h"

//LPS-Framework
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/utility.h"
#include "mcrl2/data/data_operators.h"
#include "mcrl2/data/sort.h"
#include "mcrl2/pbes/pbes2bool.h"

//ATERM-specific
#include "atermpp/substitute.h"
#include "atermpp/utility.h"
#include "atermpp/indexed_set.h"
#include "atermpp/table.h"
#include "_aterm.h"

//Tool-specific
// #include "pbes_rewrite_jfg.h"
// #include "sort_functions.h"
#include "bes.h"

using namespace std;
using namespace lps;
using namespace mcrl2::utilities;
using bes::bes_expression;

using atermpp::make_substitution;

namespace po = boost::program_options;

//Function declarations used by main program
//------------------------------------------
static t_tool_options parse_command_line(int argc, char** argv);
//Post: The command line options are parsed.
//      The program has aborted with a suitable error code, if:
//    - errors were encounterd
//    - non-standard behaviour was requested (help, version)
//Ret:  The parsed command line options

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/squadt_interface.h>

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

  private:

    static const char*  pbes_file_for_input;  ///< file containing an LPS
    static const char*  bes_file_for_output; ///< file used to write the output to

    enum bes_output_format {
      none,
      vasy,
      cwi
    };

    static const char* option_transformation_strategy;
    static const char* option_selected_output_format;

  private:

    boost::shared_ptr < tipi::datatype::enumeration > transformation_method_enumeration;
    boost::shared_ptr < tipi::datatype::enumeration > output_format_enumeration;

  public:

    /** \brief constructor */
    squadt_interactor();

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration&);
};

const char* squadt_interactor::pbes_file_for_input  = "pbes_in";
const char* squadt_interactor::bes_file_for_output = "bes_out";

const char* squadt_interactor::option_transformation_strategy = "transformation_strategy";
const char* squadt_interactor::option_selected_output_format  = "selected_output_format";

squadt_interactor::squadt_interactor() {
  transformation_method_enumeration.reset(new tipi::datatype::enumeration("lazy"));

  output_format_enumeration.reset(new tipi::datatype::enumeration("none"));

  output_format_enumeration->add_value("vasy");
  output_format_enumeration->add_value("cwi");
}

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_combination(pbes_file_for_input, tipi::mime_type("pbes", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  layout::tool_display::sptr display(new layout::tool_display);

  /* Create and add the top layout manager */
  layout::vertical_box::aptr top(new layout::vertical_box);

  /* First column */
  layout::manager* h = new layout::horizontal_box();

  h->add(new label("Output format : "));
  
  mcrl2::utilities::squadt::radio_button_helper < bes_output_format >
        format_selector(h, none, "none");

  format_selector.associate(h, vasy, "vasy");
  format_selector.associate(h, cwi, "cwi");

  if (c.option_exists(option_selected_output_format)) {
    format_selector.set_selection(static_cast < bes_output_format > (
        c.get_option_argument< size_t >(option_selected_output_format, 0)));
  }
  
  /* Attach row */
  top->add(h, margins(0,5,0,5));

  top->add(new label("Transformation strategy :"));

  mcrl2::utilities::squadt::radio_button_helper < transformation_strategy >
        transformation_selector(top, lazy, "lazy:       generate all boolean equations reachable from the initial state");

  if (c.option_exists(option_transformation_strategy)) {
    transformation_selector.set_selection(static_cast < transformation_strategy > (
        c.get_option_argument< size_t >(option_transformation_strategy, 0)));
  }
  
  button* okay_button = new button("OK");

  top->add(okay_button, layout::top);

  display->set_manager(top);

  m_communicator.send_display_layout(display);

  /* Wait until the ok button was pressed */
  okay_button->await_change();

  c.add_option(option_transformation_strategy).append_argument(transformation_method_enumeration,
                                static_cast < transformation_strategy > (transformation_selector.get_selection()));
  c.add_option(option_selected_output_format).append_argument(output_format_enumeration,
                                static_cast < bes_output_format > (format_selector.get_selection()));

  if (c.get_option_argument< size_t >(option_selected_output_format)!=none)
  {
    /* Add output file to the configuration */
    if (c.output_exists(bes_file_for_output)) {
      tipi::object& output_file = c.get_output(bes_file_for_output);
   
      output_file.set_location(c.get_output_name(".txt"));
    }
    else {
      c.add_output(bes_file_for_output, tipi::mime_type("txt", tipi::mime_type::application), 
                   c.get_output_name(".txt"));
    }
  }

  send_clear_display();
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(pbes_file_for_input);
  result &= c.option_exists(option_transformation_strategy);
  result &= c.option_exists(option_selected_output_format);
  if (result && (c.get_option_argument< size_t >(option_selected_output_format)!=none))
  { /* only check for the existence of an outputfile if the output format does
       not equal none */
    result &= c.output_exists(bes_file_for_output);
  }

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  // static std::string strategies[] = { "lazy", "fly" };
  static std::string formats[]    = { "none", "vasy", "cwi" };

  t_tool_options tool_options;

  tool_options.opt_outputformat = formats[c.get_option_argument< size_t >(option_selected_output_format)];
  switch (c.get_option_argument< size_t >(option_transformation_strategy))
  { case 1: tool_options.opt_strategy=lazy;
            break;
    case 2: tool_options.opt_strategy=optimize;
            break;
    case 3: tool_options.opt_strategy=on_the_fly;
            break;
    case 4: tool_options.opt_strategy=on_the_fly_with_fixed_points;
            break;
    default: assert(0); // other options are not possible
  }
  // tool_options.opt_strategy     = c.get_option_argument< size_t >(option_transformation_strategy);
  tool_options.infilename       = c.get_input(pbes_file_for_input).get_location();

  if (c.output_exists(bes_file_for_output)) {
    tool_options.outfilename      = c.get_output(bes_file_for_output).get_location();
  }

  send_clear_display();

  bool result = process(tool_options);
 
  return (result);
}
#endif


//Main Program
//------------
int main(int argc, char** argv)
{
  //Initialise ATerm library and lowlevel-functions
  
  ATerm bottom;
  ATinit(argc, argv, &bottom);
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
    return 0;
  }
#endif

  return process(parse_command_line(argc, argv));
}

//function parse_command_line
//---------------------------
t_tool_options parse_command_line(int argc, char** argv)
{
  t_tool_options tool_options;
  string opt_outputformat;
  string opt_strategy;
  bool opt_precompile_pbes=false;
  bool opt_use_hashtables=false;
  bool opt_construct_counter_example=false;
  bool opt_store_as_tree=false;
  bool opt_data_elm=true;

  string opt_rewriter;
  vector< string > file_names;

  po::options_description desc;

  desc.add_options()
      ("strategy,s",  po::value<string>(&opt_strategy)->default_value("0"), "use strategy arg (default '0');\n"
       "The following strategies are available:\n"
       "0) Compute all boolean equations which can be reached from the initial state, without"
       "any optimization (default). This is is the most data efficient option per generated equation.\n"
       "1) Optimize by immediately substituting the the right hand sides for already investigated"
       "variables that are true or false when generating a expression. This is as memory efficient as 0.\n"
       "2) In addition to 1, also substitute variables that are true or false into an already generated"
       "right hand sides. This can mean that certain variables become unreachable (e.g. X0 in X0 && X1,"
       "when X1 becomes false, assuming X0 does not occur elsewhere. It will be maintained which variables"
       "have become unreachable as these do not have to be investigated. Depending on the PBES, this can"
       "reduce the size of the generated BES substantially, but requires a larger memory footstamp.\n"
       "3) In addition to 2, investigate for generated variables whether they occur on a loop, such that"
       "they can be set to true or false, depending on the fixed point symbol. This can increase the time"
       "needed to generate an equation substantially")
      ("rewriter,R", po::value<string>(&opt_rewriter)->default_value("inner"), "indicate the rewriter to be used. Options are:\n"
       "inner   interpreting innermost rewriter (default),\n"
       "jitty   interpreting just in time rewriter,\n"
       "innerc  compiling innermost rewriter (not for Windows),\n"
       "jittyc  compiling just in time rewriter (fastest, not for Windows).\n")
      ("counter,c", "Print at the end a tree labelled with instantiations of the left hand side of"
       "equations. This tree is an indication of how pbes2bool came to the validity/invalidity of the PBES.")
      ("precompile,P", "Precompile the pbes for faster rewriting. Does not work when the toolset is compiled in debug mode")
      ("hashtables,H", "Use hashtables when substituting in bes equations, and translate internal expressions to binary decision diagrams (discouraged, due to heavy performance penalties).")
      ("output,o",  po::value<string>(&opt_outputformat)->default_value("none"), "use outputformat arg (default 'none');\n"
               "available outputformats are none, vasy and cwi")
      ("tree,r", "store state in a tree (for memory efficiency)")
      ("unused-data,u","do not remove unused parts of the data specification")
      ("verbose,v",  "turn on the display of short intermediate gsMessages")
      ("debug,d",    "turn on the display of detailed intermediate gsMessages")
      ("version",    "display version information")
      ("help,h",    "display this help")
      ;

  po::options_description hidden("Hidden options");
  hidden.add_options()
      ("file_names",  po::value< vector< string > >(), "input/output files")
      ;

  po::options_description cmdline_options;
  cmdline_options.add(desc).add(hidden);

  po::options_description visible("Allowed options");
  visible.add(desc);

  po::positional_options_description p;
  p.add("file_names", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    cerr << "Usage: " << argv[0] << " [OPTION]... [INFILE [OUTFILE]]" << endl;
    cerr << "Solves PBES from stdin or INFILE, or outputs an equivalent BES." << endl;
    cerr << "By default the result is written to stdout, otherwise to OUTFILE." << endl;
    cerr << endl;
    cerr << desc;

    exit(0);
  }
 
  if (vm.count("precompile"))
  { opt_precompile_pbes=true;
  }

  if (vm.count("hashtables"))
  { opt_use_hashtables=true;
  }

  if (vm.count("counter"))
  { opt_construct_counter_example=true;
  }
 
  if (vm.count("tree"))
  { opt_store_as_tree=true;
  }

  if (vm.count("unused-data"))
  { opt_data_elm=false;
  }

  if (vm.count("version"))
  {
    cerr << NAME << " " << VERSION <<  " (revision " << REVISION << ")" << endl;

    exit(0);
  }

  if (vm.count("debug"))
  {
    gsSetDebugMsg();
  }

  if (vm.count("verbose"))
  {
    gsSetVerboseMsg();
  }

  if (vm.count("output")) // Output format
  {
    opt_outputformat = vm["output"].as< string >();
    if (!((opt_outputformat == "none") || (opt_outputformat == "vasy") || (opt_outputformat == "cwi")))
    {
      gsErrorMsg("Unknown outputformat specified. Available outputformats are none, vasy and cwi\n");
      exit(1);
    }
  }

  if (vm.count("strategy")) // Bes solving strategy (currently only one available)
  {
    opt_strategy = vm["strategy"].as< string >();
    if (!(opt_strategy == "0" || opt_strategy == "1" || opt_strategy == "2" || opt_strategy == "3" ))
    {
      gsErrorMsg("Unknown strategy specified. The available strategies are 0, 1, 2, and 3\n");
      exit(1);
    }
  }

  if (vm.count("rewriter")) // Select the rewiter to be used
  { 
    opt_rewriter = vm["rewriter"].as< string >();
    if (!(opt_rewriter == "inner") &&
        !(opt_rewriter == "jitty") &&
        !(opt_rewriter == "innerc") &&
        !(opt_rewriter == "jittyc"))
    { 
      gsErrorMsg("Unknown rewriter specified. Available rewriters are inner, jitty, innerc and jittyc\n");
      exit(1);
    }

        
  }
  
  if (vm.count("file_names"))
  {
    file_names = vm["file_names"].as< vector< string > >();
  }

  string infilename;
  string outfilename;
  if (file_names.size() == 0)
  {
    // Read from and write to stdin
    infilename = "-";
    outfilename = "-";
  }
  else if ( 2 < file_names.size())
  {
    cerr << NAME << ": Too many arguments" << endl;
    exit(1);
  }
  else
  {
    infilename = file_names[0];
    if (file_names.size() == 2)
    {
      outfilename = file_names[1];
    }
    else
    {
      outfilename = "-";
    }
  }
  
  tool_options.infilename = infilename;
  tool_options.outfilename = outfilename;
  
  tool_options.opt_outputformat = opt_outputformat;

  if (opt_strategy=="0")
  { tool_options.opt_strategy=lazy;
  }
  else if (opt_strategy=="1")
  { tool_options.opt_strategy=optimize;
  }
  else if (opt_strategy=="2")
  { tool_options.opt_strategy=on_the_fly;
  }
  else if (opt_strategy=="3")
  { tool_options.opt_strategy=on_the_fly_with_fixed_points;
  }
  else assert(0); // Unknown strategy. Should not occur here, as there is a check for this above.
  
  if (opt_rewriter=="inner")
  { tool_options.rewrite_strategy=GS_REWR_INNER;
  }
  else if (opt_rewriter=="jitty")
  { tool_options.rewrite_strategy=GS_REWR_JITTY;
  }
  else if (opt_rewriter=="innerc")
  { tool_options.rewrite_strategy=GS_REWR_INNERC;
  }
  else if (opt_rewriter=="jittyc")
  { tool_options.rewrite_strategy=GS_REWR_JITTYC;
  }
  else assert(0); // Unknown rewriter specified. Should have been caught above.

  tool_options.opt_precompile_pbes=opt_precompile_pbes;
  tool_options.opt_construct_counter_example=opt_construct_counter_example;
  tool_options.opt_store_as_tree=opt_store_as_tree;
  tool_options.opt_data_elm=opt_data_elm;
  tool_options.opt_use_hashtables=opt_use_hashtables;
  
  return tool_options;
}
