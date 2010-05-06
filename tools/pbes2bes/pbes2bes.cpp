// Author(s): Alexander van Dam, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes2bes.cpp
/// \brief Add your file description here.

#include "boost.hpp" // precompiled headers

//#define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG

//C++
#include <stdexcept>
#include <iostream>
#include <string>
#include <utility>

//MCRL2-specific
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbes2bes.h"
#include "mcrl2/pbes/pbes2bes_algorithm.h"
#include "mcrl2/pbes/pbes2bes_finite_algorithm.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using utilities::command_line_parser;
using utilities::interface_description;
using utilities::make_optional_argument;
using utilities::tools::input_output_tool;
using utilities::tools::rewriter_tool;
using utilities::tools::squadt_tool;

/// The pbes2bes tool.
class pbes2bes_tool: public squadt_tool< rewriter_tool<input_output_tool> >
{
  protected:
    typedef squadt_tool< rewriter_tool<input_output_tool> > super;

    /// The transformation strategies of the tool.
    enum transformation_strategy {
      ts_lazy,
      ts_finite
    };

    transformation_strategy m_strategy;
    pbes_output_format m_output_format;

    /// Sets the transformation strategy.
    /// \param s A transformation strategy.
    void set_transformation_strategy(const std::string& s)
    {
      if (s == "finite")
      {
        m_strategy = ts_finite;
      }
      else if (s == "lazy")
      {
        m_strategy = ts_lazy;
      }
      else
      {
        throw std::runtime_error("unknown output strategy specified (got `" + s + "')");
      }
    }

    /// Sets the output format.
    /// \param format An output format.
    void set_output_format(const std::string& format)
    {
      if (format == "pbes")
      {
        m_output_format = pbes_output_pbes;
      }
      else if (format == "internal")
      {
        m_output_format = pbes_output_internal;
      }
      else if (format == "cwi")
      {
        m_output_format = pbes_output_cwi;
      }
      else
      {
        throw std::runtime_error("unknown output format specified (got `" + format + "')");
      }
    }

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      try
      {
        set_output_format(parser.option_argument("output"));
      }
      catch (std::logic_error)
      {
        set_output_format("pbes");
      }

      try
      {
        set_transformation_strategy(parser.option_argument("strategy"));
      }
      catch (std::logic_error)
      {
        set_transformation_strategy("lazy");
      }

