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

#include <stdexcept>
#include <iostream>
#include <string>
#include <utility>

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/pbes/detail/pbes2bes_variable_map_parser.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbes2bes.h"
#include "mcrl2/pbes/pbes2bes_algorithm.h"
#include "mcrl2/pbes/pbes2bes_finite_algorithm.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using utilities::command_line_parser;
using utilities::interface_description;
using utilities::make_optional_argument;
using utilities::tools::input_output_tool;
using utilities::tools::rewriter_tool;

/// The pbes2bes tool.
class pbes2bes_tool: public rewriter_tool<input_output_tool> 
{
  protected:
    typedef rewriter_tool<input_output_tool> super;

    /// The transformation strategies of the tool.
    enum transformation_strategy {
      ts_lazy,
      ts_finite
    };

    transformation_strategy m_strategy;
    pbes_output_format m_output_format;
    std::string m_finite_parameter_selection;

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

      if (parser.options.count("select") > 0)
      {
      	m_finite_parameter_selection = parser.option_argument("select");
      	boost::trim(m_finite_parameter_selection);
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
          'o').
        add_option("select",
          make_optional_argument("NAME", ""),
          "select finite parameters that need to be expanded\n"
          "  Examples: X1(b:Bool,c:Bool);X2(b:Bool)\n"
          "            *(*:Bool)\n",
          'f');
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
        pbes2bes_variable_map variable_map = detail::parse_variable_map(p, m_finite_parameter_selection);
        algorithm.run(p, variable_map);
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
};

//Main Program
//------------
/// \brief Main program for pbes2bes
int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return pbes2bes_tool().execute(argc, argv);
}
