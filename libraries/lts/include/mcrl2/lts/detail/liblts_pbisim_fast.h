// Author(s): Hector Joao Rivera Verduzco
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/liblts_pbisim.h

#ifndef _LIBLTS_PBISIM_FAST_H
#define _LIBLTS_PBISIM_FAST_H
#include <cmath>
#include <vector>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/detail/liblts_plts_merge.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{
template < class LTS_TYPE>
class prob_bisim_partitioner_fast
{
  public:
    /** \brief Creates a probabilistic bisimulation partitioner for an PLTS.
    *  \details This bisimulation partitioner applies the algorithm described in "J.F. Groote, H.J. Rivera, E.P. de Vink, 
	  *	 An O(mlogn) algorithm for probabilistic bisimulation".
    */
    prob_bisim_partitioner_fast(LTS_TYPE& l)
      : aut(l)
    {
      mCRL2log(log::verbose) << "Probabilistic bisimulation partitioner created for " <<
                                l.num_states() << " states and " <<
                                l.num_transitions() << " transitions\n";
      create_initial_partition();
      refine_partition_until_it_becomes_stable();
    }

    /** \brief Gives the number of bisimulation equivalence classes of the LTS.
    *  \return The number of bisimulation equivalence classes of the LTS.
    */
    size_t num_eq_classes() const
    {
      return action_constellations.size();
    }

    /** \brief Gives the bisimulation equivalence class number of a state.
    *  \param[in] A state number.
    *  \return The number of the bisimulation equivalence class to which the state belongs to. */
    size_t get_eq_class(const size_t s)
    {
      assert(s < action_states.size());
      return action_states[s].parent_block;
    }

    /** \brief Gives the bisimulation equivalence probabilistic class number of a probabilistic state.
    *  \param[in] A probabilistic state number.
    *  \return The number of the probabilistic class to which the state belongs to. */
    size_t get_eq_probabilistic_class(const size_t s)
    {
      assert(s<probabilistic_states.size());
      return probabilistic_states[s].parent_block; // The block index is the state number of the block.
    }

    /** \brief Destroys this partitioner. */
    ~prob_bisim_partitioner_fast()
    {}

    /** \brief Replaces the transition relation of the current lts by the transitions
    *         of the bisimulation reduced transition system.
    * \pre The bisimulation equivalence classes have been computed. */
    void replace_transitions()
    {
      std::set<transition> resulting_transitions;

      const std::vector<transition>& trans = aut.get_transitions();
      for (const transition& t : trans)
      {
        resulting_transitions.insert(
          transition(
            get_eq_class(t.from()),
            t.label(),
            get_eq_probabilistic_class(t.to())));
      }
      // Remove the old transitions
      aut.clear_transitions();

      // Copy the transitions from the set into the transition system.
      for (const transition& t : resulting_transitions)
      {
        aut.add_transition(t);
      }
    }

    /** \brief Replaces the probabilistic states of the current lts by the probabilistic
    *         states of the bisimulation reduced transition system.
    * \pre The bisimulation classes have been computed. */
    void replace_probabilistic_states()
    {
      std::vector<lts_aut_base::probabilistic_state> new_probabilistic_states;

      // get the equivalent probabilistic state of each probabilistic block and add it to aut
      for (probabilistic_block_type& prob_block : probabilistic_blocks)
      {
        lts_aut_base::probabilistic_state equivalent_ps = calculate_equivalent_probabilistic_state(prob_block);
        new_probabilistic_states.push_back(equivalent_ps);
      }

      /* Remove old probabilistic states */
      aut.clear_probabilistic_states();

      // Add new prob states to aut
      for (const lts_aut_base::probabilistic_state& new_ps : new_probabilistic_states)
      {
        aut.add_probabilistic_state(new_ps);
      }

      lts_aut_base::probabilistic_state old_initial_prob_state = aut.initial_probabilistic_state();
      lts_aut_base::probabilistic_state new_initial_prob_state = calculate_new_probabilistic_state(old_initial_prob_state);
      aut.set_initial_probabilistic_state(new_initial_prob_state);
    }

    /** \brief Returns whether two states are in the same probabilistic bisimulation equivalence class.
    *  \param[in] s A state number.
    *  \param[in] t A state number.
    *  \retval true if \e s and \e t are in the same bisimulation equivalence class;
    *  \retval false otherwise. */
    bool in_same_probabilistic_class_fast(const size_t s, const size_t t)
    {
      return get_eq_probabilistic_class(s) == get_eq_probabilistic_class(t);
    }


  protected:

    typedef size_t block_key_type;
    typedef size_t constellation_key_type;
    typedef size_t transition_key_type;
    typedef size_t state_key_type;
    typedef size_t label_type;
    typedef probabilistic_arbitrary_precision_fraction probability_label_type;
    typedef probabilistic_arbitrary_precision_fraction probability_fraction_type;

    template <typename T>
    struct list_type
    {
      T* first;
      T* last;
      size_t size;

      list_type<T>(T* l_first, T* l_last, size_t l_size) : first(l_first), last(l_last), size(l_size) { }

      list_type<T>() : first(NULL), last(NULL), size(0) { }

      void init(T* l_first, T* l_last, const size_t l_size) 
      { 
        first = l_first;
        last = l_last;
        size = l_size;
      }

    };

    struct action_transition_type
    {
      state_key_type from;
      label_type label;
      state_key_type to;
      size_t* block_to_constellation_count_ptr;

      //  double linked list
      action_transition_type* next_ptr;
      action_transition_type* prev_ptr;
    };

    struct probabilistic_transition_type
    {
      state_key_type from;
      probability_label_type label;
      state_key_type to;

      //  linked list support
      probabilistic_transition_type* next_ptr;
      probabilistic_transition_type* prev_ptr;
    };

    struct action_state_type
    {
      block_key_type parent_block;
      std::vector<probabilistic_transition_type*> incoming_transitions;
      bool mark_state;

      // Temporary
      size_t residual_transition_cnt;
      size_t* transition_count_ptr;

      //  linked list
      action_state_type* next_ptr;
      action_state_type* prev_ptr;
    };

    struct probabilistic_state_type
    {
      block_key_type parent_block;
      std::vector<action_transition_type*> incoming_transitions;
      bool mark_state;

      //  linked list
      probabilistic_state_type* next_ptr;
      probabilistic_state_type* prev_ptr;

      // Temporary.
      probability_label_type cumulative_probability;
    };

    struct action_mark_type
    {
      bool block_is_marked;
      list_type<action_state_type> left;
      list_type<action_state_type> middle;
      list_type<action_state_type> right;
      list_type<action_state_type>* large_block_ptr;

      action_mark_type() : block_is_marked(false), large_block_ptr(NULL){}
    };

    struct probabilistic_mark_type
    {
      bool block_is_marked;
      list_type<probabilistic_state_type> left;
      std::vector< list_type<probabilistic_state_type> > middle;
      list_type<probabilistic_state_type> right;
      list_type<probabilistic_state_type>* large_block_ptr;

      // temporary variables used to create middle sets
      probability_label_type max_cumulative_probability;
      probability_label_type bigger_middle_probability;  
      probability_label_type smaller_middle_probability;

      probabilistic_mark_type() : block_is_marked(false), large_block_ptr(NULL) {}
    };