      if (parser.options.count("equation_limit") > 0)
      {
      	int limit = parser.option_argument_as<int>("equation_limit");
      	pbes_system::detail::set_bes_equation_limit(limit);
      }
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.
        add_option("strategy",
          make_optional_argument("NAME", "lazy"),
          "compute the BES using strategy NAME:\n"
          "  'lazy' for computing only boolean equations which can be reached from the initial state (default), or\n"
          "  'finite' for computing all possible boolean equations.",
          's').
        add_option("output",
          make_optional_argument("NAME", "pbes"),
          "store the BES in output format NAME:\n"
          "  'pbes' for the internal binary format (default),\n"
          "  'internal' for the internal textual format, or\n"
          "  'cwi' for the format used by the CWI to solve a BES.",
          'o');
      desc.add_hidden_option("equation_limit",
         make_optional_argument("NAME", "-1"),
         "Set a limit to the number of generated BES equations",
         'l');
    }

    /// \return A string representation of the transformation strategy.
    std::string strategy_string() const
    {
      if (m_strategy == ts_finite)
      {
        return "finite";
      }
      else if (m_strategy == ts_lazy)
      {
        return "lazy";
      }
      return "unknown";
    }

    /// \return A string representation of the output format.
    std::string output_format_string() const
    {
      if (m_output_format == pbes_output_pbes)
      {
        return "pbes";
      }
      else if (m_output_format == pbes_output_cwi)
      {
        return "cwi";
      }
      else if (m_output_format == pbes_output_internal)
      {
        return "internal";
      }
      return "unknown";
    }

  public:
    /// Constructor.
    pbes2bes_tool()
      : super(
          "pbes2bes",
          "Wieger Wesselink; Alexander van Dam and Tim Willemse",
          "compute a BES out of a PBES",
          "Transforms the PBES from INFILE into an equivalent BES and writes it to OUTFILE. "
          "If INFILE is not present, standard input is used. If OUTFILE is not present,   "
          "standard output is used."
        ),
        m_strategy(ts_lazy),
        m_output_format(pbes_output_pbes)
    {}

    /// Runs the algorithm.
    bool run()
    {
      using namespace mcrl2::pbes_system;

      if (core::gsVerbose)
      {
        std::cerr << "parameters of pbes2bes:" << std::endl;
        std::cerr << "  input file:         " << m_input_filename << std::endl;
        std::cerr << "  output file:        " << m_output_filename << std::endl;
        std::cerr << "  strategy:           " << strategy_string() << std::endl;
        std::cerr << "  output format:      " << output_format_string() << std::endl;
      }

      // load the pbes
      pbes<> p;
      p.load(m_input_filename);

      if (!p.is_closed())
      {
        core::gsErrorMsg("The PBES is not closed. Pbes2bes cannot handle this kind of PBES's\nComputation aborted.\n");
        return false;
      }

      unsigned int log_level = 0;
      if (mcrl2::core::gsVerbose)
      {
        log_level = 1;
      }
      if (mcrl2::core::gsDebug)
      {
        log_level = 2;
      }     	

      if (m_strategy == ts_lazy)
      {
        pbes2bes_algorithm algorithm(p.data(), rewrite_strategy(), false, false, log_level);
        algorithm.run(p);
        p = algorithm.get_result();
      }
      else if (m_strategy == ts_finite)
      {
        pbes2bes_finite_algorithm algorithm(rewrite_strategy(), log_level);
        algorithm.run(p);
      }

      // save the result
      save_pbes(p, m_output_filename, m_output_format);

      return true;
    }

    /// Sets the output filename.
    /// \param filename The name of a file.
    void set_output_filename(const std::string& filename)
    {
      m_output_filename = filename;
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
    static bool initialise_types() {
      tipi::datatype::enumeration< transformation_strategy > transformation_strategy_enumeration;

      transformation_strategy_enumeration.
        add(ts_lazy, "lazy").
        add(ts_finite, "finite");

      tipi::datatype::enumeration< pbes_output_format> output_format_enumeration;

      output_format_enumeration.
        add(pbes_output_pbes, "pbes").
        add(pbes_output_internal, "internal").
        add(pbes_output_cwi, "cwi");

      return true;
    }

// Names for options
# define option_transformation_strategy "transformation_strategy"
# define option_selected_output_format  "selected_output_format"

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& c) const
    {
      static bool initialised = initialise_types();

      static_cast< void > (initialised); // harmless, and prevents unused variable warnings

      c.add_input_configuration("main-input",
        tipi::mime_type("pbes", tipi::mime_type::application), tipi::tool::category::transformation);
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

      // Helper for format selection
      utilities::squadt::radio_button_helper < pbes_output_format > format_selector(d);

      // Helper for strategy selection
      utilities::squadt::radio_button_helper < transformation_strategy > strategy_selector(d);

      layout::vertical_box& m = d.create< vertical_box >();

      m.append(d.create< label >().set_text("Output format : ")).
        append(d.create< horizontal_box >().
                    append(format_selector.associate(pbes_output_pbes, "pbes")).
                    append(format_selector.associate(pbes_output_internal, "internal")).
                    append(format_selector.associate(pbes_output_cwi, "cwi")),
              margins(0,5,0,5)).
        append(d.create< label >().set_text("Transformation strategy : ")).
        append(strategy_selector.associate(ts_lazy, "lazy: only boolean equations reachable from the initial state")).
        append(strategy_selector.associate(ts_finite, "finite: all possible boolean equations"));

      add_rewrite_option(d, m);

      button& okay_button = d.create< button >().set_label("OK");

      m.append(d.create< label >().set_text(" ")).
        append(okay_button, layout::right);

      /// Copy values from options specified in the configuration
      if (c.option_exists(option_transformation_strategy)) {
        strategy_selector.set_selection(
            c.get_option_argument< transformation_strategy >(option_transformation_strategy, 0));
      }
      if (c.option_exists(option_selected_output_format)) {
        format_selector.set_selection(
            c.get_option_argument< pbes_output_format >(option_selected_output_format, 0));
      }

      send_display_layout(d.manager(m));

      /* Wait until the ok button was pressed */
      okay_button.await_change();

      /* Add output file to the configuration */
      if (c.output_exists("main-output")) {
        tipi::configuration::object& output_file = c.get_output("main-output");

        output_file.location(c.get_output_name(".pbes"));
      }
      else {
        c.add_output("main-output", tipi::mime_type("pbes", tipi::mime_type::application), c.get_output_name(".pbes"));
      }

      c.add_option(option_transformation_strategy).set_argument_value< 0 >(strategy_selector.get_selection());
      c.add_option(option_selected_output_format).set_argument_value< 0 >(format_selector.get_selection());

      send_clear_display();

      update_configuration(c);
    }

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& c) const
    {
      return c.input_exists("main-input") &&
             c.output_exists("main-output") &&
             c.option_exists(option_transformation_strategy) &&
             c.option_exists(option_selected_output_format);
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration& c)
    {
      static std::string strategies[] = { "lazy", "finite" };
      static std::string formats[]    = { "pbes", "internal", "cwi" };

      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(c);

      m_input_filename = c.get_input("main-input").location();
      m_output_filename = c.get_output("main-output").location();
      set_output_format(formats[c.get_option_argument< size_t >(option_selected_output_format)]);
      set_transformation_strategy(strategies[c.get_option_argument< size_t >(option_transformation_strategy)]);
      bool result = run();

      send_clear_display();

      return result;
    }

#endif // ENABLE_SQUADT_CONNECTIVITY
};

//Main Program
//------------
/// \brief Main program for pbes2bes
int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return pbes2bes_tool().execute(argc, argv);
}
