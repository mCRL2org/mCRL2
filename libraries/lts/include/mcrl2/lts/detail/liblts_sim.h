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
#include <cstdlib>
#include <string>
#include <sstream>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/lts/lts_utilities.h"
#include "mcrl2/lts/detail/sim_hashtable.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_dot.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{

template <class LTS_TYPE>
class sim_partitioner
{
  public:
    /** Creates a partitioner for an LTS.
     * \param[in] l Pointer to the LTS. */
    sim_partitioner(LTS_TYPE& l);

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
    size_t num_eq_classes() const;

    /** Gives the equivalence class number of a state.
     * The equivalence class numbers range from 0 upto (and excluding)
     * \ref num_eq_classes().
     * \pre The simulation equivalence classes have been computed.
     * \param[in] s A state number.
     * \return The number of the equivalence class to which \e s
     * belongs. */
    size_t get_eq_class(size_t s) const;

    /** Returns whether one state is simulated by another state.
     * \pre The simulation preorder has been computed.
     * \param[in] s A state number.
     * \param[in] t A state number.
     * \retval true if \e s is simulated by \e t;
     * \retval false otherwise. */
    bool in_preorder(size_t s,size_t t) const;

    /** Returns whether two states are in the same simulation
     * equivalence class.
     * \pre The simulation equivalence classes have been computed.
     * \param[in] s A state number.
     * \param[in] t A state number.
     * \retval true if \e s and \e t are in the same simulation
     * equivalence class;
     * \retval false otherwise. */
    bool in_same_class(size_t s,size_t t) const;

  private:
    struct state_bucket
    {
      ptrdiff_t next;
      ptrdiff_t prev;
    };

    LTS_TYPE& aut;
    mcrl2::lts::outgoing_transitions_per_state_action_t trans_index;
    size_t s_Sigma;
    size_t s_Pi;
    std::vector<bool> state_touched;
    std::vector<bool> block_touched;
    std::vector<state_bucket> state_buckets;
    std::vector<size_t> block_Pi;
    std::vector<size_t> parent;
    std::vector< std::vector<size_t> > children;
    std::vector<ptrdiff_t> contents_t;
    std::vector<ptrdiff_t> contents_u;
    std::vector< std::vector<bool> > stable;
    hash_table3* exists;
    hash_table3* forall;
    std::vector< std::vector<size_t> > pre_exists;
    std::vector< std::vector<size_t> > pre_forall;
    hash_table3* match;
    std::vector< std::vector<bool> > P;
    std::vector< std::vector<bool> > Q;

    /* auxiliary variables */
    std::vector<size_t> touched_blocks;
    std::vector<size_t> contents;

    void initialise_datastructures();
    //void read_partition_from_file(char *parfile);

    void refine(bool& change);
    void update();
    //void update_lts();

    void touch(size_t a,size_t alpha);
    void untouch(size_t alpha);

    void reverse_topological_sort(std::vector<size_t> &Sort);
    void dfs_visit(size_t u,std::vector<bool> &visited,
                   std::vector<size_t> &Sort);

    void initialise_Sigma(size_t gamma,size_t l);
    void initialise_Pi(size_t gamma,size_t l);
    void filter(size_t S,std::vector< std::vector<bool> > &R,bool B);
    void cleanup(size_t alpha,size_t beta);
    void initialise_pre_EA();
    void induce_P_on_Pi();

    std::string print_Sigma_P();
    std::string print_Pi_Q();
    std::string print_Sigma();
    std::string print_Pi();
    std::string print_relation(size_t s,std::vector< std::vector<bool> > &R);
    std::string print_block(size_t b);
    std::string print_structure(hash_table3* struc);
};


#define LIST_END (-1)

template <class LTS_TYPE>
sim_partitioner<LTS_TYPE>::sim_partitioner(LTS_TYPE& l)
  : aut(l)
{ }

template <class LTS_TYPE>
sim_partitioner<LTS_TYPE>::~sim_partitioner()
{
  delete match;
  delete exists;
  delete forall;
}

/* ----------------- PARTITIONING ALGORITHM ------------------------- */

