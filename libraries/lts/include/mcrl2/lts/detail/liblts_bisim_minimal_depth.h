// Author(s): Jan Martens
//
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
/// \file lts/detail/liblts_bisim_m.h
///
/// \brief Partition refinement algorithm for guaruanteed minimal depth
/// counter-examples.
///
/// \details 
#ifndef _LIBLTS_BISIM_M
#define _LIBLTS_BISIM_M

#include <fstream>
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/lts/lts_utilities.h"
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_merge.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_dot.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{
template < class LTS_TYPE>
class bisim_partitioner_minimal_depth {
public:
	/** \brief Creates a bisimulation partitioner for an LTS.
	*  \details This partitioner is specifically for creating minimal depth counter-examples for strong bisimulation.
	*           It guarantees stability w.r.t. the old partition before consideren new splitter blocks. This might cause this
	*           implementation to be less efficient than other partitioners.
	*  \param l Reference to the LTS.
	*  \param init_l2 reference to the initial state of lts2. */
	bisim_partitioner_minimal_depth(LTS_TYPE& l, const std::size_t init_l2)
		: aut(l),
		initial_l2(init_l2),
		max_state_index(0)
	{
		create_initial_partition();
		bool splitted = true;
		level_type lvl = 1;
		while (splitted && block_index_of_a_state[initial_l2] == block_index_of_a_state[aut.initial_state()])
		{
			splitted = refine_partition(lvl);
			lvl++;
			for (typename std::vector<block>::reverse_iterator i = blocks.rbegin();
				i != blocks.rend() && (*i).level == lvl - 1; ++i) {
				to_be_processed.push_back((*i).block_index);
			}
		}

		for (state_type i = 0; i < aut.num_states(); i++)
		{
			block_index_type bid = block_index_of_a_state[i];
			if (!block_flags[bid]) {
				block_flags[bid] = true;
				partition.insert(bid);
			}
		}
		mCRL2log(mcrl2::log::info) << "Partition refinement done, partition contains: " << partition.size() << " blocks, the history contains " << lvl - 1 << " levels." << std::endl;
		save_transitions();
	}

	/** \brief Destroys this partitioner. */
	~bisim_partitioner_minimal_depth() = default;

	/** \brief Creates a state formula that distinguishes state s from state t.
	 *  \details The states s and t are non bisimilar states. A distinguishign state formula phi is
	 *           returned, which has the property that s \in \sem{phi} and  t \not\in\sem{phi}.
	 *           Based on the preprint "Computing minimal distinguishing Hennessey-Milner formulas is NP-hard
	 *           . But variants are tractable", 2023 by Jan Martens and Jan Friso Groote
	 *  \param[in] s The state number for which the resulting formula should be true
	 *  \param[in] t The state number for which the resulting formula should be false
	 *  \return A minimal observation depth distinguishing state formula, that is often also minimum negation-depth and irreducible. */
	mcrl2::state_formulas::state_formula dist_formula_mindepth(const std::size_t s, const std::size_t t)
	{
		formula f = distinguish(block_index_of_a_state[s], block_index_of_a_state[t]);
		mCRL2log(mcrl2::log::info) << "done with formula \n";
		return convert_formula(f);
	};

	bool in_same_class(const std::size_t s, const std::size_t t)
	{
		return block_index_of_a_state[s] == block_index_of_a_state[t];
	}


private:
	typedef std::size_t block_index_type;
	typedef std::size_t state_type;
	typedef std::size_t level_type;
	typedef std::size_t formula_index_type;
	typedef std::size_t label_type;
	state_type initial_l2;

	state_type max_state_index;
	LTS_TYPE& aut;
	std::set <block_index_type> partition;

	struct block
	{
		state_type state_index;                   // The state number that represent the states in this block
		block_index_type block_index;             // The sequence number of this block.
		block_index_type parent_block_index;      // Index of the parent block.
		level_type level;


		// If there is no parent block, this refers to the block itself.
		std::vector < state_type > states;
		std::vector < transition > transitions;
		std::set<std::pair < label_type, block_index_type >> outgoing_observations;

		void swap(block& b)
		{
			std::swap(b.state_index, state_index);
			std::swap(b.block_index, block_index);
			std::swap(b.parent_block_index, parent_block_index);
			std::swap(b.level, level);
			/*
			state_type state_index1 = b.state_index;
			b.state_index = state_index;
			state_index = state_index1;

			block_index_type block_index1 = b.block_index;
			b.block_index = block_index;
			block_index = block_index1;

			block_index_type parent_block_index1 = b.parent_block_index;
			b.parent_block_index = parent_block_index;
			parent_block_index = parent_block_index1;

			level_type level1 = b.level;
			b.level = level;
			level = level1;
			*/
			states.swap(b.states);
			transitions.swap(b.transitions);
		}
	};

	struct formula
	{
		formula_index_type index;
		label_type label;
		bool negated;
		std::vector <formula> conjunctions;
		std::set <block_index_type > truths;

		int depth()
		{
			int max_depth = 0;
			for (formula f : conjunctions)
			{
				max_depth = std::max(max_depth, f.depth() + 1);
			}
			return max_depth;
		}
	};

	std::vector < block > blocks;
	// Blocks that are split become inactive.
	std::vector < state_type > block_index_of_a_state;
	std::vector < bool > block_flags;
	std::vector < bool > state_flags;

	std::vector< block_index_type > to_be_processed;
	std::vector< block_index_type > BL;
	typedef std::pair<label_type, block_index_type> observation_t;
	typedef std::set <observation_t> derivatives_t;
	std::map < std::pair<block_index_type, block_index_type>, level_type> greatest_common_ancestor;


	/* Post processes the partition structure to save outgoing transitions per block */
	void save_transitions()
	{
		for (transition t : aut.get_transitions())
		{

			block_index_type sourceBlock = block_index_of_a_state[t.from()];
			block_index_type targetBlock = block_index_of_a_state[t.to()];

			blocks[sourceBlock].outgoing_observations.insert(std::make_pair(t.label(), targetBlock));
		}
	}

	void set_truths(formula& f)
	{
		std::set <block_index_type> image_truths;
		std::set <block_index_type> pre_image_truths;
		std::set<block_index_type> intersection;


		image_truths = std::set(partition);

		for (formula df : f.conjunctions)
		{
			std::set_intersection(image_truths.begin(), image_truths.end(), df.truths.begin(), df.truths.end(),
				std::inserter(intersection, intersection.begin())
			);
			image_truths.swap(intersection);
			intersection.clear();
		}

		//Now compute preimage according to label
		for (block_index_type B : image_truths)
		{
			for (transition t : blocks[B].transitions) {
				if (t.label() == f.label && (pre_image_truths.find(block_index_of_a_state[t.from()]) == pre_image_truths.end())) {
					pre_image_truths.insert(block_index_of_a_state[t.from()]);
				}
			}
		}

		if (f.negated)
		{
			image_truths.swap(pre_image_truths);
			pre_image_truths.clear();
			std::set_difference(partition.begin(), partition.end(), image_truths.begin(), image_truths.end(),
				std::inserter(pre_image_truths, pre_image_truths.begin()));
		}
		f.truths.swap(pre_image_truths);
	}

	void create_initial_partition()
	{
		to_be_processed.clear();
		block initial_block;

		initial_block = block();
		for (state_type i = 0; i < aut.num_states(); i++) {
			initial_block.states.push_back(i);
		}
		sort_transitions(aut.get_transitions(), mcrl2::lts::lbl_tgt_src);
		const std::vector<transition>& trans = aut.get_transitions();
		for (std::vector<transition>::const_iterator r = trans.begin(); r != trans.end(); ++r)
		{
			initial_block.transitions.push_back(*r);
		}
		initial_block.block_index = 0;
		initial_block.state_index = 0;
		max_state_index = 1;
		initial_block.parent_block_index = 0;
		initial_block.level = 0;
		blocks.push_back(block());
		blocks.back().swap(initial_block);
		block_index_of_a_state = std::vector < block_index_type >(aut.num_states(), 0);
		block_flags.push_back(false);
		state_flags = std::vector < bool >(aut.num_states(), false);
		to_be_processed.push_back(0);
		BL.clear();
	} // end create_initial_partition

	 // Refine the partition to a certain lvl.
	bool refine_partition(level_type lvl)
	{
		if (to_be_processed.empty()) {
			return false;
		}
		block_index_type splitter_index;
		while (!to_be_processed.empty()) {
			splitter_index = to_be_processed.front();

			to_be_processed.erase(to_be_processed.begin());
			if (blocks[splitter_index].level == lvl) {
				return true;
			}

			std::vector < transition > transitions_to_process;
			transitions_to_process = blocks[splitter_index].transitions;
			for (std::vector < transition > ::const_iterator i = transitions_to_process.begin();
				i != transitions_to_process.end(); ++i)
			{
				transition t = *i;
				state_flags[t.from()] = true;
				const block_index_type marked_block_index = block_index_of_a_state[t.from()];
				if (block_flags[marked_block_index] == false) {
					block_flags[marked_block_index] = true;
					BL.push_back(marked_block_index);
				}
				// If the label of the next action is different, we must carry out the splitting.
				if ((i != transitions_to_process.end() && next(i) == transitions_to_process.end()) ||
					t.label() != (*next(i)).label())
				{
					split_BL(t.label(), splitter_index, lvl);
					BL.clear();
				}
			}
		}
		return true;
	}

	void split_BL(
		const label_type splitter_label,
		const block_index_type splitter_block,
		level_type lvl
	)
	{
		for (block_index_type Bsplit : BL) {
			block_flags[Bsplit] = false;
			std::vector < state_type > flagged_states;
			std::vector < state_type > non_flagged_states;
			std::vector < state_type > Bsplit_states;
			Bsplit_states.swap(blocks[Bsplit].states);
			for (state_type s : Bsplit_states) {
				if (state_flags[s]) {
					// state is flagged.
					flagged_states.push_back(s);
				}
				else {
					// state is not flagged. It will be moved to a new block.
					non_flagged_states.push_back(s);
					block_index_of_a_state[s] = blocks.size();
				}
			}
			block_index_type Bparent = Bsplit;
			// Set the correct parent
			while (blocks[Bparent].level == lvl) {
				Bparent = blocks[Bparent].parent_block_index;
			}

			block_index_type reset_state_flags_block = Bsplit;
			if (!non_flagged_states.empty()) {
				// There are flagged and non flagged states. So, the block must be split.
				// Move the unflagged states to the new block.
				if (mCRL2logEnabled(log::debug)) {
					const std::size_t m = static_cast<std::size_t>(std::pow(10.0, std::floor(std::log10(static_cast<double>((blocks.size() + 1) / 2)))));
					if ((blocks.size() + 1) / 2 % m == 0) {
						mCRL2log(log::debug) << "Bisimulation partitioner: create block " << (blocks.size() + 1) / 2 << std::endl;
					}
				}
				// Create a first new block.
				blocks.push_back(block());
				block_index_type new_block1 = blocks.size() - 1;
				blocks.back().state_index = max_state_index;
				max_state_index++;
				blocks.back().block_index = new_block1;
				blocks.back().level = lvl;

				blocks.back().parent_block_index = Bparent;
				non_flagged_states.swap(blocks.back().states);

				// The flag fields of the new blocks is set to false;
				block_flags.push_back(false);
				// distribute the transitions
				// Finally the non-inert transitions are distributed over both blocks in the obvious way.
				// Note that this must be done after all states are properly put into a new block.
				std::vector < transition > old_transitions;
				std::vector < transition > flagged_transitions;
				std::vector < transition > non_flagged_transitions;

				old_transitions = blocks[Bsplit].transitions;
				for (std::vector < transition >::iterator k = old_transitions.begin();
					k != old_transitions.end(); ++k)
				{
					if (state_flags[(*k).to()]) {
						flagged_transitions.push_back(*k);
					}
					else {
						non_flagged_transitions.push_back(*k);
					}
				}

				// Create a second new block.
				block BlockLeft = block();

				// block_is_active.push_back(true);
				BlockLeft.state_index = blocks[Bsplit].state_index;
				BlockLeft.level = lvl;
				BlockLeft.parent_block_index = Bparent;
				BlockLeft.transitions.swap(flagged_transitions);
				BlockLeft.states.swap(flagged_states);
				if (blocks[Bsplit].level == lvl) {
					BlockLeft.block_index = Bsplit;
					BlockLeft.state_index = blocks[Bsplit].state_index;
					blocks[Bsplit].swap(BlockLeft);
				}
				else {
					BlockLeft.block_index = blocks.size();
					BlockLeft.state_index = max_state_index;
					max_state_index++;
					blocks.push_back(BlockLeft);
					block_flags.push_back(false);
					for (state_type s : BlockLeft.states) {
						block_index_of_a_state[s] = BlockLeft.block_index;
					}
				}

				blocks[new_block1].transitions.swap(non_flagged_transitions);
				reset_state_flags_block = BlockLeft.block_index;

				if (BlockLeft.block_index != Bsplit) {
					std::vector < state_type >& reference_to_flagged_states_of_block2 = blocks.back().states;
					for (std::vector < state_type >::const_iterator j = reference_to_flagged_states_of_block2.begin();
						j != reference_to_flagged_states_of_block2.end(); ++j)
					{
						block_index_of_a_state[*j] = BlockLeft.block_index;
					}
				}

			}
			else {
				reset_state_flags_block = Bsplit;
				blocks[Bsplit].states.swap(flagged_states);
			}
			// reset the state flags
			std::vector < state_type >& flagged_states_to_be_unflagged = blocks[reset_state_flags_block].states;
			for (state_type s : flagged_states_to_be_unflagged) {
				state_flags[s] = false;
			}
			BL.clear();
		};
	}

	label_type label(observation_t obs)
	{
		return obs.first;
	}

	block_index_type target(observation_t obs)
	{
		return obs.second;
	}

	formula distinguish(const block_index_type b1, const block_index_type b2)
	{
		derivatives_t b2_delta;
		observation_t dist_obs;

		level_type lvl = gca_level(b1, b2);

		for (observation_t obs : blocks[b2].outgoing_observations) {
			b2_delta.insert(std::make_pair(label(obs), lift_block(target(obs), lvl - 1)));
		}

		bool found_dist_obs = false;
		for (observation_t obs : blocks[b1].outgoing_observations) {
			if (b2_delta.find(std::make_pair(label(obs), lift_block(target(obs), lvl - 1))) == b2_delta.end()) {
				found_dist_obs = true;
				dist_obs = obs;
				break;
			}
		}

		if (!found_dist_obs) {
			//Greedy strategy did not find negation free formula;
			formula neg_phi = distinguish(b2, b1);
			neg_phi.negated = true;
			set_truths(neg_phi);
			return neg_phi;
		}

		// Set of t derivates we need to take care of. 
		std::set<block_index_type> dT;
		for (observation_t obs : blocks[b2].outgoing_observations) {
			if (label(obs) == label(dist_obs)) {
				dT.insert(target(obs));
			}
		}
		std::vector<formula> conjunctions;

		while (!dT.empty()) {
			level_type max_intersect = 0;
			block_index_type splitBlock = *dT.begin();
			for (block_index_type bid : dT) {
				if (gca_level(target(dist_obs), bid) > max_intersect) {
					splitBlock = bid;
					max_intersect = gca_level(target(dist_obs), bid);
				}
			}
			formula f = distinguish(target(dist_obs), splitBlock);
			conjunctions.push_back(f);

			std::set<block_index_type> dTleft;
			std::set_intersection(dT.begin(), dT.end(), f.truths.begin(), f.truths.end(),
				std::inserter(dTleft, dTleft.begin())
			);
			assert(dT.size() > dTleft.size());
			dT.swap(dTleft);
		}
		formula returnf;
		returnf.conjunctions.swap(conjunctions);
		returnf.label = label(dist_obs);
		returnf.negated = false;
		set_truths(returnf);
		return returnf;
	}

	level_type gca_level(const block_index_type B1, const block_index_type B2)
	{
		block_index_type b1 = B1, b2 = B2;
		if (B1 < B2) {
			b1 = B2;
			b2 = B1;
		}
		std::pair<block_index_type, block_index_type> bpair(b1, b2);
		if (greatest_common_ancestor.find(bpair) != greatest_common_ancestor.end()) {
			return greatest_common_ancestor[bpair];
		}
		level_type lvl1 = blocks[b1].level, lvl2 = blocks[b2].level;
		if (b1 == b2) {
			greatest_common_ancestor.emplace(bpair, lvl1);
			return lvl1;
		}
		block_index_type B1parent = b1, B2parent = b2;

		if (lvl1 <= lvl2) {
			B2parent = blocks[b2].parent_block_index;
		}
		if (lvl2 <= lvl1) {
			B1parent = blocks[b1].parent_block_index;
		}
		if (B1parent == B2parent) {
			lvl1 = blocks[b1].level;
		}
		else {
			lvl1 = gca_level(B1parent, B2parent);
		}
		greatest_common_ancestor.emplace(bpair, lvl1);
		return lvl1;
	}

	block_index_type lift_block(const block_index_type B1, level_type goal)
	{
		block_index_type B = B1;
		while (blocks[B].level > goal) {
			B = blocks[B].parent_block_index;
		}
		return B;
	}

	/*
	* \brief Converts the private formula data type to the proper mCRL2 state_formula objects
	* \param a formula f
	* \return a state_formula equivalent to f
	*/
	mcrl2::state_formulas::state_formula convert_formula(formula& f)
	{
		mcrl2::state_formulas::state_formula returnPhi = mcrl2::state_formulas::may(
			create_regular_formula(aut.action_label(f.label)),
			conjunction(f.conjunctions)
		);

		if (f.negated) {
			return mcrl2::state_formulas::not_(returnPhi);
		}
		return returnPhi;
	}

	/**
	 * \brief conjunction Creates a conjunction of state formulas
	 * \param terms The terms of the conjunction
	 * \return The conjunctive state formula
	 */
	mcrl2::state_formulas::state_formula conjunction(std::vector<formula>& conjunctions)
	{
		std::vector<mcrl2::state_formulas::state_formula> terms;
		for (formula& f : conjunctions) {
			terms.push_back(convert_formula(f));
		}
		return utilities::detail::join<mcrl2::state_formulas::state_formula>(
			terms.begin(), terms.end(),
			[](mcrl2::state_formulas::state_formula a, mcrl2::state_formulas::state_formula b)
			{ return mcrl2::state_formulas::and_(a, b); },
			mcrl2::state_formulas::true_()
			);
	}

	/**
	* \brief create_regular_formula Creates a regular formula that represents action a
	* \details In case the action comes from an LTS in the aut or fsm format.
	* \param a The action for which to create a regular formula
	* \return The created regular formula
	*/
	regular_formulas::regular_formula create_regular_formula(const mcrl2::lts::action_label_string& a) const
	{
		return mcrl2::regular_formulas::regular_formula(mcrl2::action_formulas::multi_action(
			process::action_list({ process::action(process::action_label(a, {}), {}) })));
	}

	/**
	* \brief create_regular_formula Creates a regular formula that represents action a
	* \details In case the action comes from an LTS in the lts format.
	* \param a The action for which to create a regular formula
	* \return The created regular formula
	*/
	regular_formulas::regular_formula create_regular_formula(const mcrl2::lps::multi_action& a) const
	{
		return regular_formulas::regular_formula(action_formulas::multi_action(a.actions()));
	}

};
}
}
}
#endif