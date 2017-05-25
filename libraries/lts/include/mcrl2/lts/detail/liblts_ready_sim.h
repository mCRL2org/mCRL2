/// Author(s): # Carlos Gregorio-Rodriguez, Luis LLana, Rafael Martinez-Torres.
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_ready_sim.h
/// \brief Header file for the ready-simulation preorder algorithm

#ifndef MCRL2_LTS_LIBLTS_READY_SIM_H
#define MCRL2_LTS_LIBLTS_READY_SIM_H

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
#include "mcrl2/lts/detail/liblts_sim.h"
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
  class ready_sim_partitioner : public sim_partitioner<LTS_TYPE>
{
 public:
  ready_sim_partitioner(LTS_TYPE& l) : sim_partitioner<LTS_TYPE>(l)
  {
    exists2 = new hash_table2(1000);
    forall2 = new hash_table2(1000);
  };

    /**
     * Computes the ready-simulation equivalence
     * classes and preorder relations of the LTS
     */  
  virtual void partitioning_algorithm();
    /** Destroys this partitioner. */
  ~ready_sim_partitioner();
 private :
  // Non inherited data members...
  hash_table2* exists2;
  hash_table2* forall2;  

  // Non inherited methods...
    /**
     * Drives the partition pair <Universal,Id>
     * into appropriate ready partition-pair
     * for furhter input into GCPP.
     * \pre : true
     */  	 
  void ready2sim_reduction();

  /**
   * Splits Universal partition
   * into classes alpha where either
   * { N } A<-l- alpha or
   * not { N } <-l- alpha
   * for some L
   * \pre : initialise_datastructures called.
   */
  void refinei() ;

  /*
   * Induce P on Pi, then
   * drops those pairs (alpha, beta)
   * such that
   *  {N} E<-l- Beta and not {N} A<-l- Alpha
   * \pre : refinei  called 
   */
  void updatei();


  // Non inherited auxiliary methods...
  std::string print_structure(hash_table2* struc);  
  
  // Inherited data members...
  using sim_partitioner<LTS_TYPE>::aut;
  using sim_partitioner<LTS_TYPE>::s_Pi;
  using sim_partitioner<LTS_TYPE>::s_Sigma;  
  using sim_partitioner<LTS_TYPE>::block_Pi;
  using sim_partitioner<LTS_TYPE>::children;
  using sim_partitioner<LTS_TYPE>::parent;
  using sim_partitioner<LTS_TYPE>::Q;
  using sim_partitioner<LTS_TYPE>::P;  
  using sim_partitioner<LTS_TYPE>::contents_t;
  using sim_partitioner<LTS_TYPE>::contents_u;
  using sim_partitioner<LTS_TYPE>::touched_blocks;  
  using sim_partitioner<LTS_TYPE>::block_touched;
  using sim_partitioner<LTS_TYPE>::state_buckets;

  // Inherited methods
  using sim_partitioner<LTS_TYPE>::induce_P_on_Pi;
  using sim_partitioner<LTS_TYPE>::initialise_Sigma;    
  using sim_partitioner<LTS_TYPE>::initialise_datastructures;  
  using sim_partitioner<LTS_TYPE>::partitioning_algorithmG;
  using sim_partitioner<LTS_TYPE>::touch;
  using sim_partitioner<LTS_TYPE>::untouch;

  // Auxiliary inherited methods
  using sim_partitioner<LTS_TYPE>::print_relation;
  using sim_partitioner<LTS_TYPE>::print_Sigma_P;
  using sim_partitioner<LTS_TYPE>::print_Pi_Q;

};


/* Pre : true */
template <class LTS_TYPE>
void ready_sim_partitioner<LTS_TYPE>::ready2sim_reduction()
{
  initialise_datastructures();
  refinei();
  updatei();
  s_Sigma = s_Pi; // <== Key!!
  P.swap(Q);      // <== Key!!
  mCRL2log(log::debug) << "--------------------- READY PRE-REDUCTION-------------------------" << std::endl;
  mCRL2log(log::debug) << "  prereduction; number of blocks: " << s_Sigma << std::endl;
}

/* ----------------- PARTITIONING ALGORITHM ------------------------- */
/* Pre : true */
template <class LTS_TYPE>
void ready_sim_partitioner<LTS_TYPE>::partitioning_algorithm()
{
  // ready_sim problem amounts to GCPP previous reduction
  ready2sim_reduction();
  partitioning_algorithmG();
}

/* ----------------- REFINE ----------------------------------------- */

