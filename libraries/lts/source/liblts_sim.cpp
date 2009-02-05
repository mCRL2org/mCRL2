// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_sim.cpp
#include "mcrl2/lts/detail/liblts_sim.h"
#include "mcrl2/core/messaging.h"

#define LIST_END (-1)
using namespace mcrl2::core;

sim_partitioner::sim_partitioner(mcrl2::lts::lts *l)
  : aut(l), trans_index(NULL)
{ }

sim_partitioner::~sim_partitioner()
{
  if (trans_index != NULL)
  {
    for (unsigned int l = 0; l < aut->num_labels(); ++l)
    {
      free(trans_index[l]);
    }
  }
  free(trans_index);
  aut = NULL;
  delete match;
  delete exists;
  delete forall;
}

/* ----------------- PARTITIONING ALGORITHM ------------------------- */

void sim_partitioner::partitioning_algorithm()
{
  initialise_datastructures();

  if (gsDebug)
  {
    gsMessage("--------------------- INITIALISATION ---------------------------\n");
  }

  gsVerboseMsg("  initialisation; number of blocks: %u\n",s_Sigma);
  bool change;
  uint i;

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

    if (gsDebug)
    {
      gsMessage("--------------------- ITERATION %u ----------------------------------\n",i);
    }

    gsVerboseMsg("  iteration %u; number of blocks: %u\n",i,s_Sigma);

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
  if (gsDebug)
  {
    print_Pi_Q();
  }
}

void sim_partitioner::initialise_datastructures()
{
  aut->sort_transitions(mcrl2::lts::lbl_tgt_src);
  trans_index = aut->get_transition_pre_table();

  uint N = aut->num_states();

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
  for (uint i = 0; i < N; ++i)
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
  std::vector<uint> vi;
  children.assign(s_Sigma,vi);
  std::vector<bool> vb(s_Sigma,false);
  P.assign(s_Sigma,vb);
  for (uint i = 0; i < s_Sigma; ++i)
  {
    children[i].push_back(i);
    P[i][i] = true;
  }
}

/* ----------------- INITIALISE ------------------------------------- */

