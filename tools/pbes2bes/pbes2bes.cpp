// Author(s): Alexander van Dam
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes2bes.cpp
/// \brief Add your file description here.

#define NAME "pbes2bes"
#define AUTHOR "Alexander van Dam"

//C++
#include <cstdio>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <sstream>

//Boost
#include <boost/program_options.hpp>

//MCRL2-specific
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/version_info.h"

//LPS-Framework
#include "mcrl2/pbes/pbes2bes.h"
#include "mcrl2/pbes/io.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::pbes_system;

namespace po = boost::program_options;

//Type definitions
//----------------
struct t_tool_options {
	string opt_outputformat;
	string opt_strategy;
	string infilename;
	string outfilename;
};

bool process(t_tool_options tool_options)
{
  pbes<> pbes_spec = load_pbes(tool_options.infilename);

  if (!pbes_spec.is_well_typed())
  {
    core::gsErrorMsg("The PBES is not well formed. Pbes2bes cannot handle this kind of PBES's\nComputation aborted.\n");
  }

  if (!pbes_spec.is_closed())
  {
    core::gsErrorMsg("The PBES is not closed. Pbes2bes cannot handle this kind of PBES's\nComputation aborted.\n");
  }

  if (tool_options.opt_strategy == "finite")
  {
    pbes_spec = do_finite_algorithm(pbes_spec);
  }
  else if (tool_options.opt_strategy == "lazy")
  {
    pbes_spec = do_lazy_algorithm(pbes_spec);
  }
  save_pbes(pbes_spec, tool_options.outfilename, tool_options.opt_outputformat);

  return true;
}

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/squadt_interface.h>

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

  private:

    static const char*  pbes_file_for_input;  ///< file containing an LPS
    static const char*  pbes_file_for_output; ///< file used to write the output to

    enum pbes_output_format {
      binary,
      internal,
      cwi
    };

    enum transformation_strategy {
      lazy,
      finite
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
const char* squadt_interactor::pbes_file_for_output = "pbes_out";

const char* squadt_interactor::option_transformation_strategy = "transformation_strategy";
const char* squadt_interactor::option_selected_output_format  = "selected_output_format";

squadt_interactor::squadt_interactor() {
  transformation_method_enumeration.reset(new tipi::datatype::enumeration("lazy"));

  transformation_method_enumeration->add_value("finite");

  output_format_enumeration.reset(new tipi::datatype::enumeration("binary"));

  output_format_enumeration->add_value("internal");
  output_format_enumeration->add_value("cwi");
}

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(pbes_file_for_input, tipi::mime_type("pbes", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  /* Create display */
  tipi::layout::tool_display d;

  // Helper for format selection
  mcrl2::utilities::squadt::radio_button_helper < pbes_output_format > format_selector(d);

  // Helper for strategy selection
  mcrl2::utilities::squadt::radio_button_helper < transformation_strategy > strategy_selector(d);

  layout::vertical_box& m = d.create< vertical_box >();

  m.append(d.create< label >().set_text("Output format : ")).
    append(d.create< horizontal_box >().
                append(format_selector.associate(binary, "binary")).
                append(format_selector.associate(internal, "internal")).
                append(format_selector.associate(cwi, "cwi")),
          margins(0,5,0,5)).
    append(d.create< label >().set_text("Transformation stragey : ")).
    append(strategy_selector.associate(lazy, "lazy: only boolean equations reachable from the initial state")).
    append(strategy_selector.associate(finite, "finite: all possible boolean equations"));

  button& okay_button = d.create< button >().set_label("OK");

  m.append(d.create< label >().set_text(" ")).
    append(okay_button, layout::right);

  /// Copy values from options specified in the configuration
  if (c.option_exists(option_transformation_strategy)) {
    strategy_selector.set_selection(static_cast < transformation_strategy > (
        c.get_option_argument< size_t >(option_transformation_strategy, 0)));
  }
  if (c.option_exists(option_selected_output_format)) {
    format_selector.set_selection(static_cast < pbes_output_format > (
        c.get_option_argument< size_t >(option_selected_output_format, 0)));
  }
  
  send_display_layout(d.set_manager(m));

  /* Wait until the ok button was pressed */
  okay_button.await_change();
  
  /* Add output file to the configuration */
  if (c.output_exists(pbes_file_for_output)) {
    tipi::object& output_file = c.get_output(pbes_file_for_output);
 
    output_file.set_location(c.get_output_name(".pbes"));
  }
  else {
    c.add_output(pbes_file_for_output, tipi::mime_type("pbes", tipi::mime_type::application), c.get_output_name(".pbes"));
  }

  c.add_option(option_transformation_strategy).append_argument(transformation_method_enumeration,
                                static_cast < transformation_strategy > (strategy_selector.get_selection()));
  c.add_option(option_selected_output_format).append_argument(output_format_enumeration,
                                static_cast < pbes_output_format > (format_selector.get_selection()));

  send_clear_display();
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(pbes_file_for_input);
  result &= c.output_exists(pbes_file_for_output);
  result &= c.option_exists(option_transformation_strategy);
  result &= c.option_exists(option_selected_output_format);

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  static std::string strategies[] = { "lazy", "finite" };
  static std::string formats[]    = { "binary", "internal", "cwi" };

  t_tool_options tool_options;

  tool_options.opt_outputformat = formats[c.get_option_argument< size_t >(option_selected_output_format)];
  tool_options.opt_strategy     = strategies[c.get_option_argument< size_t >(option_transformation_strategy)];
  tool_options.infilename       = c.get_input(pbes_file_for_input).get_location();
  tool_options.outfilename      = c.get_output(pbes_file_for_output).get_location();

  bool result = process(tool_options);
 
  send_clear_display();

  return (result);
}
#endif

//function parse_command_line
//---------------------------
/// \brief Parse the command line options.
t_tool_options parse_command_line(int argc, char** argv)
{
	t_tool_options tool_options;
	string opt_outputformat;
	string opt_strategy;
	vector< string > file_names;

	po::options_description desc;

	desc.add_options()
			("strategy,s",	po::value<string>(&opt_strategy)->default_value("lazy"), "use strategy arg (default 'lazy');\n"
							"The following strategies are available:\n"
							"finite  Compute all possible boolean equations\n"
							"lazy    Compute only boolean equations which can be reached from the initial state\n")
			("output,o",	po::value<string>(&opt_outputformat)->default_value("binary"), "use outputformat arg (default 'binary');\n"
			 				"available output formats are binary, internal and cwi")
			("verbose,v",	"turn on the display of short intermediate messages")
			("debug,d",		"turn on the display of detailed intermediate messages")
			("version",		"display version information")
			("help,h",		"display this help")
			;

	po::options_description hidden("Hidden options");
	hidden.add_options()
			("file_names",	po::value< vector< string > >(), "input/output files")
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
		cout << "Usage: " << argv[0] << " [OPTION]... [INFILE [OUTFILE]]" << endl;
		cout << "Transform the PBES from INFILE into an equivalent BES and write it to OUTFILE." << endl;
		cout << "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is" << endl;
                cout << "used." << endl;
		cout << endl;
                cout << "Options:" << endl;
		cout << desc;
                cout << endl;
                cout << "Report bugs at <http://www.mcrl2.org/issuetracker>." << endl;

		exit(0);
	}

	if (vm.count("version"))
	{
                print_version_information(NAME, AUTHOR);
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
		if (!((opt_outputformat == "internal") || (opt_outputformat == "binary") || (opt_outputformat == "cwi")))
		{
			gsErrorMsg("Unknown outputformat specified. Available outputformats are binary, internal and cwi\n");
			exit(1);
		}
	}

	if (vm.count("strategy")) // Output format
	{
		opt_strategy = vm["strategy"].as< string >();
		if (!((opt_strategy == "finite") || (opt_strategy == "lazy")))
		{
			gsErrorMsg("Unknown strategy specified. Available strategies are finite and lazy\n");
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
	tool_options.opt_strategy = opt_strategy;
	return tool_options;
}

//Main Program
//------------
/// \brief Main program for pbes2bes
int main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (!mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
#endif

  process(parse_command_line(argc, argv));

#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}