template <class LTS_TYPE>
void sim_partitioner<LTS_TYPE>::partitioning_algorithm()
{
  using namespace mcrl2::core;
  initialise_datastructures();

  mCRL2log(log::debug) << "--------------------- INITIALISATION ---------------------------" << std::endl;

  mCRL2log(log::verbose) << "  initialisation; number of blocks: " << s_Sigma << std::endl;
  bool change;
  size_t i;

  refine(change);
  update();

  change = true;
  i = 0;
  while (change)
  {
    change = false;

    /* Set Sigma to Pi and P to Q*/
    s_Sigma = s_Pi;
    /* The following statement simultaneously assigns P to Q and Q to P.
     * The assignment of Q to P is safe because Q is not used in
     * refine() and will be freshly computed at the start of update().
     * The advantage of using swap() is that it is executed in constant
     * time. */
    P.swap(Q);

    mCRL2log(log::debug) << "--------------------- ITERATION " << i << " ----------------------------------" << std::endl;

    mCRL2log(log::verbose) << "  iteration " << i << "; number of blocks: " << s_Sigma << std::endl;

    refine(change);
    if (change)
    {
      update();
    }
    else
    {
      /* No blocks were split by refine(), so update() need not be
       * called. However, we do need to swap P and Q again: Q currently
       * contains the P-relation of the previous iteration (due to the
       * call to swap() prior to the call to refine()) but we want it to
       * contain that of the current iteration! */
      P.swap(Q);
    }
    ++i;
  }

  if (mCRL2logEnabled(log::debug))
  {
    print_Pi_Q();
  }
}

template <class LTS_TYPE>
void sim_partitioner<LTS_TYPE>::initialise_datastructures()
{
  // aut.sort_transitions(mcrl2::lts::lbl_tgt_src);
  // trans_index = aut.get_transition_pre_table();
  trans_index=transitions_per_outgoing_state_action_pair_reversed(aut.get_transitions());

  size_t N = aut.num_states();

  match  = new hash_table3(1000);
  exists = new hash_table3(1000);
  forall = new hash_table3(1000);

  state_bucket sb = { LIST_END, LIST_END };
  state_buckets.assign(N,sb);
  state_touched.assign(N,false);
  block_Pi.assign(N,0);

  /* put all states in one block */
  s_Pi = 1;
  contents_u.push_back(0);
  contents_t.push_back(LIST_END);
  for (size_t i = 0; i < N; ++i)
  {
    if (i > 0)
    {
      state_buckets[i].prev = i-1;
    }
    else
    {
      state_buckets[i].prev = LIST_END;
    }
    if (i < N-1)
    {
      state_buckets[i].next = i+1;
    }
    else
    {
      state_buckets[i].next = LIST_END;
    }
  }

  block_touched.assign(s_Pi,false);
  s_Sigma = s_Pi;

  /* initialise P and children */
  std::vector<size_t> vi;
  children.assign(s_Sigma,vi);
  std::vector<bool> vb(s_Sigma,false);
  P.assign(s_Sigma,vb);
  for (size_t i = 0; i < s_Sigma; ++i)
  {
    children[i].push_back(i);
    P[i][i] = true;
  }
}

/* ----------------- INITIALISE ------------------------------------- */

template <class LTS_TYPE>
void sim_partitioner<LTS_TYPE>::initialise_Pi(size_t gamma,size_t l)
{
  size_t alpha, a, c;
  std::vector<size_t>::iterator ci, last;

  contents.clear();
  for (ptrdiff_t i = contents_u[gamma]; i != LIST_END;
       i = state_buckets[i].next)
  {
    contents.push_back(size_t(i));
  }
  for (ptrdiff_t i = contents_t[gamma]; i != LIST_END;
       i = state_buckets[i].next)
  {
    contents.push_back(size_t(i));
  }
  last = contents.end();
  for (ci = contents.begin(); ci != last; ++ci)
  {
    c = *ci;
    /* iterate over the incoming l-transitions of c */
    using namespace mcrl2::lts;
    for (outgoing_transitions_per_state_action_t::iterator
         t=trans_index.lower_bound(std::pair < transition::size_type, transition::size_type >(c,l));
         t!=trans_index.upper_bound(std::pair < transition::size_type, transition::size_type >(c,l)); ++t)
    {
      a = to(t); // As trans_index is reversed, this is actually the state from which the transition t goes.
      if (!state_touched[a])
      {
        alpha = block_Pi[a];
        touch(a,alpha);
        if (!block_touched[alpha])
        {
          touched_blocks.push_back(alpha);
          block_touched[alpha] = true;
        }
      }
    }
  }
}

