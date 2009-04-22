// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesconstelm.cpp
/// \brief Add your file description here.

#define MCRL2_PBES_CONSTELM_DEBUG
// #define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <boost/program_options.hpp>
#include "mcrl2/core/messaging.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/constelm.h"
#include "mcrl2/pbes/rewriter.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  typedef simplifying_rewriter<pbes_system::pbes_expression, data::rewriter> my_pbes_rewriter;

  std::string infile;            // location of pbes
  std::string outfile;           // location of result
  bool compute_conditions;
  bool remove_redundant_equations;

  try {
    //--- pbesconstelm options ---------
    boost::program_options::options_description pbesconstelm_options(
      "Usage: pbesconstelm [OPTION]... INFILE OUTFILE\n"
      "\n"
      "Reads a file containing a pbes, and applies constant parameter elimination to it.\n"
      "\n"
      "Options"
    );
    pbesconstelm_options.add_options()
      ("help,h", "display this help")
      ("verbose,v", "display short intermediate messages")
      ("debug,d", "display detailed intermediate messages")
      ("compute-conditions,c", po::value<bool>(&compute_conditions)->default_value(false), "compute propagation conditions")
      ("remove-equations,e", po::value<bool>(&remove_redundant_equations)->default_value(false), "remove redundant equations")
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
    cmdline_options.add(pbesconstelm_options).add(hidden_options);

    po::variables_map var_map;
    po::store(po::command_line_parser(argc, argv).
        options(cmdline_options).positional(positional_options).run(), var_map);
    po::notify(var_map);

    if (var_map.count("help")) {
      std::cout << pbesconstelm_options << "\n";
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
      std::cerr << "pbesconstelm parameters:" << std::endl;
      std::cerr << "  input file:         " << infile << std::endl;
      std::cerr << "  output file:        " << outfile << std::endl;
      std::cerr << "  compute conditions: " << std::boolalpha << compute_conditions << std::endl;
      std::cerr << "  remove equations  : " << std::boolalpha << remove_redundant_equations << std::endl;
    }

    // load the pbes
    pbes<> p;
    p.load(infile);

    // data rewriter
    data::rewriter datar(p.data());

    // name generator
    std::string prefix = "UNIQUE_PREFIX"; // unique_prefix(p);
    data::number_postfix_generator name_generator(prefix);

    // pbes rewriter
    my_pbes_rewriter pbesr(datar);

    // constelm algorithm
    pbes_constelm_algorithm<pbes_system::pbes_expression, data::rewriter, my_pbes_rewriter> algorithm(datar, pbesr);

    // run the algorithm
    algorithm.run(p, compute_conditions, remove_redundant_equations);

    // save the result
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
