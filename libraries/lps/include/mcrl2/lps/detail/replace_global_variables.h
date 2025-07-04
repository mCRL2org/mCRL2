// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/replace_global_variables.h
/// \brief

#ifndef MCRL2_LPS_DETAIL_REPLACE_GLOBAL_VARIABLES_H
#define MCRL2_LPS_DETAIL_REPLACE_GLOBAL_VARIABLES_H


#include "mcrl2/data/assignment.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/lps/action_summand.h"
#include "mcrl2/lps/builder.h"
#include "mcrl2/lps/is_stochastic.h"
#include <cassert>

namespace mcrl2::lps::detail
{

/// An LPS builder that traverses all the instantiations in the LPS.
class assignment_builder: public lps::data_expression_builder<assignment_builder>
{
public:
  using super = lps::data_expression_builder<assignment_builder>;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  assignment_builder(const std::set<data::variable>& global_variables)
      : m_global_variables(global_variables)
  {}

  void apply(data::assignment_list& result, const data::assignment_list& assignments)
  {
    std::vector<data::assignment> result_assignments;

    for (auto&& assignment : assignments)
    {
      if (!data::is_variable(assignment.rhs())
          || m_global_variables.count(atermpp::down_cast<data::variable>(assignment.rhs())) == 0)
      {
        // Otherwise, keep the global variable assignment
        result_assignments.emplace_back(assignment);
      }
    }

    result = data::assignment_list(result_assignments.begin(), result_assignments.end());
    }

private:
    /// \brief The global variables of the LPS.
    const std::set<data::variable>& m_global_variables;
};
    
/// \brief Replaces assignments of global variables to parameters in the LPS by an assignment of itself.
///
/// For example in the LPS P(x: D) = P(x := dc), where dc is a global variable of sort D, we replace P(x := dc) by P(x := x). This is potentially useful
/// for symbolic exploration, where we want to avoid explicit writes at the cost of a potentially larger state space.
template <typename Specification>
void replace_global_variables(Specification& lpsspec)
{
    if (is_stochastic(lpsspec))
    {
        throw mcrl2::runtime_error("replace_global_variables is not defined for stochastic LPS specifications.");
    }

    mCRL2log(log::verbose) << "Replacing global variables assignments in LPS specification." << std::endl;

    assignment_builder builder(lpsspec.global_variables());
    builder.update(lpsspec.process());       
}



} // namespace mcrl2::lps::detail



#endif // MCRL2_LPS_DETAIL_REPLACE_GLOBAL_VARIABLES_H