template <class LTS_TYPE>
void sim_partitioner<LTS_TYPE>::initialise_Sigma(size_t gamma,size_t l)
{
  std::vector<size_t>::iterator deltai, last;
  last = children[gamma].end();
  for (deltai = children[gamma].begin(); deltai != last; ++deltai)
  {
    initialise_Pi(*deltai,l);
  }
}

/* ----------------- REFINE ----------------------------------------- */

/* PRE: s_Sigma = s_Pi */
template <class LTS_TYPE>
void sim_partitioner<LTS_TYPE>::refine(bool& change)
{
  using namespace mcrl2::core;
  /* Initialise the parent and children functions */
  std::vector<size_t> v;
  children.assign(s_Pi,v);
  parent.assign(s_Pi,0);
  size_t alpha;
  for (alpha = 0; alpha < s_Pi; ++alpha)
  {
    children[alpha].push_back(alpha);
    parent[alpha] = alpha;
  }

  if (mCRL2logEnabled(log::debug))
  {
    mCRL2log(log::debug) << "--------------------- Refine ---------------------------------------" << std::endl;
    print_Sigma_P();
  }

  /* Compute a reverse topological sorting of Sigma w.r.t. P */
  std::vector<size_t> Sort;
  Sort.reserve(s_Sigma);
  reverse_topological_sort(Sort);

  if (mCRL2logEnabled(log::debug))
  {
    mCRL2log(log::debug) << "reverse topological sort is: [";
    for (size_t i = 0; i < Sort.size(); ++i)
    {
      mCRL2log(log::debug) << Sort[i];
      if (i+1 < Sort.size())
      {
        mCRL2log(log::debug) << ",";
      }
    }
    mCRL2log(log::debug) << "]" << std::endl;
  }

  /* Some local variables */
  std::vector<bool> v_false(s_Sigma,false);
  std::vector<size_t>::iterator alphai, last, gammai;
  std::vector< std::vector<bool> >::iterator stable_alpha, P_gamma;
  bool stable_alpha_gamma;
  size_t gamma, delta, l;

  /* The main loop */
  for (l = 0; l < aut.num_action_labels(); ++l)
  {
    mCRL2log(log::debug) << "---------------------------------------------------" << std::endl;
    mCRL2log(log::debug) << "Label = \"" << mcrl2::lts::pp(aut.action_label(l)) << "\"" << std::endl;

    /* reset the stable function */
    stable.assign(s_Pi,v_false);

    /* iterate over the reverse topological sorting */
    for (gammai = Sort.begin(); gammai != Sort.end(); ++gammai)
    {
      gamma = *gammai;

      touched_blocks.clear();
      initialise_Sigma(gamma,l);

      /* iterate over all alpha such that alpha -l->E gamma */
      last = touched_blocks.end();
      for (alphai = touched_blocks.begin(); alphai != last; ++alphai)
      {
        alpha = *alphai;
        /* compute stable(alpha,gamma); use a local boolean variable for
         * efficiency */
        stable_alpha_gamma = false;
        stable_alpha = stable.begin() + alpha;
        P_gamma = P.begin() + gamma;
        for (delta = 0; delta < s_Sigma && !stable_alpha_gamma; ++delta)
        {
          if ((*stable_alpha)[delta] && (*P_gamma)[delta])
          {
            stable_alpha_gamma = true;
          }
        }
        (*stable_alpha)[gamma] = stable_alpha_gamma;
        if (!stable_alpha_gamma)
        {
          /* if alpha -l->A gamma then alpha cannot be split */
          if (contents_u[alpha] != LIST_END)
          {
            /* split alpha; new block will be s_Pi */
            change = true;

            children[parent[alpha]].push_back(s_Pi);
            parent.push_back(parent[alpha]);
            stable.push_back(*stable_alpha);
            block_touched.push_back(false);
            contents_t.push_back(LIST_END);

            /* assign the untouched contents of alpha to s_Pi */
            contents_u.push_back(contents_u[alpha]);
            contents_u[alpha] = LIST_END;

            /* update the block information for the moved states */
            for (ptrdiff_t i = contents_u[s_Pi]; i != LIST_END;
                 i = state_buckets[i].next)
            {
              block_Pi[i] = s_Pi;
            }
            ++s_Pi;
          }
          stable[alpha][gamma] = true;
        }
        untouch(alpha);
      }
    }
  }
}

