#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include "mcrl2/core/text_utility.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/parser.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/rewriter2.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::pbes_system;
namespace po = boost::program_options;

template <typename PbesRewriter>
void run(const std::vector<pbes_expression>& expressions, PbesRewriter& r)
{
  for (std::vector<pbes_expression>::const_iterator i = expressions.begin(); i != expressions.end(); ++i)
  {
    std::cout << pp(*i) << " -> " << pp(r(*i)) << std::endl;
  }
}

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  std::string infile;
  int pbes_rewriter;
  int data_rewriter;
  int smt_solver_type;
  bool use_prover;
  pbes<> p;

  try {
    //--- reach options ---------
    boost::program_options::options_description pbes_rewrite_options(
      "Usage: pbes_rewrite [OPTION]... INFILE\n"
      "\n"
      "Reads a file containing pbes expressions, and applies the rewriter to it.\n"
      "\n"
      "The following choices of the pbes rewriter are available:\n"
      "  0 : enumerating rewriter (Wieger)\n"
      "  1 : enumerating rewriter (Jan Friso)\n"
      "  2 : enumerating rewriter (Jan Friso, current version)\n"
      "  3 : simplifying rewriter, uses a prover (Simona)\n"
      "\n"
      "The following choices of the data rewriter are available:\n"
      "  0 : innermost\n"
      "  1 : jitty    \n"
      "\n"
      "The following choices of the SMT solver are available:\n"
      "  0 : ario\n"
      "  1 : cvc\n"
      "  2 : cvc fast\n"
      "\n"
      "Options"
    );
    pbes_rewrite_options.add_options()
      ("help,h", "display this help")
      ("pbes-rewriter,p", po::value<int> (&pbes_rewriter)->default_value(0), "pbes rewriter type")
      ("data-rewriter,d", po::value<int> (&data_rewriter)->default_value(0), "data rewriter type")
      ("smt-solver,s", po::value<int> (&smt_solver_type)->default_value(0), "SMT solver type")
      ("use-prover,u", po::value<bool>   (&use_prover)->default_value(false), "use the eq-bdd prover")
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
    cmdline_options.add(pbes_rewrite_options).add(hidden_options);

    po::variables_map var_map;
    po::store(po::command_line_parser(argc, argv).
        options(cmdline_options).positional(positional_options).run(), var_map);
    po::notify(var_map);

    if (var_map.count("help")) {
      std::cout << pbes_rewrite_options << "\n";
      return 1;
    }

    // read the input file
    std::string text = core::read_text(infile);

    // parse the file
    std::pair<std::vector<pbes_expression>, data_specification> parse_result = parse_pbes_expressions(text);
    const std::vector<pbes_expression>& expressions = parse_result.first;
    const data_specification& data_spec = parse_result.second;

    data::rewriter::strategy s;
    if (data_rewriter == 0)
    {
      s = use_prover ? data::rewriter::innermost_prover : data::rewriter::innermost;
    }
    else
    {
      s = use_prover ? data::rewriter::jitty_prover : data::rewriter::jitty;
    }
    data::rewriter datar(data_spec, s);

    if (pbes_rewriter == 0)    
    {
      pbes_system::rewriter<data::rewriter> pbesr(datar, data_spec);
      run(expressions, pbesr);
    }
    else if (pbes_rewriter == 1)
    {
      substitute_rewriter pbesr(datar, data_spec);
      run(expressions, pbesr);
    }
    else if (pbes_rewriter == 2)
    {
      substitute_rewriter_jfg pbesr(datar, data_spec);
      run(expressions, pbesr);
    }
    else if (pbes_rewriter == 3)
    {
      SMT_Solver_Type solver;
      switch (smt_solver_type)
      {
        case 0:  solver = solver_type_ario; break;
        case 1:  solver = solver_type_cvc; break;
        case 2:  solver = solver_type_cvc_fast; break;
        default: solver = solver_type_ario; break;
      }
      RewriteStrategy rewrite_strategy;
      if (data_rewriter == 0)
      {
        rewrite_strategy = use_prover ? GS_REWR_INNER_P : GS_REWR_INNER;
      }
      else
      {
        rewrite_strategy = use_prover ? GS_REWR_JITTY_P : GS_REWR_JITTY;
      }
      pbessolve_rewriter pbesr(datar, data_spec, rewrite_strategy, solver);
      run(expressions, pbesr);
    }
  }
  catch(std::runtime_error e)
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
