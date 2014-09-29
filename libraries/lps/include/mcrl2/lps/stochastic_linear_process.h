// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/stochastic_linear_process.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_STOCHASTIC_LINEAR_PROCESS_H
#define MCRL2_LPS_STOCHASTIC_LINEAR_PROCESS_H

#include "mcrl2/lps/stochastic_action_summand.h"
#include "mcrl2/lps/linear_process.h"

namespace mcrl2 {

namespace lps {

namespace detail {

template <>
inline
stochastic_action_summand make_action_summand<stochastic_action_summand>(const data::variable_list& summation_variables,
                                                                         const data::data_expression& condition,
                                                                         const multi_action& a,
                                                                         const data::assignment_list& assignments,
                                                                         const stochastic_distribution& distribution
                                                                        )
{
  return action_summand(summation_variables, condition, a, assignments, distribution);
}

} // namespace detail

/// \brief linear process.
class stochastic_linear_process: public linear_process_base<stochastic_action_summand>
{
  friend atermpp::aterm_appl linear_process_to_aterm(const stochastic_linear_process& p);

  public:
    /// \brief Constructor.
    stochastic_linear_process()
    { }

    /// \brief Copy constructor.
    stochastic_linear_process(const stochastic_linear_process& other)
      : linear_process_base(other)
    { }

    /// \brief Constructor.
    stochastic_linear_process(const data::variable_list& process_parameters,
                   const deadlock_summand_vector& deadlock_summands,
                   const action_summand_vector& action_summands
                  )
      : linear_process_base(process_parameters, deadlock_summands, action_summands)
    { }
};

//--- start generated class stochastic_linear_process ---//
// prototype declaration
std::string pp(const stochastic_linear_process& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const stochastic_linear_process& x)
{
  return out << lps::pp(x);
}
//--- end generated class stochastic_linear_process ---//

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_STOCHASTIC_LINEAR_PROCESS_H
