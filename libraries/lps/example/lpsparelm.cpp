// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsparelm.cpp
/// \brief Add your file description here.

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <boost/program_options.hpp>
#include "mcrl2/core/messaging.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/parelm.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2::lps;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  std::string infile;            // location of lps
  std::string outfile;           // location of result

  try {
    //--- lpsparelm options ---------
    boost::program_options::options_description lpsparelm_options(
      "Usage: lpsparelm [OPTION]... INFILE OUTFILE\n"
      "\n"
      "Reads a file containing a lps, and applies parameter elimination to it.\n"
      "\n"
      "Options"
    );
    lpsparelm_options.add_options()
      ("help,h", "display this help")
      ("verbose,v", "display short intermediate messages")
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
    cmdline_options.add(lpsparelm_options).add(hidden_options);

    po::variables_map var_map;
    po::store(po::command_line_parser(argc, argv).
        options(cmdline_options).positional(positional_options).run(), var_map);
    po::notify(var_map);

    if (var_map.count("help")) {
      std::cout << lpsparelm_options << "\n";
      return 1;
    }
    if (var_map.count("verbose")) {
      mcrl2::core::gsSetVerboseMsg();
    }

    std::cout << "lpsparelm parameters:" << std::endl;
    std::cout << "  input file:         " << infile << std::endl;
    std::cout << "  output file:        " << outfile << std::endl;

    specification spec;
    spec.load(infile);
    specification result = parelm2(spec);
    result.save(outfile);
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
