// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/data/detail/match/naive_matcher.h"

#include "mcrl2/data/detail/rewrite/jitty_jittyc.h"
#include "mcrl2/data/detail/match/linearise.h"

/// \brief Print the intermediate matches that succeeded.
constexpr bool PrintMatchSteps   = false;

/// \brief When enabled, index each rewrite rule based on its head symbol for fast lookup of relevant rules.
constexpr bool EnableHeadIndexing = true;

/// \brief When enabled, enable consistency checking on the fly.
constexpr bool EnableOnTheFlyConsistencyCheck = true;

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;

using namespace mcrl2::log;

/// \brief Matches a single left-hand side with the given term and creates the substitution.
template<typename Substitution>
inline bool match_lhs(const data_expression& term,  const data_expression& lhs, Substitution& sigma)
{
  if (is_function_symbol(lhs))
  {
    return term == lhs;
  }
  else if (is_variable(lhs))
  {
    const auto& var = static_cast<const variable&>(lhs);

    if (EnableOnTheFlyConsistencyCheck && sigma(var) != var)
    {
      // If the variable was already assigned it must match the previously assigned value.
      return sigma(var) == term;
    }
    else
    {
      // Else substitute the current term for this variable.
      sigma[var] = term;
      return true;
    }
  }
  else
  {
    // The term and lhs are applications.
    const application& lhs_appl  = static_cast<const application&>(lhs);
    const application& term_appl = static_cast<const application&>(term);

    // Both must have the same arity, the head symbol must match and their arguments must match.
    if (lhs_appl.size() != term_appl.size())
    {
      return false;
    }

    if (!match_lhs(term_appl.head(), lhs_appl.head(), sigma))
    {
      return false;
    }

    for (std::size_t i = 0; i < term_appl.size(); i++)
    {
      if (!match_lhs(term_appl[i], lhs_appl[i], sigma))
      {
        return false;
      }
    }

    return true;
  }
}

/// \returns A unique index for the head symbol that the given term starts with.
inline std::size_t get_head_index(const data_expression& term)
{
  return atermpp::detail::index_traits<data::function_symbol, function_symbol_key_type, 2>::index(static_cast<const function_symbol&>(get_nested_head(term)));
}

template<typename Substitution>
NaiveMatcher<Substitution>::NaiveMatcher(const data_equation_vector& equations)
{
  // Preprocess the term rewrite system.
  for (const data_equation& original_equation : equations)
  {
    // Only rename the variables when the consistency check if performed afterwards.
    if (!EnableOnTheFlyConsistencyCheck)
    {
      // Rename the variables in the equation
      auto [equation, partition] = make_linear(original_equation);

      // Add the index of the equation
      m_equations.emplace_back(linear_data_equation(original_equation, equation, partition));
    }
    else
    {
      m_equations.emplace_back(linear_data_equation(original_equation, original_equation, {}));
    }
  }

  for (const data_equation& original_equation : equations)
  {
    // Make sure that it is possible to insert the match data for head_index left-hand side.
    std::size_t head_index = get_head_index(original_equation.lhs());
    if (head_index >= m_rewrite_system.size()) { m_rewrite_system.resize(head_index + 1); }

    // Insert the left-hand side into the rewrite rule mapping and a construction stack for its right-hand side.

    // Only rename the variables when the consistency check if performed afterwards.
    if (!EnableOnTheFlyConsistencyCheck)
    {
      // Rename the variables in the equation
      auto [equation, partition] = make_linear(original_equation);

      // Add the index of the equation
      m_rewrite_system[head_index].emplace_back(linear_data_equation(original_equation, equation, partition));
    }
    else
    {
      m_rewrite_system[head_index].emplace_back(linear_data_equation(original_equation, original_equation, {}));
    }
  }

  mCRL2log(debug) << "NaiveMatcher (EnableHeadIndexing = " << EnableHeadIndexing << ", EnableOnTheFlyConsistencyCheck = " << EnableOnTheFlyConsistencyCheck << ")\n";
}

template<typename Substitution>
typename NaiveMatcher<Substitution>::const_iterator NaiveMatcher<Substitution>::match(const data_expression& term, Substitution& matching_sigma) const
{
  std::size_t head_index = 0;
  if (EnableHeadIndexing)
  {
    head_index = get_head_index(term);
  }

  return const_iterator(*this, term, head_index, matching_sigma);
}

template<typename Substitution>
const extended_data_equation* NaiveMatcher<Substitution>::next(const data_expression& term,
  std::size_t head_index,
  std::size_t& index,
  Substitution& matching_sigma) const
{
  if (EnableHeadIndexing && head_index >= m_rewrite_system.size())
  {
    // No left-hand side starts with this head symbol, so it cannot match.
    return nullptr;
  }

  // Only tries rewrite rules that start with the correct head symbol when EnableHeadIndexing is true.
  while (index < (EnableHeadIndexing ? m_rewrite_system[head_index].size() : m_equations.size()))
  {
    matching_sigma.clear();

    const linear_data_equation& equation = (EnableHeadIndexing ? m_rewrite_system[head_index][index] : m_equations[index]);

    // Compute a matching substitution for each rule and check that the condition associated with that rule is true, either trivially or by rewrite(c^sigma, identity).
    if (match_lhs(term, equation.equation().lhs(), matching_sigma) && (EnableOnTheFlyConsistencyCheck || is_consistent(equation, matching_sigma)))
    {
      if (PrintMatchSteps)
      {
        mCRL2log(info) << "Matched rule " << equation.original_equation() << " to term " << term << "\n";
      }

      ++index;
      return &equation;
    }
    else if (PrintMatchSteps)
    {
      mCRL2log(info) << "Tried rule " << equation.original_equation() << " on term " << term << "\n";
    }

    ++index;
  }

  return nullptr;
}

// Explicit instantiations.

#include "mcrl2/data/substitutions/sequence_substitution.h"

template class mcrl2::data::detail::NaiveMatcher<sequence_substitution>;
template class mcrl2::data::detail::NaiveMatcher<mutable_map_substitution<>>;
