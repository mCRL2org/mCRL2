// Author(s): Jan Friso Groote. Based on pbessolve.cpp by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pressolve.cpp

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pres/pres_input_tool.h"
#include "mcrl2/pres/detail/pres_io.h"
#include "mcrl2/pres/pressolve_options.h"
#include "mcrl2/pres/normalize.h"
#include "mcrl2/pres/detail/instantiate_global_variables.h"
#include "mcrl2/pres/pres2res.h"
#include "mcrl2/pres/ressolve_gauss_elimination.h"
#include "mcrl2/pres/ressolve_numerical.h"
#include "mcrl2/pres/ressolve_numerical_directed.h"

using namespace mcrl2;
using namespace mcrl2::pres_system;
using namespace mcrl2::utilities::tools;
using mcrl2::pres_system::tools::pres_input_tool;
using mcrl2::data::tools::rewriter_tool;
using mcrl2::utilities::tools::input_tool;

// TODO: put this code in the utilities library?
inline std::string file_extension(const std::string& filename)
{
  std::size_t pos = filename.find_last_of('.');
  if (pos == std::string::npos)
  {
    return "";
  }
  return filename.substr(pos + 1);
}

class pressolve_tool
    : public rewriter_tool<pres_input_tool<input_tool>>
{
  protected:
    using super = rewriter_tool<pres_input_tool<input_tool>>;

    pressolve_options options;
    std::string lpsfile;

    void add_options(utilities::interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_hidden_option("no-remove-unused-rewrite-rules", "do not remove unused rewrite rules. ", 'u');
      desc.add_hidden_option("no-replace-constants-by-variables",
          "Do not move constant expressions to a substitution.");
      desc.add_option("algorithm",
          utilities::make_enum_argument<pres_system::solution_algorithm>("NAME")
              .add_value_short(pres_system::solution_algorithm::gauss_elimination, "g", true)
              .add_value_short(pres_system::solution_algorithm::numerical, "n")
              .add_value_short(pres_system::solution_algorithm::numerical_directed, "m"),
          "select the algorithm NAME to solve the res after it is generated.",
          'a');
      desc.add_option("precision",
          utilities::make_mandatory_argument("NUM"),
          "provide an answer within precision 10^-precision. [AS IT STANDS THIS IS THE NOW THE DIFFERENCE BETWEEN TWO "
          "ITERATIONS]",
          'p');
  }

  void parse_options(const utilities::command_line_parser& parser) override
  {
    super::parse_options(parser);

    options.replace_constants_by_variables =
        !parser.has_option("no-replace-constants-by-variables");
    options.remove_unused_rewrite_rules =
        !parser.has_option("no-remove-unused-rewrite-rules");
    options.rewrite_strategy = rewrite_strategy();
    

    if (parser.has_option("file"))
    {
      std::string filename = parser.option_argument("file");
      if (file_extension(filename) == "lps")
      {
        lpsfile = filename;
      }
    }

    if (parser.has_option("algorithm"))
    {
      options.algorithm = parse_algorithm(parser.option_argument("algorithm"));
    }

    if (parser.has_option("precision"))
    {
      if (options.algorithm!=pres_system::solution_algorithm::numerical && options.algorithm!=pres_system::solution_algorithm::numerical_directed)
      {
        throw mcrl2::runtime_error("Option --precision (-p) can only be used in combination with --algorithm=numerical or --algorithm=numerical_directed.");
      }   
      try 
      {
        options.precision = std::stol(parser.option_argument("precision"));
      }
      catch (...)
      {
        throw mcrl2::runtime_error("The argument of --precision (-p) is not a valid number.");
      }
      if (options.precision>=static_cast<std::size_t>(-std::numeric_limits<double>::min_exponent))
      {
        throw mcrl2::runtime_error("Precision " + std::to_string(options.precision) + " is too large. ");
      }
    }
  }

  std::set<utilities::file_format> available_input_formats() const override
  {
    return {pres_system::pres_format_internal()};
  }

  data::rewriter construct_rewriter(const pres& presspec)
  {
    std::set<data::function_symbol> used_functions = pres_system::find_function_symbols(presspec);
    used_functions.insert(data::less(data::sort_real::real_()));
    used_functions.insert(data::sort_real::divides(data::sort_real::real_(),data::sort_real::real_()));
    used_functions.insert(data::sort_real::times(data::sort_real::real_(),data::sort_real::real_()));
    used_functions.insert(data::sort_real::plus(data::sort_real::real_(),data::sort_real::real_()));
    used_functions.insert(data::sort_real::minus(data::sort_real::real_(),data::sort_real::real_()));
    used_functions.insert(data::sort_real::minimum(data::sort_real::real_(),data::sort_real::real_()));
    used_functions.insert(data::sort_real::maximum(data::sort_real::real_(),data::sort_real::real_()));

    return data::rewriter(presspec.data(),
                          data::used_data_equation_selector(presspec.data(), used_functions, presspec.global_variables(), !options.remove_unused_rewrite_rules),
                          options.rewrite_strategy);
  }


  public:
  pressolve_tool(const std::string& toolname)
      : super(toolname, 
              "Jan Friso Groote",
              "Generate a BES from a PRES and solve it. ",
              "Solves (P)BES from INFILE. "
              "If INFILE is not present, stdin is used. "
              "The PRES is first instantiated into a parity game, "
              "which is then solved using Zielonka's algorithm. "
              "It supports the generation of a witness or counter "
              "example for the property encoded by the PRES.")
  {
  }

  bool run() override
  {
    pres_system::pres presspec = pres_system::detail::load_pres(input_filename());
    enumerate_quantifiers_rewriter m_R(construct_rewriter(presspec), presspec.data());
 
    // mCRL2log(log::debug) << "INPUT PRES\n" << presspec << "\n";
    data::mutable_map_substitution<> sigma;
    sigma = pres_system::detail::instantiate_global_variables(presspec);
    pres_system::detail::replace_global_variables(presspec, sigma);

    pres_system::normalize(presspec);
    // mCRL2log(log::debug) << "RESULTING PRES\n" << presspec << "\n";
    
    mCRL2log(log::verbose) << "Generating RES..." << std::endl;
    timer().start("instantiation");
    pres2res_algorithm pres2res(options,presspec,m_R);
    pres resulting_res = pres2res.run();
    timer().finish("instantiation");

    // mCRL2log(log::debug) << "RESULTING RES\n" << resulting_res << "\n";

    mCRL2log(log::verbose) << "Solving RES..." << std::endl;
    timer().start("solving");

    if (options.algorithm==gauss_elimination)
    {
      ressolve_by_gauss_elimination_algorithm solver(options, resulting_res);
      pres_expression result = solver.run();
      std::cout << result << std::endl;
    }
    else if (options.algorithm==numerical)
    { 
      ressolve_by_numerical_iteration solver(options, resulting_res);
      double result = solver.run();
      std::cout << std::setprecision(options.precision) << result << std::endl;
    }  
    else if (options.algorithm==numerical_directed)
    { 
      ressolve_by_numerical_iteration_directed solver(options, resulting_res);
      double result = solver.run();
      std::cout << std::setprecision(options.precision) << result << std::endl;
    }  
    timer().finish("solving");
    return true;
  }
};

int main(int argc, char* argv[])
{
  return pressolve_tool("pressolve").execute(argc, argv);
}
