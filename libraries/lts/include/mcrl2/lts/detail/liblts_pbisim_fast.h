// Author(s): Joao Rivera
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/liblts_pbisim.h

#ifndef _LIBLTS_PBISIM_FAST_H
#define _LIBLTS_PBISIM__FAST_H
#include <cmath>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lts/lts_aut.h"

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
     *  \details 
	   */
    prob_bisim_partitioner_fast(
      LTS_TYPE& l):aut(l)
    {
      mCRL2log(log::verbose) << "Probabilistic bisimulation partitioner created for "
                  << l.num_states() << " states and " <<
                  l.num_transitions() << " transitions\n";
      create_initial_partition();
      refine_partition_until_it_becomes_stable();
    }

    /** \brief Gives the number of bisimulation equivalence classes of the LTS.
    *  \return The number of bisimulation equivalence classes of the LTS.
    */
    size_t num_eq_classes() const
    {
      return pi_partition.action_blocks.size();
    }

    /** \brief Replaces the transition relation of the current lts by the transitions
    *         of the bisimulation reduced transition system.
    * \pre The bisimulation equivalence classes have been computed. */
    void replace_transitions()
    {
      std::set < transition > resulting_transitions;

      block_index_type block_index_of_initial_state = probabilistic_state_to_block_index[aut.num_probabilistic_labels()];
      probabilistic_block_type& block_of_initial_state =  pi_partition.probabilistic_blocks[block_index_of_initial_state];
      ignore_block = pi_partition.probabilistic_blocks.size(); //it will not ignore any in this case
      if (block_of_initial_state.states.size() == 1)
      {
        // only th einitial state is in the block, then we should ignor it
        ignore_block = block_index_of_initial_state;
      }

      const std::vector<transition>& trans = aut.get_transitions();
      for (const transition &t : trans)
      {
        size_t prob_equivalent_class =  get_eq_step_class(t.to());
        if (prob_equivalent_class > ignore_block)
        {
          prob_equivalent_class--;
        }
        resulting_transitions.insert(
          transition(
            get_eq_class(t.from()),
            t.label(),
            prob_equivalent_class));
      }
      // Remove the old transitions
      aut.clear_transitions();

      // Copy the transitions from the set into the transition system.
      for (const transition &t : resulting_transitions)
      {
        aut.add_transition(t);
      }
    }

    /** \brief Gives the bisimulation equivalence class number of a state.
    *  \param[in] s A state number.
    *  \return The number of the bisimulation equivalence class to which \e s belongs. */
    size_t get_eq_class(const size_t s) 
    {
      assert(s < state_to_block_index_map.size());
      return state_to_block_index_map[s]; // The block index is the state number of the block.
    }

    /** \brief Gives the bisimulation equivalence step class number of a probabilistic state.
    *  \param[in] s A probabilistic state number.
    *  \return The number of the step class to which d belongs. */
    size_t get_eq_step_class(const size_t s)
    {
      assert(s<probabilistic_state_to_block_index.size());
      return probabilistic_state_to_block_index[s]; // The block index is the state number of the block.
    }

    /** \brief Replaces the probabilistic states of the current lts by the probabilistic
    *         states of the bisimulation reduced transition system.
    * \pre The bisimulation step classes have been computed. */
    void replace_probabilistic_states()
    {
      std::vector<probabilistic_block_type>& p_blocks = pi_partition.probabilistic_blocks;
      std::vector<lts_aut_base::probabilistic_state> new_probabilistic_states;

      // get the equivalent probabilistic state of each probabilistic block and add it to aut
      block_index_type block_index = 0;
      for (probabilistic_block_type& pb : p_blocks)
      {
        if (block_index != ignore_block)
        {
         lts_aut_base::probabilistic_state equivalent_ps = calculate_equivalent_probabilistic_state(pb);
          new_probabilistic_states.push_back(equivalent_ps);
        }
        block_index++;
      }

      /* Remove old probabilistic states */
      aut.clear_probabilistic_states();

      // Add new prob states to aut
      for (lts_aut_base::probabilistic_state new_ps : new_probabilistic_states)
      {
        aut.add_probabilistic_state(new_ps);
      }

      lts_aut_base::probabilistic_state old_initial_prob_state = aut.initial_probabilistic_state();
      lts_aut_base::probabilistic_state new_initial_prob_state = calculate_new_probabilistic_state(old_initial_prob_state);
      aut.set_initial_probabilistic_state(new_initial_prob_state);
      
    }

    /** \brief Destroys this partitioner. */
    ~prob_bisim_partitioner_fast()
    {}

  private:

		typedef size_t block_index_type;
    typedef size_t constellation_index_type;
    typedef size_t transition_index_type;
		typedef size_t step_class_index_type;
		typedef size_t state_type;
    typedef size_t probabilistic_state_type;
    typedef size_t label_type;
    typedef probabilistic_arbitrary_precision_fraction probability_label_type;
		typedef probabilistic_arbitrary_precision_fraction probability_fraction_type;

    struct action_transition_type
    {
      //transition* a_transition;
      state_type from;
      label_type label;
      state_type to;
      size_t block_to_constelation_count;
    };

    struct probabilistic_transition_type
    {
      state_type from;
      probability_label_type label;
      state_type to;
      size_t block_to_constelation_count; //not used
    };

    struct action_block_type 
    {
      std::set< state_type > states;
      //std::map<label_type, > incomming_transitions; //not sure yet what is going to be here
      std::set<transition_index_type> incomming_probabilistic_transitions;
    };

    struct probabilistic_block_type
    {
     std::set<state_type> states;
     std::map<label_type, std::set<transition_index_type> > incomming_action_transitions;  // a probabilistic block has incomming action transitions ordered by label
    };

    struct blocks_type {
      std::vector<action_block_type> action_blocks;
      std::vector<probabilistic_block_type> probabilistic_blocks;
    };

    struct constellation_type {
      std::set<block_index_type> blocks;
      size_t size; //number of states in probabilistic constellation
    };

    struct global_type
    {
      std::vector<constellation_type> action_constellations;
      std::vector<constellation_type> probabilistic_constellations;
    };

    struct block_to_constellation_map_type
    {
      std::map< block_index_type, block_index_type> action_map;
      std::map< block_index_type, block_index_type> probabilistic_map;
    };

    struct non_trivial_constellations_type
    {
      std::set<constellation_index_type> action;
      std::set<constellation_index_type> probabilistic;
    };

    blocks_type pi_partition;
    global_type global_constellations;
    std::vector<action_transition_type> action_transitions;
    std::vector<probabilistic_transition_type> probabilistic_transitions;
    std::map< state_type, block_index_type> state_to_block_index_map;
    std::map< state_type, block_index_type> probabilistic_state_to_block_index;
    std::map< state_type, std::set<transition_index_type> > incomming_probabilistic_transitions_per_state; // maybe it is better to have a vector of pointers
    std::map< state_type, std::set<transition_index_type> > incomming_action_transitions_per_state;
    block_to_constellation_map_type block_to_constellation;
    non_trivial_constellations_type non_trivial_constellations;
    block_index_type ignore_block;

		LTS_TYPE& aut;

  /** \brief Creates the initial partition.
   *  \detal The blocks are initially partitioned based on the actions that can perform.
   *         The step classes are partitioned based on the action that leads to the probabilistic state */
    void create_initial_partition(void)
		{
      const std::vector<transition> & transitions = aut.get_transitions();
      const size_t num_action_states = aut.num_states();
      const size_t num_prob_states = aut.num_probabilistic_labels();
      std::map< label_type, std::set <state_type> > source_states_per_label;
      std::map< state_type, std::map<label_type, size_t> > label_count_per_state;

      // Initially there is only one action constellation and one probabilistic constellation.
      // The constellations are Sp and Sa. Time complexity: O(n).
      global_constellations.action_constellations.resize(1);
      global_constellations.probabilistic_constellations.resize(1);
      constellation_type& a_constellation = global_constellations.action_constellations[0];
      constellation_type& p_constellation = global_constellations.probabilistic_constellations[0];
      a_constellation.size = num_action_states;
      p_constellation.size = num_prob_states + 1; // plus one to add the initial probabilistic state

      // initially there is only one block in each section of the pi_partition
      pi_partition.action_blocks.resize(1);
      pi_partition.probabilistic_blocks.resize(1);

      // Insert the index of the initial probabilistic block into the probabilistic constellation
      p_constellation.blocks.insert(0);
      block_to_constellation.probabilistic_map[0] = 0; // first (and only) probabilistic block in the first probabilistic constellation

      for (size_t i = 0; i < num_action_states; i++)
      {
        pi_partition.action_blocks[0].states.insert(i);
      }
      for (size_t i = 0; i < num_prob_states; i++)
      {
        pi_partition.probabilistic_blocks[0].states.insert(i);
        probabilistic_state_to_block_index[i] = 0; // initially all probabilistic states are in the first block
      }
      pi_partition.probabilistic_blocks[0].states.insert(num_prob_states); // insert initial state with index "num_prob_states"
      probabilistic_state_to_block_index[num_prob_states] = 0;

      // Group states per outgoing action transition
      for (transition t : transitions)
      {
        source_states_per_label[t.label()].insert(t.from());
        label_count_per_state[t.from()][t.label()]++;
      }

      // Refine the block in the action constellation according to the outgoing transitions to preserve the invariant
      for (std::map<label_type, std::set <state_type>>::iterator i = source_states_per_label.begin(); i != source_states_per_label.end(); i++)
      {
        std::vector <action_block_type> new_blocks;
        for (action_block_type& block_to_split : pi_partition.action_blocks) // Access all the blocks of action_partition to start splitting
        {
          action_block_type a_block;
          for (state_type s : i->second)  //Iterating over all states in every subset of source_states_per_label. It will be m iterations in combination with fisrt loop.
          {
            if (block_to_split.states.count(s) > 0)
            {
              //change s to a new block
              block_to_split.states.erase(s);
              a_block.states.insert(s);
            }
          }
          if (a_block.states.size() > 0) {
            // add to new blocks if it is not empty
            new_blocks.push_back(a_block);
          }
        }

        // add new blocks to pi partition and add block_id to constellation
        for (action_block_type b : new_blocks)
        {
          pi_partition.action_blocks.push_back(b);
        }
      }

      // remove all empty blocks in action_constellation. todo: find a more efficient way to remove these blocks
      size_t partition_size = pi_partition.action_blocks.size();
      typename std::vector<action_block_type>::iterator block_iterator = pi_partition.action_blocks.end();
      block_iterator--; // to point to last reachable element
      for (size_t i = partition_size; i > 0; i--)
      {
        typename std::vector<action_block_type>::iterator block_iterator_temp;

        block_iterator_temp = block_iterator;
        if (i > 1) // avoid decrement iterator in the last iteration because it would go out of scope
        {
          block_iterator--;
        }

        if (pi_partition.action_blocks[i-1].states.size() == 0)
        {
          pi_partition.action_blocks.erase(block_iterator_temp);
        }
      }

      // add the blocks ids to the constellation
      for (block_index_type block_index = 0; block_index < pi_partition.action_blocks.size(); block_index++)
      {
        a_constellation.blocks.insert(block_index);
        block_to_constellation.action_map[block_index] = 0; // all action blocks are in the first action constellation
      }

      // Initialise the block to constelation count for each transition// O(nr of action transition)
      for (transition t : transitions)
      {
        action_transition_type at;

        //this is using a_transition as a pointer instead of copying the elements
        //at.a_transition = &t;
        //at.block_to_constelation_count = label_count_per_state[t.from()][t.label()];

        at.from = t.from();
        at.label = t.label();
        at.to = t.to();
        at.block_to_constelation_count = label_count_per_state[t.from()][t.label()];

        action_transitions.push_back(at);
        incomming_action_transitions_per_state[at.to].insert(action_transitions.size()-1);
      }

      // Initialize the probabilistic transitions. // O(nr of probabilistic transition)
      for (size_t i = 0; i < num_prob_states; i++) 
      {
        const lts_aut_base::probabilistic_state& ps = aut.probabilistic_state(i);

        for (const lts_aut_base::state_probability_pair &sp_pair : ps)
        {
          probabilistic_transition_type pt;
          pt.from = i;
          pt.label = sp_pair.probability();
          pt.to = sp_pair.state();
          pt.block_to_constelation_count = ps.size();

          probabilistic_transitions.push_back(pt);
          incomming_probabilistic_transitions_per_state[pt.to].insert(probabilistic_transitions.size()-1);
        }
      }
      // Add transitions of the initial probabilistic state
      const lts_aut_base::probabilistic_state& initial_ps = aut.initial_probabilistic_state();
      for (const lts_aut_base::state_probability_pair &sp_pair : initial_ps)
      {
        probabilistic_transition_type pt;
        pt.from = num_prob_states;
        pt.label = sp_pair.probability();
        pt.to = sp_pair.state();
        pt.block_to_constelation_count = initial_ps.size();

        probabilistic_transitions.push_back(pt);
        incomming_probabilistic_transitions_per_state[pt.to].insert(probabilistic_transitions.size() - 1);
      }

      // Group the incomming action transitions in each block per label
      // initially there is only one block with incomming action transitions, namely Sp (p_constellation[0])
      // O(nr of action transition)
      //for (action_transition_type at : action_transitions) 
      for (transition_index_type t_index = 0; t_index < action_transitions.size(); t_index++)
      {
        action_transition_type& at = action_transitions[t_index];
        pi_partition.probabilistic_blocks[0].incomming_action_transitions[at.label].insert(t_index);
      }

      // Initialize auxilary mapping state_to_block_index_map
      // iterate over all states of each block in the action_constellation and determine the block_index to which it belongs
      // O(nr of action states)
      for (block_index_type block_index = 0; block_index < pi_partition.action_blocks.size(); block_index++)
      {
        for (state_type s : pi_partition.action_blocks[block_index].states)
        {
          state_to_block_index_map[s] = block_index;
        }
      }

      // Initialize incomming probabilistic transitions to action blocks. todo: find a way to do this more efficient. 
      // O(nr of probabilistic transitions)
     // for (probabilistic_transition_type& pt : probabilistic_transitions)
      for (transition_index_type t_index = 0; t_index < probabilistic_transitions.size(); t_index++)
      {
        probabilistic_transition_type& pt = probabilistic_transitions[t_index];
        block_index_type b_index = state_to_block_index_map[pt.to];
        pi_partition.action_blocks[b_index].incomming_probabilistic_transitions.insert(t_index);
      }

      // the only non trivial constellatin after initialization is the action constellation
      non_trivial_constellations.action.insert(0);
	}

    struct mark_structure_type
    {
      std::set<block_index_type> marked_blocks;
      std::map<block_index_type, std::set<state_type> > left;
      std::map<block_index_type, std::vector< std::set<state_type> > > middle;
      std::map<block_index_type, std::set<state_type> > right;
      std::map<block_index_type, std::set<state_type>* > large_block_ptr;
    };
    
    /* */
    void refine_partition_until_it_becomes_stable (void) 
    {
      std::vector<constellation_type>& a_constellations = global_constellations.action_constellations;
      std::vector<constellation_type>& p_constellations = global_constellations.probabilistic_constellations;
      std::vector<action_block_type>& a_blocks = pi_partition.action_blocks;
      std::vector<probabilistic_block_type>& p_blocks = pi_partition.probabilistic_blocks;

      // refine until all the constellations are trivial
      while (non_trivial_constellations.action.size() > 0 || non_trivial_constellations.probabilistic.size() > 0) {

        // Refine probabilistic blocks if a non-trivial action constellation exists
        if (non_trivial_constellations.action.size() > 0)
        {
          // take first non_trivial constellation
          std::set<constellation_index_type>::iterator i = non_trivial_constellations.action.begin();
          constellation_index_type non_trivial_constellation_index = *i;

          // Choose splitter block Bc of a non-trivial constellation C, such that |Bc| <= 1/2|C|
          action_block_type* Bc_ptr = choose_action_splitter(non_trivial_constellation_index);
          action_block_type& Bc = *Bc_ptr;

          // Check if the constellation is still non-trivial, if not remove from non-trivial constellations set
          constellation_type& non_trivial_action_constellation = a_constellations[non_trivial_constellation_index];
          if (non_trivial_action_constellation.blocks.size() < 2)
          {
            //the constellation is trivial
            non_trivial_constellations.action.erase(non_trivial_constellation_index);
          }

          //derive the left, right and middle sets from mark function
          mark_structure_type probabilistic_mark_struct;
          mark_probabilistic(Bc, probabilistic_mark_struct);

          //Split every probabilistic block based on left, middle and right
          for (block_index_type block_index : probabilistic_mark_struct.marked_blocks)
          {
            // add left block
            if (probabilistic_mark_struct.left[block_index].size() != 0 &&
              probabilistic_mark_struct.large_block_ptr[block_index] != &probabilistic_mark_struct.left[block_index])
            {
              split_probabilistic_block(block_index, probabilistic_mark_struct.left[block_index]);
              // add the constellation where the splitted block is located to the set of non-trivial constellations
              non_trivial_constellations.probabilistic.insert(block_to_constellation.probabilistic_map[block_index]);
            }

            // add right block
            if (probabilistic_mark_struct.right[block_index].size() != 0 &&
              probabilistic_mark_struct.large_block_ptr[block_index] != &probabilistic_mark_struct.right[block_index])
            {
              split_probabilistic_block(block_index, probabilistic_mark_struct.right[block_index]);
              // add the constellation where the splitted block is located to the set of non-trivial constellations
              non_trivial_constellations.probabilistic.insert(block_to_constellation.probabilistic_map[block_index]);
            }

            // Add the middle blocks
            for (std::set<state_type> middle_i : probabilistic_mark_struct.middle[block_index])
            {
              if (middle_i.size() != 0 &&
                probabilistic_mark_struct.large_block_ptr[block_index] != &middle_i)
              {
                split_probabilistic_block(block_index, middle_i);
                // add the constellation where the splitted block is located to the set of non-trivial constellations
                non_trivial_constellations.probabilistic.insert(block_to_constellation.probabilistic_map[block_index]);
              }
            }
          }

        }

        if (non_trivial_constellations.probabilistic.size() > 0)
        {
          // take first non_trivial constellation
          std::set<constellation_index_type>::iterator i = non_trivial_constellations.probabilistic.begin();
          constellation_index_type non_trivial_constellation_index = *i;

          // Choose splitter block Bc of a non-trivial constellation C, such that |Bc| <= 1/2|C|
          probabilistic_block_type* Bc_ptr = choose_probabilistic_splitter(non_trivial_constellation_index);
          probabilistic_block_type& Bc = *Bc_ptr;

          // Check if the constellation is still non-trivial, if not remove from non-trivial constellations set
          constellation_type& non_trivial_probabilistic_constellation = p_constellations[non_trivial_constellation_index];
          if (non_trivial_probabilistic_constellation.blocks.size() < 2)
          {
            //the constellation is trivial
            non_trivial_constellations.probabilistic.erase(non_trivial_constellation_index);
          }

          // for all incoming actions a for each state in BC call the mark function and split the blocks
          for (std::map<label_type, std::set<transition_index_type> >::iterator i = Bc.incomming_action_transitions.begin(); i != Bc.incomming_action_transitions.end(); i++)
          {
            //derive the left, right and middle sets from mark function
            mark_structure_type action_mark_struct;
            label_type a = i->first;
            mark_action(Bc, action_mark_struct, a);

            //Split every probabilistic block based on left, middle and right
            for (block_index_type block_index : action_mark_struct.marked_blocks)
            {
              // add left block
              if (action_mark_struct.left[block_index].size() != 0 &&
                action_mark_struct.large_block_ptr[block_index] != &action_mark_struct.left[block_index])
              {
                split_action_block(block_index, action_mark_struct.left[block_index]);
                // add the constellation where the splitted block is located to the set of non-trivial constellations
                non_trivial_constellations.action.insert(block_to_constellation.action_map[block_index]);
              }

              // add right block
              if (action_mark_struct.right[block_index].size() != 0 &&
                action_mark_struct.large_block_ptr[block_index] != &action_mark_struct.right[block_index])
              {
                split_action_block(block_index, action_mark_struct.right[block_index]);
                // add the constellation where the splitted block is located to the set of non-trivial constellations
                non_trivial_constellations.action.insert(block_to_constellation.action_map[block_index]);
              }

              // Add the middle blocks
              for (std::set<state_type>& middle_i : action_mark_struct.middle[block_index])
              {
                if (middle_i.size() != 0 &&
                  action_mark_struct.large_block_ptr[block_index] != &middle_i)
                {
                  split_action_block(block_index, action_mark_struct.right[block_index]);
                  // add the constellation where the splitted block is located to the set of non-trivial constellations
                  non_trivial_constellations.action.insert(block_to_constellation.action_map[block_index]);
                }
              }
            }

          }
        }
      }

    }

    void mark_probabilistic(action_block_type& Bc, mark_structure_type& mark_struct)
    {
      // Declare some intermidiate variables for Mark procedure
      std::set<block_index_type>& marked_blocks = mark_struct.marked_blocks;
      std::map<block_index_type, std::set<state_type> >& left = mark_struct.left;
      std::map<block_index_type, std::vector< std::set<state_type> > >& middle = mark_struct.middle;
      std::map<block_index_type, std::set<state_type> >& right = mark_struct.right;
      std::map<block_index_type, std::set<state_type>* >& large_block_ptr = mark_struct.large_block_ptr;
      std::map<state_type, probability_label_type> commulative_probability;
      std::map<block_index_type, probability_label_type> max_commulative_probability;

      // for all incomming probabilistic transitions of block BC calculate left, middle and right
      //for (probabilistic_transition_type prob_transition : Bc.incomming_probabilistic_transitions)
      for (transition_index_type pt_index : Bc.incomming_probabilistic_transitions)
      {
        probabilistic_transition_type& prob_transition = probabilistic_transitions[pt_index];

        state_type u = prob_transition.from;
        probability_label_type p = prob_transition.label;
        block_index_type B = probabilistic_state_to_block_index[u]; // this is currently only for the action blocks not probabilistic blocks
        if (marked_blocks.count(B) == 0)
        {
          if (left[B].size() == 0 && right[B].size() == 0)
          {
            // just to initialize right[B]
            right[B] = pi_partition.probabilistic_blocks[B].states;
            large_block_ptr[B] = &right[B];
          }
          // the block is new. Add the block to marked_blocks
          marked_blocks.insert(B);

          max_commulative_probability[B] = p;
        }

        // if u is not yet in left, then init commulative probability and move u from right to left
        if (left[B].count(u) == 0)
        {
          commulative_probability[u] = p;
          left[B].insert(u);
          right[B].erase(u);
        }
        else {
          // u was already added to left, then just add its commulative probability
          commulative_probability[u] = commulative_probability[u] + p;
        }

        if (max_commulative_probability[B] < commulative_probability[u])
        {
          max_commulative_probability[B] = commulative_probability[u];
        }

      }

      // Group all states with the same commulative probability to construct the middle set
      for (std::map<block_index_type, std::set<state_type> >::iterator i = left.begin(); i != left.end(); i++)
      {
        std::map<probability_label_type, std::set<state_type> >  sorted_probabilities_in_block;
        block_index_type B = i->first;
        std::set<state_type> left_B = i->second;

        for (state_type u : left_B)
        {
          if (commulative_probability[u] < max_commulative_probability[B])
          {
            // sort u by its probability and erase from left set
            sorted_probabilities_in_block[commulative_probability[u]].insert(u);
            left[B].erase(u);
          }
        }

        // construct the middle set based on the sorted probabilities
        for (std::map<probability_label_type, std::set<state_type> >::iterator i = sorted_probabilities_in_block.begin(); i != sorted_probabilities_in_block.end(); i++)
        {
          middle[B].push_back(i->second);
        }

        // finde the large set
        if (left[B].size() > large_block_ptr[B]->size())
        {
          large_block_ptr[B] = &left[B];
        }

        if (middle.count(B) > 0)
        {
          for (std::set<state_type>& middle_set : middle[B])
          {
            if (middle_set.size() > large_block_ptr[B]->size())
            {
              large_block_ptr[B] = &middle_set;
            }
          }
        }
      }

    }

    void mark_action(probabilistic_block_type& Bc, mark_structure_type& mark_struct, label_type a)
    {
      // Declare some intermidiate variables for Mark procedure
      std::set<block_index_type>& marked_blocks = mark_struct.marked_blocks;
      std::map<block_index_type, std::set<state_type> >& left = mark_struct.left;
      std::map<block_index_type, std::vector< std::set<state_type> > >& middle = mark_struct.middle;
      std::map<block_index_type, std::set<state_type> >& right = mark_struct.right;
      std::map<block_index_type, std::set<state_type>* >& large_block_ptr = mark_struct.large_block_ptr;
      std::map<state_type, size_t> residual_transition_cnt;

      // for all incomming a transitions of block Bc calculate left, middle and right
      for (transition_index_type at_index : Bc.incomming_action_transitions[a])
      {
        action_transition_type& a_transition = action_transitions[at_index];

        state_type u = a_transition.from;
        block_index_type B = state_to_block_index_map[u];  //get the block B where state u is located
        if (marked_blocks.count(B) == 0)
        {
          if (left[B].size() == 0 && right[B].size() == 0)
          {
            // just to initialize right[B]
            right[B] = pi_partition.action_blocks[B].states;
            large_block_ptr[B] = &right[B];
          }
          // the block is new. Add the block to marked_blocks
          marked_blocks.insert(B);
        }
        
        // init residual transition count of state u
        if (left[B].count(u) == 0)
        {
          residual_transition_cnt[u] = a_transition.block_to_constelation_count;
          left[B].insert(u);
          right[B].erase(u);
        }
        
        residual_transition_cnt[u]--;

      }

      // Group all states which its residual_transition_cnt is not zero to form the middle set
      for (std::map<block_index_type, std::set<state_type> >::iterator i = left.begin(); i != left.end(); i++)
      {
        std::set<state_type> middle_set;
        block_index_type B = i->first;
        std::set<state_type> left_B = i->second;

        // iterate over all states of left[B]
        for (state_type u : left_B)
        {
          if (residual_transition_cnt[u] > 0)
          {
            // the residual count is not zero, then move to middle_set
            middle_set.insert(u);
            left[B].erase(u);
          }
        }

        // construct the middle set
        middle[B].push_back(middle_set);

        // finde the large set
        if (left[B].size() > large_block_ptr[B]->size())
        {
          large_block_ptr[B] = &left[B];
        }

        if (middle.count(B) > 0)
        {
          for (std::set<state_type>& middle_set : middle[B])
          {
            if (middle_set.size() > large_block_ptr[B]->size())
            {
              large_block_ptr[B] = &middle_set;
            }
          }
        }
      }

    }

    void split_probabilistic_block(block_index_type block_to_split_index, std::set<state_type>& states_of_new_block)
    {
      probabilistic_block_type new_prob_block;
      probabilistic_block_type& block_to_split = pi_partition.probabilistic_blocks[block_to_split_index];
      new_prob_block.states = states_of_new_block;

      for (state_type s : new_prob_block.states)
      {
        block_to_split.states.erase(s);
        // add all the incomming transitions of the states to the new block and delete from previous block
        for (transition_index_type at_index : incomming_action_transitions_per_state[s])
        {
          const action_transition_type& at = action_transitions[at_index];
          new_prob_block.incomming_action_transitions[at.label].insert(at_index);
          block_to_split.incomming_action_transitions[at.label].erase(at_index);

          // erase key from mapping if empty
          if (block_to_split.incomming_action_transitions[at.label].size() == 0)
          {
            block_to_split.incomming_action_transitions.erase(at.label);
          }
        }
        // update the block of s because it was moved to another block (that will be in the back of the vector)
        probabilistic_state_to_block_index[s] = pi_partition.probabilistic_blocks.size();
      }
      block_index_type new_block_index = pi_partition.probabilistic_blocks.size();
      block_index_type parent_constellation_index = block_to_constellation.probabilistic_map[block_to_split_index];
      // The new block is in the same constellation as the block to split
      global_constellations.probabilistic_constellations[parent_constellation_index].blocks.insert(new_block_index);
      block_to_constellation.probabilistic_map[new_block_index] = parent_constellation_index;
      pi_partition.probabilistic_blocks.push_back(new_prob_block);
    }

    void split_action_block(block_index_type block_to_split_index, std::set<state_type>& states_of_new_block)
    {
      action_block_type new_action_block;
      action_block_type& block_to_split = pi_partition.action_blocks[block_to_split_index];
      new_action_block.states = states_of_new_block;

      for (state_type s : new_action_block.states)
      {
        block_to_split.states.erase(s);
        // add all the incomming transitions of the states to the new block and delete from previous block
        for (transition_index_type pt_index : incomming_probabilistic_transitions_per_state[s])
        {
          //const probabilistic_transition_type& pt = probabilistic_transitions[pt_index];
          new_action_block.incomming_probabilistic_transitions.insert(pt_index);
          block_to_split.incomming_probabilistic_transitions.erase(pt_index);
        }
        // update the block of s because it was moved to another block (that will be in the back of the vector)
        state_to_block_index_map[s] = pi_partition.action_blocks.size();
      }
      block_index_type new_block_index = pi_partition.action_blocks.size();
      block_index_type parent_constellation_index = block_to_constellation.action_map[block_to_split_index];
      // The new block is in the same constellation as the block to split
      global_constellations.action_constellations[parent_constellation_index].blocks.insert(new_block_index);
      block_to_constellation.action_map[new_block_index] = parent_constellation_index;
      pi_partition.action_blocks.push_back(new_action_block);
    }

    action_block_type* choose_action_splitter(constellation_index_type constellation_index)
    {
      action_block_type* splitter;
      constellation_type& non_trivial_a_constellation = global_constellations.action_constellations[constellation_index];
      block_index_type splitter_index;
      for (block_index_type i : non_trivial_a_constellation.blocks)
      {
        splitter = &pi_partition.action_blocks[i];
        if (splitter->states.size() <= (non_trivial_a_constellation.size / 2))
        {
          splitter_index = i;
          break;
        }
      }

      // split constellation C into Bc and C\Bc. First create another constellation with only this block
      constellation_type new_a_constellation;
      new_a_constellation.blocks.insert(splitter_index);
      new_a_constellation.size = splitter->states.size();
      non_trivial_a_constellation.blocks.erase(splitter_index);
      non_trivial_a_constellation.size -= splitter->states.size();
      block_to_constellation.action_map[splitter_index] = global_constellations.action_constellations.size();
      global_constellations.action_constellations.push_back(new_a_constellation);

      return splitter;
    }

    probabilistic_block_type*  choose_probabilistic_splitter(constellation_index_type constellation_index)
    {
      probabilistic_block_type* splitter;
      constellation_type& non_trivial_p_constellation = global_constellations.probabilistic_constellations[constellation_index];
      block_index_type splitter_index;
      for (block_index_type i : non_trivial_p_constellation.blocks)
      {
        splitter = &pi_partition.probabilistic_blocks[i];
        if (splitter->states.size() <= (non_trivial_p_constellation.size / 2))
        {
          splitter_index = i;
          break;
        }
      }

      // split constellation C into Bc and C\Bc. First create another constellation with only this block
      constellation_type new_p_constellation;
      new_p_constellation.blocks.insert(splitter_index);
      new_p_constellation.size = splitter->states.size();
      non_trivial_p_constellation.blocks.erase(splitter_index);
      non_trivial_p_constellation.size -= splitter->states.size();
      block_to_constellation.probabilistic_map[splitter_index] = global_constellations.probabilistic_constellations.size();
      global_constellations.probabilistic_constellations.push_back(new_p_constellation);

      return splitter;
    }


    lts_aut_base::probabilistic_state calculate_new_probabilistic_state(lts_aut_base::probabilistic_state ps)
    {
      lts_aut_base::probabilistic_state new_prob_state;
      std::map <state_type, probability_fraction_type> prob_state_map;

      /* Iterate over all state probability pairs in the selected probabilistic state*/
      for (const lts_aut_base::state_probability_pair &sp_pair : ps)
      {
        /* Check the resulting action state in the final State partition */
        state_type new_state = get_eq_class(sp_pair.state());

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
      for (std::map<state_type, probability_fraction_type>::iterator i = prob_state_map.begin(); i != prob_state_map.end(); i++)
      {
        new_prob_state.add(i->first, i->second);
      }

      return new_prob_state;
    }

    lts_aut_base::probabilistic_state calculate_equivalent_probabilistic_state(probabilistic_block_type& pb)
    {
      lts_aut_base::probabilistic_state equivalent_prob_state;

      /* Select the first probabilistic state of the step class */
      std::set<state_type>::iterator i = pb.states.begin();
      state_type s = *i;

      lts_aut_base::probabilistic_state old_prob_state = aut.probabilistic_state(s);

      equivalent_prob_state = calculate_new_probabilistic_state(old_prob_state);

      return equivalent_prob_state;
    }

};


/** \brief Reduce transition system l with respect to probabilistic bisimulation.
 * \param[in/out] l The transition system that is reduced.
 */
template < class LTS_TYPE>
void probabilistic_bisimulation_reduce(LTS_TYPE& l);


template < class LTS_TYPE>
void probabilistic_bisimulation_reduce_fast(LTS_TYPE& l)
{
  // Apply the probabilistic bisimulation reduction algorithm.
  detail::prob_bisim_partitioner_fast<LTS_TYPE> prob_bisim_part(l);

  // Clear the state labels of the LTS l
  l.clear_state_labels();

  // Assign the reduced LTS
  l.set_num_states(prob_bisim_part.num_eq_classes());
  //l.set_initial_state(prob_bisim_part.get_eq_class(l.initial_state()));
  prob_bisim_part.replace_transitions();
  prob_bisim_part.replace_probabilistic_states();
}

}
}
}
#endif //_LIBLTS_PBISIM_FAST_H