template <class LTS_TYPE>
void sim_partitioner<LTS_TYPE>::reverse_topological_sort(std::vector<size_t> &Sort)
{
  std::vector<bool> visited(s_Sigma,false);
  for (size_t u = 0; u < s_Sigma; ++u)
  {
    if (!visited[u])
    {
      dfs_visit(u,visited,Sort);
    }
  }
}

template <class LTS_TYPE>
void sim_partitioner<LTS_TYPE>::dfs_visit(size_t u,std::vector<bool> &visited,
    std::vector<size_t> &Sort)
{
  visited[u] = true;
  for (size_t v = 0; v < s_Sigma; ++v)
  {
    if (!visited[v] && P[u][v])
    {
      dfs_visit(v,visited,Sort);
    }
  }
  Sort.push_back(u);
}

template <class LTS_TYPE>
void sim_partitioner<LTS_TYPE>::touch(size_t a,size_t alpha)
{
  state_touched[a] = true;
  ptrdiff_t p = state_buckets[a].prev;
  ptrdiff_t n = state_buckets[a].next;
  if (p == LIST_END)
  {
    contents_u[alpha] = n;
  }
  else
  {
    state_buckets[p].next = n;
  }
  if (n != LIST_END)
  {
    state_buckets[n].prev = p;
  }
  state_buckets[a].prev = LIST_END;
  state_buckets[a].next = contents_t[alpha];
  if (contents_t[alpha] != LIST_END)
  {
    state_buckets[contents_t[alpha]].prev = a;
  }
  contents_t[alpha] = a;
}

/* PRE: contents_t[alpha] != LIST_END */
template <class LTS_TYPE>
void sim_partitioner<LTS_TYPE>::untouch(size_t alpha)
{
  // search linearly for the last element of contents_t[alpha];
  // untouch every state we encounter along the way
  ptrdiff_t i = contents_t[alpha];
  while (state_buckets[i].next != LIST_END)
  {
    state_touched[i] = false;
    i = state_buckets[i].next;
  }
  // last element has not been untouched yet
  state_touched[i] = false;

  // insert the list contents_t[alpha] at the beginning of
  // contents_u[alpha]
  state_buckets[i].next = contents_u[alpha];
  if (contents_u[alpha] != LIST_END)
  {
    state_buckets[contents_u[alpha]].prev = i;
  }
  contents_u[alpha] = contents_t[alpha];
  contents_t[alpha] = LIST_END;
  block_touched[alpha] = false;
}

/* ----------------- UPDATE ----------------------------------------- */

