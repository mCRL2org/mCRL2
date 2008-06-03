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
#include "mcrl2/lts/liblts.h"
#include "mcrl2/lts/detail/sim_hashtable.h"

typedef unsigned int uint;

class sim_partitioner
{
  public:
    sim_partitioner(mcrl2::lts::lts *l);
    ~sim_partitioner();
    bool reduce();
  private:
    struct state_bucket
    {
      int next;
      int prev;
    };

    mcrl2::lts::lts *aut;
    uint **trans_index;
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

    void initialise();
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

    /*
    void print_Sigma_P();
    void print_Pi_Q();
    void print_Sigma();
    void print_Pi();
    void print_relation(uint s,std::vector< std::vector<bool> > &R);
    void print_block(uint b);
    void print_structure(hash_table3 *struc);*/
};
#endif
