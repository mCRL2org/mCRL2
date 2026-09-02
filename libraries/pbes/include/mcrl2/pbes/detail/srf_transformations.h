// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/srf_transformations.h
/// \brief Transformations on PBESs in standard recursive form.

#ifndef MCRL2_PBES_DETAIL_SRF_TRANSFORMATIONS_H
#define MCRL2_PBES_DETAIL_SRF_TRANSFORMATIONS_H

#include "mcrl2/data/join.h"
#include "mcrl2/pbes/srf_pbes.h"

namespace mcrl2::pbes_system
{

/// \brief Splits SRF summand conditions to reduce their dependencies.
template <bool allow_ce>
inline detail::pre_srf_pbes<allow_ce> split_conditions(const detail::pre_srf_pbes<allow_ce>& pbes,
  std::size_t granularity)
{
  mCRL2log(log::debug) << "splitting conditions" << std::endl;

  data::set_identifier_generator id_generator;
  for (const detail::pre_srf_equation<allow_ce>& equation : pbes.equations())
  {
    id_generator.add_identifier(equation.variable().name());
  }

  const propositional_variable Xtrue = pbes.equations()[pbes.equations().size() - 2].variable();
  const propositional_variable Xfalse = pbes.equations()[pbes.equations().size() - 1].variable();

  detail::pre_srf_pbes<allow_ce> result = pbes;
  std::vector<detail::pre_srf_equation<allow_ce>> added_equations;
  for (detail::pre_srf_equation<allow_ce>& equation : result.equations())
  {
    std::vector<detail::pre_srf_summand<allow_ce>> split_summands;
    for (const detail::pre_srf_summand<allow_ce>& summand : equation.summands())
    {
      const bool should_split = summand.parameters().empty() && granularity > 1;
      if constexpr (allow_ce)
      {
        if (is_universal_or(summand.condition()))
        {
          for (const pbes_expression& clause : split_or(summand.condition(), true))
          {
            split_summands.emplace_back(summand.parameters(), clause, summand.variable());
          }
        }
        else if (should_split && is_and(summand.condition()))
        {
          const bool simple = granularity == 3 || summand.variable().name() == Xtrue.name()
                              || summand.variable().name() == Xfalse.name();
          std::vector<detail::pre_srf_summand<allow_ce>> split_summands_inner;
          for (const pbes_expression& clause : split_and(summand.condition()))
          {
            if (simple)
            {
              split_summands_inner.emplace_back(data::variable_list(),
                not_(clause),
                !equation.is_conjunctive() ? propositional_variable_instantiation(Xtrue.name(), {})
                                           : propositional_variable_instantiation(Xfalse.name(), {}));
            }
            else
            {
              const propositional_variable& Y = equation.variable();
              const propositional_variable Y1(id_generator(Y.name()), Y.parameters());
              split_summands_inner.emplace_back(data::variable_list(), true_(), propositional_variable_instantiation(Y1.name(), data::make_data_expression_list(Y1.parameters())));
              added_equations.emplace_back(equation.symbol(), Y1,
                std::vector<detail::pre_srf_summand<allow_ce>>{detail::pre_srf_summand<allow_ce>(data::variable_list(), clause, summand.variable())},
                !equation.is_conjunctive());
            }
          }

          if (simple)
          {
            split_summands_inner.emplace_back(data::variable_list(), true_(), summand.variable());
          }

          if (equation.summands().size() == 1)
          {
            split_summands = split_summands_inner;
            equation.is_conjunctive() = !equation.is_conjunctive();
          }
          else
          {
            const propositional_variable& Y = equation.variable();
            const propositional_variable Y1(id_generator(Y.name()), Y.parameters());
            split_summands.emplace_back(data::variable_list(), true_(), propositional_variable_instantiation(Y1.name(), data::make_data_expression_list(Y1.parameters())));
            added_equations.emplace_back(equation.symbol(), Y1, split_summands_inner, !equation.is_conjunctive());
          }
        }
        else
        {
          split_summands.emplace_back(summand);
        }
      }
      else if (data::sort_bool::is_or_application(summand.condition()))
      {
        for (const data::data_expression& clause : data::split_or(summand.condition()))
        {
          split_summands.emplace_back(summand.parameters(), atermpp::down_cast<pbes_expression>(clause), summand.variable());
        }
      }
      else
      {
        split_summands.emplace_back(summand);
      }
    }
    equation.summands() = split_summands;
  }

  result.equations().insert(result.equations().end() - 2, added_equations.begin(), added_equations.end());
  return result;
}

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_DETAIL_SRF_TRANSFORMATIONS_H