template <class LTS_TYPE>
void sim_partitioner<LTS_TYPE>::update()
{
  using namespace mcrl2::core;
  mCRL2log(log::debug) << "--------------------- Update ---------------------------------------" << std::endl;

  size_t l,alpha,gamma;
  std::vector<size_t>::iterator alphai, last;

  induce_P_on_Pi();

  initialise_pre_EA();

  /* Compute the pre_exists and pre_forall functions */
  for (l = 0; l < aut.num_action_labels(); ++l)
  {
    pre_exists[l].reserve(s_Sigma + 1);
    pre_forall[l].reserve(s_Sigma + 1);
    pre_exists[l].push_back(exists->get_num_elements());
    pre_forall[l].push_back(forall->get_num_elements());
    for (gamma = 0; gamma < s_Sigma; ++gamma)
    {
      touched_blocks.clear();
      initialise_Sigma(gamma,l);
      last = touched_blocks.end();
      for (alphai = touched_blocks.begin(); alphai != last; ++alphai)
      {
        alpha = *alphai;
        exists->add(alpha,l,gamma);
        if (contents_u[alpha] == LIST_END)
        {
          forall->add(alpha,l,gamma);
        }
        untouch(alpha);
      }
      pre_exists[l].push_back(exists->get_num_elements());
      pre_forall[l].push_back(forall->get_num_elements());
    }
  }

  mCRL2log(log::debug) << "------ Filter(false) ------\nExists: ";
  mCRL2log(log::debug) << print_structure(exists);
  mCRL2log(log::debug) << "\nForall: ";
  mCRL2log(log::debug) << print_structure(forall);
  mCRL2log(log::debug) << "\nSimulation relation: ";
  mCRL2log(log::debug) << print_relation(s_Pi,Q);

  /* Apply the first filtering to Q */
  filter(s_Sigma,P,false);


  initialise_pre_EA();

  /* Compute the pre_exists and pre_forall functions */
  for (l = 0; l < aut.num_action_labels(); ++l)
  {
    pre_exists[l].reserve(s_Pi + 1);
    pre_forall[l].reserve(s_Pi + 1);
    pre_exists[l].push_back(exists->get_num_elements());
    pre_forall[l].push_back(forall->get_num_elements());
    for (gamma = 0; gamma < s_Pi; ++gamma)
    {
      touched_blocks.clear();
      initialise_Pi(gamma,l);
      last = touched_blocks.end();
      for (alphai = touched_blocks.begin(); alphai != last; ++alphai)
      {
        alpha = *alphai;
        exists->add(alpha,l,gamma);
        if (contents_u[alpha] == LIST_END)
        {
          forall->add(alpha,l,gamma);
        }
        untouch(alpha);
      }
      pre_exists[l].push_back(exists->get_num_elements());
      pre_forall[l].push_back(forall->get_num_elements());
    }
  }

  mCRL2log(log::debug) << "------ Filter(true) ------\nExists: ";
  mCRL2log(log::debug) << print_structure(exists);
  mCRL2log(log::debug) << "\nForall: ";
  mCRL2log(log::debug) << print_structure(forall);
  mCRL2log(log::debug) << "\nSimulation relation: ";
  mCRL2log(log::debug) << print_relation(s_Pi,Q);

  /* Apply the second filtering to Q */
  filter(s_Pi,Q,true);
}

template <class LTS_TYPE>
void sim_partitioner<LTS_TYPE>::initialise_pre_EA()
{
  /* Initialise the pre_exists and pre_forall data structures */
  exists->clear();
  forall->clear();
  std::vector<size_t> v;
  pre_exists.assign(aut.num_action_labels(),v);
  pre_forall.assign(aut.num_action_labels(),v);
}

template <class LTS_TYPE>
void sim_partitioner<LTS_TYPE>::induce_P_on_Pi()
{
  /* Compute the relation induced on Pi by P, store it in Q */
  std::vector<bool> v(s_Pi,false);
  Q.assign(s_Pi,v);

  size_t alpha,beta;
  std::vector< std::vector<bool> >::iterator P_parent_alpha;
  for (alpha = 0; alpha < s_Pi; ++alpha)
  {
    P_parent_alpha = P.begin() + parent[alpha];
    for (beta = 0; beta < s_Pi; ++beta)
    {
      Q[alpha][beta] = (*P_parent_alpha)[parent[beta]];
    }
  }
}


/* ----------------- FILTER ----------------------------------------- */

