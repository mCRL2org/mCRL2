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
 * \brief This file contains some utility functions to manipulate lts's
 * \details The function in this file typically allow to present the
 *          transitions of a state space differently.
 * \author Jan Friso Groote
 */

#ifndef __LIBLTS_UTILITIES_H
#define __LIBLTS_UTILITIES_H

// #include <string>
#include <map>
#include <set>
#include "mcrl2/lts/lts.h"

namespace mcrl2
{

namespace lts
{
  /// \brief Type for exploring transitions per state. 
  typedef std::multimap < transition::size_type, std::pair < transition::size_type, transition::size_type > >
                               outgoing_transitions_per_state_t;
  
  /// \brief From state of an iterator exploring transitions per outgoing state.
  inline unsigned int from(const outgoing_transitions_per_state_t::const_iterator &i)
  { return i->first;
  }

  /// \brief Label of an iterator exploring transitions per outgoing state.
  inline unsigned int label(const outgoing_transitions_per_state_t::const_iterator &i)
  { return i->second.first;
  }

  /// \brief To state of an iterator exploring transitions per outgoing state.
  inline unsigned int to(const outgoing_transitions_per_state_t::const_iterator &i)
  { return i->second.second;
  }
  
  /// \brief Provide the transitions as a multimap accessible per outgoing state, useful
  ///        for for instance state space exploration.
  inline outgoing_transitions_per_state_t
               transitions_per_outgoing_state(transition_const_range r)
  { outgoing_transitions_per_state_t result;
    for( ; !r.empty() ; r.advance_begin(1))
    { 
      const transition t=r.front();
      result.insert(std::pair< transition::size_type, std::pair < transition::size_type, transition::size_type > >
                (t.from(), std::pair < transition::size_type, transition::size_type >(t.label(),t.to())));
    }
    return result;
  }

  /// \brief Type for exploring transitions per state and action. 
  typedef std::multimap < std::pair < transition::size_type, transition::size_type >, transition::size_type >
                               outgoing_transitions_per_state_action_t;

  /// \brief From state of an iterator exploring transitions per outgoing state and action.
  inline unsigned int from(const outgoing_transitions_per_state_action_t::const_iterator &i)
  { return i->first.first;
  }

  /// \brief Label of an iterator exploring transitions per outgoing state and action.
  inline unsigned int label(const outgoing_transitions_per_state_action_t::const_iterator &i)
  { return i->first.second;
  }

  /// \brief To state of an iterator exploring transitions per outgoing state and action.
  inline unsigned int to(const outgoing_transitions_per_state_action_t::const_iterator &i)
  { return i->second;
  }
  
  /// \brief Provide the transitions as a multimap accessible per from state and label.
  inline outgoing_transitions_per_state_action_t
       transitions_per_outgoing_state_action_pair(transition_const_range r)
  { outgoing_transitions_per_state_action_t result;
    for( ; !r.empty() ; r.advance_begin(1))
    {
      const transition t=r.front();
      result.insert(std::pair < std::pair < transition::size_type, transition::size_type >, transition::size_type > 
                (std::pair < transition::size_type, transition::size_type >(t.from(), t.label()),t.to()));
    }
    return result;
  }
}
}

#endif
