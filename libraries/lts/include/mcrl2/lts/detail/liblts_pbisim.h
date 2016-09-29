// Author(s): Joao Rivera
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
#include <set>
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
    }


    /** \brief Destroys this partitioner. */
    ~prob_bisim_partitioner()
    {}

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
    /* Remove old probabilistic states */
    aut.clear_probabilistic_states();

    /* Copy the probability states from the reduced_step_partition map into the transition system. */
    for (std::unordered_map<lts_aut_base::probabilistic_state,
      std::set<step_class_index_type> >::iterator i = reduced_step_partition.begin();
      i != reduced_step_partition.end(); i++)
    {
      aut.add_probabilistic_state(i->first);
    }
    
    lts_aut_base::probabilistic_state old_initial_prob_state = aut.initial_probabilistic_state();
    lts_aut_base::probabilistic_state new_initial_prob_state = calculate_new_probabilistic_state(old_initial_prob_state);
    aut.set_initial_probabilistic_state(new_initial_prob_state);
	}


	/** \brief Gives the number of bisimulation equivalence classes of the LTS.
   *  \return The number of bisimulation equivalence classes of the LTS.
   */
  size_t num_eq_classes() const
  {
    return State_partition.size();
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
		assert(d<step_class_index_of_a_distribution.size());
    assert(step_class_index_of_a_distribution[d]<reduced_index_of_step_class.size());
		return reduced_index_of_step_class[step_class_index_of_a_distribution[d]]; // The block index is the state number of the block.
	}

  private:

	typedef size_t block_index_type;
  typedef size_t step_class_index_type;
  typedef size_t state_type;
  typedef size_t label_type;
	typedef size_t distribution_type;
	typedef probabilistic_arbitrary_precision_fraction probability_fraction_type;

	struct block
  {
    std::set < state_type > states;			  // The states in the block
  };

	struct step_class {
	  label_type action;								// action label of the pair <a,M>.
	  std::set < distribution_type > distributions;		// The distributions in the step class <a,M>.
	  std::set< state_type > Pre;						// Set of previous states that can reach the distributions M with action a.
	};

	std::vector < block_index_type > block_index_of_a_state;
	std::vector < block_index_type > step_class_index_of_a_distribution;
  std::vector < block > State_partition;
	std::vector < block > NewBlocks;
	std::vector < step_class > Step_partition;
	std::vector < step_class > NewStepClasses;
	std::vector < std::set < state_type > > Pre_states;  // Set of states that can reach the distribution given by the key
  std::unordered_map<lts_aut_base::probabilistic_state, std::set<step_class_index_type> > reduced_step_partition;  // mapping from resulting probability state to its corresponding index in step partition
  std::vector<step_class_index_type> reduced_index_of_step_class; //vector of the index of a step class in its resulting reduced_step_partition

	LTS_TYPE& aut;
	typedef struct TreeNode{
	  size_t depth;				  // The depth of the node in the tree.
	  std::set<size_t> actions;   // Subset of actions
	  std::set<size_t> states;	  // Subste of states
	  size_t counter;			  // Counter of states in leafs.
      TreeNode *left;			  // Pointer to the left subtree.
      TreeNode *right;			  // Pointer to the right subtree.
	} tree_node;

	void destroy_tree(tree_node *node)
	{
	  if( node != NULL )
	  {
		destroy_tree(node->left);
        destroy_tree(node->right);
		delete node;
	  }
	}

	tree_node* create_node(size_t depth)
	{
	  tree_node* n = new tree_node;
	  n->depth = depth;
	  n->counter = 0;
	  n->left = NULL;
	  n->right = NULL;

	  return n;
	}

  /** \brief Creates the initial partition of step clases and blocks.
   *  \detal The blocks are initially partitioned based on the actions that can perform.
   *         The step classes are partitioned based on the action that leads to the probabilistic state */
  void create_initial_partition (void) 
  {
	  tree_node* v0 =  create_node(0);
	  tree_node* v;
	  tree_node* Maxblock;
	  std::vector<tree_node*> leafs_ptr;
	  const std::vector<transition> & trans=aut.get_transitions();
	  size_t max_blocksize = 0;

	  /* Resize Pre_states based on the number of probabilistic states */
	  Pre_states.resize(aut.num_probabilistic_labels());

	  /* Creation of Steps[i][j] bi-dimensional vector*/ //todo: find another way to do this more efficient
	  std::vector< std::vector< std::set<distribution_type> > > Steps;
	  Steps.resize(aut.num_states());
	  for (size_t i = 0; i < Steps.size(); i++) 
    {
		  Steps[i].resize(aut.num_action_labels());
	  }

	  /* Add step values to Steps bi-dim vector */
	  for (std::vector<transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r) 
    {
		  const transition t = *r;
		  size_t dist_id = t.to();
		  Pre_states[dist_id].insert(t.from());
		  Steps[t.from()][t.label()].insert(dist_id); // todo: what happends if it is a repeated distribution?
	  }

	  // The number of step classes in the first step partition is the same as the number of actions (steps)
	  Step_partition.resize(aut.num_action_labels());
  
	  /* Iterate over all states an labels to build binary tree */
	  for (size_t s = 0; s < aut.num_states(); s++) 
    {
		  v = v0;
		  for (size_t i = 0; i < aut.num_action_labels(); i++) 
      {
		    Step_partition[i].action = i;
	      if (Steps[s][i].size() != 0) 
        {
			    /* Action i can be donde in state s */
			    /* Add the distributions of Steps to Step_partition */
			    Step_partition[i].distributions.insert(Steps[s][i].begin(), Steps[s][i].end());
			    Step_partition[i].Pre.insert(s);
	
			    if (v->left == NULL) 
          {
			      /* Create left node with depth i+1 */
			      tree_node* w = create_node(i+1);
			      v->left = w;
			      w->actions = v->actions; //not sure if these is necessary
			      w->actions.insert(i);
			      if (i == aut.num_action_labels()-1) 
            {  // it is a leaf
				      leafs_ptr.push_back(w);
			      }
			    }
			    v = v->left;

		    } 
        else 
        {
			    /* Action i cannot be done in state s */
			    if (v->right == NULL) 
          {
			      /* Create right node with depth i+1 */
			      tree_node* w = create_node(i+1);
			      v->right = w;
			      w->actions = v->actions; //not sure if these is necessary
			      if (i == aut.num_action_labels()-1) 
            {  // it is a leaf
			        leafs_ptr.push_back(w);
			      }
			    }
			    v = v->right;
		    }

		  }
      /* Instert state into leaf node*/
		  v->states.insert(s);
		  v->counter++;

      /* Keep track of the number of states in a leaf to determine Maxblock */
		  if (v->counter >= max_blocksize) 
      {
		    Maxblock = v;
		    max_blocksize = v->counter; 
		  }
	  }

	  /* Init NewBlocks and NewStepClasses */
	  NewStepClasses = Step_partition;

    /* Add every leaf to State_partition and NewBlocks */
	  for (const tree_node* leaf : leafs_ptr) 
    {
		  block b;
		  b.states = leaf->states;
		  State_partition.push_back(b);
		  if (leaf != Maxblock) 
      {
		    NewBlocks.push_back(b);
		  }
	  }

	  /* Destroy tree */
	  destroy_tree(v0);
	}

	struct balanced_tree
  {
	  probability_fraction_type key;						// The key is the probability.
	  std::set<state_type> states;	    // Subset of states
	  std::set<distribution_type> distr;  // Subset of distributions
    balanced_tree *left;			    // Pointer to the left subtree.
    balanced_tree *right;			    // Pointer to the right subtree.


	  void init_node (probability_fraction_type key_new_node) 
    {
		  key = key_new_node;
      left = NULL;
	    right = NULL;
	  }

	  void init_node(void) 
    {
		  //key = key_new_node;
		  left = NULL;
		  right = NULL;
	  }

	  balanced_tree* create_node (probability_fraction_type key_new_node)
    {
      balanced_tree* n = new balanced_tree;
		  n->key = key_new_node;
	    n->left = NULL;
	    n->right = NULL;

     	return n;
	  }

	  balanced_tree* add_node (probability_fraction_type key_new_node)
    {
		  balanced_tree* n;

		  if (key_new_node > key)
      {
		    /* The key of the new node is grater than current node. Go to the right node */
		    if (right == NULL)
        {
			    /* The node does not exist yet. Create one. */
		      n = create_node(key_new_node);
			    right = n;
		    }
        else
        { 
			    /* A node exists. Add a new node there */
			    n = right->add_node(key_new_node);
		    }
		  } 
      else if (key_new_node < key)
      {
		    /* The key of the new node is smaller than current node. Go to the left node */
		    if (left == NULL)
        {
			    /* The node does not exist yet. Create one. */
		      n = create_node(key_new_node);
			    left = n;
		    }
        else
        {
			    /* A node already exists. Add a new node there */
			    n = left->add_node(key_new_node);
		    } 
		  }
      else
      {
		    /* The current node has the same key */
		    n = this;
		  }

		  return n;
	  }

	  void destroy_tree(void)
	  {
	    if( left != NULL )
	    {
		    left->destroy_tree();
		  }

		  if( right != NULL )
	    {
		    right->destroy_tree();
		  }

		  delete this; 
	  }
	};
	
  /** \brief Calculates the probability to reach block b from ditribution d.
  *  \param[in] d A probabilistic state (distribution).
  *             b A block of states.
  *  \return The probability to reach block b. */
	probability_fraction_type probability_to_block(distribution_type d, block& b)
  {
	  /* todo: there should be a way to optimize this (iterate over all states and all prob_pairs is not very efficient)*/
	  probability_fraction_type prob_to_block;
	  lts_aut_base::probabilistic_state prob_state = aut.probabilistic_state(d);

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

  lts_aut_base::probabilistic_state calculate_equivalent_probabilistic_state(step_class sc) 
  {
    lts_aut_base::probabilistic_state equivalent_prob_state;

    /* Select the first probabilistic state of the step class */
    std::set<distribution_type>::iterator i = sc.distributions.begin();
    distribution_type d = *i;

    lts_aut_base::probabilistic_state old_prob_state = aut.probabilistic_state(d);

    equivalent_prob_state = calculate_new_probabilistic_state(old_prob_state);

    return equivalent_prob_state;
  }

	/* Two-phased partitioning algorithm */
	void refine_partition_until_it_becomes_stable (void) 
  {
	  std::vector < step_class > Step_partition_old; // todo: try to optimize this later. It shouldn't be necessary to mantain this copy.
	  probability_fraction_type key;
	  
	  /* Iterate until no new blocks and no new step classes */
	  while (NewBlocks.size() != 0 || NewStepClasses.size() != 0) 
    {
		  /* Phase 1: Splitting of Step_Partition via Split(M,C) */
		  if (NewBlocks.size() != 0) 
      {
		    block& C = NewBlocks.back();
		    Step_partition_old = Step_partition;
		    Step_partition.clear();

		    /* Iterate over all step classes of State_partition_old */
		    for (const step_class &M : Step_partition_old) 
        {
			    /* Balanced tree for current step class*/
		      balanced_tree* v0 = new balanced_tree;
			    v0->init_node(); // Dummy init;
			    balanced_tree* n;
			    std::set < balanced_tree* > bt_nodes;

			    /* Iterate over all distributions d of the step class and construct balanced tree using block C*/
			    for (const distribution_type &d : M.distributions) 
          {
			      /* todo: Can all this be replaced by a map <float, vector<distributions> > ?? So, instead of generating the balanced tree
			               just create a mapping with the key to a vector of distributions. Inserting a new node would be inseting a new
					           element to the map */
			      key = probability_to_block(d,C); 
			      if (d == *M.distributions.begin()) 
            {
				      /* Init root of the balanced tree with the first iteration */
			        v0->init_node(key);
				      n = v0;
			      }
            else 
            {
				      /* Add node to the balanced tree with the calculated key */
				      n = v0->add_node(key);
			      }
			      n->distr.insert(d);
			      n->states.insert(Pre_states[d].begin(), Pre_states[d].end());
			      bt_nodes.insert(n);
			    }

			  /* Iterate over all nodes in the balanced tree and construct new Step Partition */
			    for (balanced_tree* n : bt_nodes) 
          {
			      step_class sc;
			      sc.distributions = n->distr;
			      sc.action = M.action;
			      sc.Pre = n->states;
			      Step_partition.push_back(sc);

			      if (bt_nodes.size() >= 2) 
            {
				      NewStepClasses.push_back(sc);
			      }
			    }

			    v0->destroy_tree();
		    }

		    NewBlocks.pop_back();

	      } /* Phase 1 partinioner ends */

	    /* Phase 2: Refinment of State_Partition via Refine(X,action,M) */
		  if (NewStepClasses.size() != 0) 
      {
		    step_class &M = NewStepClasses.back();
		    std::vector < block > State_partition_old = State_partition;
		    State_partition.clear();

		    /* Iterate over all blocks of the partition */
		    for (const block &B : State_partition_old) 
        {
		      block B1, B2;

			    /* Calculate intersection of B and M.Pre sets*/
			    for (const state_type s : B.states) 
          { // todo: replace all this with intersection function?
			      size_t c;
			      c = M.Pre.count(s);

			      if (c != 0)
            {
				      /* State s is inside of block B. Add to B1 */
				      B1.states.insert(s);
			      }
            else
            {
				      /* State s is not an element of block B. Add to B2 */
				      B2.states.insert(s);
			      }
			    }

			    /* Add blocks B1 and B2 to State Partition */
			    if (B1.states.size() != 0)
          {
			      State_partition.push_back(B1);
			    }
			    if (B2.states.size() != 0) 
          {
			      State_partition.push_back(B2);
			    }
		   
			    /* Add the smallest of the blocks B1 and B2 to NewBloks */
			    if (B1.states.size() != 0 && B1.states.size() != 0) 
          {
			      if (B1.states.size() < B2.states.size())
            {
				      NewBlocks.push_back(B1);
			      }
            else 
            {
				      NewBlocks.push_back(B2);
			      }
			    }
		    }
		    NewStepClasses.pop_back();
		  } /* Phase 2 partinioner ends */
	  }
	
	  /* Construct block_index_of_a_state vector */
	  block_index_of_a_state.resize(aut.num_states());
	  int block_index = 0;
	  for (const block b : State_partition)
    {
		  for (const state_type s : b.states)
      {
		    block_index_of_a_state[s] = block_index;
		  }
		block_index++;
	  }

	  /* Construct step_class_index_of_a_distribution vector */
	  step_class_index_of_a_distribution.resize(aut.num_probabilistic_labels());
	  int step_class_index = 0;
	  for (const step_class sc : Step_partition)
    {
		  for (const distribution_type d : sc.distributions)
      {
			  step_class_index_of_a_distribution[d] = step_class_index;
		  }
		  step_class_index++;
	  }

    /* Merge equivalent step classes in step partition. The algorithm initially separates classes by actions, 
       but it is possible that a probabilistic sate has multiple incoming actions, hence it will be repeated 
       among multiple step classes */
    lts_aut_base::probabilistic_state new_prob_state;

    reduced_index_of_step_class.resize(Step_partition.size());

    step_class_index_type step_class_id = 0;
    /* Iterate over all step classes of Step_partition*/
    for (const step_class &sc : Step_partition)
    {
      lts_aut_base::probabilistic_state new_prob_state = calculate_equivalent_probabilistic_state(sc);

      /* Add the step class index to the new probability state*/
      reduced_step_partition[new_prob_state].insert(step_class_id);
      step_class_id++;
    }

    step_class_index_type reduced_id = 0;
    for (std::unordered_map<lts_aut_base::probabilistic_state,
                            std::set<step_class_index_type> >::iterator i = reduced_step_partition.begin();
            i != reduced_step_partition.end(); i++)
    {
      for (step_class_index_type sc_id : i->second)
      {
        reduced_index_of_step_class[sc_id] = reduced_id;
      }
      reduced_id++;
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
  //l.set_initial_state(prob_bisim_part.get_eq_class(l.initial_state()));
  prob_bisim_part.replace_transitions();
  prob_bisim_part.replace_probabilistic_states();
}

}
}
}
#endif