template <class LTS_TYPE>
void sim_partitioner<LTS_TYPE>::filter(size_t S,std::vector< std::vector<bool> > &R,
                                       bool B)
{
  /* Initialise the match function */
  match->clear();

  size_t alpha,beta,gamma,delta,l;
  hash_table3_iterator etrans(exists);
  for (l = 0; l < aut.num_action_labels(); ++l)
  {
    for (delta = 0; delta < S; ++delta)
    {
      etrans.set_end(pre_exists[l][delta+1]);
      for (etrans.set(pre_exists[l][delta]); !etrans.is_end(); ++etrans)
      {
        beta = etrans.get_x();
        for (gamma = 0; gamma < S; ++gamma)
        {
          if (R[gamma][delta])
          {
            match->add(l,beta,gamma);
          }
        }
      }
    }
  }

  hash_table3_iterator atrans(forall);
  /* The main for loop */
  for (l = 0; l < aut.num_action_labels(); ++l)
  {
    for (gamma = 0; gamma < S; ++gamma)
    {
      atrans.set_end(pre_forall[l][gamma+1]);
      for (atrans.set(pre_forall[l][gamma]); !atrans.is_end(); ++atrans)
      {
        alpha = atrans.get_x();
        for (beta = 0; beta < s_Pi; ++beta)
        {
          if (Q[alpha][beta] && !match->find(l,beta,gamma))
          {
            Q[alpha][beta] = false;
            if (B)
            {
              cleanup(alpha,beta);
            }
          }
        }
      }
    }
  }
}

/* ----------------- CLEANUP ---------------------------------------- */

template <class LTS_TYPE>
void sim_partitioner<LTS_TYPE>::cleanup(size_t alpha,size_t beta)
{
  size_t l,alpha1,beta1,delta;
  bool match_l_beta1_alpha;
  hash_table3_iterator alpha1i(forall);
  hash_table3_iterator beta1i(exists);
  for (l = 0; l < aut.num_action_labels(); ++l)
  {
    alpha1i.set_end(pre_forall[l][alpha+1]);
    beta1i.set_end(pre_exists[l][beta+1]);
    for (beta1i.set(pre_exists[l][beta]); !beta1i.is_end(); ++beta1i)
    {
      beta1 = beta1i.get_x();
      match_l_beta1_alpha = false;
      for (delta = 0; delta < s_Pi && !match_l_beta1_alpha; ++delta)
      {
        if (exists->find(beta1,l,delta) && Q[alpha][delta])
        {
          match_l_beta1_alpha = true;
        }
      }
      if (!match_l_beta1_alpha)
      {
        match->remove(l,beta1,alpha);
        for (alpha1i.set(pre_forall[l][alpha]); !alpha1i.is_end();
             ++alpha1i)
        {
          alpha1 = alpha1i.get_x();
          if (Q[alpha1][beta1])
          {
            Q[alpha1][beta1] = false;
            cleanup(alpha1,beta1);
          }
        }
      }
    }
  }
}

/* ----------------- FOR POST-PROCESSING ---------------------------- */

template <class LTS_TYPE>
std::vector < mcrl2::lts::transition> sim_partitioner<LTS_TYPE>::get_transitions() const
{
  using namespace mcrl2::lts;

  std::vector < mcrl2::lts::transition> ts;
  ts.reserve(forall->get_num_elements());

  std::vector<bool> pre_sim;
  transition::size_type l,beta,gamma;
  hash_table3_iterator alphai(exists);
  hash_table3_iterator gammai(forall);
  for (beta = 0; beta < s_Pi; ++beta)
  {
    for (l = 0; l < aut.num_action_labels(); ++l)
    {
      // there is an l-transition from alpha to beta iff:
      // - alpha -l->A [beta]
      // - not Exists gamma : beta Q gamma  /\  alpha -l->E gamma
      // first compute for which alpha the latter statement does not
      // hold
      pre_sim.assign(s_Pi,false);
      for (gamma = 0; gamma < s_Pi; ++gamma)
      {
        // only consider gammas that are unequal to beta
        if (gamma != beta && Q[beta][gamma])
        {
          alphai.set_end(pre_exists[l][gamma+1]);
          for (alphai.set(pre_exists[l][gamma]); !alphai.is_end();
               ++alphai)
          {
            pre_sim[alphai.get_x()] = true;
          }
        }
      }
      gammai.set_end(pre_forall[l][beta+1]);
      for (gammai.set(pre_forall[l][beta]); !gammai.is_end(); ++gammai)
      {
        gamma = gammai.get_x();
        if (!pre_sim[gamma])
        {
          // add the transition gamma -l-> beta
          ts.push_back(transition(gamma,l,beta));
        }
      }
    }
  }
  return ts;
}