    struct action_block_type
    {
      block_key_type key;
      constellation_key_type parent_constellation;
      list_type<action_state_type> states;
      list_type<probabilistic_transition_type> incoming_probabilistic_transitions;
      action_mark_type mark;

      //  linked list
      action_block_type* next_ptr;
      action_block_type* prev_ptr;
    };

    struct probabilistic_block_type
    {
      block_key_type key;
      constellation_key_type parent_constellation;
      list_type<probabilistic_state_type> states;
      probabilistic_mark_type mark;

      // a probabilistic block has incoming action transitions ordered by label
      std::vector< list_type<action_transition_type> > incoming_action_transitions;
      std::vector<label_type> incoming_labels;

      //  linked list
      probabilistic_block_type* next_ptr;
      probabilistic_block_type* prev_ptr;
    };

    struct action_constellation_type
    {
      constellation_key_type key;
      list_type<action_block_type> blocks;
      size_t number_of_states;    // number of states in constellation

      // linke list
      action_constellation_type* next_ptr;
      action_constellation_type* prev_ptr;
    }; 
    
    struct probabilistic_constellation_type 
    {
      constellation_key_type key;
      list_type<probabilistic_block_type> blocks;
      size_t number_of_states;

      // linke list
      probabilistic_constellation_type* next_ptr;
      probabilistic_constellation_type* prev_ptr;
    };

    struct global_type
    {
      list_type<probabilistic_constellation_type> probabilistic_constellations;
      list_type<action_constellation_type> action_constellations;
    };

    std::vector<action_transition_type> action_transitions;
    std::deque<probabilistic_transition_type> probabilistic_transitions;
    std::vector<action_state_type> action_states;
    std::vector<probabilistic_state_type> probabilistic_states;
    std::deque<action_block_type> action_blocks;
    std::deque<probabilistic_block_type> probabilistic_blocks;
    std::deque<action_constellation_type> action_constellations;
    std::deque<probabilistic_constellation_type> probabilistic_constellations;
    global_type global;
    std::deque<size_t> block_to_constellation_count;

    LTS_TYPE& aut;

    /** \brief Creates the initial partition.
    *  \details The blocks are initially partitioned based on the actions that can perform. 
    */
    void create_initial_partition(void)
    {
      // Preprocessing initialization. First we have to initialise the action/probabilistic states and
      // transitions.
      preprocessing_stage();

      // Action blocks have to be initialized based on the outgoing transitions.
      // First order the transitions per label.
      std::vector< list_type<action_transition_type> > transitions_per_label;
      transitions_per_label.resize(aut.num_action_labels());

      // Add action transitions to its respective list ordered by label
      for (action_transition_type& t : action_transitions)
      {
        if (NULL == transitions_per_label[t.label].first)
        {
          // if the list is empty, add to first and last pointes
          transitions_per_label[t.label].first = &t;
          transitions_per_label[t.label].last = &t;
        }
        else
        {
          // if the list is not empty, connect element to the end of the list
          t.prev_ptr = transitions_per_label[t.label].last;
          transitions_per_label[t.label].last->next_ptr = &t;
        }

        transitions_per_label[t.label].last = &t;
        transitions_per_label[t.label].size++;
      }

      // We start with all the action states in one block and then we refine this block
      // according to the outgoing transitions.
      action_block_type initial_action_block;
      initial_action_block.key = 0;
      initial_action_block.prev_ptr = NULL;
      initial_action_block.next_ptr = NULL;

      // Link all the action states together to the initial block
      action_state_type* prev_action_state_ptr = NULL;
      for (action_state_type& s : action_states)
      {
        s.parent_block = initial_action_block.key;
        if (prev_action_state_ptr != NULL)
        {
          // link previous state with current state
          prev_action_state_ptr->next_ptr = &s;
          s.prev_ptr = prev_action_state_ptr;
        }
        prev_action_state_ptr = &s;
      }

      // Add the linked states to the list of states in the initial block
      initial_action_block.states.init(&action_states.front(), &action_states.back(), aut.num_states());
      action_blocks.push_back(initial_action_block);

      // Refine the intial action block based on the outgoing transitions.
      refine_initial_action_block(transitions_per_label);

      // Initialise the probabilistic block. Initally, there is only one block of probabilistic states.
      probabilistic_block_type initial_probabilistic_block;
      initial_probabilistic_block.key = 0;
      initial_probabilistic_block.parent_constellation = 0;
      initial_probabilistic_block.prev_ptr = NULL;
      initial_probabilistic_block.next_ptr = NULL;

      // Link all the probabilistic states together to the initial block
      probabilistic_state_type* prev_state_ptr = NULL;
      for (probabilistic_state_type& s : probabilistic_states)
      {
        s.parent_block = initial_probabilistic_block.key;
        if (prev_state_ptr != NULL)
        {
          // link previous state with current state
          prev_state_ptr->next_ptr = &s;
          s.prev_ptr = prev_state_ptr;
        }
        prev_state_ptr = &s;
      }

      // Add the states to the list of states in the initial block
      initial_probabilistic_block.states.init(&probabilistic_states.front(), 
        &probabilistic_states.back(), aut.num_probabilistic_states());

      // Since the transitions are already grouped by label, add them to the
      // initial probabilistic block as incoming transitions.
      initial_probabilistic_block.incoming_action_transitions.swap(transitions_per_label);

      // add all labels that are being used to the initial probabilistic block.
      for (list_type<action_transition_type>& t_list : initial_probabilistic_block.incoming_action_transitions)
      {
        if (NULL != t_list.first)
        {
          // The list is not empty, add the label to the incomming labels vector of the block.
          initial_probabilistic_block.incoming_labels.push_back(t_list.first->label);
        }
      }

      probabilistic_blocks.push_back(initial_probabilistic_block);

      // Initialise the probabilistic and action constellations; they will contain
      // all the blocks.
      probabilistic_constellation_type initial_probabilistic_const;
      initial_probabilistic_const.key = 0;
      initial_probabilistic_const.number_of_states = aut.num_probabilistic_states();
      initial_probabilistic_const.prev_ptr = NULL;
      initial_probabilistic_const.next_ptr = NULL;

      // Initially there is only one block in the probabilistic constellation.
      initial_probabilistic_const.blocks.init(&probabilistic_blocks.front(), &probabilistic_blocks.front(), 1);
      probabilistic_constellations.push_back(initial_probabilistic_const);

      // Initialise the initial action constellation.
      action_constellation_type initial_action_const;
      initial_action_const.key = 0;
      initial_action_const.number_of_states = aut.num_states();
      initial_action_const.next_ptr = NULL;
      initial_action_const.prev_ptr = NULL;

      // Construct the list of action blocks by linking them together.
      action_block_type* prev_block_ptr = NULL;
      for (action_block_type& b : action_blocks)
      {
        b.parent_constellation = initial_action_const.key;
        if (prev_block_ptr != NULL)
        {
          // link previous state with current state
          prev_block_ptr->next_ptr = &b;
          b.prev_ptr = prev_block_ptr;
        }
        prev_block_ptr = &b;
      }

      // Add the blocks to the list of blocks in the initial constellation.
      initial_action_const.blocks.init(&action_blocks.front(), &action_blocks.back(), action_blocks.size());
      action_constellations.push_back(initial_action_const);

      // Initialise the incoming probabilistic transitions for all action blocks. To that end,
      // iterate over all probabilistic transitions and add it to its respective destination block.
      for (probabilistic_transition_type& t : probabilistic_transitions)
      {
        action_state_type& s = action_states[t.to];
        action_block_type& block = action_blocks[s.parent_block];

        if (NULL == block.incoming_probabilistic_transitions.first)
        {
          // The list is empty. Add transition to the first an last element.
          block.incoming_probabilistic_transitions.init(&t, &t, 1);
        }
        else
        {
          // add to the back of the list
          block.incoming_probabilistic_transitions.last->next_ptr = &t;
          t.prev_ptr = block.incoming_probabilistic_transitions.last;
          block.incoming_probabilistic_transitions.last = &t;
          block.incoming_probabilistic_transitions.size++;
        }
      }

      // Initialise the block to constellation count for each action transition.
      // Initially there is only one constellation, so we only need to count 
      // the number of outgoing transitions per label of each state.
      std::vector< list_type<action_transition_type> >& transitions_per_label_temp = initial_probabilistic_block.incoming_action_transitions;

      // block_to_const_count_temp is used to keep track of the block to constellation count per label of
      // each state.
      std::vector<size_t> block_to_const_count_temp;
      std::vector<size_t*> new_count_ptr;
      block_to_const_count_temp.resize(aut.num_states());
      new_count_ptr.resize(aut.num_states());

      for (list_type<action_transition_type>& at_list_per_label : transitions_per_label_temp)
      {
        action_transition_type* transition_ptr = at_list_per_label.first;

        // First, iterate over all transition of the current particular label to count
        // the number of times the source state occurs. This is the block to constellation count.
        while (NULL != transition_ptr)
        {
          block_to_const_count_temp[transition_ptr->from]++;
          transition_ptr = transition_ptr->next_ptr;
        }

        // Now iterate again over all transitions of the current label to assign to each
        // transition the pointer where the value of its current block to constellation count is 
        // located. Furtheremore, allocate space in block_to_constellation_count if we register a new
        // count.
        transition_ptr = at_list_per_label.first;
        while (NULL != transition_ptr)
        {
          if (NULL == new_count_ptr[transition_ptr->from])
          {
            // The count is not yet allocated. Push back in block_to_constellation_count and register
            // in new_count_ptr.
            block_to_constellation_count.push_back(block_to_const_count_temp[transition_ptr->from]);
            new_count_ptr[transition_ptr->from] = &block_to_constellation_count.back();
          }

          // Assign the location of the block to contellation count to the current transition.
          transition_ptr->block_to_constellation_count_ptr = new_count_ptr[transition_ptr->from];
          
          transition_ptr = transition_ptr->next_ptr;
        }

        // Reset all the variables used to prepare to next iteration.
        transition_ptr = at_list_per_label.first;
        while (NULL != transition_ptr)
        {
          block_to_const_count_temp[transition_ptr->from] = 0;
          new_count_ptr[transition_ptr->from] = NULL;
          transition_ptr = transition_ptr->next_ptr;
        }

      }

      // Add the initial constellations to global
      global.probabilistic_constellations.init(&probabilistic_constellations.front(),
                                             &probabilistic_constellations.front(), 1);

      global.action_constellations.init(&action_constellations.front(),
                                       &action_constellations.front(), 1);

    }