void sim_partitioner::initialise_Pi(uint gamma,uint l)
{
  uint alpha, a, c, t, t_last;
  std::vector<uint>::iterator ci, last;

  contents.clear();
  for (int i = contents_u[gamma]; i != LIST_END;
      i = state_buckets[i].next)
  {
    contents.push_back(uint(i));
  }
  for (int i = contents_t[gamma]; i != LIST_END;
      i = state_buckets[i].next)
  {
    contents.push_back(uint(i));
  }
  last = contents.end();
  for (ci = contents.begin(); ci != last; ++ci)
  {
    c = *ci;
    /* iterate over the incoming l-transitions of c */
    t_last = trans_index[l][c+1];
    for (t = trans_index[l][c]; t < t_last; ++t)
    {
      a = aut->transition_from(t);
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

void sim_partitioner::initialise_Sigma(uint gamma,uint l)
{
  std::vector<uint>::iterator deltai, last;
  last = children[gamma].end();
  for (deltai = children[gamma].begin(); deltai != last; ++deltai)
  {
    initialise_Pi(*deltai,l);
  }
}

/* ----------------- REFINE ----------------------------------------- */

/* PRE: s_Sigma = s_Pi */
void sim_partitioner::refine(bool &change)
{
  /* Initialise the parent and children functions */
  std::vector<uint> v;
  children.assign(s_Pi,v);
  parent.assign(s_Pi,0);
  uint alpha;
  for (alpha = 0; alpha < s_Pi; ++alpha)
  {
    children[alpha].push_back(alpha);
    parent[alpha] = alpha;
  }

  if (gsDebug)
  {
    gsMessage("--------------------- Refine ---------------------------------------\n");
    print_Sigma_P();
  }

  /* Compute a reverse topological sorting of Sigma w.r.t. P */
  std::vector<uint> Sort;
  Sort.reserve(s_Sigma);
  reverse_topological_sort(Sort);

  if (gsDebug)
  {
    gsMessage("reverse topological sort is: [");
    for (uint i = 0; i < Sort.size(); ++i)
    {
      gsMessage("%u",Sort[i]);
      if (i+1 < Sort.size())
      {
        gsMessage(",");
      }
    }
    gsMessage("]\n");
  }

  /* Some local variables */
  std::vector<bool> v_false(s_Sigma,false);
  std::vector<uint>::iterator alphai, last, gammai;
  std::vector< std::vector<bool> >::iterator stable_alpha, P_gamma;
  bool stable_alpha_gamma;
  uint gamma, delta, l;
  int i;

  /* The main loop */
  for (l = 0; l < aut->num_labels(); ++l)
  {
    if (gsDebug)
    {
      gsMessage("---------------------------------------------------\n");
      gsMessage("Label = \"%s\"\n", aut->label_value_str(l).c_str());
    }

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
            for (i = contents_u[s_Pi]; i != LIST_END;
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

void sim_partitioner::reverse_topological_sort(std::vector<uint> &Sort)
{
  std::vector<bool> visited(s_Sigma,false);
  for (uint u = 0; u < s_Sigma; ++u)
  {
    if (!visited[u])
    {
      dfs_visit(u,visited,Sort);
    }
  }
}

void sim_partitioner::dfs_visit(uint u,std::vector<bool> &visited,
    std::vector<uint> &Sort)
{
  visited[u] = true;
  for (uint v = 0; v < s_Sigma; ++v)
  {
    if (!visited[v] && P[u][v])
    {
      dfs_visit(v,visited,Sort);
    }
  }
  Sort.push_back(u);
}

void sim_partitioner::touch(uint a,uint alpha)
{
  state_touched[a] = true;
  int p = state_buckets[a].prev;
  int n = state_buckets[a].next;
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
void sim_partitioner::untouch(uint alpha)
{
  // search linearly for the last element of contents_t[alpha];
  // untouch every state we encounter along the way
  int i = contents_t[alpha];
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

void sim_partitioner::update()
{
  if (gsDebug)
  {
    gsMessage("--------------------- Update ---------------------------------------\n");
  }

  uint l,alpha,gamma;
  std::vector<uint>::iterator alphai, last;

  induce_P_on_Pi();

  initialise_pre_EA();

  /* Compute the pre_exists and pre_forall functions */
  for (l = 0; l < aut->num_labels(); ++l)
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

  if (gsDebug)
  {
    gsMessage("------ Filter(false) ------\nExists: ");
    print_structure(exists);
    gsMessage("\nForall: ");
    print_structure(forall);
    gsMessage("\nSimulation relation: ");
    print_relation(s_Pi,Q);
  }

  /* Apply the first filtering to Q */
  filter(s_Sigma,P,false);


  initialise_pre_EA();

  /* Compute the pre_exists and pre_forall functions */
  for (l = 0; l < aut->num_labels(); ++l)
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
  if (gsDebug)
  {
    gsMessage("------ Filter(true) ------\nExists: ");
    print_structure(exists);
    gsMessage("\nForall: ");
    print_structure(forall);
    gsMessage("\nSimulation relation: ");
    print_relation(s_Pi,Q);
  }

  /* Apply the second filtering to Q */
  filter(s_Pi,Q,true);
}

void sim_partitioner::initialise_pre_EA()
{
  /* Initialise the pre_exists and pre_forall data structures */
  exists->clear();
  forall->clear();
  std::vector<uint> v;
  pre_exists.assign(aut->num_labels(),v);
  pre_forall.assign(aut->num_labels(),v);
}

void sim_partitioner::induce_P_on_Pi()
{
  /* Compute the relation induced on Pi by P, store it in Q */
  std::vector<bool> v(s_Pi,false);
  Q.assign(s_Pi,v);

  uint alpha,beta;
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

void sim_partitioner::filter(uint S,std::vector< std::vector<bool> > &R,
    bool B)
{
  /* Initialise the match function */
  match->clear();

  uint alpha,beta,gamma,delta,l;
  hash_table3_iterator etrans(exists);
  for (l = 0; l < aut->num_labels(); ++l)
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
  for (l = 0; l < aut->num_labels(); ++l)
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

void sim_partitioner::cleanup(uint alpha,uint beta)
{
  uint l,alpha1,beta1,delta;
  bool match_l_beta1_alpha;
  hash_table3_iterator alpha1i(forall);
  hash_table3_iterator beta1i(exists);
  for (l = 0; l < aut->num_labels(); ++l)
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


mcrl2::lts::transition* sim_partitioner::get_transitions(uint& nt,uint& size) const
{
  using namespace mcrl2::lts;

  size = forall->get_num_elements();
  transition* ts = (transition*)malloc(size*sizeof(transition));
  if (ts == NULL)
  {
    gsErrorMsg("out of memory\n");
    exit(1);
  }

  nt = 0;
  std::vector<bool> pre_sim;
  uint l,beta,gamma;
  hash_table3_iterator alphai(exists);
  hash_table3_iterator gammai(forall);
  for (beta = 0; beta < s_Pi; ++beta)
  {
    for (l = 0; l < aut->num_labels(); ++l)
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
          ts[nt].from = gamma;
          ts[nt].label = l;
          ts[nt].to = beta;
          ++nt;
        }
      }
    }
  }
  return ts;
}

uint sim_partitioner::num_eq_classes() const
{
  return s_Pi;
}

uint sim_partitioner::get_eq_class(uint s) const
{
  return block_Pi[s];
}

bool sim_partitioner::in_preorder(uint s,uint t) const
{
  return Q[block_Pi[s]][block_Pi[t]];
}

bool sim_partitioner::in_same_class(uint s,uint t) const
{
  return (block_Pi[s] == block_Pi[t]);
}

/*
// PRE: FORALL and EXISTS structures on Pi are computed
void sim_partitioner::update_nfa()
{
  std::vector< std::vector<uint> > v(s_Pi);
  std::vector< std::vector< std::vector<uint> > > post_trans(L,v);

  std::vector<bool> pre_sim;
  uint l,beta,alpha,gamma;
  hash_table3_iterator alphai(exists);
  hash_table3_iterator gammai(forall);
  for (beta = 0; beta < s_Pi; ++beta)
  {
    for (l = 0; l < aut->num_labels(); ++l)
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
  std::vector<uint>::iterator post_it,post_last;
  std::queue<uint> todo;
  std::vector<uint> block_id(s_Pi,s_Pi);
  todo.push(block_Pi[aut->initial_state()]);
  block_id[block_Pi[aut->initial_state()]] = 0;
  uint n_s = 1;
  uint n_f = 0;
  if (aut->is_final(aut->initial_state()))
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
          if (aut->is_final(contents_u[alpha]))
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
  aut->clear_transition_relation();
  aut->clear_final();
  aut->set_num_states(n_s);
  aut->set_num_transitions(trans->size());
  aut->set_num_final(n_f);
  aut->set_init(0);
  aut->set_trans(trans);
  aut->set_final(final);

  // assign the simulation relation to aut
  aut->init_simulation_relation();
  for (beta = 0; beta < s_Pi; ++beta)
  {
    // only consider reachable blocks
    if (block_id[beta] < s_Pi)
    {
      for (gamma = 0; gamma < s_Pi; ++gamma)
      {
        if (block_id[gamma] < s_Pi && Q[beta][gamma])
        {
          aut->add_simulation(block_id[beta],block_id[gamma]);
        }
      }
    }
  }
}
*/

/* ----------------- FOR DEBUGGING ---------------------------------- */

void sim_partitioner::print_Sigma_P()
{
  print_Sigma();
  gsMessage( "Simulation relation: ");
  print_relation(s_Sigma,P);
}

void sim_partitioner::print_Pi_Q()
{
  print_Pi();
  gsMessage("Simulation relation: ");
  print_relation(s_Pi,Q);
}

void sim_partitioner::print_Sigma()
{
  std::vector<uint>::iterator ci, last;
  for (uint b = 0; b < s_Sigma; ++b)
  {
    gsMessage("block %u: {",b);
    last = children[b].end();
    for (ci = children[b].begin(); ci != last; ++ci)
    {
      print_block(*ci);
    }
    gsMessage("}\n");
  }
}

void sim_partitioner::print_Pi()
{
  for (uint b = 0; b < s_Pi; ++b)
  {
    gsMessage("block %u: {",b);
    print_block(b);
    gsMessage("}\n");
  }
}

void sim_partitioner::print_block(uint b)
{
  for (int i = contents_u[b]; i != LIST_END; i = state_buckets[i].next)
  {
    gsMessage("%d,",i);
  }
  for (int i = contents_t[b]; i != LIST_END; i = state_buckets[i].next)
  {
    gsMessage("%d,",i);
  }
}

void sim_partitioner::print_relation(uint s,
    std::vector< std::vector<bool> > &R)
{
  gsMessage("{");
  uint beta,gamma;
  for (beta = 0; beta < s; ++beta)
  {
    for (gamma = 0; gamma < s; ++gamma)
    {
      if (R[beta][gamma])
      {
        gsMessage("(%u,%u),",beta,gamma);
      }
    }
  }
  gsMessage("}\n");
}

void sim_partitioner::print_structure(hash_table3 *struc)
{
  gsMessage("{");
  hash_table3_iterator i(struc);
  for ( ; !i.is_end(); ++i)
  {
    gsMessage("(%u,%s,%u),", i.get_x(),
        aut->label_value_str(i.get_y()).c_str(), i.get_z());
  }
  gsMessage("}");
}

/* Needed only for reading an initial partition from a file; this is
 * probably not needed in the LTS library
void sim_partitioner::read_partition_from_file(char *fn)
{
  std::ifstream f(fn);
  if (!f)
  {
    std::cerr << "error: cannot open partition file for reading"
      << std::endl;
    exit(1);
  }
  char c = 0;

  s_Pi = 0;
  uint s;
  int prev;
  while (f >> c)
  {
    if (c != '(')
    {
      std::cerr << "error: '(' expected but '" << c << "' found\n";
      exit(1);
    }
    // found another block; read it
    prev = LIST_END;
    contents_u.push_back(LIST_END);
    contents_t.push_back(LIST_END);
    while (c != ')')
    {
      if (!(f >> s))
      {
        std::cerr << "error: could not read state ";
        std::cerr << "while reading block " << s_Sigma << std::endl;
        exit(1);
      }
      // add state s to partition block s_Pi
      block_Pi[s] = s_Pi;
      if (prev == LIST_END)
      {
        contents_u[s_Pi] = s;
      }
      else
      {
        state_buckets[prev].next = s;
      }
      state_buckets[s].prev = prev;
      prev = s;

      if (!(f >> c))
      {
        std::cerr << "error: could not read ',' or ')' ";
        std::cerr << "while reading block " << s_Sigma << std::endl;
        exit(1);
      }
      if (c != ',' && c != ')')
      {
        std::cerr << "error: ',' or ')' expected but '" << c
          << "' found\n";
        exit(1);
      }
    }
    ++s_Pi;
  }
}
*/