template <class LTS_TYPE>
size_t sim_partitioner<LTS_TYPE>::num_eq_classes() const
{
  return s_Pi;
}

template <class LTS_TYPE>
size_t sim_partitioner<LTS_TYPE>::get_eq_class(size_t s) const
{
  return block_Pi[s];
}

template <class LTS_TYPE>
bool sim_partitioner<LTS_TYPE>::in_preorder(size_t s,size_t t) const
{
  return Q[block_Pi[s]][block_Pi[t]];
}

template <class LTS_TYPE>
bool sim_partitioner<LTS_TYPE>::in_same_class(size_t s,size_t t) const
{
  return (block_Pi[s] == block_Pi[t]);
}

/*
// PRE: FORALL and EXISTS structures on Pi are computed
void sim_partitione<LTS_TYPE>r::update_nfa()
{
  std::vector< std::vector<size_t> > v(s_Pi);
  std::vector< std::vector< std::vector<size_t> > > post_trans(L,v);

  std::vector<bool> pre_sim;
  size_t l,beta,alpha,gamma;
  hash_table3_iterator alphai(exists);
  hash_table3_iterator gammai(forall);
  for (beta = 0; beta < s_Pi; ++beta)
  {
    for (l = 0; l < aut.num_action_labels(); ++l)
    {
      // there is an l-transition from alpha to beta iff:
      // - alpha -l->A [beta]
      // - not Exists gamma : beta Q gamma  /\  alpha -l->E gamma
      // first compute for which alpha the latter statement does not
      // hold
      pre_sim.assign(s_Pi,false);
      for (gamma = 0; gamma < s_Pi; ++gamma)
      {
        // only consider gammas that are unequal to beta
        if (gamma != beta && Q[beta][gamma])
        {
          alphai.set_end(pre_exists[l][gamma+1]);
          for (alphai.set(pre_exists[l][gamma]); !alphai.is_end();
              ++alphai)
          {
            pre_sim[alphai.get_x()] = true;
          }
        }
      }
      gammai.set_end(pre_forall[l][beta+1]);
      for (gammai.set(pre_forall[l][beta]); !gammai.is_end(); ++gammai)
      {
        gamma = gammai.get_x();
        if (!pre_sim[gamma])
        {
          post_trans[l][gamma].push_back(beta);
        }
      }
    }
  }

  std::vector<transition> *trans = new std::vector<transition> ();
  std::vector<bool> *final = new std::vector<bool> ();

  // Do a breadth first search over the computed graph to assign ids to
  // the reachable blocks and construct the final transition relation
  std::vector<size_t>::iterator post_it,post_last;
  std::queue<size_t> todo;
  std::vector<size_t> block_id(s_Pi,s_Pi);
  todo.push(block_Pi[aut.initial_state()]);
  block_id[block_Pi[aut.initial_state()]] = 0;
  size_t n_s = 1;
  size_t n_f = 0;
  if (aut.is_final(aut.initial_state()))
  {
    final->push_back(true);
    ++n_f;
  }
  else
  {
    final->push_back(false);
  }
  while (!todo.empty())
  {
    // visit todo.front()
    beta = todo.front();
    for (l = 0; l < L; ++l)
    {
      post_last = post_trans[l][beta].end();
      for (post_it = post_trans[l][beta].begin(); post_it != post_last;
          ++post_it)
      {
        alpha = *post_it;
        if (block_id[alpha] == s_Pi)
        {
          todo.push(alpha);
          block_id[alpha] = n_s;
          ++n_s;
          if (aut.is_final(contents_u[alpha]))
          {
            final->push_back(true);
            ++n_f;
          }
          else
          {
            final->push_back(false);
          }
        }
        transition t = {block_id[beta], l, block_id[alpha]};
        trans->push_back(t);
      }
    }
    todo.pop();
  }

  // assign the new transition relation to aut
  aut.clear_transition_relation();
  aut.clear_final();
  aut.set_num_states(n_s);
  aut.set_num_transitions(trans->size());
  aut.set_num_final(n_f);
  aut.set_init(0);
  aut.set_trans(trans);
  aut.set_final(final);

  // assign the simulation relation to aut
  aut.init_simulation_relation();
  for (beta = 0; beta < s_Pi; ++beta)
  {
    // only consider reachable blocks
    if (block_id[beta] < s_Pi)
    {
      for (gamma = 0; gamma < s_Pi; ++gamma)
      {
        if (block_id[gamma] < s_Pi && Q[beta][gamma])
        {
          aut.add_simulation(block_id[beta],block_id[gamma]);
        }
      }
    }
  }
}
*/