    /** \brief Perform the preprocessing stage to prepare to apply the algorithm.
    *  \details Initialise the action and probabilistic states and transitions.
    */
    void preprocessing_stage()
    {
      // Allocate space for states and transitions
      action_states.resize(aut.num_states());
      probabilistic_states.resize(aut.num_probabilistic_states());
      action_transitions.resize(aut.num_transitions());

      // Initialise the action transitions
      transition_key_type t_key = 0;
      for (const transition& t : aut.get_transitions())
      {
        action_transition_type& at = action_transitions[t_key];
        at.from = t.from();
        at.label = t.label();
        at.to = t.to();
        at.block_to_constellation_count_ptr = NULL;
        at.next_ptr = NULL;
        at.prev_ptr = NULL;

        // save incomming transition in state
        probabilistic_states[at.to].incoming_transitions.push_back(&at);

        t_key++;
      }

      // Initialise the probabilistic transitions. To this end, we have to iterate over
      // all probabilistic states.
      for (size_t i = 0; i < aut.num_probabilistic_states(); i++)
      {
        const lts_aut_base::probabilistic_state& ps = aut.probabilistic_state(i);

        for (const lts_aut_base::state_probability_pair& sp_pair : ps)
        {
          probabilistic_transition_type pt;
          pt.from = i;
          pt.label = sp_pair.probability();
          pt.to = sp_pair.state();
          pt.next_ptr = NULL;
          pt.prev_ptr = NULL;
          probabilistic_transitions.push_back(pt);

          // save incomming transition in state
          action_states[pt.to].incoming_transitions.push_back(&probabilistic_transitions.back());
        }
      }

      // End of preprocessing.
    }

    /** \brief Refine the initial block according to its outgoing transitions.
    *  \details
    */
    void refine_initial_action_block(std::vector< list_type<action_transition_type> >& transitions_per_label)
    {
      // Iterate over all transitions ordered by label, and refine the block.
      for (list_type<action_transition_type>& t_list : transitions_per_label)
      {
        action_transition_type* at = t_list.first;
        std::vector<action_block_type*> marked_blocks;

        while (NULL != at) {
          action_state_type& s = action_states[at->from];
          action_block_type& parent_block = action_blocks[s.parent_block];
          list_type<action_state_type>& marked_states = parent_block.mark.left;

          // Move state s to marked states if not already added.
          if (false == s.mark_state)
          {
            // Add parent block to the list of marked blocks if not yet added
            if (0 == marked_states.size)
            {
              marked_blocks.push_back(&parent_block);
            }

            move_list_element_back<action_state_type>(s, parent_block.states, marked_states);
            s.mark_state = true;
          }

          at = at->next_ptr;
        }
        
        // Split the marked blocks.
        for (action_block_type* block_ptr : marked_blocks)
        {
          if (0 == block_ptr->states.size)
          {
            // If all states in the block are marked, then return all marked states to the block.
            block_ptr->states = block_ptr->mark.left;
          }
          else
          {
            // Split the block if no all states are marked.
            action_block_type new_block;
            new_block.key = action_blocks.size();
            new_block.states = block_ptr->mark.left;
            new_block.next_ptr = NULL;
            new_block.prev_ptr = NULL;
            new_block.mark.left = { NULL, NULL, 0 };
            new_block.incoming_probabilistic_transitions = { NULL, NULL, 0 };

            // Init parent block of each state in new block.
            action_state_type* s = new_block.states.first;
            while (NULL != s)
            {
              s->parent_block = new_block.key;
              s = s->next_ptr;
            }

            action_blocks.push_back(new_block);
          }
          
          // clean mark list
          block_ptr->mark.left = { NULL, NULL, 0 };

        }

        // Clean the marked states.
        at = t_list.first;
        while (NULL != at) {
          action_state_type& s = action_states[at->from];
          s.mark_state = false;

          at = at->next_ptr;
        }
      }

    }

