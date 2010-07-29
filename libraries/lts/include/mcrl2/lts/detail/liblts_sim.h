// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_sim.h
/// \brief Header file for the simulation preorder algorithm

#ifndef LIBLTS_SIM_H
#define LIBLTS_SIM_H
#include <vector>
#include "mcrl2/lts/lts.h"
#include "mcrl2/lts/lts_utilities.h"
#include "mcrl2/lts/detail/sim_hashtable.h"

typedef unsigned int uint;

class sim_partitioner
{
  public:
    /** Creates a partitioner for an LTS.
     * \param[in] l Pointer to the LTS. */
    sim_partitioner(mcrl2::lts::lts &l);

    /** Destroys this partitioner. */
    ~sim_partitioner();

    /** Computes the simulation equivalence classes and preorder
     * relations of the LTS. */
    void partitioning_algorithm();

    /** Gives the transition relation on the computed equivalence
     * classes of the LTS. The label numbers of the transitions
     * correspond to the label numbers of the LTS that was passed as an
     * argument to the constructor of this partitioner.
     * The state numbers of the transitions are the equivalence class
     * numbers which range from 0 upto (and excluding) \ref num_eq_classes().
     *
     * \pre The simulation equivalence classes have been computed.
     * \return A vector containing the transitions between the
     * simulation equivalence classes. */
    std::vector < mcrl2::lts::transition> get_transitions() const;

    /** Gives the number of simulation equivalence classes of the LTS.
     * \pre The simulation equivalence classes have been computed.
     * \return The number of simulation equivalence classes of the LTS.
     */
    uint num_eq_classes() const;

    /** Gives the equivalence class number of a state.
     * The equivalence class numbers range from 0 upto (and excluding)
     * \ref num_eq_classes().
     * \pre The simulation equivalence classes have been computed.
     * \param[in] s A state number.
     * \return The number of the equivalence class to which \e s
     * belongs. */
    uint get_eq_class(uint s) const;

    /** Returns whether one state is simulated by another state.
     * \pre The simulation preorder has been computed.
     * \param[in] s A state number.
     * \param[in] t A state number.
     * \retval true if \e s is simulated by \e t;
     * \retval false otherwise. */
    bool in_preorder(uint s,uint t) const;

    /** Returns whether two states are in the same simulation
     * equivalence class.
     * \pre The simulation equivalence classes have been computed.
     * \param[in] s A state number.
     * \param[in] t A state number.
     * \retval true if \e s and \e t are in the same simulation
     * equivalence class;
     * \retval false otherwise. */
    bool in_same_class(uint s,uint t) const;

  private:
    struct state_bucket
    {
      int next;
      int prev;
    };

    mcrl2::lts::lts &aut;
    mcrl2::lts::outgoing_transitions_per_state_action_t trans_index;
    uint s_Sigma;
    uint s_Pi;
    std::vector<bool> state_touched;
    std::vector<bool> block_touched;
    std::vector<state_bucket> state_buckets;
    std::vector<uint> block_Pi;
    std::vector<uint> parent;
    std::vector< std::vector<uint> > children;
    std::vector<int> contents_t;
    std::vector<int> contents_u;
    std::vector< std::vector<bool> > stable;
    hash_table3 *exists;
    hash_table3 *forall;
    std::vector< std::vector<uint> > pre_exists;
    std::vector< std::vector<uint> > pre_forall;
    hash_table3 *match;
    std::vector< std::vector<bool> > P;
    std::vector< std::vector<bool> > Q;

    /* auxiliary variables */
    std::vector<uint> touched_blocks;
    std::vector<uint> contents;

    void initialise_datastructures();
    //void read_partition_from_file(char *parfile);

    void refine(bool &change);
    void update();
    //void update_lts();

    void touch(uint a,uint alpha);
    void untouch(uint alpha);

    void reverse_topological_sort(std::vector<uint> &result);
    void dfs_visit(uint u,std::vector<bool> &visited,
        std::vector<uint> &result);

    void initialise_Sigma(uint gamma,uint l);
    void initialise_Pi(uint gamma,uint l);
    void filter(uint S,std::vector< std::vector<bool> > &R,bool B);
    void cleanup(uint alpha,uint beta);
    void initialise_pre_EA();
    void induce_P_on_Pi();

    void print_Sigma_P();
    void print_Pi_Q();
    void print_Sigma();
    void print_Pi();
    void print_relation(uint s,std::vector< std::vector<bool> > &R);
    void print_block(uint b);
    void print_structure(hash_table3 *struc);
};
#endif
