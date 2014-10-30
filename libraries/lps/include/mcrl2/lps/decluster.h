// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file decluster.h
/// \brief Split summands with disjuncts as conditions

#ifndef MCRL2_LPS_DECLUSTER_H
#define MCRL2_LPS_DECLUSTER_H

#include "mcrl2/utilities/logger.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/data/join.h"

namespace mcrl2
{
namespace lps
{

class decluster_algorithm: public detail::lps_algorithm<>
{
  protected:

    template <typename SummandType, typename OutIter>
    void decluster_summand(const SummandType& summand, OutIter& out)
    {
      std::set<data::data_expression> disjuncts = data::split_or(summand.condition());
      for(std::set<data::data_expression>::const_iterator i = disjuncts.begin(); i != disjuncts.end(); ++i)
      {
        SummandType s(summand);
        s.condition() = *i;
        *out++ = s;
      }
    }

  public:
    decluster_algorithm(specification& spec)
      : detail::lps_algorithm(spec)
    {}

    void run()
    {
      action_summand_vector action_summands;
      action_summand_vector declustered_action_summands;
      std::back_insert_iterator<action_summand_vector> act_it (declustered_action_summands);
      for (action_summand_vector::iterator i = m_spec.process().action_summands().begin(); i != m_spec.process().action_summands().end(); ++i)
      {
        decluster_summand(*i, act_it);
      }

      deadlock_summand_vector deadlock_summands;
      deadlock_summand_vector declustered_deadlock_summands;
      std::back_insert_iterator<deadlock_summand_vector> dl_it (declustered_deadlock_summands);
      for (deadlock_summand_vector::iterator i = m_spec.process().deadlock_summands().begin(); i != m_spec.process().deadlock_summands().end(); ++i)
      {
        decluster_summand(*i, dl_it);
      }

      m_spec.process().action_summands() = declustered_action_summands;
      m_spec.process().deadlock_summands() = declustered_deadlock_summands;
    }

}; // decluster_algorithm

} // namespace lps
} // namespace mcrl2

#endif // MCRL2_LPS_DECLUSTER_H