    /** \brief Move an element of a list to the back of another list.
    *  \details 
    */
    template<typename list_element_type>
    void move_list_element_back(list_element_type& s, list_type<list_element_type>& source_list, list_type<list_element_type>& dest_list)
    {
      // first remove s from source list
      if (NULL != s.prev_ptr)
      {
        s.prev_ptr->next_ptr = s.next_ptr;

        // update last if necessary
        if (source_list.last == &s)
        {
          source_list.last = s.prev_ptr;
        }
      }
      if (NULL != s.next_ptr)
      {
        s.next_ptr->prev_ptr = s.prev_ptr;

        // update first if necessary
        if (source_list.first == &s)
        {
          source_list.first = s.next_ptr;
        }
      }
      source_list.size--;

      // Set first and last to NULL if no more states in the list
      if (0 == source_list.size)
      {
        source_list.first = NULL;
        source_list.last = NULL;
      }

      // Add state s to destination list
      if (dest_list.size == 0)
      {
        // the list is empty, add to first element of list
        s.prev_ptr = NULL;
        s.next_ptr = NULL;
        dest_list.first = &s;
        dest_list.last = &s;
        dest_list.size = 1;
      }
      else
      {
        // add the state at the end of the list
        s.next_ptr = NULL;
        s.prev_ptr = dest_list.last;
        dest_list.last->next_ptr = &s;
        dest_list.last = &s;
        dest_list.size++;
      }
    }

    /** \brief Move an element of a list to the front of another the list.
    *  \details
    */
    template<typename list_element_type>
    void move_list_element_front(list_element_type& s, list_type<list_element_type>& source_list, list_type<list_element_type>& dest_list)
    {
      // first remove s from source list
      if (NULL != s.prev_ptr)
      {
        s.prev_ptr->next_ptr = s.next_ptr;

        // update last if necessary
        if (source_list.last == &s)
        {
          source_list.last = s.prev_ptr;
        }
      }
      if (NULL != s.next_ptr)
      {
        s.next_ptr->prev_ptr = s.prev_ptr;

        // update first if necessary
        if (source_list.first == &s)
        {
          source_list.first = s.next_ptr;
        }
      }
      source_list.size--;

      // Set first and last to NULL if no more states in the list
      if (0 == source_list.size)
      {
        source_list.first = NULL;
        source_list.last = NULL;
      }

      // Add state s to destination list
      if (dest_list.size == 0)
      {
        // the list is empty, add to first element of list
        s.prev_ptr = NULL;
        s.next_ptr = NULL;
        dest_list.first = &s;
        dest_list.last = &s;
        dest_list.size = 1;
      }
      else
      {
        // add the state at the front of the list
        s.next_ptr = dest_list.first; 
        s.prev_ptr =  NULL;
        dest_list.first->prev_ptr = &s;
        dest_list.first = &s;
        dest_list.size++;
      }
    }

    /** \brief Refine partition until it becomes stable.
    *  \details
    */
    void refine_partition_until_it_becomes_stable(void)
    {
      // Non-trivial contellations are always at the front of the list.
      probabilistic_constellation_type* non_trivial_probabilistic_const = global.probabilistic_constellations.first;
      action_constellation_type* non_trivial_action_const = global.action_constellations.first;

      // Refine until all the constellations are trivial.
      while (non_trivial_probabilistic_const->blocks.size > 1 || non_trivial_action_const->blocks.size > 1)
      {
        // Refine probabilistic blocks if a non-trivial action constellation exists.
        if (non_trivial_action_const->blocks.size > 1)
        {
          // Choose splitter block Bc of a non-trivial constellation C, such that |Bc| <= 1/2|C|.
          // And also split constellation C into BC and C\BC in the set of constellations.
          action_block_type* Bc_ptr = choose_action_splitter(non_trivial_action_const);

          // Derive the left, right and middle sets from mark function.
          std::vector<probabilistic_block_type*> marked_blocks;
          mark_probabilistic(Bc_ptr, marked_blocks);

          // Split every marked probabilistic block based on left, middle and right.
          for (probabilistic_block_type* B : marked_blocks)
          {
            // Variable unstable_const is used to determine whether the parent constellation holding
            // the current block becomes unstable; this happends when the block is splitted.
            bool unstable_const = false;

            // First return the largest of left, middle or right to the states of current processed block.
            B->states = *B->mark.large_block_ptr;
            B->mark.large_block_ptr->init(NULL, NULL, 0);

            // Split left set of the block to another block if left has states and it is not
            // the largest block.
            if (B->mark.left.size > 0)
            {
              split_probabilistic_block(*B, B->mark.left);
              unstable_const = true;
            }

            // Split right set of the block to another block if right has states and it is not
            // the largest block.
            if (B->mark.right.size > 0)
            {
              split_probabilistic_block(*B, B->mark.right);
              unstable_const = true;
            }

            // Iterate over all middle sets. Split middle sets of the current block to another block if 
            //  the middle set has states and it is not the largest block.
            for (list_type<probabilistic_state_type>& middle : B->mark.middle)
            {
              if (middle.size > 0)
              {
                split_probabilistic_block(*B, middle);
                unstable_const = true;
              }
            }

            // Move the parent constellation of the current block to the front of the
            // constellation list if it became unstable.
            if (true == unstable_const)
            {
              probabilistic_constellation_type& parent_const = probabilistic_constellations[B->parent_constellation];
              move_list_element_front<probabilistic_constellation_type>(parent_const,
                global.probabilistic_constellations, global.probabilistic_constellations);
            }

            // Reset middle vector to prepare for the next mark process
            B->mark.middle.clear();
          }
        }

        // Refine action blocks if a non-trivial probabilistic constellation exists.
        if (non_trivial_probabilistic_const->blocks.size > 1)
        {
          // Choose splitter block Bc of a non-trivial constellation C, such that |Bc| <= 1/2|C|.
          // And also split constellation C into BC and C\BC in the set of constellations.
          probabilistic_block_type* Bc_ptr = choose_probabilistic_splitter(non_trivial_probabilistic_const);

          // For all incoming labeled "a" transitions of each state in BC call the mark function and split the blocks.
          for (const label_type a : Bc_ptr->incoming_labels)
          {
            // Derive the left, right and middle sets from mark function based on the incomming
            // labeled "a" transitions.
            std::vector<action_block_type*> marked_blocks;
            mark_action(Bc_ptr, marked_blocks, a);

            // Split every marked probabilistic block based on left, middle and right.
            for (action_block_type* B : marked_blocks)
            {
              // Variable unstable_const is used to determine whether the parent constellation holding
              // the current block becomes unstable; this happends when the block is splitted.
              bool unstable_const = false;

              // First return the largest of left, middle or right to the states of current processed block.
              B->states = *B->mark.large_block_ptr;
              B->mark.large_block_ptr->init(NULL, NULL, 0);

              // Split left set of the block to another block if left has states and it is not
              // the largest block.
              if (B->mark.left.size > 0)
              {
                split_action_block(*B, B->mark.left);
                unstable_const = true;
              }

              // Split right set of the block to another block if right has states and it is not
              // the largest block.
              if (B->mark.right.size > 0)
              {
                split_action_block(*B, B->mark.right);
                unstable_const = true;
              }

              // Split middle set of the block to another block if middle has states and it is not
              // the largest block.
              if (B->mark.middle.size > 0)
              {
                split_action_block(*B, B->mark.middle);
                unstable_const = true;
              }

              // Move the parent constellation of the current block to the front of the
              // constellation list if it became unstable.
              if (true == unstable_const)
              {
                action_constellation_type& parent_const = action_constellations[B->parent_constellation];
                move_list_element_front<action_constellation_type>(parent_const,
                  global.action_constellations, global.action_constellations);
              }

            }
          }
        }

        // Select another non-trivial constellation.
        non_trivial_probabilistic_const = global.probabilistic_constellations.first;
        non_trivial_action_const = global.action_constellations.first;
      }
    }

