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

//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG

#define NAME "pbes2bes"
#define AUTHOR "Alexander van Dam, Wieger Wesselink"

//C++
#include <cstdio>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <sstream>

//MCRL2-specific
#include "mcrl2/core/messaging.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbes2bes.h"
#include "mcrl2/pbes/pbes2bes_algorithm.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/filter_tool_with_rewriter.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities;

/// The output formats of the tool.
enum pbes_output_format {
  of_binary,
  of_internal,
  of_cwi
};

/// The transformation strategies of the tool.
enum transformation_strategy {
  ts_lazy,
  ts_finite,
  ts_oldlazy
};

/// The pbes2bes tool.
class pbes2bes_tool: public utilities::filter_tool_with_rewriter
{
  protected:
    transformation_strategy m_strategy;
    pbes_output_format m_output_format;

  public:
    /// Sets the transformation strategy.
    /// \param s A transformation strategy.
    void set_transformation_strategy(const std::string& s)
    {
      if (s == "oldlazy")
      {
        m_strategy = ts_oldlazy;
      }
      else if (s == "finite")
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
      if (format == "binary")
      {
        m_output_format = of_binary;
      }
      else if (format == "internal")
      {
        m_output_format = of_internal;
      }
      else if (format == "cwi")
      {
        m_output_format = of_cwi;
      }
      else
      {
        throw std::runtime_error("unknown output format specified (got `" + format + "')");
      }
    }

