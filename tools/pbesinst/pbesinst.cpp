// Author(s): Alexander van Dam, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesinst.cpp
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
#include "mcrl2/pbes/detail/pbes_parameter_map.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/is_bes.h"
#include "mcrl2/pbes/pbesinst.h"
#include "mcrl2/pbes/pbesinst_algorithm.h"
#include "mcrl2/pbes/pbesinst_finite_algorithm.h"
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

/// The pbesinst tool.
class pbesinst_tool: public rewriter_tool<input_output_tool>
{
  protected:
    typedef rewriter_tool<input_output_tool> super;

    /// The transformation strategies of the tool.
    enum transformation_strategy
    {
      ts_lazy,
      ts_finite
    };

    transformation_strategy m_strategy;
    pbes_output_format m_output_format;
    std::string m_finite_parameter_selection;
    bool m_aterm_ascii;

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
      else if (format == "bes")
      {
        m_output_format = pbes_output_bes;
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
        if (parser.options.count("output") == 0)
        {
          // Use the filename extension to determine the output format
          if (boost::ends_with(m_output_filename, std::string(".bes")))
          {
            set_output_format("bes");
          }
          else if (boost::ends_with(m_output_filename, std::string(".cwi")))
          {
            set_output_format("cwi");
          }
          else
          {
            set_output_format("pbes");
          }
        }
        else
        {
          set_output_format(parser.option_argument("output"));
        }
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
      m_aterm_ascii = parser.options.count("aterm-ascii") > 0;
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
                 "  'pbes' for the mCRL2 PBES format (default),\n"
                 "  'bes'  for the mCRL2 BES format\n"
                 "  'cwi'  for the CWI BES format",
                 'o').
      add_option("select",
                 make_optional_argument("NAME", ""),
                 "select finite parameters that need to be expanded\n"
                 "  Examples: X1(b:Bool,c:Bool);X2(b:Bool)\n"
                 "            *(*:Bool)\n",
                 'f');
      desc.add_option("aterm-ascii", "store ATerms in ascii format (default: false)", 'a');
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
      else if (m_output_format == pbes_output_bes)
      {
        return "bes";
      }
      else if (m_output_format == pbes_output_cwi)
      {
        return "cwi";
      }
      return "unknown";
    }

  public:
    /// Constructor.
    pbesinst_tool()
      : super(
        "pbesinst",
        "Wieger Wesselink; Alexander van Dam and Tim Willemse",
        "compute a BES out of a PBES",
        "Transforms the PBES from INFILE into an equivalent BES and writes it to OUTFILE. "
        "If INFILE is not present, standard input is used. If OUTFILE is not present,   "
        "standard output is used."
        "The format of OUTFILE is determined by its extension (unless it is specified "
        "by an option). The supported formats are:\n"
        "  'pbes' for the mCRL2 PBES format,\n"
        "  'bes'  for the mCRL2 BES format,\n"
        "  'cwi'  for the CWI BES format\n"
      ),
      m_strategy(ts_lazy),
      m_output_format(pbes_output_pbes),
      m_aterm_ascii(false)
    {}

    /// Runs the algorithm.
    bool run()
    {
      using namespace mcrl2::pbes_system;

      mCRL2log(verbose) << "parameters of pbesinst:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  strategy:           " << strategy_string() << std::endl;
      mCRL2log(verbose) << "  output format:      " << output_format_string() << std::endl;

      // load the pbes
      pbes<> p;
      p.load(m_input_filename);

      if (!p.is_closed())
      {
        core::gsErrorMsg("The PBES is not closed. Pbes2bes cannot handle this kind of PBES's\nComputation aborted.\n");
        return false;
      }

      unsigned int log_level = 0;
      if (mCRL2logEnabled(verbose))
      {
        log_level = 1;
      }
      if (mCRL2logEnabled(debug))
      {
        log_level = 2;
      }

      if (m_strategy == ts_lazy)
      {
        pbesinst_algorithm algorithm(p.data(), rewrite_strategy(), false, false, log_level);
        algorithm.run(p);
        p = algorithm.get_result();
      }
      else if (m_strategy == ts_finite)
      {
        pbesinst_finite_algorithm algorithm(rewrite_strategy());
        detail::pbes_parameter_map parameter_map = detail::parse_pbes_parameter_map(p, m_finite_parameter_selection);
        algorithm.run(p, parameter_map);
      }

      if (mcrl2_logger::get_reporting_level() >= log_verbose)
      {
        if (is_bes(p))
        {
          mCRL2log(debug) << "The result is a BES.\n";
        }
        else
        {
           mCRL2log(debug) << "The result is a PBES.\n";
        }
      }

      // save the result
      save_pbes(p, m_output_filename, m_output_format, m_aterm_ascii);

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
/// \brief Main program for pbesinst
int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return pbesinst_tool().execute(argc, argv);
}