    /** \brief Split a probabilistic block.
    *  \details Creates another block containing the states specified in states_of_new_block. It adds the new block
    *           to the same constellation as the current block to split.
    */
    void split_probabilistic_block(probabilistic_block_type& block_to_split, list_type<probabilistic_state_type>& states_of_new_block)
    {
      // First create the new block to be allocated, and initialise its parameters
      probabilistic_block_type new_block;
      new_block.key = probabilistic_blocks.size();
      new_block.parent_constellation = block_to_split.parent_constellation; // The new block is in the same constellation as B
      new_block.states = states_of_new_block;
      states_of_new_block.init(NULL, NULL, 0);
      new_block.incoming_action_transitions.resize(aut.num_action_labels());

      // Add the incoming action transition of the new block. To this end, iterate over all
      // states in the new block and add the incoming transitions of each state to the
      // incoming transitions of the new block. Also keep track of the labels of the incoming
      // transitions.
      probabilistic_state_type* s = new_block.states.first;
      while (NULL != s)
      {
        // Update the parent block of the state
        s->parent_block = new_block.key;

        // Iterate over all incoming transitions of the state, to add them to the new block
        for (action_transition_type* t : s->incoming_transitions)
        {
          // If it is a transition with a new label, add the label to the incoming labels of the block
          if (new_block.incoming_action_transitions[t->label].size == 0)
          {
            new_block.incoming_labels.push_back(t->label);
          }

          // Move transition from list of transitions of previous block to new block
          move_list_element_back((*t), block_to_split.incoming_action_transitions[t->label], 
            new_block.incoming_action_transitions[t->label]);
        }

        s = s->next_ptr;
      }

      // Update the incoming labels of the block_to_split. To this end iterate over the old incoming labels
      // of the block and chech whether there are still transitions with such a label.
      std::vector<label_type> old_incoming_labels;
      block_to_split.incoming_labels.swap(old_incoming_labels);

      for (const label_type& l : old_incoming_labels)
      {
        // If there are transitions with label l, add them to the incoming labels of block to split
        if (block_to_split.incoming_action_transitions[l].size > 0)
        {
          block_to_split.incoming_labels.push_back(l);
        }
      }

      // Add the new block to the vector of probabilistic blocks
      probabilistic_blocks.push_back(new_block);

      // Add the new block to the back of the list of blocks in the parent constellation.
      probabilistic_constellation_type& parent_const = probabilistic_constellations[new_block.parent_constellation];
      probabilistic_blocks.back().prev_ptr = parent_const.blocks.last;
      probabilistic_blocks.back().next_ptr = NULL;
      parent_const.blocks.last->next_ptr = &probabilistic_blocks.back();
      parent_const.blocks.last = &probabilistic_blocks.back();
      parent_const.blocks.size++;
    }

    /** \brief Split an action block.
    *  \details Creates another block containing the states specified in states_of_new_block. It adds the new block
    *           to the same constellation as the current block to split.
    */
    void split_action_block(action_block_type& block_to_split, list_type<action_state_type>& states_of_new_block)
    {
      // First create the new block to be allocated, and initialise its parameters
      action_block_type new_block;
      new_block.key = action_blocks.size();
      new_block.parent_constellation = block_to_split.parent_constellation; // The new block is in the same constellation as block to split
      new_block.states = states_of_new_block;
      states_of_new_block.init(NULL, NULL, 0);

      // Add the incoming action transition of the new block. To this end, iterate over all
      // states in the new block and add the incoming transitions of each state to the
      // incoming transitions of the new block.
      action_state_type* s = new_block.states.first;
      while (NULL != s)
      {
        // Update the parent block of the state
        s->parent_block = new_block.key;

        // Iterate over all incoming transitions of the state, to add them to the new block
        for (probabilistic_transition_type* t : s->incoming_transitions)
        {
          // Move transition from list of transitions of previous block to new block
          move_list_element_back((*t), block_to_split.incoming_probabilistic_transitions,
            new_block.incoming_probabilistic_transitions);
        }

        s = s->next_ptr;
      }

      // Add the new block to the vector of action blocks
      action_blocks.push_back(new_block);

      // Add the new block to the back of the list of blocks in the parent constellation.
      action_constellation_type& parent_const = action_constellations[new_block.parent_constellation];
      action_blocks.back().prev_ptr = parent_const.blocks.last;
      action_blocks.back().next_ptr = NULL;
      parent_const.blocks.last->next_ptr = &action_blocks.back();
      parent_const.blocks.last = &action_blocks.back();
      parent_const.blocks.size++;
    }