  protected:   
    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser)
    {
      try
      {
        set_output_format(parser.option_argument("output"));
      }
      catch (std::logic_error)
      {
        set_output_format("binary");
      }

      try
      {
        set_transformation_strategy(parser.option_argument("strategy"));
      }
      catch (std::logic_error)
      {
        set_transformation_strategy("lazy");
      }
    }

    void add_options(interface_description& clinterface)
    {
      clinterface.
        add_option("strategy",
          make_optional_argument("NAME", "lazy"),
          "compute the BES using strategy NAME:\n"
          "  'lazy' for computing only boolean equations which can be reached from the initial state (default), or\n"
          "  'finite' for computing all possible boolean equations, or\n"
          "  'oldlazy' for the previous version of the lazy algorithm.",
          's').
        add_option("output",
          make_optional_argument("NAME", "binary"),
          "store the BES in output format NAME:\n"
          "  'binary' for the internal binary format (default),\n"
          "  'internal' for the internal textual format, or\n"
          "  'cwi' for the format used by the CWI to solve a BES.",
          'o');
    }

    /// \return A string representation of the transformation strategy.
    std::string strategy_string() const
    {
      if (m_strategy == ts_oldlazy)
      {
        return "oldlazy";
      }
      else if (m_strategy == ts_finite)
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
      if (m_output_format == of_binary)
      {
        return "binary";
      }
      else if (m_output_format == of_cwi)
      {
        return "cwi";
      }
      else if (m_output_format == of_internal)
      {
        return "internal";
      }
      return "unknown";
    }

  public:
    /// Constructor.
    pbes2bes_tool()
      : filter_tool_with_rewriter(
          "pbes2bes",
          "Alexander van Dam, Wieger Wesselink",
          "Transforms the PBES from INFILE into an equivalent BES and writes it to OUTFILE. "
          "If INFILE is not present, standard input is used. If OUTFILE is not present,   "
          "standard output is used."
        ),
        m_strategy(ts_lazy),
        m_output_format(of_binary)
    {}

    /// Runs the algorithm.
    bool run()
    {
      if (mcrl2::core::gsVerbose)
      {
        std::cout << "pbes2bes parameters:" << std::endl;
        std::cout << "  input file:         " << m_input_filename << std::endl;
        std::cout << "  output file:        " << m_output_filename << std::endl;
        std::cout << "  strategy:           " << strategy_string() << std::endl;
        std::cout << "  output format:      " << output_format_string() << std::endl;
      }

      // load the pbes
      pbes<> p;
      p.load(m_input_filename);

      if (!p.is_closed())
      {
        gsErrorMsg("The PBES is not closed. Pbes2bes cannot handle this kind of PBES's\nComputation aborted.\n");
        return false;
      }

      if (m_strategy == ts_lazy)
      {
        pbes2bes_algorithm algorithm(p.data(), rewrite_strategy());
        algorithm.run(p);
        p = algorithm.get_result();
      }
      else
      {
        // data rewriter
        data::rewriter datar = create_rewriter(p.data());
        
        // name generator
        std::string prefix = "UNIQUE_PREFIX"; // TODO: compute a unique prefix
        data::number_postfix_generator name_generator(prefix);
        
        // data enumerator
        data::data_enumerator<data::number_postfix_generator> datae(p.data(), datar, name_generator);
        
        // pbes rewriter
        data::rewriter_with_variables datarv(datar);
        pbes_system::enumerate_quantifiers_rewriter<pbes_system::pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > pbesr(datarv, datae, false);   
        
        if (m_strategy == ts_finite)
        {
          p = do_finite_algorithm(p, pbesr);
        }
        else if (m_strategy == ts_oldlazy)
        {
          p = do_lazy_algorithm(p, pbesr);
        }
      }

      // save the result
      save_pbes(p, m_output_filename, output_format_string());

      return true;
    }

    /// Sets the output filename.
    /// \param filename The name of a file.
    void set_output_filename(const std::string& filename)
    {
      m_output_filename = filename;
    }
};

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/mcrl2_squadt_interface.h>

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

  private:

    static const char*  pbes_file_for_input;  ///< file containing an LPS
    static const char*  pbes_file_for_output; ///< file used to write the output to
    static const char* option_transformation_strategy;
    static const char* option_selected_output_format;

    static bool initialise_types() {
      tipi::datatype::enumeration< transformation_strategy > transformation_strategy_enumeration;
    
      transformation_strategy_enumeration.
        add(ts_oldlazy, "oldlazy").
        add(ts_finite, "finite");
    
      tipi::datatype::enumeration< pbes_output_format> output_format_enumeration;
    
      output_format_enumeration.
        add(of_binary, "binary").
        add(of_internal, "internal").
        add(of_cwi, "cwi");

      return true;
    }

  public:

    /** \brief constructor */
    squadt_interactor() {
      static bool initialised = initialise_types();

      static_cast< void > (initialised); // harmless, and prevents unused variable warnings
    }

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

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(pbes_file_for_input, tipi::mime_type("pbes", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  /* Create display */
  tipi::tool_display d;

  // Helper for format selection
  mcrl2::utilities::squadt::radio_button_helper < pbes_output_format > format_selector(d);

  // Helper for strategy selection
  mcrl2::utilities::squadt::radio_button_helper < transformation_strategy > strategy_selector(d);

  layout::vertical_box& m = d.create< vertical_box >();

  m.append(d.create< label >().set_text("Output format : ")).
    append(d.create< horizontal_box >().
                append(format_selector.associate(of_binary, "binary")).
                append(format_selector.associate(of_internal, "internal")).
                append(format_selector.associate(of_cwi, "cwi")),
          margins(0,5,0,5)).
    append(d.create< label >().set_text("Transformation strategy : ")).
    append(strategy_selector.associate(ts_lazy, "lazy: only boolean equations reachable from the initial state")).
    append(strategy_selector.associate(ts_finite, "finite: all possible boolean equations"));

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
  if (c.output_exists(pbes_file_for_output)) {
    tipi::configuration::object& output_file = c.get_output(pbes_file_for_output);

    output_file.location(c.get_output_name(".pbes"));
  }
  else {
    c.add_output(pbes_file_for_output, tipi::mime_type("pbes", tipi::mime_type::application), c.get_output_name(".pbes"));
  }

  c.add_option(option_transformation_strategy).set_argument_value< 0 >(strategy_selector.get_selection());
  c.add_option(option_selected_output_format).set_argument_value< 0 >(format_selector.get_selection());

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

  pbes2bes_tool tool;
  tool.set_input_filename(c.get_input(pbes_file_for_input).location());
  tool.set_output_filename(c.get_output(pbes_file_for_output).location());
  tool.set_output_format(formats[c.get_option_argument< size_t >(option_selected_output_format)]);
  tool.set_transformation_strategy(strategies[c.get_option_argument< size_t >(option_transformation_strategy)]);
  bool result = tool.run();

  send_clear_display();

  return result;
}
#endif

//Main Program
//------------
/// \brief Main program for pbes2bes
int main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  try {
#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv))
    {
      return EXIT_SUCCESS;
    }
#endif
    pbes2bes_tool tool;

    return tool.execute(argc, argv);
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_FAILURE;
}
