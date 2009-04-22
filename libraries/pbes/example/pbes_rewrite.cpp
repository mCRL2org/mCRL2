// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING

// #define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG
#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG2

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/xpressive/xpressive.hpp>
#include "mcrl2/core/text_utility.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes_parse.h"
#include "mcrl2/pbes/rewriter.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::pbes_system;
namespace po = boost::program_options;

typedef data::data_enumerator<number_postfix_generator> my_enumerator;
typedef simplifying_rewriter<pbes_system::pbes_expression, data::rewriter> my_simplify_rewriter;
typedef enumerate_quantifiers_rewriter<pbes_system::pbes_expression, data::rewriter, my_enumerator> my_enumerate_quantifiers_rewriter;

// Use boost::variant to create a heterogenous container of rewriters.
typedef boost::variant<my_simplify_rewriter,
                       my_enumerate_quantifiers_rewriter
                      > rewriter_variant;

// This class specifies how to call the rewriter that is stored in a rewriter_variant.
class rewriter_visitor: public boost::static_visitor<pbes_expression>
{
  public:
    pbes_expression t_;

    rewriter_visitor(pbes_expression t)
      : t_(t)
    {}

    template <typename Rewriter>
    pbes_expression operator()(Rewriter& r) const
    {
      return r(t_);
    }
};

data::rewriter::strategy data_rewriter_strategy(int i)
{
  switch (i)
  {
    case 0: return data::rewriter::innermost       ;
    case 1: return data::rewriter::innermost_prover;
    case 2: return data::rewriter::jitty           ;
    case 3: return data::rewriter::jitty_prover    ;
  }
  return data::rewriter::jitty_prover;
}

std::string data_rewriter_name(int i)
{
  switch (i)
  {
    case 0: return "[datarewr inner  ]";
    case 1: return "[datarewr inner_p]";
    case 2: return "[datarewr comp   ]";
    case 3: return "[datarewr comp_p ]";
  }
  return "";
}

std::string pbes_rewriter_name(int i)
{
  switch (i)
  {
    case 0: return "[pbesrewr 0]";
    case 1: return "[pbesrewr 1]";
    case 2: return "[pbesrewr 2]";
  }
  return "";
}

std::string smt_solver_name(int i)
{
  switch (i)
  {
    case 0: return "[ario]";
    case 1: return "[cvc ]";
    case 2: return "[cvcf]";
  }
  return "";
}

std::string rewriter_name(int data_rewriter_string, int pbes_rewriter_string, int smt_solver)
{
  return pbes_rewriter_name(pbes_rewriter_string) + data_rewriter_name(data_rewriter_string) + smt_solver_name(smt_solver);
}

/// Extracts the values of a comma-separated list of integers.
std::vector<int> parse_integers(std::string text)
{
  using namespace boost;
  using namespace boost::xpressive;

  std::vector<int> result;
  sregex splitter = sregex::compile(",");

  // the -1 below directs the token iterator to display the parts of
  // the string that did NOT match the regular expression.
  sregex_token_iterator cur( text.begin(), text.end(), splitter, -1 );
  sregex_token_iterator end;

  for( ; cur != end; ++cur )
  {
    try
    {
      result.push_back(lexical_cast<int>(*cur));
    }
    catch(bad_lexical_cast &)
    {
      std::cerr << "Error: could not parse integer value " << *cur << std::endl;
    }
  }
  return result;
}