    /** \brief Gives the probabilistic blocks that are marked by block Bc.
    *  \details Derives the left, middle and rigth sets of the marked probabilistic blocks, based on the
    *           incoming probabilistic transitions in block Bc.
    */
    void mark_probabilistic(action_block_type* Bc_ptr, std::vector<probabilistic_block_type*>& marked_blocks)
    {
      action_block_type& Bc = *Bc_ptr;

      // First, iterate over all incomming transitions of block Bc. Mark the blocks that are reached
      // and calculate the cumulative probability of the reached state. This is the probability of
      // a state to reach block Bc.
      probabilistic_transition_type* pt = Bc.incoming_probabilistic_transitions.first;
      while (NULL != pt)
      {
        probabilistic_state_type& s = probabilistic_states[pt->from];
        probability_label_type p = pt->label;
        probabilistic_block_type& B = probabilistic_blocks[s.parent_block];

        // If the block was not previously marked, then mark the block and add all states to right
        if (false == B.mark.block_is_marked)
        {
          B.mark.block_is_marked = true;
          marked_blocks.push_back(&B);
          B.mark.right = B.states;
          B.states = {NULL, NULL, 0};

          // Also initialise the larger block pointer to the right set and the maximum cumulative
          // probability of the block to p
          B.mark.large_block_ptr = &B.mark.right;
          B.mark.max_cumulative_probability = p;
        }

        // Since state s can reach block Bc, move state s to left set if not yet added, and mark the state
        if (false == s.mark_state)
        {
          // State s is not yet marked. Mark the state and move it to left set. In addition, initialise
          // its cumulative probability.
          s.mark_state = true;
          s.cumulative_probability = p;
          move_list_element_back<probabilistic_state_type>(s, B.mark.right, B.mark.left);
        }
        else {
          // State s was already added to left, then just update its cumulative probability
          s.cumulative_probability = s.cumulative_probability + p;
        }

        // Keep track of the maximum cumulative probability of the block
        if (B.mark.max_cumulative_probability < s.cumulative_probability)
        {
          B.mark.max_cumulative_probability = s.cumulative_probability;
        }

        pt = pt->next_ptr;
      }

      // Group all states with the same cumulative probability to construct the middle sets.
      // To this end, iterate over all marked blocks. For each block, first add all the states
      // with probability lower than the max_cumulative_probability of the block to the middle set.
      for (probabilistic_block_type* B : marked_blocks)
      {
        std::vector< std::pair<probability_label_type, probabilistic_state_type*> > grouped_states_per_probability_in_block;
        list_type<probabilistic_state_type> middle_temp = {NULL,NULL,0};

        // First, add all states lower than max_cumulative_probability to the middle set.
        probabilistic_state_type* s = B->mark.left.first;
        while (NULL != s)
        {
          probabilistic_state_type* next_state_ptr = s->next_ptr;

          if (s->cumulative_probability < B->mark.max_cumulative_probability)
          {
            // State s has probability lower than max_cumulative_probability. Add to middle set.
            move_list_element_back<probabilistic_state_type>((*s), B->mark.left, middle_temp);
          }

          // Also reset the marked_state variable in the state here, taiking advantage that we
          // are iterating over all marked states
          s->mark_state = false;

          s = next_state_ptr;
        }

        // Now, for all the states in the middle set, we have to identify the ones with the higher
        // and lower probabilities and add them to a new sub set in middle.
        if (middle_temp.size > 0)
        {
          // There are states in middle; hence, calculate the higher and lower probabilities. To this end,
          // iterate over all states in middle and keep track of the bigger and smaller probability.
          B->mark.smaller_middle_probability = probability_label_type().one();
          B->mark.bigger_middle_probability = probability_label_type().zero();

          s = middle_temp.first;
          while (NULL != s)
          {
            if (B->mark.smaller_middle_probability > s->cumulative_probability)
            {
              B->mark.smaller_middle_probability = s->cumulative_probability;
            }
            if (B->mark.bigger_middle_probability < s->cumulative_probability)
            {
              B->mark.bigger_middle_probability = s->cumulative_probability;
            }
            s = s->next_ptr;
          }
          
          // If the bigger and smaller probabilities happen to be the same, then all the states
          // have the same probability to go to block Bc; hence, only one set in middle is needed.
          // On the other hand, if the bigger and smaller probability are not equal, then we have to 
          // add its respective states to different sub-sets in middle. Furtheremore, the states with
          // probabilities in between are placed in a vector to be ordered later.
          if (B->mark.bigger_middle_probability == B->mark.smaller_middle_probability)
          {
            // Add all the states from middle_temp to the middle set
            B->mark.middle.push_back(middle_temp);
          }
          else
          {
            // If  bigger and smaller probability are not equal, then add them in different sets
            // of middle and sort the remaining probabilities in between.
            list_type<probabilistic_state_type> middle_big = {NULL, NULL, 0};
            list_type<probabilistic_state_type> middle_small = {NULL, NULL, 0};

            // Add all the states corresponding to the bigger and smaller probability to middle.
            // Save the remaining states in a vector to sort them later.
            s = middle_temp.first;
            while (NULL != s)
            {
              probabilistic_state_type* next_state_ptr = s->next_ptr;

              if (s->cumulative_probability == B->mark.bigger_middle_probability)
              {
                // State s has the highest probabilty in the middle temp. Add to middle set.
                move_list_element_back<probabilistic_state_type>((*s), middle_temp, middle_big);
              }
              else if (s->cumulative_probability == B->mark.smaller_middle_probability)
              {
                // State s the lowest probabilty in the middle temp. Add to middle set.
                move_list_element_back<probabilistic_state_type>((*s), middle_temp, middle_small);
              }
              else
              {
                // The cumulative probability of state s is netiher the highest or lowest. Add the
                // state to a vector to sort them later.
                grouped_states_per_probability_in_block.emplace_back(s->cumulative_probability, s);
              }

              s = next_state_ptr;
            }

            // Add the the sets with the biggest and smallest probability to middle.
            B->mark.middle.push_back(middle_big);
            B->mark.middle.push_back(middle_small);

            // Sort the probabilities of middle, not including the biggest and smallest probability
            std::sort(grouped_states_per_probability_in_block.begin(), grouped_states_per_probability_in_block.end());
            
            // Construct the rest of the middle set based on the grouped probabilities. To this end, 
            // traverse all the vector with the grouped states by probability. Store the states with
            // the same probability to a new sub-set in middle set. current_probability is used to
            // keep track of the probability that is currently being processed.
            probability_label_type current_probability = probability_label_type().zero();
            for (const std::pair<probability_label_type, probabilistic_state_type*>& cumulative_prob_state_pair : grouped_states_per_probability_in_block)
            {
              s = cumulative_prob_state_pair.second;
              if (current_probability != cumulative_prob_state_pair.first)
              {
                // The current state has different probability as the current probability. Allocate
                // another space in middle to store this state and change the current probability.
                current_probability = cumulative_prob_state_pair.first;
                B->mark.middle.emplace_back(list_type<probabilistic_state_type>{NULL, NULL, 0});
              }

              move_list_element_back<probabilistic_state_type>((*s), middle_temp, B->mark.middle.back());
            }
            
          }
          
        }

        // Now that we have all the states in left, middle and right; we have to find the largest
        // set. The large block is initialised to be the right set; hence, we only compare if
        // left and middle are larger.
        if (B->mark.left.size > B->mark.large_block_ptr->size)
        {
          B->mark.large_block_ptr = &B->mark.left;
        }

        // Iterate over all subsets of middle set to see if there is a one that is the largest.
        for (list_type<probabilistic_state_type>& middle_set : B->mark.middle)
        {
          if (middle_set.size > B->mark.large_block_ptr->size)
          {
            B->mark.large_block_ptr = &middle_set;
          }
        }

        // Finally, reset the block_is_marked variable of the current block.
        B->mark.block_is_marked = false;

      }

    }