/* PRE: s_Sigma = s_Pi = 1 */
template <class LTS_TYPE>
void ready_sim_partitioner<LTS_TYPE>::refinei()
{
  using namespace mcrl2::core;  
  /* Initialise the parent and children functions */
  std::vector<size_t> v;
  children.clear();
  children.assign(s_Pi,v);
  parent.clear();
  parent.assign(s_Pi,UNIVERSAL_PART);
  size_t alpha;
  for (alpha = 0; alpha < s_Pi; ++alpha)
  {
    children[alpha].push_back(alpha);
    parent[alpha] = alpha;
  }

  if (mCRL2logEnabled(log::debug))
  {
    mCRL2log(log::debug) << "---------------- Refinei ---------------------------------------" << std::endl;
    mCRL2log(log::debug) << print_Sigma_P();
  }

  /* Some local variables */
  std::vector<size_t>::iterator alphai, last;
  size_t l;  

  /* The main loop */  
  for (l = 0; l < aut.num_action_labels(); ++l)
  {
    mCRL2log(log::debug) << "---------------------------------------------------" << std::endl;
    mCRL2log(log::debug) << "Label = \"" << mcrl2::lts::pp(aut.action_label(l)) << "\"" << std::endl;

    touched_blocks.clear();
    initialise_Sigma(UNIVERSAL_PART,l);  // Interested on {N} E<-l- alpha
    
    /* iterate over all alpha such that {N} E<-l- alpha  */
    last = touched_blocks.end();
    for (alphai = touched_blocks.begin(); alphai != last; ++alphai)
      {
	alpha = *alphai;
	/* if {N} A<-l- alpha then alpha cannot be split */
	if (contents_u[alpha] != LIST_END)
	  {
	    /* split alpha; new block will be s_Pi */
	    children[parent[alpha]].push_back(s_Pi);  
	    parent.push_back(parent[alpha]);
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
	untouch(alpha); 
      }
  }
}

 
/* ----------------- UPDATEI ----------------------------------------- */
template <class LTS_TYPE>
void ready_sim_partitioner<LTS_TYPE>::updatei()
{
  using namespace mcrl2::core;
  mCRL2log(log::debug) << "---------------  Updatei ---------------------------------------" << std::endl;
  
  size_t l,alpha,gamma;
  std::vector<size_t>::iterator alphai, last;

  induce_P_on_Pi(); 

  exists2->clear();
  forall2->clear();

  for (l = 0; l < aut.num_action_labels(); ++l)
  {
    for (gamma = 0; gamma < s_Sigma; ++gamma) 
    {
      touched_blocks.clear();
      initialise_Sigma(gamma,l); 
      last = touched_blocks.end(); 
      for (alphai = touched_blocks.begin(); alphai != last; ++alphai)
      {
        alpha = *alphai;
        exists2->add(alpha,l); 
        if (contents_u[alpha] == LIST_END)
          {
            forall2->add(alpha,l);
          }
        untouch(alpha);
      }
    }
  }

  mCRL2log(log::debug) << "------ Before Filter ------\nExists2: ";
  mCRL2log(log::debug) << print_structure(exists2);
  mCRL2log(log::debug) << "\nForall2: ";
  mCRL2log(log::debug) << print_structure(forall2);
  mCRL2log(log::debug) << "\nReady Preorder: ";
  mCRL2log(log::debug) << print_relation(s_Pi,Q);  

  size_t beta;

  /* Apply the filtering to discard non "ready-init" pairs,
   * in Q[alpha][beta], i.e.,  when
   * {N} E<-l- Beta and not {N} A<-l- Alpha
   */
  for (l = 0; l < aut.num_action_labels(); ++l)
  {
    for (beta = 0; beta < s_Pi; ++beta) 
    {
      if (exists2->find(beta,l)) 
        {
          for (alpha = 0; alpha < s_Pi; ++alpha)
          if (Q[alpha][beta] && !forall2->find(alpha,l))
            Q[alpha][beta] = false ;
        }
    }
  };

  mCRL2log(log::debug) << "-----  After Filter ------\nExists2: ";
  mCRL2log(log::debug) << print_structure(exists2);
  mCRL2log(log::debug) << "\nForall2: ";
  mCRL2log(log::debug) << print_structure(forall2);
  mCRL2log(log::debug) << "\nReady Preorder: ";
  mCRL2log(log::debug) << print_relation(s_Pi,Q);  
}

template <class LTS_TYPE>
std::string ready_sim_partitioner<LTS_TYPE>::print_structure(hash_table2* struc)
{
  using namespace mcrl2::core;
  std::stringstream result;
  result << "{";
  hash_table2_iterator i(struc);
  for (; !i.is_end(); ++i)
  {
    result << "(" << i.get_x() << "," << mcrl2::lts::pp(aut.action_label(i.get_y()))<< "),";
  }
  result << "}";
  return result.str();  
}

template <class LTS_TYPE>
ready_sim_partitioner<LTS_TYPE>::~ready_sim_partitioner()
{
  delete exists2;
  delete forall2;
}

 
} // namespace detail
} // namespace lts
} // namespacemcrl2
#endif
