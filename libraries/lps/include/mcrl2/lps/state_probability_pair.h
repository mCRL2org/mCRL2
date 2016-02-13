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

#include <cassert>

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

    /// \Brief constructor
    state_probability_pair(const STATE& state, const PROBABILITY& probability)
     : m_state(state), 
       m_probability(probability)
    {
      assert(probability!=PROBABILITY::zero());
    } 

    /// \brief Get the state from a state probability pair.
    const STATE& state() const
    {
      return m_state;
    }

    /// \brief Set the state in a state probability pair.
    void set_state(const STATE& state) 
    {
      m_state=state;
    }

    /// \brief get the probability from a state proability pair.
    const PROBABILITY& probability() const
    {
      return m_probability;
    }

    /// \brief Set the probability in a state probability pair.
    void set_state(const PROBABILITY& probability) 
    {
      assert(probability!=PROBABILITY::zero());
      m_probability=probability;
    }

};

} // namespace lps
} // namespace mcrl2 

#endif // MCRL2_LPS_STATE_PROBABILITY_PAIR_H
