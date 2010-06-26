// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbespp.cpp

#include "boost.hpp" // precompiled headers

#define NAME "pbespp"
#define AUTHOR "Aad Mathijssen and Jeroen Keiren"

#include <string>
#include <iostream>
#include <fstream>

#include "mcrl2/core/messaging.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/pbes/pbes.h"

using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2;

//local declarations

class pbespp_tool: public squadt_tool< input_output_tool >
{
  private:
    typedef squadt_tool< input_output_tool > super;

  public:
    pbespp_tool()
      : super(NAME, AUTHOR,
          "pretty print a PBES",
          "Print the PBES in INFILE to OUTFILE in a human readable format. If OUTFILE "
          "is not present, stdout is used. If INFILE is not present, stdin is used."
        ),
        format(ppDefault)
    {}

    bool run()
    {
      print_specification();
      return true;
    }

  protected:
    t_pp_format  format;

    void add_options(interface_description& desc)
    {
      input_output_tool::add_options(desc);
      desc.add_option("format", make_mandatory_argument("FORMAT"),
        "print the PBES in the specified FORMAT:\n"
        "  'default' for a PBES specification (default),\n"
        "  'debug' for 'default' with the exceptions that data expressions are printed in prefix notation using identifiers from the internal format, and each data equation is put in a separate data equation section,\n"
        "  'internal' for a textual ATerm representation of the internal format, or\n"
        "  'internal-debug' for 'internal' with an indented layout", 'f');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      if (parser.options.count("format")) {
        std::string str_format(parser.option_argument("format"));
        if (str_format == "internal") {
          format = ppInternal;
        } else if (str_format == "internal-debug") {
          format = ppInternalDebug;
        } else if (str_format == "debug") {
          format = ppDebug;
        } else if (str_format != "default") {
          parser.error("option -f/--format has illegal argument '" + str_format + "'");
        }
      }
    }

  private:
    void print_specification()
    {
      pbes_system::pbes<> pbes_specification;
      pbes_specification.load(input_filename());

      gsVerboseMsg("printing PBES from %s to %s in the %s format\n",
        input_filename().empty()?"standard input":input_filename().c_str(),
        output_filename().empty()?"standard output":output_filename().c_str(),
        pp_format_to_string(format).c_str());

      if(output_filename().empty())
      {
        std::cout << pbes_system::pp(pbes_specification, format);
      }
      else
      {
        std::ofstream output_stream(output_filename().c_str());
        if(output_stream.is_open())
        {
          output_stream << pbes_system::pp(pbes_specification, format);
          output_stream.close();
        }
        else
        {
          throw mcrl2::runtime_error("could not open output file " + output_filename() + " for writing");
        }
      }
    }

//Squadt connectivity
#ifdef ENABLE_SQUADT_CONNECTIVITY
    protected:

# define option_print_format         "print_format"

      static bool initialise_types()
      {
        tipi::datatype::enumeration< t_pp_format > pp_format_enumeration;

        pp_format_enumeration.
          add(ppDefault, "default").
          add(ppInternal, "internal").
          add(ppInternalDebug, "internal-debug").
          add(ppDebug, "debug");

        return true;
      }

      /** \brief configures tool capabilities */
      void set_capabilities(tipi::tool::capabilities& capabilities) const
      {
        static bool initialised = initialise_types();

        static_cast< void > (initialised); // harmless, and prevents unused variable warnings

        // The tool has only one main input combination
        capabilities.add_input_configuration("main-input",
            tipi::mime_type("pbes", tipi::mime_type::application), tipi::tool::category::transformation);
      }

      /** \brief queries the user via SQuADT if needed to obtain configuration information */
      void user_interactive_configuration(tipi::configuration& configuration)
      {
        using namespace tipi;
        using namespace tipi::layout;
        using namespace tipi::layout::elements;

        // Let squadt_tool update configuration for rewriter and add output file configuration
        synchronise_with_configuration(configuration);

        if (!configuration.output_exists("main-output")) {
          configuration.add_output("main-output", tipi::mime_type("txt", tipi::mime_type::text), configuration.get_output_name(".txt"));
        }

        if (!configuration.option_exists(option_print_format)) {
          configuration.add_option(option_print_format).set_argument_value< 0 >(ppDefault);
        }

        /* Create display */
        tipi::tool_display d;

        // Helper for format selection
        mcrl2::utilities::squadt::radio_button_helper < t_pp_format > format_selector(d);

        tipi::layout::vertical_box& m = d.create< vertical_box >();
        m.append(d.create< label >().set_text("Output format : ")).
          append(d.create< horizontal_box >().
            append(format_selector.associate(ppDefault, "default")).
            append(format_selector.associate(ppInternal, "internal")).
            append(format_selector.associate(ppInternalDebug, "internal-debug")).
            append(format_selector.associate(ppDebug, "debug")));

        format_selector.set_selection(ppDefault);

        button& okay_button = d.create< button >().set_label("OK");
        m.append(d.create< label >().set_text(" ")).
          append(okay_button, layout::right);

        send_display_layout(d.manager(m));

        okay_button.await_change();

        // let squadt_tool update configuration for rewriter and input/output files
        update_configuration(configuration);

        configuration.get_option(option_print_format).set_argument_value< 0 >(format_selector.get_selection());
      }

      /** \brief check an existing configuration object to see if it is usable */
      bool check_configuration(tipi::configuration const& configuration) const
      {
        // Check if everything present
        return configuration.input_exists("main-input") &&
               configuration.output_exists("main-output") &&
                   configuration.option_exists(option_print_format);
      }

      /** \brief performs the task specified by a configuration */
      bool perform_task(tipi::configuration& configuration)
      {
        // Let squadt_tool update configuration for rewriter and add output file configuration
        synchronise_with_configuration(configuration);

        return run();
      }
#endif //ENABLE_SQUADT_CONNECTIVITY
};

class pbespp_gui_tool: public mcrl2_gui_tool<pbespp_tool> {
public:
	pbespp_gui_tool() {

		std::vector<std::string> values;

		values.clear();
		values.push_back("default");
		values.push_back("debug");
		values.push_back("internal");
		values.push_back("internal-debug");
		m_gui_options["format"] = create_radiobox_widget(values);
	}
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return pbespp_gui_tool().execute(argc, argv);
}

