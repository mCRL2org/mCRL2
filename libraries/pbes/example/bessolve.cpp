#include <iostream>
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/bisimulation.h"
#include "mcrl2/pbes/pbes2bes.h"
#include "mcrl2/pbes/bes_algorithms.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::pbes_expr;
namespace po = boost::program_options;

std::string print_type(int type)
{
  switch (type)
  {
    case 0: return "lazy algorithm";
    case 1: return "finite algorithm";
  }
  return "unknown type";
}

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  typedef data::rewriter my_data_rewriter;
  typedef data::data_enumerator<number_postfix_generator> my_enumerator;
  typedef enumerate_quantifiers_rewriter<pbes_system::pbes_expression, data::rewriter, my_enumerator> my_pbes_rewriter;

  std::string infile;
  int type;
  pbes<> p;

  try {
    //--- reach options ---------
    boost::program_options::options_description bessolve_options(
      "Usage: bessolve [OPTION]... INFILE\n"
      "\n"
      "Reads a pbes from INFILE, applies pbes2bes to it, and solves the\n"
      "resulting bes using Gauss elimination\n"
      "\n"
      "Two variants of pbes2bes are supported:\n"
      "  0 : lazy algorithm\n"
      "  1 : finite algorithm\n"
      "\n"
      "Options"
    );
    bessolve_options.add_options()
      ("help,h", "display this help")
      ("variant,v", po::value<int> (&type)->default_value(0), "variant of pbes2bes")
      ;

    //--- hidden options ---------
    po::options_description hidden_options;
    hidden_options.add_options()
      ("input-file", po::value<std::string>(&infile), "input file")
    ;

    //--- positional options ---------
    po::positional_options_description positional_options;
    positional_options.add("input-file", 1);

    //--- command line options ---------
    po::options_description cmdline_options;
    cmdline_options.add(bessolve_options).add(hidden_options);

    po::variables_map var_map;
    po::store(po::command_line_parser(argc, argv).
        options(cmdline_options).positional(positional_options).run(), var_map);
    po::notify(var_map);

    if (var_map.count("help")) {
      std::cout << bessolve_options << "\n";
      return 1;
    }

    std::cout << "bessolve parameters:" << std::endl;
    std::cout << "  input  file      : " << infile << std::endl;
    std::cout << "  pbes2bes variant : " << print_type(type) << std::endl;

    p.load(infile);
    pbes<> q;

    // data rewriter
    my_data_rewriter datar(p.data());

    // identifier generator for data enumerator
    number_postfix_generator name_generator;

    // data enumerator
    my_enumerator datae(p.data(), datar, name_generator);

    // pbes rewriter
    my_pbes_rewriter pbesr(datar, datae);

    switch (type)
    {
      case 0: q = do_lazy_algorithm(p, pbesr); break;
      case 1: q = do_finite_algorithm(p, pbesr); break;
    }
    int result = pbes_gauss_elimination(q);
    if (result == 0)
    {
      std::cout << "FALSE" << std::endl;
    }
    else if (result == 1)
    {
      std::cout << "TRUE" << std::endl;
    }
    else
    {
      std::cout << "UNKNOWN" << std::endl;
    }
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
