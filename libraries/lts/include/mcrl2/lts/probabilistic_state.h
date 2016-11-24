// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file probabilistic_lts.h
 *
 * \brief The file containing the core class for transition systems.
 * \details This is the LTS library's main header file. It declares all publicly
 * accessible data structures that form the interface of the library.
 * \author Jan Friso Groote
 */

#ifndef MCRL2_LTS_PROBABILISTIC_STATE_H
#define MCRL2_LTS_PROBABILISTIC_STATE_H

#include <vector>
#include <sstream>
#include "mcrl2/lps/state_probability_pair.h"


namespace mcrl2
{

namespace lts
{

/** \brief A class that contains a probabilistic state.
    \details A probabilistic state is essentially a sequence
             of pairs of a state and a probability. The probability
             indicates the likelyhood with which that particular 
             state can be reached. The sum of all probabilities
             in a probabilistic state is one.
*/

template < class STATE, class PROBABILITY >
class probabilistic_state
{
  public:
    
    friend std::hash<probabilistic_state>;

    typedef typename lps::state_probability_pair< STATE, PROBABILITY > state_probability_pair;
    typedef STATE state_t;
    typedef PROBABILITY probability_t;
    typedef typename std::vector<state_probability_pair>::iterator iterator;
    typedef typename std::vector<state_probability_pair>::const_iterator const_iterator;

  protected:

    std::vector<state_probability_pair> m_probabilistic_state; // A vector with states and their associated probability.
                                                               // The sum of all probabilities in the vector must be one.

  public:

    /** \brief Default constructor 
     */
    probabilistic_state()
    {
      m_probabilistic_state.reserve(1);
    }

    /** \brief Creates an empty LTS.
     */
    explicit probabilistic_state(const STATE& s)
      : m_probabilistic_state(1,state_probability_pair(s,PROBABILITY::one()))
    {
      shrink_to_fit();
    }

    probabilistic_state(const probabilistic_state& s)
      : m_probabilistic_state(s.m_probabilistic_state)
    {
      shrink_to_fit();
    }

    /** \brief Creates a copy of the supplied LTS.
     * \param[in] l The LTS to copy. */
    template <class STATE_PROBABILITY_PAIR_ITERATOR>
    probabilistic_state(const STATE_PROBABILITY_PAIR_ITERATOR begin, const STATE_PROBABILITY_PAIR_ITERATOR end)
      : m_probabilistic_state(begin,end)
    {
      assert(begin!=end); 
      shrink_to_fit();
    };

    /** \brief Standard equality operator.
     *  \result Returns true iff the probabilistic states are equal.
     */
    bool operator==(const probabilistic_state& other) const
    {
      return m_probabilistic_state==other.m_probabilistic_state;
    }

    /** \brief Swap this lts with the supplied supplied LTS.
     * \param[in] l The LTS to swap. */
    void swap(probabilistic_state& s)
    {
      m_probabilistic_state.swap(s.m_probabilistic_state);
    };

    /** \brief Set the vector to a single state with probability zero.
     * \detail It is assumed that the given state probability pair does not have
               any element. 
     * \param[in] s The state. */
    void set(const STATE& s)
    {
      assert(size()==0);
      m_probabilistic_state.emplace_back(s, PROBABILITY::one());
      shrink_to_fit();
    };

    /** \brief Add a state with a probability to the probabilistic state
     * \param[in] s The state to be added.
     * \param[in] p The probability of this state.
     **/
    void add(const STATE& s, const PROBABILITY& p)
    {
      m_probabilistic_state.emplace_back(s,p);
    }

    /** \brief If a probabilistic state is ready, shrinking it to minimal size might be useful
     *         to reduce its memory usage. A requirement is that the sum of the probabilities must
     *         be one.
     */
    void shrink_to_fit()
    {
      m_probabilistic_state.shrink_to_fit();  
#ifndef NDEBUG
      PROBABILITY sum=PROBABILITY::zero();
      for(const state_probability_pair& p: m_probabilistic_state)
      {
        assert(p.probability()>PROBABILITY::zero());
        assert(p.probability()<=PROBABILITY::one());
        sum=sum+p.probability();
      }
      // SVC studio copies empty probabilistic states. 
      assert(m_probabilistic_state.size()==0 || sum==PROBABILITY::one()); 
#endif
    }

    /** \brief Gets the number of probabilistic labels of this LTS.
     * \return The number of action labels of this LTS. */
    size_t size() const
    {
      return m_probabilistic_state.size();
    }

    /** \brief Makes the probabilistic state empty.
     */
    void clear()
    {
      m_probabilistic_state.clear();
    }

    /** \brief Gets an iterator over pairs of state and probability.
     *  \return The iterator pointing at the first state probability pair. */
    const_iterator begin() const
    {
      return m_probabilistic_state.cbegin();
    }

    /** \brief Gets the end iterator over pairs of state and probability.
     *  \return The iterator pointing beyond the last state probability pair in a probabilistic state. */
    const_iterator end() const
    {
      return m_probabilistic_state.cend();
    }

    /** \brief Gets an iterator over pairs of state and probability.
     *  \return The iterator pointing at the first state probability pair. */
    iterator begin()
    {
      return m_probabilistic_state.begin();
    }

    /** \brief Gets the end iterator over pairs of state and probability.
     *  \return The iterator pointing beyond the last state probability pair in a probabilistic state. */
    iterator end()
    {
      return m_probabilistic_state.end();
    }

};


/* \brief A pretty print operator on action labels, returning it as a string.
 * */
template < class STATE, class PROBABILITY >
inline std::string pp(const probabilistic_state<STATE, PROBABILITY>& l)
{
  std::stringstream str;
  bool first=true;
  for(const lps::state_probability_pair<STATE, PROBABILITY>& p: l)
  {
    if (first)
    {
      first=false;
    }
    else
    {
      str << ", ";
    }
    str << "[" << p.state() << ", " << p.probability() << "]";
  }
  
  return str.str();
}

/** \brief Pretty print to an outstream.
 */
template < class STATE, class PROBABILITY >
inline
std::ostream& operator<<(std::ostream& out, const probabilistic_state<STATE, PROBABILITY>& l)
{
  return out << pp(l);
}



} // namespace lts
} // namespace mcrl2

namespace std
{

/// \brief specialization of the standard std::hash function.
template < class STATE, class PROBABILITY >
struct hash< mcrl2::lts::probabilistic_state<STATE, PROBABILITY> >
{
  std::size_t operator()(const mcrl2::lts::probabilistic_state<STATE, PROBABILITY>& p) const
  {
    hash<vector<typename mcrl2::lps::state_probability_pair< STATE, PROBABILITY > > > hasher;
    return hasher(p.m_probabilistic_state);
  }
};

} // namespace std

#endif // MCRL2_LTS_PROBABILISTIC_STATE_H