/* ----------------- FOR DEBUGGING ---------------------------------- */

template <class LTS_TYPE>
std::string sim_partitioner<LTS_TYPE>::print_Sigma_P()
{
  using namespace mcrl2::core;
  std::stringstream result;
  result << print_Sigma() << "Simulation relation: " << print_relation(s_Sigma,P);
  return result.str();
}

template <class LTS_TYPE>
std::string sim_partitioner<LTS_TYPE>::print_Pi_Q()
{
  using namespace mcrl2::core;
  std::stringstream result;
  result << print_Pi() << "Simulation relation: " << print_relation(s_Pi,Q);
  return result.str();
}

template <class LTS_TYPE>
std::string sim_partitioner<LTS_TYPE>::print_Sigma()
{
  using namespace mcrl2::core;
  std::stringstream result;
  std::vector<size_t>::iterator ci, last;
  for (size_t b = 0; b < s_Sigma; ++b)
  {
    result << "block " << b << ": {";
    last = children[b].end();
    for (ci = children[b].begin(); ci != last; ++ci)
    {
      result << print_block(*ci);
    }
    result << "}" << std::endl;
  }
  return result.str();
}

template <class LTS_TYPE>
std::string sim_partitioner<LTS_TYPE>::print_Pi()
{
  std::stringstream result;
  using namespace mcrl2::core;
  for (size_t b = 0; b < s_Pi; ++b)
  {
    result << "block " << b << ": {" << print_block(b) << "}" << std::endl;
  }
  return result.str();
}

template <class LTS_TYPE>
std::string sim_partitioner<LTS_TYPE>::print_block(size_t b)
{
  std::stringstream result;
  using namespace mcrl2::core;
  for (ptrdiff_t i = contents_u[b]; i != LIST_END; i = state_buckets[i].next)
  {
    result << i << ",";
  }
  for (ptrdiff_t i = contents_t[b]; i != LIST_END; i = state_buckets[i].next)
  {
    result << i << ",";
  }
  return result.str();
}

template <class LTS_TYPE>
std::string sim_partitioner<LTS_TYPE>::print_relation(size_t s,
    std::vector< std::vector<bool> > &R)
{
  using namespace mcrl2::core;
  std::stringstream result;
  result << "{";
  size_t beta,gamma;
  for (beta = 0; beta < s; ++beta)
  {
    for (gamma = 0; gamma < s; ++gamma)
    {
      if (R[beta][gamma])
      {
        result << "(" << beta << "," << gamma << "),";
      }
    }
  }
  result << "}" << std::endl;
  return result.str();
}

template <class LTS_TYPE>
std::string sim_partitioner<LTS_TYPE>::print_structure(hash_table3* struc)
{
  using namespace mcrl2::core;
  std::stringstream result;
  result << "{";
  hash_table3_iterator i(struc);
  for (; !i.is_end(); ++i)
  {
    result << "(" << i.get_x() << "," << mcrl2::lts::pp(aut.action_label(i.get_y()))
           << "," << i.get_z() << "),";
  }
  result << "}";
  return result.str();
}

} // namespace detail
} // namespace lts
} // namespace mcrl2
#endif
