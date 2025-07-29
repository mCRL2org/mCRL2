// Author(s): Jeroen Keiren, Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sumelm.h
/// \brief Provides an implementation of the sum elimination lemma,
///        as well as the removal of unused summation variables.
///        The sum elimination lemma is the following:
///          sum d:D . d == e -> X(d) = X(e).
///        Removal of unused summation variables is according to the
///        following lemma:
///          d not in x implies sum d:D . x = x

#ifndef MCRL2_LPS_SUMELM_H
#define MCRL2_LPS_SUMELM_H

#include "mcrl2/data/equality_one_point_substitution.h"
#include "mcrl2/data/find_equalities.h"
#include "mcrl2/lps/decluster.h"

namespace mcrl2::lps
{

/// \brief Class implementing the sum elimination lemma.
template <typename Specification = specification>
class sumelm_algorithm: public detail::lps_algorithm<Specification>
{
  using super = typename detail::lps_algorithm<Specification>;
  using super::m_spec;

  protected:
    /// Stores the number of summation variables that has been removed.
    std::size_t m_removed = 0;

    /// Whether to decluster disjunctive conditions first.
    bool m_decluster;

  public:
    /// \brief Constructor.
    /// \param spec The specification to which sum elimination should be
    ///             applied.
    /// \param decluster Control whether disjunctive conditions need to be split
    ///        into multiple summands.
    sumelm_algorithm(Specification& spec, bool decluster = false)
        : lps::detail::lps_algorithm<Specification>(spec),

          m_decluster(decluster)
    {}

    /// \brief Apply the sum elimination lemma to all summands in the
    ///        specification.
    void run()
    {
      if (m_decluster)
      {
        // First decluster specification
        decluster_algorithm<Specification>(m_spec).run();
      }

      m_removed = 0; // Re-initialise number of removed variables for a fresh run.

      for (action_summand& s: m_spec.process().action_summands())
      {
        (*this)(s);
      }

      for (deadlock_summand& s: m_spec.process().deadlock_summands())
      {
        (*this)(s);
      }

      mCRL2log(log::verbose) << "Removed " << m_removed << " summation variables" << std::endl;
    }

    /// \brief Apply the sum elimination lemma to summand s.
    /// \param s an action_summand.
    template <class Summand>
    void operator()(Summand& s)
    {
      std::map<data::variable, std::set<data::data_expression> > equalities = data::find_equalities(s.condition());
      auto [sigma,remaining_variables] = data::make_one_point_rule_substitution(equalities, s.summation_variables());

      std::size_t original_num_vars = s.summation_variables().size();
      if (remaining_variables.size() != original_num_vars)
      {
        // A substitution was found
        // Temporarily remove sumvars, otherwise the capture avoiding substitution will modify them
        s.summation_variables() = data::variable_list();
        lps::replace_variables_capture_avoiding(s, sigma);
        s.summation_variables() = data::variable_list(remaining_variables.begin(), remaining_variables.end());
      }

      super::summand_remove_unused_summand_variables(s);
      m_removed += original_num_vars - s.summation_variables().size();
    }

    /// \brief Returns the amount of removed summation variables.
    std::size_t removed() const
    {
      return m_removed;
    }
};

/// \brief Apply the sum elimination lemma to summand s.
/// \param s an action summand
/// \return \c true if any summation variables have been removed, or \c false otherwise.
inline
bool sumelm(action_summand& s)
{
  specification spec;
  sumelm_algorithm<specification> algorithm(spec);
  algorithm(s);
  return algorithm.removed() > 0;
}

/// \brief Apply the sum elimination lemma to summand s.
/// \param s a stochastic action summand
/// \return \c true if any summation variables have been removed, or \c false otherwise.
inline
bool sumelm(stochastic_action_summand& s)
{
  stochastic_specification spec;
  sumelm_algorithm<stochastic_specification> algorithm(spec);
  algorithm(s);
  return algorithm.removed() > 0;
}

/// \brief Apply the sum elimination lemma to summand s.
/// \param s a deadlock summand
/// \return \c true if any summation variables have been removed, or \c false otherwise.
inline
bool sumelm(deadlock_summand& s)
{
  specification spec;
  sumelm_algorithm<specification> algorithm(spec);
  algorithm(s);
  return algorithm.removed() > 0;
}

} // namespace mcrl2::lps


#endif // MCRL2_LPS_SUMELM_H

