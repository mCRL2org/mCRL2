// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_PBES_DETAIL_PBES_REMOVE_COUNTEREXAMPLE_INFO_H
#define MCRL2_PBES_DETAIL_PBES_REMOVE_COUNTEREXAMPLE_INFO_H

#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_equation.h"
#include "mcrl2/pbes/builder.h"

#include <regex>
 
namespace mcrl2::pbes_system::detail
{  

struct subsitute_counterexample: public pbes_expression_builder<subsitute_counterexample>
{
  typedef pbes_expression_builder<subsitute_counterexample> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;
    
  std::regex positive;
  std::regex negative;

  subsitute_counterexample()
    : positive("Zpos_(\\d+)_.*"),
      negative("Zneg_(\\d+)_.*")
  {}

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    std::smatch match;
    if (std::regex_match(static_cast<const std::string&>(x.name()), match, positive))
    {
      result = true_();
    }
    else if (std::regex_match(static_cast<const std::string&>(x.name()), match, negative))
    {
      result = false_();
    }
    else
    {
      result = x;
    }
  }
};

/// Removes all equations and expressions related to counter examples from the input PBES.
inline 
mcrl2::pbes_system::pbes remove_counterexample_info(const pbes_system::pbes& pbes)
{
  // Remove all equations related to the countexamples
  std::vector<mcrl2::pbes_system::pbes_equation> equations;
  subsitute_counterexample f;

   std::regex re("Z(neg|pos)_(\\d+)_.*");

  /// Replace the corresponding PBES variables by true and false respectively.
  for (auto& equation : pbes.equations())
  {
    std::smatch match;
    if (!std::regex_match(static_cast<const std::string&>(equation.variable().name()), match, re))
    {
      pbes_expression expression;
      f.apply(expression, equation.formula());
      std::cerr << expression;

      /// TODO: Apply the rewriter to simplify the expressions.
      equations.emplace_back(equation.symbol(), equation.variable(), expression);
    }
  }

  return mcrl2::pbes_system::pbes(pbes.data(), equations, pbes.initial_state());
}

} // mcrl2::pbes_system::detail

#endif // MCRL2_PBES_DETAIL_PBES_REMOVE_COUNTEREXAMPLE_INFO_H