// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/data/detail/rewrite/innermost.h"

#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/bool.h"

#include <assert.h>

constexpr bool PrintRewriteSteps = true;
constexpr bool PrintMatchSteps   = true;

using namespace mcrl2::data;
using namespace mcrl2::data::detail;

using namespace mcrl2::log;

InnermostRewriter::InnermostRewriter(const data_specification& data_spec, const used_data_equation_selector& selector)
  : Rewriter(data_spec, selector)
{
  // This probably belongs inside the Rewriter class, checks for all the selected equations whether they are valid rewrite rules.
  for (const data_equation& equation : data_spec.equations())
  {
    if (selector(equation))
    {
      try
      {
        CheckRewriteRule(equation);
      }
      catch (std::runtime_error& e)
      {
        mCRL2log(warning) << e.what() << std::endl;
        continue;
      }

      if (equation.condition() != sort_bool::true_())
      {
        mCRL2log(warning) << "Conditions are not supported, so rule " << equation << " has been removed.\n";
      }
      else
      {
        m_rewrite_system.emplace_back(equation);
      }
    }
    else
    {
      mCRL2log(info) << "Rule " << equation << " ignored by selector.\n";
    }
  }
}

data_expression InnermostRewriter::rewrite(const data_expression& term, substitution_type& sigma)
{
  return rewrite_impl(term, sigma);
}

// Private functions

data_expression InnermostRewriter::rewrite_impl(const data_expression& term, substitution_type& sigma)
{
  // If t in variables
  if (is_variable(term))
  {
    const auto& var = static_cast<const data::variable&>(term);
    return sigma(var);
  }
  // Else if t in function_symbols
  else if (is_function_symbol(term))
  {
    return term;
  }
  // Else if t is of the form lambda x . u
  else if (is_abstraction(term))
  {
    const auto& abstraction = static_cast<const data::abstraction&>(term);
    return rewrite_abstraction(abstraction, sigma);
  }
  // Else (t is of the form h(u_1, ..., u_n)).
  else
  {
    assert(is_application(term));
    const auto& appl = static_cast<const data::application&>(term);
    return rewrite_application(appl, sigma);
  }
}

data_expression InnermostRewriter::rewrite_abstraction(const abstraction& abstraction, substitution_type& sigma)
{
  // u' := rewrite(u, sigma[x := y]) where y are fresh variables.
  data::variable_list new_variables;
  for (const auto& var : abstraction.variables())
  {
    const variable fresh_variable(m_generator(), var.sort());
    new_variables.push_front(fresh_variable);
    sigma[var] = fresh_variable;
  }

  data_expression body_rewritten = rewrite_impl(abstraction.body(), sigma);

  // Return lambda y . u'
  auto result = data::abstraction(abstraction.binding_operator(), new_variables, body_rewritten);

  if (PrintRewriteSteps)
  {
    mCRL2log(info) << "Applied alpha-reduction to" << abstraction << " resulting in " << result << "\n";
  }

  return result;
}

data_expression InnermostRewriter::rewrite_application(const application& appl, substitution_type& sigma)
{
  // h' := rewrite(h, sigma, V)
  auto head_rewritten = rewrite_impl(appl.head(), sigma);

  // For i in {1, ..., n} do u' := rewrite(u, sigma)
  std::vector<data_expression> arguments;
  for (auto& argument : appl)
  {
    arguments.emplace_back(rewrite_impl(argument, sigma));
  }

  // If h' is of the form lambda x . u
  if (is_abstraction(head_rewritten))
  {
    const auto& abstraction = static_cast<const data::abstraction&>(head_rewritten);

    // sigma := sigma[x gets u']
    for (auto& variable : abstraction.variables())
    {
      assert(false);
      sigma[variable] = arguments[0];
    }

    data_expression result = rewrite_impl(abstraction.body(), sigma);

    if (PrintRewriteSteps)
    {
      mCRL2log(info) << "Applied beta-reduction to" << appl << " resulting in " << result << "\n";
    }

    return result;
  }
  else
  {
    application appl = application(head_rewritten, arguments.begin(), arguments.end());

    // (R, sigma') := match(h'(u_1', ..., u_n'))
    substitution_type match_sigma;
    data_expression rhs;

    // If R not empty
    if (match(appl, rhs, match_sigma))
    {
      // Return rewrite(r, sigma')
      return rewrite_impl(rhs, match_sigma);
    }
    else
    {
      // Return h'(u_1', ..., u_n')
      return appl;
    }
  }

}

bool InnermostRewriter::match_lhs(const data_expression& term,  const data_expression& lhs, substitution_type& sigma)
{
  if (is_function_symbol(lhs))
  {
    return term == lhs;
  }
  else if (is_variable(lhs))
  {
    const auto& var = static_cast<const variable&>(lhs);

    if (sigma.defined(var))
    {
      // If the variable was already assigned they must match.
      return sigma(var) == term;
    }
    else
    {
      // Else substitute the given term for the current variable.
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

bool InnermostRewriter::match(const data_expression& term, data_expression& rhs, substitution_type& sigma)
{
  // Searches for a left-hand side and a substitution such that when the substitution is applied to this left-hand side it is (syntactically) equivalent
  // to the given term.
  for (auto& equation : m_rewrite_system)
  {
    if (match_lhs(term, equation.lhs(), sigma))
    {
      // Only consider trivial conditions
      assert(equation.condition() == sort_bool::true_());

      if(PrintMatchSteps)
      {
        mCRL2log(info) << "Matched rule " << equation << " with term " << term << "\n";
      }

      // The right-hand side and substitution are a valid match.
      rhs = equation.rhs();
      return true;
    }
  }

  return false;
}
