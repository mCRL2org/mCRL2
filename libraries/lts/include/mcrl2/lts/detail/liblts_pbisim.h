// Author(s): Hector Joao Rivera Verduzco
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/liblts_pbisim.h

#ifndef _LIBLTS_PBISIM_H
#define _LIBLTS_PBISIM_H
#include <cmath>
#include <vector>
#include <map>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lts/lts_aut.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{

template < class LTS_TYPE>
class prob_bisim_partitioner
{

  public:
    /** \brief Creates a probabilistic bisimulation partitioner for an PLTS.
     *  \details This bisimulation partitioner applies the algorithm defined in C. Baier, B. Engelen and M. Majster-Cederbaum. 
	   *	 Deciding Bisimilarity and Similarity for Probabilistic Processes. In Journal of Computer and System Sciences 60, 187-237 (2000)
	   */
    prob_bisim_partitioner(
      LTS_TYPE& l):aut(l)
    {
      mCRL2log(log::verbose) << "Probabilistic bisimulation partitioner created for "
                  << l.num_states() << " states and " <<
                  l.num_transitions() << " transitions\n";
      create_initial_partition();
      refine_partition_until_it_becomes_stable();
      postprocessing_stage();
    }


    /** \brief Destroys this partitioner. */
    ~prob_bisim_partitioner()
    {}

    /** \brief Gives the number of bisimulation equivalence classes of the LTS.
    *  \return The number of bisimulation equivalence classes of the LTS.
    */
    size_t num_eq_classes() const
    {
      return state_partition.size();
    }

	  /** \brief Gives the bisimulation equivalence class number of a state.
     *  \param[in] s A state number.
     *  \return The number of the bisimulation equivalence class to which \e s belongs. */
    size_t get_eq_class(const size_t s) const
    {
      assert(s<block_index_of_a_state.size());
      return block_index_of_a_state[s]; // The block index is the state number of the block.
    }

	  /** \brief Gives the bisimulation equivalence step class number of a probabilistic state.
	  *  \param[in] s A probabilistic state number.
	  *  \return The number of the step class to which s belongs. */
	  size_t get_eq_step_class(const size_t d) const
	  {
		  assert(d < step_class_index_of_a_distribution.size());
      assert(step_class_index_of_a_distribution[d] < step_classes.size());
      assert(step_classes[step_class_index_of_a_distribution[d]].equivalent_step_class < step_classes.size());
		  return step_classes[step_class_index_of_a_distribution[d]].equivalent_step_class;
	  }

