// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \filepbes2bes.cpp
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
#include "mcrl2/core/messaging.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes2bes_algorithm.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2::pbes_system;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  std::string infile;            // location of pbes
  std::string outfile;           // location of result
  bool print_equations;          // print the generated equations to standard out
  bool print_rewriter_output;    // print rewriter output

  try {
    //---pbes2bes options ---------
    boost::program_options::options_description pbes2bes_options(
      "Usage:pbes2bes [OPTION]... INFILE OUTFILE\n"
      "\n"
      "Reads a file containing a pbes, and applies the pbes2bes algorithm to it.\n"
      "\n"
      "Options"
    );
   pbes2bes_options.add_options()
      ("help,h", "display this help")
      ("verbose,v", "display short intermediate messages")
      ("debug,d", "display detailed intermediate messages")
      ("print-equations,e", po::value<bool>(&print_equations)->default_value(false), "print generated equations")
      ("print-rewriter,r", po::value<bool>(&print_rewriter_output)->default_value(false), "print rewriter output")
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
    cmdline_options.add(pbes2bes_options).add(hidden_options);

    po::variables_map var_map;
    po::store(po::command_line_parser(argc, argv).
        options(cmdline_options).positional(positional_options).run(), var_map);
    po::notify(var_map);

    if (var_map.count("help")) {
      std::cout <<pbes2bes_options << "\n";
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
      std::cout << "pbes2bes parameters:" << std::endl;
      std::cout << "  input file:         " << infile << std::endl;
      std::cout << "  output file:        " << outfile << std::endl;
    }

    pbes<> p;
    p.load(infile);
    pbes2bes_algorithm algorithm(p.data(), mcrl2::data::rewriter::jitty, print_equations, print_rewriter_output);
    algorithm.run(p);
    p = algorithm.get_result();
    p.save(outfile);
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
