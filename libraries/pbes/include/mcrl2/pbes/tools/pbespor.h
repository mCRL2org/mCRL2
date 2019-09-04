// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbespor.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TOOLS_PBESPOR_H
#define MCRL2_PBES_TOOLS_PBESPOR_H

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/partial_order_reduction.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/join.h"
#include "mcrl2/utilities/logger.h"

#include <utility>

namespace mcrl2 {

namespace pbes_system {

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
    auto i = equations.find(X);
    if (i == equations.end())
    {
      i = equations.insert(std::make_pair(X, equation_info(make_variable(X)))).first;
    }
    i->second.add(make_variable(Y));
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

  pbes run(const pbes& p, data::rewrite_strategy rewrite_strategy, bool use_condition_L)
  {
    partial_order_reduction_algorithm algorithm(p, rewrite_strategy);
    algorithm.print();

    algorithm.explore(
      algorithm.initial_state(),

      // emit_node
      [&](const propositional_variable_instantiation& X, bool is_conjunctive, std::size_t rank)
      {
        mCRL2log(log::verbose) << "emit node " << X << std::endl;
        add_equation(X, is_conjunctive, rank, algorithm.symbol(X.name()));
      },

      // emit_edge
      [&](const propositional_variable_instantiation& X, const propositional_variable_instantiation& Y)
      {
        mCRL2log(log::verbose) << "emit edge " << X << " -> " << Y << std::endl;
        add_expression(X, Y);
      },

      use_condition_L
    );

    return compose_result(p.data(), algorithm.initial_state());
  }
};

void pbespor(const std::string& input_filename,
             const std::string& output_filename,
             const utilities::file_format& input_format,
             const utilities::file_format& output_format,
             data::rewrite_strategy rewrite_strategy,
             bool use_condition_L
            )
{
  pbes p;
  load_pbes(p, input_filename, input_format);
  algorithms::normalize(p);
  pbespor_pbes_composer composer;
  pbes result = composer.run(p, rewrite_strategy, use_condition_L);
  save_pbes(result, output_filename, output_format);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_PBESPOR_H
