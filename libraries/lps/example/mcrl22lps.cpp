// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl22lps.cpp
/// \brief Add your file description here.

//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
//#define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG
//#define MCRL2_PBES2BES_REWRITER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG2

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <boost/program_options.hpp>
#include "mcrl2/core/text_utility.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/process.h"
#include "mcrl2/lps/detail/linear_process_conversion_visitor.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::lps;
namespace po = boost::program_options;

inline
specification convert_linear_process_specification(const process_specification& pspec)
{
  detail::linear_process_conversion_visitor visitor;
  specification result;
  try
  {
    result = visitor.convert(pspec);
  }
  catch(detail::linear_process_conversion_visitor::unsupported_linear_process)
  {
    std::cerr << "Specification contains unsupported linear process expressions" << std::endl;
  }
  catch(detail::linear_process_conversion_visitor::non_linear_process)
  {
    std::cerr << "Specification contains non-linear process expressions" << std::endl;
  }
  return result;
}

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  std::string infile;  // location of pbes
  std::string outfile; // location of result
  bool no_linearization;

  try {
    //---mcrl22lps options ---------
    boost::program_options::options_description mcrl22lps_options(
      "Usage:mcrl22lps [OPTION]... INFILE OUTFILE\n"
      "\n"
      "Reads a file containing a pbes, and applies the mcrl22lps algorithm to it.\n"
      "\n"
      "Options"
    );
   mcrl22lps_options.add_options()
      ("help,h", "display this help")
      ("verbose,v", "display short intermediate messages")
      ("debug,d", "display detailed intermediate messages")
      ("no-linearization,l", po::value<bool>(&no_linearization)->default_value(false), "do not linearize if already linear")
      ;

    //--- hidden options ---------
    po::options_description hidden_options;
    hidden_options.add_options()
      ("input-file", po::value<std::string>(&infile), "input file")
      ("output-file", po::value<std::string>(&outfile), "output file")
    ;

    //--- positional options ---------
    po::positional_options_description positional_options;
    positional_options.add("input-file", 1);
    positional_options.add("output-file", 1);

    //--- command line options ---------
    po::options_description cmdline_options;
    cmdline_options.add(mcrl22lps_options).add(hidden_options);

    po::variables_map var_map;
    po::store(po::command_line_parser(argc, argv).
        options(cmdline_options).positional(positional_options).run(), var_map);
    po::notify(var_map);

    if (var_map.count("help")) {
      std::cout <<mcrl22lps_options << "\n";
      return 1;
    }
    if (var_map.count("debug")) {
      mcrl2::core::gsSetDebugMsg();
    }
    if (var_map.count("verbose")) {
      mcrl2::core::gsSetVerboseMsg();
    }

    if (mcrl2::core::gsVerbose)
    {
      std::cerr << "mcrl22lps parameters:" << std::endl;
      std::cerr << "  input file:         " << infile << std::endl;
      std::cerr << "  output file:        " << outfile << std::endl;
      std::cerr << "  no_linearization:   " << no_linearization << std::endl;
    }

    specification spec;
    std::string text = core::read_text(infile);
    if (no_linearization)
    {
      process_specification pspec = parse_process_specification(text);
      spec = convert_linear_process_specification(pspec);
    }
    else
    {
      spec = linearise(text);
    }
    spec.save(outfile);
  }
  catch(mcrl2::runtime_error e)
  {
    std::cerr << "runtime error: " << e.what() << std::endl;
    std::exit(1);
  }
  catch(std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
  catch(...) {
    std::cerr << "exception of unknown type!\n";
  }

  return 0;
}
