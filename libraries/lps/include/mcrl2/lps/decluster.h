// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file decluster.h
/// \brief Split summands with disjuncts as conditions

#ifndef MCRL2_LPS_DECLUSTER_H
#define MCRL2_LPS_DECLUSTER_H

#include "mcrl2/data/join.h"
#include "mcrl2/lps/detail/lps_algorithm.h"

namespace mcrl2::lps
{

template <typename Specification>
class decluster_algorithm: public detail::lps_algorithm<Specification>
{
  using super = typename detail::lps_algorithm<Specification>;
  using process_type = typename Specification::process_type;
  using action_summand_type = typename process_type::action_summand_type;
  using action_summand_vector_type = std::vector<action_summand_type>;
  using super::m_spec;

  protected:

    template <typename SummandType, typename OutIter>
    void decluster_summand(const SummandType& summand, OutIter& out)
    {
      mCRL2log(log::debug) << "Splitting summand condition " << summand.condition() << " into:\n";
      for(const data::data_expression& disjunct: data::split_or_aggressive(summand.condition()))
      {
        mCRL2log(log::debug) << "- " << disjunct << "\n";
        SummandType s(summand);
        s.condition() = disjunct;
        *out++ = s;
      }
    }

  public:
    decluster_algorithm(Specification& spec)
      : detail::lps_algorithm<Specification>(spec)
    {}

    void run()
    {
      action_summand_vector_type declustered_action_summands;
      std::back_insert_iterator<action_summand_vector_type> act_it(declustered_action_summands);
      for (const action_summand& as:  m_spec.process().action_summands())
      {
        decluster_summand(as, act_it);
      }

      deadlock_summand_vector declustered_deadlock_summands;
      std::back_insert_iterator<deadlock_summand_vector> dl_it (declustered_deadlock_summands);
      for (const deadlock_summand& ds: m_spec.process().deadlock_summands())
      {
        decluster_summand(ds, dl_it);
      }

      m_spec.process().action_summands() = declustered_action_summands;
      m_spec.process().deadlock_summands() = declustered_deadlock_summands;
    }

}; // decluster_algorithm

} // namespace mcrl2::lps


#endif // MCRL2_LPS_DECLUSTER_H

