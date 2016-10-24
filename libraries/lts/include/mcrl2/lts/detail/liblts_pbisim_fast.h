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
#define _LIBLTS_PBISIM_FAST_H
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
          LTS_TYPE& l) :aut(l)
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
          return a_constellations.size();
        }

        /** \brief Gives the bisimulation equivalence class number of a state.
        *  \param[in] s A state number.
        *  \return The number of the bisimulation equivalence class to which \e s belongs. */
        size_t get_eq_class(const size_t s)
        {
          assert(s < action_states_iter.size());
          return action_states_iter[s]->block_key;; // The block index is the state number of the block.
        }

        /** \brief Gives the bisimulation equivalence step class number of a probabilistic state.
        *  \param[in] s A probabilistic state number.
        *  \return The number of the step class to which d belongs. */
        size_t get_eq_step_class(const size_t s)
        {
          assert(s<probabilistic_states_iter.size());
          return probabilistic_states_iter[s]->block_key; // The block index is the state number of the block.
        }

        /** \brief Replaces the transition relation of the current lts by the transitions
        *         of the bisimulation reduced transition system.
        * \pre The bisimulation equivalence classes have been computed. */
        void replace_transitions()
        {
          std::set<transition> resulting_transitions;

          block_key_type block_key_of_initial_state = probabilistic_states_iter[aut.num_probabilistic_labels()]->block_key;//probabilistic_state_to_block_index[aut.num_probabilistic_labels()];
          probabilistic_block_type& block_of_initial_state = *probabilistic_blocks_iter[block_key_of_initial_state];
          ignore_block = probabilistic_blocks_iter.size(); //it will not ignore any in this case
          if (block_of_initial_state.states.size() == 1)
          {
            // only th einitial state is in the block, then we should ignor it
            ignore_block = block_key_of_initial_state;
          }

          const std::vector<transition>& trans = aut.get_transitions();
          for (const transition &t : trans)
          {
            size_t prob_equivalent_class = get_eq_step_class(t.to());
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

        /** \brief Replaces the probabilistic states of the current lts by the probabilistic
        *         states of the bisimulation reduced transition system.
        * \pre The bisimulation step classes have been computed. */
        void replace_probabilistic_states()
        {
          //std::vector<probabilistic_block_type>& p_blocks = pi_partition.probabilistic_blocks;
          std::vector<lts_aut_base::probabilistic_state> new_probabilistic_states;

          // get the equivalent probabilistic state of each probabilistic block and add it to aut
          for (typename std::list<probabilistic_block_type>::iterator& pb_iter : probabilistic_blocks_iter)
          {
            probabilistic_block_type& pb = *pb_iter;
            if (pb.key != ignore_block)
            {
              lts_aut_base::probabilistic_state equivalent_ps = calculate_equivalent_probabilistic_state(pb);
              new_probabilistic_states.push_back(equivalent_ps);
            }
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

        typedef size_t block_key_type;
        typedef size_t constellation_key_type;
        typedef size_t transition_key_type;
        typedef size_t state_key_type;
        typedef size_t label_type;
        typedef probabilistic_arbitrary_precision_fraction probability_label_type;
        typedef probabilistic_arbitrary_precision_fraction probability_fraction_type;

        struct state_type
        {
          state_key_type key;
          block_key_type block_key;
          std::vector<transition_key_type> incomming_transitions;
        };

        struct mark_type
        {
          size_t marked_block;  // to check if the block is marked
          std::list<state_type> left;
          std::vector< std::list<state_type> > middle;
          std::list<state_type> right;
          std::list<state_type>* large_block_ptr;
        };

        struct action_transition_type
        {
          transition_key_type key;
          state_key_type from;
          label_type label;
          state_key_type to;
          size_t block_to_constellation_count;
        };

        struct probabilistic_transition_type
        {
          transition_key_type key;
          state_key_type from;
          probability_label_type label;
          state_key_type to;
        };

        struct probabilistic_block_type
        {
          block_key_type key;
          std::list<state_type> states;
          constellation_key_type constellation_key;
          std::vector< std::list <action_transition_type> > incomming_action_transitions;  // a probabilistic block has incomming action transitions ordered by label
          std::vector<label_type> incomming_labels;
          mark_type mark;
        };

        struct action_block_type
        {
          block_key_type key;
          std::list<state_type> states;
          constellation_key_type constellation_key;
          std::list<probabilistic_transition_type> incomming_probabilistic_transitions;
          mark_type mark;
        };

        struct blocks_type {
          std::list<action_block_type> action_blocks;
          std::list<probabilistic_block_type> probabilistic_blocks;
        };

        struct action_constellation_type {
          constellation_key_type key;
          std::list<action_block_type> blocks;
          size_t size;
        };

        struct probabilistic_constellation_type {
          constellation_key_type key;
          std::list<probabilistic_block_type> blocks;
          size_t size;
        };

        struct global_type
        {
          std::list<action_constellation_type> action_constellations;
          std::list<probabilistic_constellation_type> probabilistic_constellations;
        };

        struct non_trivial_constellations_type
        {
          std::vector<constellation_key_type> action;
          std::vector<constellation_key_type> probabilistic;
        };



        global_type global;
        std::list<action_constellation_type>& a_constellations = global.action_constellations;
        std::list<probabilistic_constellation_type>& p_constellations = global.probabilistic_constellations;

        std::vector<typename std::list<state_type>::iterator> probabilistic_states_iter;
        std::vector<typename std::list<state_type>::iterator> action_states_iter;
        std::vector<typename std::list<action_block_type>::iterator> action_blocks_iter;
        std::vector<typename std::list<probabilistic_block_type>::iterator> probabilistic_blocks_iter;
        std::vector<typename std::list<action_constellation_type>::iterator> action_constellations_iter;
        std::vector<typename std::list<probabilistic_constellation_type>::iterator> probabilistic_constellations_iter;
        std::vector<typename std::list<action_transition_type>::iterator > action_transitions_iter;
        std::vector<typename std::list<probabilistic_transition_type>::iterator > probabilistic_transitions_iter;
        //non_trivial_constellations_type non_trivial_constellations;
        block_key_type ignore_block;

        LTS_TYPE& aut;

        /** \brief Creates the initial partition.
        *  \detal The blocks are initially partitioned based on the actions that can perform.
        *         The step classes are partitioned based on the action that leads to the probabilistic state */
        void create_initial_partition(void)
        {
          const std::vector<transition> & action_transitions = aut.get_transitions();
          const size_t num_action_states = aut.num_states();
          const size_t num_prob_states = aut.num_probabilistic_labels()+1; // plose one initial state
          const size_t num_action_labels = aut.num_action_labels();
          std::list<state_type> a_states;
          std::list<state_type> p_states;
          std::vector< std::list<action_transition_type> > a_transitions_per_label;
          std::list<probabilistic_transition_type> p_transitions;
          std::vector<label_type> incomming_labels;

          // Preprocessing initialization:
          //  - Initialize list of states and transitions with its keys and its iterators

          // Init action states
          a_states.resize(num_action_states);
          state_key_type key = 0;
          for (typename std::list<state_type>::iterator i = a_states.begin(); i != a_states.end(); ++i)
          {
            state_type& state = *i;
            state.key = key;
            action_states_iter.push_back(i);
            key++;
          }

          // Init probabilistic states
          p_states.resize(num_prob_states);
          key = 0;
          for (typename std::list<state_type>::iterator i = p_states.begin(); i != p_states.end(); ++i)
          {
            state_type& state = *i;
            state.key = key;
            probabilistic_states_iter.push_back(i);
            key++;
          }

          // Initialize action transitions per label
          key = 0;
          a_transitions_per_label.resize(num_action_labels);
          for (transition t : action_transitions)
          {
            action_transition_type at;
            at.key = key;
            at.from = t.from();
            at.label =  t.label();
            at.to = t.to();
            at.block_to_constellation_count = 0;

            if (a_transitions_per_label[at.label].size() == 0)
            {
              incomming_labels.push_back(at.label);  // keep track of labels
            }

            a_transitions_per_label[at.label].push_back(at);
            action_transitions_iter.push_back(--a_transitions_per_label[at.label].end());

            // init incomming action transitions in states
            state_type& s = *probabilistic_states_iter[at.to];
            s.incomming_transitions.push_back(at.key);

            key++;
          }

          // Initialize the probabilistic transitions.
          key = 0;
          for (size_t i = 0; i < num_prob_states - 1; i++)
          {
            const lts_aut_base::probabilistic_state& ps = aut.probabilistic_state(i);

            for (const lts_aut_base::state_probability_pair &sp_pair : ps)
            {
              probabilistic_transition_type pt;
              pt.key = key;
              pt.from = i;
              pt.label = sp_pair.probability();
              pt.to = sp_pair.state();
              p_transitions.push_back(pt);
              probabilistic_transitions_iter.push_back(--p_transitions.end());

              // init incomming action transitions in states
              state_type& s = *action_states_iter[pt.to];
              s.incomming_transitions.push_back(pt.key);

              key++;
            }
          }

          // Add transitions of the initial probabilistic state
          const lts_aut_base::probabilistic_state& initial_ps = aut.initial_probabilistic_state();
          for (const lts_aut_base::state_probability_pair &sp_pair : initial_ps)
          {
            probabilistic_transition_type pt;
            pt.key = key;
            pt.from = num_prob_states - 1;
            pt.label = sp_pair.probability();
            pt.to = sp_pair.state();
            p_transitions.push_back(pt);
            probabilistic_transitions_iter.push_back(--p_transitions.end());

            // init incomming action transitions in states
            state_type& s = *action_states_iter[pt.to];
            s.incomming_transitions.push_back(pt.key);

            key++;
          }

          // 1.3: Initialize pi partition and constellations
          a_constellations.resize(1);
          p_constellations.resize(1);
          action_constellation_type& a_constellation = a_constellations.back();
          probabilistic_constellation_type& p_constellation = p_constellations.back();
          action_constellations_iter.push_back(a_constellations.begin());
          probabilistic_constellations_iter.push_back(p_constellations.begin());

          a_constellation.key = 0;
          a_constellation.size = num_action_states;
          std::list<action_block_type>& action_pi_partition = a_constellation.blocks;
          p_constellation.key = 0;
          p_constellation.size = num_prob_states;
          std::list<probabilistic_block_type>& prob_pi_partition = p_constellation.blocks;
          
          //------------ 1.2: Initialize Pi Partition --------------//
          // Action Pi partition
          action_pi_partition.resize(1);
          action_block_type& a_block = action_pi_partition.back();
          a_block.key = 0;
          a_block.states.swap(a_states);
          a_block.constellation_key = 0;
          //a_block.incomming_probabilistic_transitions.swap(p_transitions);

          // split action partition based on outgoing transitions
          for (label_type l : incomming_labels)
          {
            std::vector <action_block_type> new_blocks;
            for (action_block_type& block_to_split : action_pi_partition) // Access all the blocks of action_partition to start splitting
            {
              //action_block_type new_a_block;
              std::list<state_type> new_a_block_states;
              for (action_transition_type& at : a_transitions_per_label[l])
              {
                state_type& s = *action_states_iter[at.from];
                if (s.block_key == block_to_split.key)
                {
                  //change s to a new block
                  new_a_block_states.splice(new_a_block_states.begin(), block_to_split.states, action_states_iter[s.key]);
                  s.block_key = action_pi_partition.size() + new_blocks.size();
                }
              }
              if (new_a_block_states.size() > 0) {
                // add to new blocks if it is not empty
                new_blocks.resize(new_blocks.size()+1);
                action_block_type& new_a_block = new_blocks.back();
                new_a_block.states.swap(new_a_block_states);
                new_a_block.key = action_pi_partition.size() + new_blocks.size() -1;
              }
            }
            // add new blocks to pi partition and add block_id to constellation
            for (action_block_type& b : new_blocks)
            {
              action_pi_partition.push_back(b);
              action_pi_partition.back().states.swap(b.states);
            }
          }

          // remove all empty blocks in action_pi_partition.
          std::vector<typename std::list<action_block_type>::iterator > empty_blocks;
          for (typename std::list<action_block_type>::iterator i = action_pi_partition.begin(); i != action_pi_partition.end(); ++i)
          {
            action_block_type block = *i;
            if (block.states.size() == 0)
            {
              empty_blocks.push_back(i);
            }
          }
          for (typename std::list<action_block_type>::iterator empty_i : empty_blocks)
          {
            action_pi_partition.erase(empty_i);
          }

          // Adjust the block key of all states and init incomming probabilistic transitions per block
          key = 0;
          for (typename std::list<action_block_type>::iterator i = action_pi_partition.begin(); i != action_pi_partition.end(); ++i)
          {
            action_block_type& ab = *i;
            for (state_type& s : ab.states)
            {
              s.block_key = key;
            }
            ab.key = key;
            action_blocks_iter.push_back(i);
            key++;
          }

          for (typename std::list<probabilistic_transition_type>::iterator pt_iter : probabilistic_transitions_iter)
          {
            probabilistic_transition_type& pt = *pt_iter;
            state_type& s = *action_states_iter[pt.to];
            action_block_type& ab = *action_blocks_iter[s.block_key];
            ab.incomming_probabilistic_transitions.splice(ab.incomming_probabilistic_transitions.begin(), 
                                                          p_transitions, pt_iter);
          }

          // Probabilistic Pi partition
          prob_pi_partition.resize(1);
          probabilistic_block_type& p_block = prob_pi_partition.back();
          probabilistic_blocks_iter.push_back(prob_pi_partition.begin());
          p_block.key = 0;
          p_block.states.swap(p_states);
          p_block.constellation_key = 0;
          p_block.incomming_action_transitions.swap(a_transitions_per_label);
          p_block.incomming_labels.swap(incomming_labels);

          //1.4: Initialise the block to constellation count for each tranition.
          // Initially there is only one constellation, so we only need to count 
          // the number of outgoing transitions per label of each state
          std::vector<size_t> block_to_const_count_temp; 
          block_to_const_count_temp.resize(num_action_states);

          // Iterate over all transition of each label
          for (std::list <action_transition_type>& at_list_per_label : p_block.incomming_action_transitions)
          {
            for (action_transition_type& at : at_list_per_label)
            {
              block_to_const_count_temp[at.from]++;
            }
            for (action_transition_type& at : at_list_per_label)
            {
              at.block_to_constellation_count = block_to_const_count_temp[at.from];
            }
            for (action_transition_type& at : at_list_per_label)
            {
              block_to_const_count_temp[at.from] = 0;
            }
          }

          // Add action block to not trivial constellation
          //if (action_pi_partition.size() > 1)
          //{
          //  non_trivial_constellations.action.push_back(0); // add initial constellation to non trivials
          //}
        }

        /* */
        void refine_partition_until_it_becomes_stable(void)
        {
          // refine until all the constellations are trivial
          while (a_constellations.front().blocks.size() > 1 || p_constellations.front().blocks.size() > 1)
          {
            // Refine probabilistic blocks if a non-trivial action constellation exists
            if (a_constellations.front().blocks.size() > 1)
            {
              // take back non_trivial constellation
              action_constellation_type& nt_action_const = a_constellations.front();
              constellation_key_type nt_const_key = nt_action_const.key;

              // Choose splitter block Bc of a non-trivial constellation C, such that |Bc| <= 1/2|C|
              action_block_type* Bc_ptr = choose_action_splitter(nt_const_key);
              action_block_type& Bc = *Bc_ptr;

              // Check if the constellation is still non-trivial, if not remove from non-trivial constellations set
              if (nt_action_const.blocks.size() < 2)
              {
                //the constellation is trivial, send to the back of the list
                a_constellations.splice(a_constellations.end(), a_constellations, action_constellations_iter[nt_const_key]);
              }

              //derive the left, right and middle sets from mark function
              std::vector<block_key_type> marked_blocks_keys;
              mark_probabilistic(Bc, marked_blocks_keys);

              //Split every probabilistic block based on left, middle and right
              for (block_key_type B_key : marked_blocks_keys)
              {
                probabilistic_block_type& B = *probabilistic_blocks_iter[B_key];
                size_t unstable_const = 0;

                // First return the largest of left, middle or right to the states of the block B
                B.states.swap(*B.mark.large_block_ptr);
                
                // Split left block
                if (B.mark.left.size() != 0 &&
                  B.mark.large_block_ptr != &B.mark.left)
                {
                  
                  split_probabilistic_block(B, B.mark.left);
                  unstable_const = 1;
                }

                // add right block
                if (B.mark.right.size() != 0 &&
                  B.mark.large_block_ptr != &B.mark.right)
                {
                  split_probabilistic_block(B, B.mark.right);
                  unstable_const = 1;
                }

                // Add the middle blocks
                for (std::list<state_type>& middle_i : B.mark.middle)
                {
                  if (middle_i.size() != 0 &&
                    B.mark.large_block_ptr != &middle_i)
                  {
                    split_probabilistic_block(B, middle_i);
                    unstable_const = 1;
                  }
                }

                B.mark.marked_block = 0;
                // add the constellation where the splitted block is located to the set of non-trivial constellations
                if (unstable_const == 1)
                {
                  //non_trivial_constellations.probabilistic.push_back(B.constellation_key);
                  p_constellations.splice(p_constellations.begin(), p_constellations, 
                                            probabilistic_constellations_iter[B.constellation_key]);
                }
                
              }
            }

            // Refine action blocks if a non-trivial probabilistic constellation exists
            if (p_constellations.front().blocks.size()  > 1)
            {
              // take back of non_trivial constellation
              probabilistic_constellation_type& nt_probabilistic_const = p_constellations.front();
              constellation_key_type nt_const_key = nt_probabilistic_const.key;

              // Choose splitter block Bc of a non-trivial constellation C, such that |Bc| <= 1/2|C|
              probabilistic_block_type* Bc_ptr = choose_probabilistic_splitter(nt_const_key);
              probabilistic_block_type& Bc = *Bc_ptr;

              // Check if the constellation is still non-trivial, if not remove from non-trivial constellations set
              if (nt_probabilistic_const.blocks.size() < 2)
              {
                //the constellation is trivial, send to the back of the list
                p_constellations.splice(p_constellations.end(), p_constellations, probabilistic_constellations_iter[nt_const_key]);
              }

              // for all incoming actions a for each state in BC call the mark function and split the blocks
              for (label_type a :Bc.incomming_labels)
              {
                //derive the left, right and middle sets from mark function
                std::vector<block_key_type> marked_blocks_keys;
                mark_action(Bc, marked_blocks_keys, a);

                //Split every probabilistic block based on left, middle and right
                for (block_key_type B_key : marked_blocks_keys)
                {
                  action_block_type& B = *action_blocks_iter[B_key];
                  size_t unstable_const = 0;

                  // First return the largest of left, middle or right to the states of the block B
                  B.states.swap(*B.mark.large_block_ptr);

                  // Split left block
                  if (B.mark.left.size() != 0 &&
                    B.mark.large_block_ptr != &B.mark.left)
                  {

                    split_action_block(B, B.mark.left);
                    unstable_const = 1;
                  }

                  // add right block
                  if (B.mark.right.size() != 0 &&
                    B.mark.large_block_ptr != &B.mark.right)
                  {
                    split_action_block(B, B.mark.right);
                    unstable_const = 1;
                  }

                  // Add the middle blocks
                  for (std::list<state_type>& middle_i : B.mark.middle)
                  {
                    if (middle_i.size() != 0 &&
                      B.mark.large_block_ptr != &middle_i)
                    {
                      split_action_block(B, middle_i);
                      unstable_const = 1;
                    }
                  }

                  B.mark.marked_block = 0;
                  // add the constellation where the splitted block is located to the set of non-trivial constellations
                  if (unstable_const == 1)
                  {
                    //non_trivial_constellations.action.push_back(B.constellation_key);
                    a_constellations.splice(a_constellations.begin(), a_constellations,
                      action_constellations_iter[B.constellation_key]);
                  }

                }

              }

            }

          }
        }

        void split_probabilistic_block(probabilistic_block_type& B, std::list<state_type>& states_of_new_block)
        {
          probabilistic_constellation_type& p_const_of_B = *probabilistic_constellations_iter[B.constellation_key];
          p_const_of_B.blocks.resize(p_const_of_B.blocks.size() + 1);
          probabilistic_blocks_iter.push_back(--p_const_of_B.blocks.end());
          probabilistic_block_type& new_block = p_const_of_B.blocks.back();
          new_block.key = probabilistic_blocks_iter.size()-1;//p_const_of_B.blocks.size() - 1;
          new_block.constellation_key = B.constellation_key;
          new_block.states.swap(states_of_new_block);
          new_block.incomming_action_transitions.resize(B.incomming_action_transitions.size());

          // iterate over all incomming transitions of each state of the new block
          for (state_type& s : new_block.states)
          {
            s.block_key = new_block.key;
            for (transition_key_type t_key : s.incomming_transitions)
            {
              action_transition_type& at = *action_transitions_iter[t_key];
              
              if (new_block.incomming_action_transitions[at.label].size() == 0)
              {
                new_block.incomming_labels.push_back(at.label);
              }
              
              //move transition from previous block to new block
              new_block.incomming_action_transitions[at.label].splice(new_block.incomming_action_transitions[at.label].begin(),
                                                                     B.incomming_action_transitions[at.label],
                                                                     action_transitions_iter[t_key]);
              
            }
          }

          // Update the incomming labels of B
          std::vector<label_type> old_incomming_labels;
          B.incomming_labels.swap(old_incomming_labels);

          for (label_type l : old_incomming_labels)
          {
            if (B.incomming_action_transitions[l].size() != 0)
            {
              B.incomming_labels.push_back(l);
            }
          }

        }

        void split_action_block(action_block_type& B, std::list<state_type>& states_of_new_block)
        {
          action_constellation_type& a_const_of_B = *action_constellations_iter[B.constellation_key];
          a_const_of_B.blocks.resize(a_const_of_B.blocks.size() + 1);
          action_blocks_iter.push_back(--a_const_of_B.blocks.end());
          action_block_type& new_block = a_const_of_B.blocks.back();
          new_block.key = action_blocks_iter.size() - 1;//a_const_of_B.blocks.size() - 1;
          new_block.constellation_key = B.constellation_key;
          new_block.states.swap(states_of_new_block);

          // iterate over all incomming transitions of each state of the new block
          for (state_type& s : new_block.states)
          {
            s.block_key = new_block.key;
            for (transition_key_type t_key : s.incomming_transitions)
            {
              probabilistic_transition_type& pt = *probabilistic_transitions_iter[t_key];

              //move transition from previous block to new block
              new_block.incomming_probabilistic_transitions.splice(new_block.incomming_probabilistic_transitions.begin(),
                                                                       B.incomming_probabilistic_transitions,
                                                                       probabilistic_transitions_iter[t_key]);

            }
          }

        }

        void mark_probabilistic(action_block_type& Bc, std::vector<block_key_type>& marked_blocks)
        {
          std::vector< std::vector<state_type> > left_temp;
          std::vector<probability_label_type> commulative_probability;
          std::vector<probability_label_type> max_commulative_probability;
          std::vector<size_t> marked_states;
          commulative_probability.resize(probabilistic_states_iter.size());
          marked_states.resize(probabilistic_states_iter.size());
          max_commulative_probability.resize(probabilistic_blocks_iter.size());
          left_temp.resize(probabilistic_blocks_iter.size());
          
         
          // for all incomming probabilistic transitions of block BC calculate left, middle and right
          for (probabilistic_transition_type& pt : Bc.incomming_probabilistic_transitions)
          {
            state_type& u = *probabilistic_states_iter[pt.from];
            probability_label_type p = pt.label;
            probabilistic_block_type& B = *probabilistic_blocks_iter[u.block_key];
            // if the block was not previously marked then mark the block and add all states to right
            if (B.mark.marked_block == 0)
            {
              B.mark.marked_block = 1;
              marked_blocks.push_back(B.key);
              B.mark.right.swap(B.states);
              B.mark.large_block_ptr = &B.mark.right;
              max_commulative_probability[B.key] = p;
            }
            // if u is not yet in left, then init commulative probability and move u from right to left
            if (marked_states[u.key] == 0)
            {
              marked_states[u.key] = 1;
              commulative_probability[u.key] = p;

              B.mark.left.splice(B.mark.left.begin(), B.mark.right, probabilistic_states_iter[u.key]);
              left_temp[B.key].push_back(u);
            }
            else {
              // u was already added to left, then just add its commulative probability
              commulative_probability[u.key] = commulative_probability[u.key] + p;
            }

            if (max_commulative_probability[B.key] < commulative_probability[u.key])
            {
              max_commulative_probability[B.key] = commulative_probability[u.key];
            }
          }

          // Group all states with the same commulative probability to construct the middle set
          for (block_key_type B_key : marked_blocks)
          {
            probabilistic_block_type& B = *probabilistic_blocks_iter[B_key];
            std::unordered_map<probability_label_type, std::list<state_type> >  grouped_probabilities_in_block;
            for (state_type& u : left_temp[B_key])
            {
              if (commulative_probability[u.key] < max_commulative_probability[B.key])
              {
                // group u by its probability and erase from left set
                grouped_probabilities_in_block[commulative_probability[u.key]].splice(
                                                             grouped_probabilities_in_block[commulative_probability[u.key]].begin(),
                                                             B.mark.left, probabilistic_states_iter[u.key]);
              }
            }

            // construct the middle set based on the gropued probabilities
            B.mark.middle.resize(grouped_probabilities_in_block.size());
            size_t middle_key = 0;
            for (typename std::unordered_map<probability_label_type, std::list<state_type> >::iterator i = grouped_probabilities_in_block.begin();
                    i != grouped_probabilities_in_block.end(); i++)
            {
              B.mark.middle[middle_key].swap(i->second);
              middle_key++;
            }

            // find the large set
            if (B.mark.left.size() > B.mark.large_block_ptr->size())
            {
              B.mark.large_block_ptr = &B.mark.left;
            }

            for (std::list<state_type>& middle_set : B.mark.middle)
            {
              if (middle_set.size() > B.mark.large_block_ptr->size())
              {
                B.mark.large_block_ptr = &middle_set;
              }
            }
          }

        }

        void mark_action(probabilistic_block_type& Bc, std::vector<block_key_type>& marked_blocks, label_type a)
        {
          std::vector< std::vector<state_type> > left_temp;
          std::vector<size_t> residual_transition_cnt;
          std::vector<size_t> marked_states;
          marked_states.resize(action_states_iter.size());
          residual_transition_cnt.resize(action_states_iter.size());
          left_temp.resize(action_blocks_iter.size());

          // for all incomming a transitions of block Bc calculate left, middle and right
          for (action_transition_type& at : Bc.incomming_action_transitions[a])
          {
            state_type& u = *action_states_iter[at.from];
            action_block_type& B = *action_blocks_iter[u.block_key];  //get the block B where state u is located
            // if the block was not previously marked then mark the block and add all states to right
            if (B.mark.marked_block == 0)
            {
              B.mark.marked_block = 1;
              marked_blocks.push_back(B.key);
              B.mark.right.swap(B.states);
              B.mark.large_block_ptr = &B.mark.right;
            }

            // if u is not yet in left, then init commulative probability and move u from right to left
            if (marked_states[u.key] == 0)
            {
              marked_states[u.key] = 1;
              residual_transition_cnt[u.key] = at.block_to_constellation_count;
              B.mark.left.splice(B.mark.left.begin(), B.mark.right, action_states_iter[u.key]);
              left_temp[B.key].push_back(u);
            }

            residual_transition_cnt[u.key]--;
          }

          // Group all states which its residual_transition_cnt is not zero to form the middle set
          for (block_key_type B_key : marked_blocks)
          {
            action_block_type& B = *action_blocks_iter[B_key];

            // iterate over all states of B.mark.left
            for (state_type& u : left_temp[B_key])
            {
              if (residual_transition_cnt[u.key] > 0)
              {
                if (B.mark.middle.size() == 0) 
                {
                  B.mark.middle.resize(1);
                }
                // the residual count is not zero, then move to middle_set
                B.mark.middle[0].splice(B.mark.middle[0].begin(), B.mark.left, action_states_iter[u.key]);
              }
            }

            // finde the large set
            if (B.mark.left.size() > B.mark.large_block_ptr->size())
            {
              B.mark.large_block_ptr = &B.mark.left;
            }

            for (std::list<state_type>& middle_set : B.mark.middle)
            {
              if (middle_set.size() > B.mark.large_block_ptr->size())
              {
                B.mark.large_block_ptr = &middle_set;
              }
            }
          }

        }

        action_block_type* choose_action_splitter(constellation_key_type constellation_key)
        {
          action_constellation_type& nt_action_const = *action_constellations_iter[constellation_key];
          a_constellations.resize(a_constellations.size() + 1);
          action_constellation_type& new_action_const = a_constellations.back();
          action_constellations_iter.push_back(--a_constellations.end());
          action_block_type* block_to_split;
         // block_key_type block_to_split_key;
            
          //assing key of new constellation
          new_action_const.key = a_constellations.size() - 1;

          // Determine the block to split from constellation it is |Bc| < 1/2|C|
          block_to_split = &nt_action_const.blocks.front();
          if (block_to_split->states.size() > (nt_action_const.size / 2))
          {
            // Choose another block to split
            block_to_split = &nt_action_const.blocks.back();
          }

          // split constellation C into Bc and C\Bc. 
          new_action_const.blocks.splice(new_action_const.blocks.begin(), nt_action_const.blocks, action_blocks_iter[block_to_split->key]);
          new_action_const.size = block_to_split->states.size();
          block_to_split->constellation_key = new_action_const.key;
          nt_action_const.size -= block_to_split->states.size();

          return block_to_split;
        }

        probabilistic_block_type* choose_probabilistic_splitter(constellation_key_type constellation_key)
        {
          probabilistic_constellation_type& nt_probabilistic_const = *probabilistic_constellations_iter[constellation_key];
          p_constellations.resize(p_constellations.size() + 1);
          probabilistic_constellation_type& new_probabilistic_const = p_constellations.back();
          probabilistic_constellations_iter.push_back(--p_constellations.end());
          probabilistic_block_type* block_to_split;
          // block_key_type block_to_split_key;

          //assing key of new constellation
          new_probabilistic_const.key = p_constellations.size() - 1;

          // Determine the block to split from constellation it is |Bc| < 1/2|C|
          block_to_split = &nt_probabilistic_const.blocks.front();
          if (block_to_split->states.size() > (nt_probabilistic_const.size / 2))
          {
            // Choose another block to split
            block_to_split = &nt_probabilistic_const.blocks.back();
          }

          // split constellation C into Bc and C\Bc. 
          new_probabilistic_const.blocks.splice(new_probabilistic_const.blocks.begin(), nt_probabilistic_const.blocks, probabilistic_blocks_iter[block_to_split->key]);
          new_probabilistic_const.size = block_to_split->states.size();
          block_to_split->constellation_key = new_probabilistic_const.key;
          nt_probabilistic_const.size -= block_to_split->states.size();

          return block_to_split;
        }

        lts_aut_base::probabilistic_state calculate_new_probabilistic_state(lts_aut_base::probabilistic_state ps)
        {
          lts_aut_base::probabilistic_state new_prob_state;
          std::map <state_key_type, probability_fraction_type> prob_state_map;

          /* Iterate over all state probability pairs in the selected probabilistic state*/
          for (const lts_aut_base::state_probability_pair &sp_pair : ps)
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

          /* Select the first probabilistic state of the step class */
          typename std::list<state_type>::iterator i = pb.states.begin();
          state_type& s = *i;

          lts_aut_base::probabilistic_state old_prob_state = aut.probabilistic_state(s.key);

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
        prob_bisim_part.replace_transitions();
        prob_bisim_part.replace_probabilistic_states();
      }

    }
  }
}
#endif //_LIBLTS_PBISIM_FAST_H