    /** \brief Gives the action blocks that are marked by probabilistic block Bc.
    *  \details Derives the left, middle and rigth sets of the marked action blocks, based on the
    *           incoming action transitions labeled with "a" in block Bc.
    */
    void mark_action(probabilistic_block_type* Bc_ptr, std::vector<action_block_type*>& marked_blocks, label_type a)
    {
      probabilistic_block_type& Bc = *Bc_ptr;

      // For all incoming transitions with label "a" of block Bc calculate left, middle and right.
      // To this end, first move all the states of the block that was reached by traversing the
      // transition backwards to its right set, then move all the states that can reach block Bc with 
      // an "a" action to left and decrement the residual transition count of the state.
      action_transition_type* t = Bc.incoming_action_transitions[a].first;
      while (NULL != t)
      {
        action_state_type& s = action_states[t->from];
        action_block_type& B = action_blocks[s.parent_block];  
        
        // If the block was not previously marked, then mark the block and add all states to right.
        if (false == B.mark.block_is_marked)
        {
          B.mark.block_is_marked = true;
          marked_blocks.push_back(&B);
          B.mark.right = B.states;
          B.states = { NULL, NULL, 0 };
          // Also initialise the larger block pointer to the right set.
          B.mark.large_block_ptr = &B.mark.right;
        }

        // Since state s can reach block Bc, move state s to left set if not yet added, and mark the state
        if (false == s.mark_state)
        {
          // State s is not yet marked. Mark the state and move it to left set. In addition, initialise
          // its residual transition count.
          s.mark_state = true;
          s.residual_transition_cnt = *t->block_to_constellation_count_ptr;
          move_list_element_back<action_state_type>(s, B.mark.right, B.mark.left);
        }

        s.residual_transition_cnt--;

        t = t->next_ptr;
      }

      // Now, for all marked blocks, check the residual transition count of all the states in left. If the transition 
      // count is zero, it means that the state only can reach block BC. If the transition count is greater than 
      // zero, the state has transitions to the other part of the constellation; hence, those states have to be
      // moved to middle.
      for (action_block_type* B : marked_blocks)
      {
        // Iterate over all left states in B and check whether the state has to be moved to middle.
        action_state_type* s = B->mark.left.first;
        while (NULL != s)
        {
          action_state_type* next_state = s->next_ptr;
          if (s->residual_transition_cnt > 0)
          {
            // The transition count is greater than zero. Move state to middle set.
            move_list_element_back<action_state_type>((*s), B->mark.left, B->mark.middle);
          }

          // Also reset the marked_state variable in the state here, taiking advantage that we
          // are iterating over all marked states
          s->mark_state = false;

          s = next_state;
        }

        // Find the largest set.
        if (B->mark.left.size > B->mark.large_block_ptr->size)
        {
          B->mark.large_block_ptr = &B->mark.left;
        }
        if (B->mark.middle.size > B->mark.large_block_ptr->size)
        {
          B->mark.large_block_ptr = &B->mark.middle;
        }

        // Finally, reset the block_is_marked variable of the current block.
        B->mark.block_is_marked = false;
      }

      // Update the block_to_constellation_count of each transition
      t = Bc.incoming_action_transitions[a].first;
      while (NULL != t)
      {
        action_state_type& s = action_states[t->from];

        // If the residual_transition_cnt is greater than zero, it means that the state
        // is in the middle set; hence, the block_to_constellation_count has to be updated.
        if (s.residual_transition_cnt > 0)
        {
          size_t block_to_constellation_count_old = *t->block_to_constellation_count_ptr;

          if (block_to_constellation_count_old != s.residual_transition_cnt)
          {
            // First update the block_to_constellation_count in with the residual_transition_cnt.
            *t->block_to_constellation_count_ptr = s.residual_transition_cnt;

            // Now allocate another block_to_constellation_count for the Bc block
            block_to_constellation_count.emplace_back(block_to_constellation_count_old - s.residual_transition_cnt);
            s.transition_count_ptr = &block_to_constellation_count.back();
          }

          t->block_to_constellation_count_ptr = s.transition_count_ptr;
        }

        t = t->next_ptr;
      }

    }

    /** \brief Choose an splitter block from a non trivial constellation.
    *  \details The number of states in the chosen splitter is always less than the half of the non 
    *           trivial constellation. Furtheremore, the selected splitter is moved to a new constellation.
    */
    action_block_type* choose_action_splitter(action_constellation_type* non_trivial_action_const)
    {
      // First, determine the block to split from constellation. It is the block |Bc| < 1/2|C|
      action_block_type* Bc;

      // First try with the first block in the list.
      Bc = non_trivial_action_const->blocks.first;
      if (Bc->states.size > (non_trivial_action_const->number_of_states / 2))
      {
        // The block is bigger than 1/2|C|. Choose another one.
        Bc = non_trivial_action_const->blocks.last;
      }

      // Now split the block of the constellation.
      // First unlink Bc from the list of blocks of the non trivial constellation.
      if (Bc == non_trivial_action_const->blocks.first)
      {
        non_trivial_action_const->blocks.first = Bc->next_ptr;
        non_trivial_action_const->blocks.first->prev_ptr = NULL;
      }
      if (Bc == non_trivial_action_const->blocks.last)
      {
        non_trivial_action_const->blocks.last = Bc->prev_ptr;
        non_trivial_action_const->blocks.last->next_ptr = NULL;
      }
      Bc->next_ptr = NULL;
      Bc->prev_ptr = NULL;

      // Update the number of states and blocks of the non trivial block
      non_trivial_action_const->number_of_states -= Bc->states.size;
      non_trivial_action_const->blocks.size--;

      // Check if the constellation is still non-trivial; if not, move it to the end 
      // of the constellations list if it is not already at the end
      if (non_trivial_action_const->blocks.size < 2 &&
           global.action_constellations.last != non_trivial_action_const)
      {
        //The constellation is trivial, send to the back of the constellation list.
        // First remove it from its current position and update first pointer of the list 
        // if necessary.
        if (global.action_constellations.first == non_trivial_action_const)
        {
          global.action_constellations.first = non_trivial_action_const->next_ptr;
          global.action_constellations.first->prev_ptr = NULL;
        }
        else
        {
          non_trivial_action_const->prev_ptr->next_ptr = non_trivial_action_const->next_ptr;
          non_trivial_action_const->next_ptr->prev_ptr = non_trivial_action_const->prev_ptr;
        }

        // Now place the trivial constellation to the back of the list
        non_trivial_action_const->prev_ptr = global.action_constellations.last;
        non_trivial_action_const->next_ptr = NULL;
        global.action_constellations.last->next_ptr = non_trivial_action_const;
        global.action_constellations.last = non_trivial_action_const;
      }

      // Add Bc to a new constellation
      action_constellation_type new_action_const;
      new_action_const.key = action_constellations.size();
      Bc->parent_constellation = new_action_const.key;
      new_action_const.blocks = {Bc, Bc, 1};
      new_action_const.number_of_states = Bc->states.size;
      action_constellations.push_back(new_action_const);

      // Finally add the new constellation (with Bc) to the list of constellations in global
      action_constellation_type* new_action_const_ptr = &action_constellations.back();
      global.action_constellations.last->next_ptr = new_action_const_ptr;
      new_action_const_ptr->prev_ptr = global.action_constellations.last;
      new_action_const_ptr->next_ptr = NULL;
      global.action_constellations.last = new_action_const_ptr;
      global.action_constellations.size++;

      return Bc;
    }

