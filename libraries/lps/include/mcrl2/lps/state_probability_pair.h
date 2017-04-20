// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file
 *
 * \brief This file contains a class with a state/probability pair. 
 * \details A state probability pair consists of a state and a probability.
 *          The probability should be larger than 0 and smaller or equal to 1.
 * \author Jan Friso Groote
 */


#ifndef MCRL2_LPS_STATE_PROBABILITY_PAIR_H
#define MCRL2_LPS_STATE_PROBABILITY_PAIR_H

#include <iostream>
#include <cassert>
#include "mcrl2/utilities/hash_utility.h"

namespace mcrl2
{
namespace lps
{

template < class STATE, class PROBABILITY >
class state_probability_pair
{
  protected:
    // A state 
    STATE m_state;
    // A data expression of sort Real that indicates the probability of this state
    PROBABILITY m_probability;
  public:

    /// \brief constructor.
    /// \param state The state.
    /// \param probability The probability of the occurrence of this state. 
    state_probability_pair(const STATE& state, const PROBABILITY& probability)
     : m_state(state), 
       m_probability(probability)
    {
      assert(probability!=PROBABILITY::zero());
    } 

    /// \brief Standard assignment.
    /// \param p The state probability pair to be assigned.
    /// \return A reference to the assigned object.
    state_probability_pair& operator=(const state_probability_pair& p)=default; 

    /** \brief Standard equality operator.
     *  \result Returns true iff the probabilistic states are equal.
     */
    bool operator==(const state_probability_pair& other) const
    {
      return m_state==other.m_state && m_probability==other.m_probability;
    }

    /// \brief Get the state from a state probability pair.
    const STATE& state() const
    {
      return m_state;
    }

    /// \brief Get the state in a state probability pair.
    STATE& state() 
    {
      return m_state;
    }

    /// \brief get the probability from a state proability pair.
    const PROBABILITY& probability() const
    {
      return m_probability;
    }

    /// \brief Set the probability in a state probability pair.
    PROBABILITY& probability() 
    {
      return m_probability;
    }

};

} // namespace lps
} // namespace mcrl2 

namespace std
{

template < class STATE, class PROBABILITY >
struct hash<mcrl2::lps::state_probability_pair<STATE,PROBABILITY> >
{
  std::size_t operator()(const mcrl2::lps::state_probability_pair<STATE,PROBABILITY>& p) const
  {
    hash<STATE> state_hasher;
    hash<PROBABILITY> probability_hasher;
    return mcrl2::utilities::detail::hash_combine(state_hasher(p.state()), probability_hasher(p.probability()));
  }
};

} // end namespace std

#endif // MCRL2_LPS_STATE_PROBABILITY_PAIR_H