    /** \brief Replaces the transition relation of the current lts by the transitions
    *         of the bisimulation reduced transition system.
    * \pre The bisimulation equivalence classes have been computed. */
    void replace_transitions()
    {
      std::set < transition > resulting_transitions;

      const std::vector<transition>& trans = aut.get_transitions();
      for (const transition &t : trans)
      {
        resulting_transitions.insert(
          transition(
            get_eq_class(t.from()),
            t.label(),
            get_eq_step_class(t.to())));
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
      std::vector<lts_aut_base::probabilistic_state> new_probabilistic_states;

      // get the equivalent probabilistic state of each probabilistic block and add it to aut
      for (step_class_type& sc : step_classes)
      {
        lts_aut_base::probabilistic_state equivalent_ps;
        if (sc.distributions.size() > 0)
        {
          equivalent_ps = calculate_equivalent_probabilistic_state(sc);
        }
        else
        {
          equivalent_ps.add(0, probability_fraction_type().one()); //dummy
        }
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

  private:

	typedef size_t block_key_type;
  typedef size_t step_class_key_type;
  typedef size_t state_type;
  typedef size_t label_type;
	typedef size_t distribution_key_type;
	typedef probabilistic_arbitrary_precision_fraction probability_fraction_type;

  struct distribution_type
  {
    distribution_key_type key;
    std::vector< std::list<transition*> > incoming_transitions_per_label; // Incoming transitions organized per label
  };
  
	struct block_type
  {
    block_key_type key;
    std::list<state_type> states;			  // The states in the block
    bool is_in_new_blocks;
  };

	struct step_class_type {
    step_class_key_type key;
	  label_type action;								                // action label of the pair <a,M>.
	  std::list<distribution_type*> distributions;	    // The distributions in the step class <a,M>.
    std::vector< bool > prev_states;				          // Previous states that can reach step_class <a,M>
    bool is_in_new_step_classes;
    size_t equivalent_step_class;
	};

  std::list<block_type*> state_partition;
  std::list<step_class_type*> step_partition;
  std::vector<distribution_type> distributions;
  std::deque<step_class_type> step_classes;
  std::deque<block_type> blocks;
  std::vector<block_key_type> block_index_of_a_state;
  std::vector<step_class_key_type> step_class_index_of_a_distribution;

	LTS_TYPE& aut;

  struct tree_type
  {
    std::list<state_type> states;
    size_t count;
    tree_type* left;
    tree_type* right;

    void init_node(size_t depth_node)
    {
      count = 0;
      left = NULL;
      right = NULL;
    }
  };

  /** \brief Creates the initial partition of step classes and blocks.
   *  \detail The blocks are initially partitioned based on the actions that can perform.
   *          The step classes are partitioned based on the action that leads to the probabilistic state */
  void create_initial_partition (void) 
  {
    std::vector< std::vector< std::list<distribution_type*> > > steps; // Representation of transition in 2-d array
    std::vector<transition> & transitions = aut.get_transitions();
    std::vector< std::vector<bool> > distribution_per_step_class;
    
    distribution_per_step_class.resize(aut.num_action_labels());
    for (std::vector<bool>& i : distribution_per_step_class)
    {
      i.resize(aut.num_probabilistic_labels());
    }

    //---- Preprocessing stage to transform the PLTS to the data structures suggested by Baier ---- //

    // Construct vector of distributions
    distributions.resize(aut.num_probabilistic_labels());
    size_t key = 0;
    for (distribution_type& distribution : distributions)
    {
      distribution.key = key;
      distribution.incoming_transitions_per_label.resize(aut.num_action_labels());
      key++;
    }

    // Initialize the Steps bi-dimientional array
    steps.resize(aut.num_states());
    for (size_t i = 0; i < steps.size(); i++)
    {
      steps[i].resize(aut.num_action_labels());
    }

    for (transition& t : transitions)
    {
      steps[t.from()][t.label()].push_back(&distributions[t.to()]);
      distributions[t.to()].incoming_transitions_per_label[t.label()].push_back(&t);
    }

    //---- Start the initialization process (page 207. Fig. 10. Baier) ---- //

    // Initially there are as many step classes as lables
    step_classes.resize(aut.num_action_labels());

    for (step_class_type& sc : step_classes)
    {
      sc.prev_states.resize(aut.num_states());
    }

    // create tree structure to group action states based on the outgoing transitions
    size_t max_block_size = 0;
    tree_type* max_block;
    std::deque<tree_type> tree_nodes;
    std::vector<tree_type*> leaves;
    tree_type v0;
    v0.init_node(0);
    tree_nodes.push_back(v0);

    // For all s in S do
    for (state_type s = 0; s < aut.num_states(); s++)
    {
      // (1) Create pointer to root of the tree
      tree_type* v = &tree_nodes[0];

      // (2) Construct tree
      for (size_t i = 0; i < aut.num_action_labels(); i++)
      {
        step_classes[i].key = i;
        step_classes[i].action = i;
        if (steps[s][i].size() > 0)
        {
          for (distribution_type* d : steps[s][i])
          {
            if (distribution_per_step_class[i][d->key] == false)
            {
              step_classes[i].distributions.push_back(d);
              distribution_per_step_class[i][d->key] = true;
            }
          }
          
          step_classes[i].prev_states[s] = true;

          if (v->left == NULL)
          {
            // create left node
            tree_type w;
            w.init_node(i+1);
            tree_nodes.push_back(w);
            v->left = &tree_nodes.back();

            // add new node to the leaf nodes if it is a leaf
            if (i == aut.num_action_labels() - 1)
            {
              leaves.push_back(v->left);
            }

          }
          v = v->left;
        }
        else
        {
          if (v->right == NULL)
          {
            // create left node
            tree_type w;
            w.init_node(i + 1);
            tree_nodes.push_back(w);
            v->right = &tree_nodes.back();

            // add new node to the leaf nodes if it is a leaf
            if (i == aut.num_action_labels() - 1)
            {
              leaves.push_back(v->right);
            }
          }
          v = v->right;
        }
      }

      // (3) Add the state to the leaf and increment its state counter
      v->states.push_back(s);
      v->count++;

      // (4) Keep track of the leave containing more states
      if (v->count > max_block_size)
      {
        max_block = v;
        max_block_size = v->count;
      }
    }

    // insert all states of the leaves to blocks
    blocks.resize(leaves.size());

    key = 0;
    size_t larger_key = 0;
    for (tree_type* leaf_ptr : leaves)
    {
      block_type& block = blocks[key];
      block.key = key;
      block.states.splice(block.states.end(), leaf_ptr->states);

      if (leaf_ptr == max_block)
      {
        larger_key = key;
      }

      key++;
    }

    // Add all blocks to the state partition.
    // Initially only the larger block has to be at the end of the list
    for (block_type& b : blocks)
    {
      if (b.key != larger_key)
      {
        // Push the non-larger blocks to the front of the list. This are the so called new blocks
        b.is_in_new_blocks = true;
        state_partition.push_front(&b);
      }
      else
      {
        // push the larger block to the end of the list
        b.is_in_new_blocks = false;
        state_partition.push_back(&b);
      }
    }
    
    // Add all step classes to the step partition.
    // Initially all are new step classes
    for (step_class_type& sc : step_classes)
    {
      if (sc.distributions.size() > 0)
      { 
        step_partition.push_front(&sc);
        sc.is_in_new_step_classes = true;
      }
    }

	}

  /** \brief Calculates the probability to reach block b from ditribution d.
  *   \param[in] d is a probabilistic state (distribution).
  *              b is a block of states.
  *   \return The probability to reach block b. */
  probability_fraction_type probability_to_block(distribution_type& d, block_type& b)
  {
    // iterating over all states in block b and in all probability pair of d is inneficient;
    // however, this achives the time complexity described by Baier of O(m|C|) to calculate all the
    // probabilities to block C. See page 208, proof of Lemma 4.8 for more details.

    probability_fraction_type prob_to_block;
    const lts_aut_base::probabilistic_state& prob_state = aut.probabilistic_state(d.key);

    /* Iterate over all states of block b. Check wether the state is in the
    distribution d and add up the probability*/
    for (const state_type &s : b.states)
    {
      for (const lts_aut_base::state_probability_pair &prob_pair : prob_state)
      {
        if (prob_pair.state() == s)
        {
          prob_to_block = prob_to_block + prob_pair.probability();
        }
      }
    }

    return prob_to_block;
  }

  lts_aut_base::probabilistic_state calculate_new_probabilistic_state(lts_aut_base::probabilistic_state ps)
  {
    lts_aut_base::probabilistic_state new_prob_state;
    std::map<state_type, probability_fraction_type> prob_state_map;

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
    for (const std::pair<state_type, probability_fraction_type>& i : prob_state_map)
    {
      new_prob_state.add(i.first, i.second);
    }

    return new_prob_state;
  }

  lts_aut_base::probabilistic_state calculate_equivalent_probabilistic_state(step_class_type& sc)
  {
    lts_aut_base::probabilistic_state equivalent_prob_state;

    /* Select the first probabilistic state of the step class */
    distribution_type* d = sc.distributions.front();

    lts_aut_base::probabilistic_state old_prob_state = aut.probabilistic_state(d->key);

    equivalent_prob_state = calculate_new_probabilistic_state(old_prob_state);

    return equivalent_prob_state;
  }

  /** \brief  Two-phased partitioning algorithm described in page 204. Fig 9. Baier.
  *   \detail Refinement of state partition and step partition until no new blocks/step classes
  *           are in front of the partition lists
  */
	void refine_partition_until_it_becomes_stable (void) 
  {
    std::list<step_class_type*> step_partition_old;
    std::list<block_type*> state_partition_old;

    // Repeat until no new blocks in front of the partition lists
    while (state_partition.front()->is_in_new_blocks == true || step_partition.front()->is_in_new_step_classes == true)
    { 
      // Phase 1: Splitting of step_partition via split(M,C)
      if (state_partition.front()->is_in_new_blocks == true)
      {
        // Choose a new block in front of the state partition and change it to the back of the list
        block_type* c_block = state_partition.front();
        state_partition.pop_front();
        state_partition.push_back(c_block);
        c_block->is_in_new_blocks = false;

        // swap elements of step_partition (M) to step_partition_old (M_old)
        step_partition_old.swap(step_partition);

        // vector to add the new step classes
        std::vector<typename std::list<step_class_type*>::iterator> pending_new_step_classes;

        // iterate over all step classes
        for (typename std::list<step_class_type*>::iterator sc_iter = step_partition_old.begin(); 
          sc_iter != step_partition_old.end(); ++sc_iter)
        {
          step_class_type* sc_ptr = *sc_iter;

          // Mapping to sort the distributions based on its probability to reach a block, instead of using
          // an unordered balanced tree as suggsted in Baier
          std::map< probability_fraction_type, std::list<distribution_type*> > distributions_ordered_by_prob;

          // Iterate over all distributions d of the step class and add probability to block to vector
          for (distribution_type* d : sc_ptr->distributions)
          {
            probability_fraction_type probability = probability_to_block(*d, *c_block);
            distributions_ordered_by_prob[probability].push_back(d);
          }

          // if there are multiple elements in the distributions_ordered_by_prob then we have to 
          // add them to the step partition as a new step class
          if (distributions_ordered_by_prob.size() >= 2)
          {
            step_class_type* new_step_class_ptr;

            step_classes.resize(step_classes.size() + distributions_ordered_by_prob.size() - 1);
            // iterate over mapping
            size_t new_class_count = 0;
            for (std::pair< probability_fraction_type, std::list<distribution_type*> > ordered_dist : distributions_ordered_by_prob)
            {
              std::list<distribution_type*>& distribution_list = ordered_dist.second;
              
              if (new_class_count == 0)
              {
                // it it is the first element of the mapping then we do not creat a new step class; instead, we 
                // add its elements into the current step class that is being splitted
                sc_ptr->distributions.swap(distribution_list);

                // clear the prev states of the current step class
                for (size_t i = 0; i < sc_ptr->prev_states.size(); i++)
                {
                  sc_ptr->prev_states[i] = false;
                }

                // recalculate prev states based ont incomming transitions of each distribution
                for (distribution_type* d : sc_ptr->distributions)
                {
                  for (transition* t_ptr : d->incoming_transitions_per_label[sc_ptr->action])
                  {
                    sc_ptr->prev_states[t_ptr->from()] = true;
                  }
                }

                // add to the front of the step partition (as a new step class) if not yet added 
                if (sc_ptr->is_in_new_step_classes == false)
                {
                  // since we are iterating over this step class, we save its iterator in a pending vector,
                  // so later we can add it to the front of the step partition (outside the loop)
                  pending_new_step_classes.push_back(sc_iter);
                  sc_ptr->is_in_new_step_classes = true;
                }
              }
              else
              {
                new_step_class_ptr = &step_classes[step_classes.size() - new_class_count];

                //init new step class
                new_step_class_ptr->key = step_classes.size() - new_class_count;
                new_step_class_ptr->action = sc_ptr->action;
                new_step_class_ptr->is_in_new_step_classes = true;
                new_step_class_ptr->prev_states.resize(aut.num_states());
                new_step_class_ptr->distributions.swap(distribution_list);

                // recalculate prev states based ont incomming transitions of each distribution
                for (distribution_type* d : new_step_class_ptr->distributions)
                {
                  for (transition* t_ptr : d->incoming_transitions_per_label[new_step_class_ptr->action])
                  {
                    new_step_class_ptr->prev_states[t_ptr->from()] = true;
                  }
                }

                // add new step class to step partition and new_step_classes
                step_partition.push_front(new_step_class_ptr);
              }
              new_class_count++;
            }
          }
        }

        // Add the pending new blocks to the front of the list
        for (typename std::list<step_class_type*>::iterator sc_iter : pending_new_step_classes)
        {
          step_partition.splice(step_partition.begin(), step_partition_old, sc_iter);
        }

        // more remaining step classes to the end of step partition
        step_partition.splice(step_partition.end(), step_partition_old);
      }

      // Phase 2: Refinment of state_partition via Refine(X,a,M)
      if (step_partition.front()->is_in_new_step_classes == true)
      {
        // Choose some step class <a,M> in new_step_classes and remove it from new_step_classes
        step_class_type* step_class = step_partition.front();
        step_partition.pop_front();
        step_class->is_in_new_step_classes = false;
        step_partition.push_back(step_class);

        // swap elements of state_partition (X) to state_partition_old (X_old)
        state_partition_old.swap(state_partition);

        std::vector<typename std::list<block_type*>::iterator > blocks_to_move_to_front;
        std::vector<block_type*> new_blocks_to_move_to_front;
        // for all blocks B in X_old
        //for (block_type* b_to_split : state_partition_old)
        for (typename std::list<block_type*>::iterator block_iter = state_partition_old.begin(); 
          block_iter != state_partition_old.end(); ++block_iter)
        {
          block_type* b_to_split = *block_iter;
          block_type new_block;
          new_block.is_in_new_blocks = false;
          block_type temp_block;
          block_type* new_block_ptr;
          new_block.key = blocks.size();

          // iterate over all states in the block to split
          for (state_type s : b_to_split->states)
          {
            // if block b can reach step class <a,M> starting from state s then we move it to antoher block
            if (step_class->prev_states[s] == true)
            {
              new_block.states.push_back(s);
            }
            else
            {
              temp_block.states.push_back(s);
            }
          }

          // if both, the new block and temp block has elements, then we add a new block into the state partition
          if (new_block.states.size() > 0 && temp_block.states.size() > 0)
          {
            blocks.push_back(new_block);
            new_block_ptr = &blocks.back();

            // return states from temp_block to b_to_split
            b_to_split->states.swap(temp_block.states);
            
            // if the current block is not currently a new block then add the smaller
            // block (between new block and current block) to the list of new blocks; 
            // hence, in front of state partition
            if (b_to_split->is_in_new_blocks == false)
            {
              if (new_block_ptr->states.size() < b_to_split->states.size())
              {
                new_blocks_to_move_to_front.push_back(new_block_ptr);
                new_block_ptr->is_in_new_blocks = true;
              }
              else
              {
                b_to_split->is_in_new_blocks = true;
                blocks_to_move_to_front.push_back(block_iter);

                // add new block to the back of the state_partition list
                state_partition.push_back(new_block_ptr);
              }
            }
            else
            {
              // the current block is already in new blocks; hence, just add the new block
              // to the front of the partition as well.

              //new_blocks.push_back(new_block_ptr);
              new_block_ptr->is_in_new_blocks = true;

              // add new block to the back of the state_partition list
              new_blocks_to_move_to_front.push_back(new_block_ptr);
            }
          }
        }

        // move the remaning blocks to the end of state_partition
        state_partition.splice(state_partition.begin(), state_partition_old);

        // move the blocks that should be in new blocks to the begining of the list
        for (typename std::list<block_type*>::iterator block_iter : blocks_to_move_to_front)
        {
          state_partition.splice(state_partition.begin(), state_partition, block_iter);
        }

        // move the blocks that are new to the front of state_partition
        for (block_type* block_ptr : new_blocks_to_move_to_front)
        {
          state_partition.push_front(block_ptr);
        }

      }
    }
	}

  void postprocessing_stage(void)
  {
    //---- Post processing to keep track of the parent block of each state ----//
    block_index_of_a_state.resize(aut.num_states());
    for (const block_type& b : blocks)
    {
      for (const state_type s : b.states)
      {
        block_index_of_a_state[s] = b.key;
      }
    }

    step_class_index_of_a_distribution.resize(aut.num_probabilistic_labels());
    for (step_class_type& sc : step_classes)
    {
      sc.equivalent_step_class = sc.key;

      for (const distribution_type* d : sc.distributions)
      {
        step_class_index_of_a_distribution[d->key] = sc.key;
      }
    }

    // some distributions may be repeated among different step classes.
    // To avoid this, we assign a common step class (the one with higher key)
    // for all step classes sharing distributions.

    // iterate over all step classes in reverese
    for (typename std::deque<step_class_type>::reverse_iterator i = step_classes.rbegin();
      i != step_classes.rend(); ++i)
    {
      step_class_type& sc = *i;

      // first check which distribution has the higher step_class parent key
      for (const distribution_type* d : sc.distributions)
      {
        if (step_classes[step_class_index_of_a_distribution[d->key]].equivalent_step_class > sc.equivalent_step_class)
        {
          sc.equivalent_step_class = step_classes[step_class_index_of_a_distribution[d->key]].equivalent_step_class;
        }
      }

      // update all the distributions in this stape class with the equivalent max step class key
      for (const distribution_type* d : sc.distributions)
      {
        step_classes[step_class_index_of_a_distribution[d->key]].equivalent_step_class = sc.equivalent_step_class;
      }

    }

  }
};


/** \brief Reduce transition system l with respect to probabilistic bisimulation.
 * \param[in/out] l The transition system that is reduced.
 */
template < class LTS_TYPE>
void probabilistic_bisimulation_reduce(LTS_TYPE& l);


template < class LTS_TYPE>
void probabilistic_bisimulation_reduce(LTS_TYPE& l)
{

  // Apply the probabilistic bisimulation reduction algorithm.
  detail::prob_bisim_partitioner<LTS_TYPE> prob_bisim_part(l);

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
#endif
