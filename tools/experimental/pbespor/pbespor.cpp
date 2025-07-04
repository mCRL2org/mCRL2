// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbespor.cpp

// #define MCRL2_PBES_CONSTELM_DEBUG
// #define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/partial_order_reduction.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/pbes_rewriter_tool.h"
#include "mcrl2/pbes/pbes_input_tool.h"
#include "mcrl2/pbes/pbes_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using pbes_system::tools::pbes_input_tool;
using pbes_system::tools::pbes_output_tool;
using pbes_system::tools::pbes_rewriter_tool;
using data::tools::rewriter_tool;

namespace mcrl2::pbes_system
{

struct pbespor_pbes_composer
{
  struct equation_info
  {
    propositional_variable_instantiation X;
    bool is_conjunctive = false;
    std::size_t rank = 0;
    fixpoint_symbol symbol;
    std::vector<pbes_expression> expressions;

    equation_info() = default;
    equation_info(const equation_info&) = default;
    equation_info(equation_info&&) = default;
    equation_info& operator=(const equation_info&) = default;
    equation_info& operator=(equation_info&&) = default;

    explicit equation_info(propositional_variable_instantiation X_, bool is_conjunctive_ = false, std::size_t rank_ = 0, fixpoint_symbol symbol_ = fixpoint_symbol::mu())
      : X(std::move(X_)), is_conjunctive(is_conjunctive_), rank(rank_), symbol(symbol_)
    {}

    void add(const pbes_expression& x)
    {
      expressions.push_back(x);
    }

    pbes_equation make_equation() const
    {
      pbes_expression rhs = is_conjunctive ? join_and(expressions.begin(), expressions.end()) : join_or(expressions.begin(), expressions.end());
      return pbes_equation(symbol, propositional_variable(X.name(), data::variable_list()), rhs);
    }
  };

  std::map<propositional_variable_instantiation, equation_info> equations;

  propositional_variable_instantiation make_variable(const propositional_variable_instantiation& X) const
  {
    std::ostringstream out;
    out << std::string(X.name());
    for (const data::data_expression& param: X.parameters())
    {
      out << "_" << data::pp(param);
    }
    return propositional_variable_instantiation(core::identifier_string(out.str()), data::data_expression_list());
  };

  void add_equation(const propositional_variable_instantiation& X, bool is_conjunctive, std::size_t rank, const fixpoint_symbol& symbol)
  {
    auto i = equations.find(X);
    if (i == equations.end())
    {
      equations.insert(std::make_pair(X, equation_info(make_variable(X), is_conjunctive, rank, symbol)));
    }
    else
    {
      equation_info& eqn = i->second;
      eqn.rank = rank;
      eqn.is_conjunctive = is_conjunctive;
      eqn.symbol = symbol;
    }
  };

  void add_expression(const propositional_variable_instantiation& X, const propositional_variable_instantiation& Y)
  {
    auto i_X = equations.find(X);
    if (i_X == equations.end())
    {
      i_X = equations.insert(std::make_pair(X, equation_info(make_variable(X)))).first;
    }
    auto i_Y = equations.find(Y);
    if (i_Y == equations.end())
    {
      i_Y = equations.insert(std::make_pair(Y, equation_info(make_variable(Y)))).first;
    }
    i_X->second.add(i_Y->second.X);
  }

  pbes compose_result(const data::data_specification& dataspec, const propositional_variable_instantiation& initial_state) const
  {
    pbes result;
    result.data() = dataspec;
    result.initial_state() = make_variable(initial_state);

    std::vector<equation_info> E;
    for (const auto& q: equations)
    {
      E.push_back(q.second);
    }
    std::sort(E.begin(), E.end(), [](const equation_info& x, const equation_info& y) { return x.rank < y.rank; });
    for (const equation_info& eqn: E)
    {
      result.equations().push_back(eqn.make_equation());
    }

    assert(result.is_well_typed());
    return result;
  }

  pbes run(const pbes& p,
           pbespor_options options
         )
  {
    partial_order_reduction_algorithm algorithm(p, options);

    auto emit_node = [&](const propositional_variable_instantiation& X, bool is_conjunctive, std::size_t rank)
    {
      mCRL2log(log::debug) << "emit node " << X << std::endl;
      add_equation(X, is_conjunctive, rank, algorithm.symbol(X.name()));
    };

    auto emit_edge = [&](const propositional_variable_instantiation& X, const propositional_variable_instantiation& Y)
    {
      mCRL2log(log::debug) << "emit edge " << X << " -> " << Y << std::endl;
      add_expression(X, Y);
    };

    if(options.reduction)
    {
      algorithm.print();
      algorithm.explore(algorithm.initial_state(), emit_node, emit_edge);
    }
    else
    {
      algorithm.explore_full(algorithm.initial_state(), emit_node, emit_edge);
    }

    return compose_result(p.data(), algorithm.initial_state());
  }
};

} // namespace mcrl2::pbes_system

class pbespor_tool: public pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool>>>>
{
  protected:
    using super = pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool>>>>;

    pbespor_options m_options;

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      m_options.use_condition_L = !parser.has_option("no-l");
      m_options.compute_weak_conditions = parser.has_option("weak-conditions");
      m_options.compute_determinism = !parser.has_option("no-determinism");
      m_options.compute_triangle_accordance = parser.has_option("triangle");
      m_options.compute_left_accordance = parser.has_option("left");
      m_options.compute_NES = parser.has_option("nes");
      m_options.reduction = !parser.has_option("full");
      if(parser.has_option("use-smt-solver"))
      {
        m_options.use_smt_solver = true;
        m_options.smt_timeout = std::chrono::milliseconds{parser.option_argument_as<std::size_t>("use-smt-solver")};
      }
    }

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("no-l",
                  "do not apply the condition L (might affect correctness)");
      desc.add_option("weak-conditions",
                  "use weak accordance conditions (cheaper, but less exact, static analysis)", 'w');
      desc.add_option("no-determinism",
                  "do not check whether transitions are deterministic (cheaper, but less powerful, static analysis)");
      desc.add_option("triangle",
                  "compute the triangle accordance relation (more expensive, more powerful, static analysis)");
      desc.add_option("left",
                  "compute the left accordance relation (more expensive, more powerful, static analysis)");
      desc.add_option("nes",
                  "compute the necessary enabling relation (more expensive, more powerful, static analysis)");
      desc.add_option("full",
                  "explore the full state space. No static analysis is performed.");
      desc.add_option("use-smt-solver", utilities::make_optional_argument("TIMEOUT", "0"),
                  "Use the SMT solver Z3 (must be in the path). "
                  "The timeout should be given in milliseconds (0 = no timeout). "
                  "Very small values may lead to errors.", 's');
    }

  public:
    pbespor_tool()
      : super(
        "pbespor",
        "Wieger Wesselink; Thomas Neele and Tim Willemse",
        "apply partial order reduction to a PBES",
        "Reads a file containing a PBES, and applies partial order reduction to it. If OUTFILE "
        "is not present, standard output is used. If INFILE is not present, standard input is used."
      )
    {}

    bool run() override
    {
      mCRL2log(verbose) << "pbespor parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;

      m_options.rewrite_strategy = rewrite_strategy();

      pbes p;
      load_pbes(p, input_filename(), pbes_input_format());
      algorithms::normalize(p);
      pbespor_pbes_composer composer;
      pbes result = composer.run(p, m_options);
      save_pbes(result, output_filename(), pbes_output_format());

      return true;
    }

};

int main(int argc, char* argv[])
{
  return pbespor_tool().execute(argc, argv);
}