void run(std::map<std::string, rewriter_variant>& rewriters, const atermpp::vector<pbes_expression>& expressions)
{
  for (std::map<std::string, rewriter_variant>::iterator i = rewriters.begin(); i != rewriters.end(); ++i)
  {
    for (atermpp::vector<pbes_expression>::const_iterator j = expressions.begin(); j != expressions.end(); ++j)
    {
      pbes_expression result = boost::apply_visitor(rewriter_visitor(*j), i->second);
      std::cout << i->first << " " << core::pp(*j) << " -> " << core::pp(result) << std::endl;
    }
  }
}

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  // a map of rewriters that should be applied to the sequence of terms
  std::map<std::string, rewriter_variant> rewriters;
  std::vector<int> pbes_rewriter_indices;
  std::vector<int> data_rewriter_indices;
  std::vector<int> smt_solver_indices;

  std::string infile;
  std::string pbes_rewriter_string;
  std::string data_rewriter_string;
  std::string smt_solver_string;
  pbes<> p;

  try {
    //--- reach options ---------
    boost::program_options::options_description pbes_rewrite_options(
      "Usage: pbes_rewrite [OPTION]... INFILE\n"
      "\n"
      "Reads a file containing pbes expressions, and applies the rewriter to it.\n"
      "\n"
      "The following choices of the pbes rewriter are available:\n"
      "  0 : simplifying_rewriter\n"
      "  1 : enumerate_quantifiers_rewriter\n"
      "\n"
      "The following choices of the data rewriter are available:\n"
      "  0 : innermost\n"
      "  1 : innermost with prover\n"
      "  2 : jitty\n"
      "  3 : jitty with prover\n"
      "\n"
      // "The following choices of the SMT solver are available:\n"
      // "  0 : ario\n"
      // "  1 : cvc\n"
      // "  2 : cvc fast\n"
      // "\n"
      "Options"
    );
    pbes_rewrite_options.add_options()
      ("help,h", "display this help")
      ("pbes-rewriter,p", po::value<std::string> (&pbes_rewriter_string)->default_value("0"), "pbes rewriter types, comma-separated list")
      ("data-rewriter,d", po::value<std::string> (&data_rewriter_string)->default_value("0"), "data rewriter types, comma-separated list")
      //("smt-solver,s",    po::value<std::string> (&smt_solver_string)   ->default_value("0"), "SMT solver types, comma-separated list")
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

    // parse the input file
    std::string text = core::read_text(infile);
    std::pair<atermpp::vector<pbes_expression>, data_specification> parse_result = parse_pbes_expressions(text);
    const atermpp::vector<pbes_expression>& expressions = parse_result.first;
    const data_specification& data_spec = parse_result.second;

    // parse the program options
    pbes_rewriter_indices = parse_integers(pbes_rewriter_string);
    data_rewriter_indices = parse_integers(data_rewriter_string);
    smt_solver_indices    = parse_integers(smt_solver_string);
    if (pbes_rewriter_indices.empty() || data_rewriter_indices.empty() || smt_solver_indices.empty())
    {
      std::cerr << "Warning: no pbes rewriters specified!" << std::endl;
      return 0;
    }

    // identifier generator for data enumerator
    number_postfix_generator name_generator;

    // store the data rewriters
    std::vector<data::rewriter> data_rewriters;

    // store the corresponding data enumerators
    std::vector<my_enumerator> data_enumerators;

    // create a mapping of rewriters
    for (std::vector<int>::iterator i = data_rewriter_indices.begin(); i != data_rewriter_indices.end(); ++i)
    {
      // Make sure the references to data rewriters stay valid after exiting the loop.
      data_rewriters.push_back(data::rewriter(data_spec, data_rewriter_strategy(*i)));
      data::rewriter& datar = data_rewriters.back();
      data_enumerators.push_back(my_enumerator(data_spec, data_rewriters.back(), name_generator));
      my_enumerator& datae = data_enumerators.back();

      for (std::vector<int>::iterator j = pbes_rewriter_indices.begin(); j != pbes_rewriter_indices.end(); ++j)
      {
        switch (*j)
        {
          case 0: {
            my_simplify_rewriter pbesr(datar);
            rewriters.insert(std::make_pair(rewriter_name(*i, *j, -1), pbesr));
            break;
          }
          case 1: {
            my_enumerate_quantifiers_rewriter pbesr(datar, datae);
            rewriters.insert(std::make_pair(rewriter_name(*i, *j, -1), pbesr));
            break;
          }
        }
      }
    }

    // apply the rewriters to the sequence of expressions
    run(rewriters, expressions);
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