    /** \brief Choose an splitter block from a non trivial constellation.
    *  \details The number of states in the chosen splitter is always less than the half of the non
    *           trivial constellation. Furtheremore, the selected splitter is moved to a new constellation.
    */
    probabilistic_block_type* choose_probabilistic_splitter(probabilistic_constellation_type* non_trivial_probabilistic_const)
    {
      // First, determine the block to split from constellation. It is the block |Bc| < 1/2|C|
      probabilistic_block_type* Bc;

      // First try with the first block in the list.
      Bc = non_trivial_probabilistic_const->blocks.first;
      if (Bc->states.size > (non_trivial_probabilistic_const->number_of_states / 2))
      {
        // The block is bigger than 1/2|C|. Choose another one.
        Bc = non_trivial_probabilistic_const->blocks.last;
      }

      // Now split the block of the constellation.
      // First unlink Bc from the list of blocks of the non trivial constellation.
      if (Bc == non_trivial_probabilistic_const->blocks.first)
      {
        non_trivial_probabilistic_const->blocks.first = Bc->next_ptr;
        non_trivial_probabilistic_const->blocks.first->prev_ptr = NULL;
      }
      if (Bc == non_trivial_probabilistic_const->blocks.last)
      {
        non_trivial_probabilistic_const->blocks.last = Bc->prev_ptr;
        non_trivial_probabilistic_const->blocks.last->next_ptr = NULL;
      }
      Bc->next_ptr = NULL;
      Bc->prev_ptr = NULL;

      // Update the number of states and blocks of the non trivial block
      non_trivial_probabilistic_const->number_of_states -= Bc->states.size;
      non_trivial_probabilistic_const->blocks.size--;

      // Check if the constellation is still non-trivial; if not, move it to the end 
      // of the constellations list if it is not already at the end
      if (non_trivial_probabilistic_const->blocks.size < 2 &&
        global.probabilistic_constellations.last != non_trivial_probabilistic_const)
      {
        //The constellation is trivial, send to the back of the constellation list.
        // First remove it from its current position and update first pointer of the list 
        // if necessary.
        if (global.probabilistic_constellations.first == non_trivial_probabilistic_const)
        {
          global.probabilistic_constellations.first = non_trivial_probabilistic_const->next_ptr;
          global.probabilistic_constellations.first->prev_ptr = NULL;
        }
        else
        {
          non_trivial_probabilistic_const->prev_ptr->next_ptr = non_trivial_probabilistic_const->next_ptr;
          non_trivial_probabilistic_const->next_ptr->prev_ptr = non_trivial_probabilistic_const->prev_ptr;
        }

        // Now place the trivial constellation to the back of the list
        non_trivial_probabilistic_const->prev_ptr = global.probabilistic_constellations.last;
        non_trivial_probabilistic_const->next_ptr = NULL;
        global.probabilistic_constellations.last->next_ptr = non_trivial_probabilistic_const;
        global.probabilistic_constellations.last = non_trivial_probabilistic_const;
      }

      // Add Bc to a new constellation
      probabilistic_constellation_type new_probabilistic_const;
      new_probabilistic_const.key = probabilistic_constellations.size();
      Bc->parent_constellation = new_probabilistic_const.key;
      new_probabilistic_const.blocks = { Bc, Bc, 1 };
      new_probabilistic_const.number_of_states = Bc->states.size;
      probabilistic_constellations.push_back(new_probabilistic_const);

      // Finally add the new constellation (with Bc) to the list of constellations in global
      probabilistic_constellation_type* new_probabilistic_const_ptr = &probabilistic_constellations.back();
      global.probabilistic_constellations.last->next_ptr = new_probabilistic_const_ptr;
      new_probabilistic_const_ptr->prev_ptr = global.probabilistic_constellations.last;
      new_probabilistic_const_ptr->next_ptr = NULL;
      global.probabilistic_constellations.last = new_probabilistic_const_ptr;
      global.probabilistic_constellations.size++;

      return Bc;
    }

    lts_aut_base::probabilistic_state calculate_new_probabilistic_state(lts_aut_base::probabilistic_state ps)
    {
      lts_aut_base::probabilistic_state new_prob_state;
      std::map <state_key_type, probability_fraction_type> prob_state_map;

      /* Iterate over all state probability pairs in the selected probabilistic state*/
      for (const lts_aut_base::state_probability_pair& sp_pair : ps)
      {
        /* Check the resulting action state in the final State partition */
        state_key_type new_state = get_eq_class(sp_pair.state());

        if (prob_state_map.count(new_state) == 0)
        {
          /* The state is not yet in the mapping. Add the state with its probability*/
          prob_state_map[new_state] = sp_pair.probability();
        }
        else
        {
          /* The state is already in the mapping. Sum up probabilities */
          prob_state_map[new_state] = prob_state_map[new_state] + sp_pair.probability();
        }
      }

      /* Add all the state probabilities pairs in the mapping to its actual data type*/
      for (std::map<state_key_type, probability_fraction_type>::iterator i = prob_state_map.begin(); i != prob_state_map.end(); i++)
      {
        new_prob_state.add(i->first, i->second);
      }

      return new_prob_state;
    }

    lts_aut_base::probabilistic_state calculate_equivalent_probabilistic_state(probabilistic_block_type& pb)
    {
      lts_aut_base::probabilistic_state equivalent_prob_state;

      // Select the first probabilistic state of the probabilistic block.
      probabilistic_state_type* s = pb.states.first;

      // Take the an incoming transition to know the key of the state
      state_key_type s_key = s->incoming_transitions.back()->to;

      const lts_aut_base::probabilistic_state& old_prob_state = aut.probabilistic_state(s_key);

      equivalent_prob_state = calculate_new_probabilistic_state(old_prob_state);

      return equivalent_prob_state;
    }
};


/** \brief Reduce transition system l with respect to probabilistic bisimulation.
* \param[in/out] l The transition system that is reduced.
*/
template < class LTS_TYPE>
void probabilistic_bisimulation_reduce_fast(LTS_TYPE& l);

/** \brief Checks whether the two initial states of two plts's are probabilistic bisimilar.
* \details This lts and the lts l2 are not usable anymore after this call.
* \param[in/out] l1 A first probabilistic transition system.
* \param[in/out] l2 A second probabilistic transition system.
* \retval True iff the initial states of the current transition system and l2 are probabilistic bisimilar */
template < class LTS_TYPE>
bool destructive_probabilistic_bisimulation_fast_compare(LTS_TYPE& l1, LTS_TYPE& l2);

/** \brief Checks whether the two initial states of two plts's are probabilistic bisimilar.
*  \details The current transitions system and the lts l2 are first duplicated and subsequently
*           reduced modulo bisimulation. If memory space is a concern, one could consider to
*           use destructive_bisimulation_compare.
* \param[in/out] l1 A first transition system.
* \param[in/out] l2 A second transistion system.
* \retval True iff the initial states of the current transition system and l2 are probabilistic bisimilar */
template < class LTS_TYPE>
bool probabilistic_bisimulation_fast_compare(const LTS_TYPE& l1, const LTS_TYPE& l2);

template < class LTS_TYPE>
void probabilistic_bisimulation_reduce_fast(LTS_TYPE& l)
{
  // Apply the probabilistic bisimulation reduction algorithm.
  detail::prob_bisim_partitioner_fast<LTS_TYPE> prob_bisim_part(l);

  // Clear the state labels of the LTS l
  l.clear_state_labels();

  // Assign the reduced LTS
  l.set_num_states(prob_bisim_part.num_eq_classes());
  prob_bisim_part.replace_transitions();
  prob_bisim_part.replace_probabilistic_states();
}

template < class LTS_TYPE>
bool probabilistic_bisimulation_fast_compare(
  const LTS_TYPE& l1,
  const LTS_TYPE& l2)
{
  LTS_TYPE l1_copy(l1);
  LTS_TYPE l2_copy(l2);
  return destructive_probabilistic_bisimulation_fast_compare(l1_copy, l2_copy);
}

template < class LTS_TYPE>
bool destructive_probabilistic_bisimulation_fast_compare(
  LTS_TYPE& l1,
  LTS_TYPE& l2)
{
  size_t initial_probabilistic_state_key_l1;
  size_t initial_probabilistic_state_key_l2;

  // Merge states
  mcrl2::lts::detail::plts_merge(l1, l2);
  l2.clear(); // No use for l2 anymore.

  // The last two probabilistic states are the initial states of l2 and l1
  // in the merged plts.
  initial_probabilistic_state_key_l2 = l1.num_probabilistic_states() - 1;
  initial_probabilistic_state_key_l1 = l1.num_probabilistic_states() - 2;

  detail::prob_bisim_partitioner_fast<LTS_TYPE> prob_bisim_part(l1);

  return prob_bisim_part.in_same_probabilistic_class_fast(initial_probabilistic_state_key_l2,
    initial_probabilistic_state_key_l1);
}

} // end namespace detail
} // end namespace lts
} // end namespace mcrl2
#endif //_LIBLTS_PBISIM_FAST_H
