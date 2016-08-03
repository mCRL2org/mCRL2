// Author(s): Jan Friso Groote, Anton Wijs
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/liblts_bisim_gw.h

#ifndef _LIBLTS_BISIM_MLOGN_H
#define _LIBLTS_BISIM_MLOGN_H
#include <cmath>
#include <unordered_map>
#include <string>
#include <tuple>
#include <stack>
#include <queue>
#include <algorithm> // for sorting of transitions by target state
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/trace/trace.h"
#include "mcrl2/lts/lts_utilities.h"
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_merge.h"
#include "mcrl2/lts/detail/sized_forward_list_gw.h"
#include "mcrl2/lts/detail/pool_gw.h"
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

// markers to differentiate various states.
typedef enum { BTM_STATE, NON_BTM_STATE, MARKED_BTM_STATE, MARKED_NON_BTM_STATE, DEFAULT_STATE } state_marker;

// definition to distinguish two types of blocks
typedef enum { STD_BLOCK, EXTRA_KRIPKE_BLOCK } block_marker;

// definition to distinguish two types of constellations
typedef enum { TRIVIAL, NONTRIVIAL } constellation_marker;

// return values for procedures in lockstep search
typedef enum { CONTINUE, TERMINATED, STOPPED } lockstep_search_mode;

typedef size_t state_type;
typedef size_t trans_type;
typedef size_t label_type;

class state_T;
class block_T;
class transition_T;
class constellation_T;
class to_constlns_element_T;

class counter_T
{
  protected:
    size_t m_cnt;

    // A deque containing all the counters.
    static std::deque<counter_T>& counter_pool()
    {
      static std::deque<counter_T> m_counter_pool;
      return m_counter_pool;
    }

    // First free position is 0 if there is no
    // free position. Otherwise it points to the
    // first_free_position-1 in the counter_pool,
    // where an unused counter can be found.
    static counter_T*& first_free_position()
    {
      static counter_T* m_first_free_position=nullptr;
      return m_first_free_position;
    }

    // The constructor is intentionally not public.
    // A counter must be created by new_counter.
    counter_T()
     : m_cnt(0)
    {}

    bool check_counter_free() const
    {
      for(counter_T* i= first_free_position(); i!=nullptr; i=reinterpret_cast<counter_T*>(i->m_cnt))
      {
        if (this==i)
        { 
          return true;
        }
      }
      return false;
    }

  public:
    void increment()
    {
      assert(!check_counter_free());
      m_cnt++;
    }

    void decrement()
    {
      assert(!check_counter_free());
      assert(m_cnt>0);
      m_cnt--;
    }

    size_t counter_value() const
    {
      assert(!check_counter_free());
      return m_cnt;
    }

    void delete_counter()
    {
      assert(!check_counter_free());
      assert(m_cnt==0);
      // garbage collect this counter.
      m_cnt=reinterpret_cast<size_t>(first_free_position());
      first_free_position()=this;
    }

    // A counter must be created using new_counter, and
    // it cannot be constructed using a constructor.
    static counter_T* new_counter()
    {
      if (first_free_position()!=nullptr)
      {
        counter_T* result=first_free_position();
        first_free_position()=reinterpret_cast<counter_T*>(first_free_position()->m_cnt);
        result->m_cnt=0;
      assert(!result->check_counter_free());
        return result;
      }
      counter_pool().emplace_back(counter_T());
      assert(!counter_pool().back().check_counter_free());
      return &counter_pool().back();
    }
};

class state_T
{
  public:
    // block B' containing state s
    block_T *block;
    // begin index for transitions from s (s -> s')
    trans_type Ttgt_begin;
    // begin index for transitions to s (s <- s')
    trans_type Tsrc_begin;

    // ADDITIONAL INFO to keep track of where the state is listed into which list (pointers point to positions preceding the ones for the state)
    sized_forward_list<state_T >::iterator ptr_in_list;
    std::multiset<state_T*>::iterator pos_in_P_detect2;
    // is the state in stack of detect1?
    bool is_in_L_detect1;
    // is the state in priority queue of detect2?
    bool is_in_P_detect2;
    // iterator pointing to the position of the state in P
    // is the state in Lp of detect2?
    bool is_in_Lp_detect2;

    state_marker type;

    // reference to counter of number of transitions from B' to block B in constellation C (splitting is done under C)
    counter_T *constln_cnt;
    // reference to counter of number of transitions from B' to constellation C without block B (splitting is done under C)
    counter_T *coconstln_cnt;
    // number of inert transitions from s to a state in B'. Use unsigned int to reduce memory footprint.
    unsigned int inert_cnt;
    // priority used for priority queue in detect2 when splitting. Use unsigned int to reduce memory footprint.
    unsigned int priority;
  
    // constructor
    state_T()
     : block(nullptr),
       Ttgt_begin(0),
       Tsrc_begin(0),
       is_in_L_detect1(false), 
       is_in_P_detect2(false), 
       is_in_Lp_detect2(false),
       type(DEFAULT_STATE),
       constln_cnt(nullptr), 
       coconstln_cnt(nullptr),
       inert_cnt(0),
       priority(0)
    {}
};

class transition_T
{
  public:
    // Struct containing information about transition s->s'. Let B be block containing s, C constellation containing s'
    // source of transition (s)
    state_T* source;
    // target of transition (s')
    state_T* target;
    // pointer to constellation counter for corresponding (s, C) combination
    counter_T* to_constln_cnt;
    // pointer to C entry in to_constlns list of B
    to_constlns_element_T* to_constln_ref;
    
    // ADDITIONAL INFO to keep track of where the transition is listed in a (block)
    // constln_transitions list
    // typename
    sized_forward_list<transition_T >::iterator ptr_in_list;
    
    // constructor
    transition_T()
     : source(nullptr), 
       target(nullptr), 
       to_constln_cnt(nullptr), 
       to_constln_ref(nullptr) 
    {
    }
};

class constellation_T
{
  public:
    // size in number of states
    size_t size;
    // list of blocks
    sized_forward_list < block_T > blocks;
    
    // ADDITIONAL INFO to keep track of where the constellation is listed
    sized_forward_list<constellation_T>::iterator ptr_in_list;
    
    // constructor. This is intentionally the only constructor. No other
    // constructors, nor an assignment are supposed to exist.
    constellation_T()
     : size(0)
    {}

    constellation_marker type() const
    {
      return (blocks.size()==1?TRIVIAL:NONTRIVIAL);
    }
};

class to_constlns_element_T
{
  protected:

    constellation_T* m_C;
    to_constlns_element_T* m_new_element;

    // A deque containing all the counters.
    static std::deque<to_constlns_element_T>& element_pool()
    {
      static std::deque<to_constlns_element_T> m_element_pool;
      return m_element_pool;
    }

    // First free position is 0 if there is no
    // free position. Otherwise it points to the
    // first_free_position-1 in the counter_pool,
    // where an unused counter can be found.
    static to_constlns_element_T*& first_free_position()
    {
      static to_constlns_element_T* m_first_free_position=nullptr;
      return m_first_free_position;
    }

  public:
    // !!! Not present in pseudo-code: a pointer to the list of transitions from block containing this element (in to_constlns list) to constellation C,
    // EXCLUDING the inert transitions, i.e., the transitions must leave the source block;
    // This is needed to efficiently find these transitions when preparing for lockstep search detect1 when stabilising blocks
    sized_forward_list < transition_T > trans_list;
    
    // A list S_C for constellation C
    pooled_sized_forward_list < state_T >* SClist;

    // ADDITIONAL INFO to keep track of where the element is listed in a (block)
    // to_constlns list
    // typename
    sized_forward_list<to_constlns_element_T>::iterator ptr_in_list;

    to_constlns_element_T()
     : m_C(nullptr),
       m_new_element(nullptr),
       SClist(nullptr)
    {}
  
    to_constlns_element_T* new_element() const
    {
      return m_new_element;
    }

    void set_new_element(to_constlns_element_T* e) 
    {
      m_new_element=e;
    }

    constellation_T* C() const
    {
      return m_C;
    }

    void delete_to_constlns_element()
    {
      // garbage collect this counter.
      m_new_element=first_free_position();
      first_free_position()=this;
    }

    // A counter must be created using new_counter, and
    // it cannot be constructed using a constructor.
    static to_constlns_element_T* new_to_constlns_element(constellation_T* C)
    {
      to_constlns_element_T* result;
      if (first_free_position()!=nullptr)
      {
        result=first_free_position();
        // reset data of this element
        result->set_new_element(nullptr);
        result->SClist = nullptr;
        first_free_position()=first_free_position()->m_new_element;
      }
      else
      {
        element_pool().emplace_back();\
        result= &element_pool().back();
      }
      result->m_C=C;
      return result;
    }
};

class block_T
{
  protected:
    // type of block
    // block_marker type;
    // ID of block. This is size_t(-1) when the type of the block is an EXTRA_KRIPKE_BLOCK.
    // For a STD_BLOCK it is the sequence number of such a block.
    const size_t id;

  public:
    // constellation C containing B
    constellation_T *constellation;
    // list of bottom states
    sized_forward_list < state_T > btm_states;
    // list of non-bottom states
    sized_forward_list < state_T > non_btm_states;
    // list of constellations reachable from block
    sized_forward_list < to_constlns_element_T > to_constlns;
    // marked bottom states
    sized_forward_list < state_T > marked_btm_states;
    // marked non-bottom states
    sized_forward_list < state_T > marked_non_btm_states;
    // a pointer to a to_constlns object that contains a list of transitions from state in block
    // to constellation on which splitting is done
    to_constlns_element_T* constln_ref;
    // a pointer to a to_constlns object that contains a list of transitions from state in block
    // to (constellation \ block) on which splitting is done
    to_constlns_element_T* coconstln_ref;
    // this pointer is used to find in constant time the element in to_constlns that belongs to the constellation
    // of which the current block is a member. This is needed to allow inert transitions when becoming non-inert to be added to the associated
    // trans_list in constant time.
    to_constlns_element_T* inconstln_ref;
    // list of new bottom states
    sized_forward_list < state_T > new_btm_states;
    
    // ADDITIONAL INFO to keep track of where the block is listed in a constellation.blocks
    sized_forward_list<block_T>::iterator ptr_in_list;
    
    // constructor for block of given type. This is intentionally the only
    // constructor of a block_T. It is not allowed to copy or assign a block.
    block_T(block_marker t, size_t& index)
     : id(t==STD_BLOCK?index++:size_t(-1)),
       constellation(nullptr),
       constln_ref(nullptr), 
       coconstln_ref(nullptr), 
       inconstln_ref(nullptr) 
    {}

  public:
    block_marker type() const
    {
      if (id==size_t(-1))
      {
        return EXTRA_KRIPKE_BLOCK;
      }
      return STD_BLOCK;
    }
    
    size_t block_id() const
    {
      return id;
    }
};

// compare function to sort transitions based on target state
inline bool compare_targets_of_transitions (const transition_T& t1, const transition_T& t2) 
{ 
  return (t1.target < t2.target); 
}

// compare function to sort pointers to transitions based on source state
inline bool compare_sources_of_transition_pointers (transition_T* t1, transition_T* t2) 
{
  return (t1->source < t2->source); 
};

template < class LTS_TYPE>
class bisim_partitioner_gw
{
  protected:
    // Local class variables

    size_t max_block_index;

    size_t nr_of_splits;
     
    LTS_TYPE& aut;

    // trivial and non-trivial constellations
    sized_forward_list < constellation_T > non_trivial_constlns;
    sized_forward_list < constellation_T > trivial_constlns;
    // the number of states
    size_t nr_of_states;
    // the original number of states of input LTS
    size_t orig_nr_of_states;
    // the list of states
    std::vector < state_T > states;
    // the list of transitions (for backwards traversal)
    std::vector < transition_T > transitions;
    // the list of pointers to transitions (for forward traversal)
    std::vector < transition_T* > transitionpointers;
    // the list of blocks
    std::deque < block_T > blocks;
    // the list of constellations
    std::deque < constellation_T > constellations;
    // the pool of SClists
    pool < pooled_sized_forward_list < state_T > > SClists;
    // temporary map to keep track of states to be added when converting LTS to Kripke structure

    const label_type tau_label; // tau_label is size_t(-1) if it does not exist.

    // start structures and functions for lockstep search

    // A Comparator class to compare states in the priority queue
    class LessThanByInert
    {
      public:
        bool operator()(const state_T* lhs, const state_T* rhs) const 
        {
          return lhs->priority < rhs->priority;
        }
    };

    // detect1
    std::vector < state_T* > Q; // We use a vector for this stack datatype, as we can clear it.
    sized_forward_list < state_T > L;
    // to keep track of state of detect 1
    state_T *current_state_detect1;
    trans_type current_trans_detect1;
    // to keep track of adding states to detect1
    typename sized_forward_list<state_T>::iterator iter_state_added_detect1;
    typename sized_forward_list<transition_T>::iterator iter_trans_state_added_detect1;
    // detect2
    //std::priority_queue < state_T*, std::vector< state_T* >, LessThanByInert>* P;
    std::multiset< state_T*, LessThanByInert> P;
    sized_forward_list < state_T > Lp;
    // to keep track of state of detect 2
    state_T *current_state_detect2;
    trans_type current_trans_detect2;
    // to keep track of adding states to detect 2
    typename sized_forward_list<state_T>::iterator iter_state_added_detect2;
    typename sized_forward_list<state_T>::iterator iter_sclist_detect2;
    bool sclist_is_empty_detect2;
    // required for forward check in detect2 in nested split
    bool in_forward_check_detect2;
    // required in both procedures
    block_T* block_detect;
    to_constlns_element_T* e_detect;
    size_t maxsize_detect;
    // old constellation of the splitter
    constellation_T* constellation_splitter_detect;

    // key and hash function for (action, target state) pair. Required since unordered_map does not
    // directly allow to use pair keys
    class Key 
    {
      public:
        label_type first;
        state_type second;

        Key(const label_type& f, const state_type& s)
         : first(f),
           second(s)
        {} 

        bool operator==(const Key &other) const 
        {
          return (first == other.first && second == other.second);
        }
    };

    class KeyHasher 
    {
      public:
        std::size_t operator()(const Key& k) const 
        {
          using std::size_t;
          using std::hash;

          return (hash<label_type>()(k.first) ^ (hash<state_type>()(k.second) << 1));
        }
    };
    // Map used to convert LTS to Kripke structure
    // (also used when converting Kripke structure back to LTS)
    std::unordered_map < Key, state_type, KeyHasher > extra_kripke_states;
  
    // begin auxiliary functions

    size_t state_id(state_T* s)
    {
      return (s-&states[0]);
    }
    
    // move state to block
    void move_state_to_block(state_T* s, block_T* B)
    {
      switch (s->type) 
      {
           case BTM_STATE:
                s->block->btm_states.move_linked(s, B->btm_states);
                break;
           case NON_BTM_STATE:
                s->block->non_btm_states.move_linked(s, B->non_btm_states);
                break;
           case MARKED_BTM_STATE:
                s->block->marked_btm_states.move_linked(s, B->marked_btm_states);
                break;
           case MARKED_NON_BTM_STATE:
                s->block->marked_non_btm_states.move_linked(s, B->marked_non_btm_states);
           case DEFAULT_STATE:
                break;
      }
      s->block = B;
    }
    
    // function to check the size of a to_constlns_element_T object. This size is determined by the size of the
    // associated trans_list. The function can handle NULL pointers.
    size_t size(to_constlns_element_T* l)
    {
      if (l == nullptr) 
      {
        return 0;
      }
      else 
      {
        return l->trans_list.size();
      }
    }
    
    // iterator functions to iterate over all unmarked states of a given block
    bool iterating_non_bottom;
    bool iterating_non_bottom2;
    
    sized_forward_list < state_T>::iterator unmarked_states_begin(block_T* B)
    {
      if (B->btm_states.size() > 0) 
      {
        iterating_non_bottom2 = false;
        return B->btm_states.begin();
      }
      else 
      {
        iterating_non_bottom2 = true;
        return B->non_btm_states.begin();
      }
    }
    
    sized_forward_list < state_T>::iterator unmarked_states_end(block_T* B)
    {
      return B->non_btm_states.end();
    }
    
    sized_forward_list < state_T>::iterator unmarked_states_next(block_T* B, sized_forward_list < state_T >::iterator it)
    {
      auto tmpit = it;
      ++tmpit;
      if (!iterating_non_bottom2 && tmpit == B->btm_states.end()) 
      {
           iterating_non_bottom2 = true;
           tmpit = B->non_btm_states.begin();
      }
      return tmpit;
    }
    
    // iterator functions to iterate over all marked states of a given block
    sized_forward_list < state_T >::iterator marked_states_begin(block_T* B)
    {
      if (B->marked_btm_states.size() > 0) 
      {
        iterating_non_bottom2 = false;
        return B->marked_btm_states.begin();
      }
      else 
      {
        iterating_non_bottom2 = true;
        return B->marked_non_btm_states.begin();
      }
    }
    
    sized_forward_list < state_T >::iterator marked_states_end(block_T* B)
    {
      return B->marked_non_btm_states.end();
    }
    
    sized_forward_list < state_T >::iterator marked_states_next(block_T* B, sized_forward_list < state_T >::iterator it)
    {
      auto tmpit = it;
      ++tmpit;
      if (!iterating_non_bottom2 && tmpit == B->marked_btm_states.end()) {
           iterating_non_bottom2 = true;
           tmpit = B->marked_non_btm_states.begin();
      }
      return tmpit;
    }
    
    // unmark all marked states in the given block, reset s->constln_cnt and s->coconstln_cnt for all s in B, and reset
    // B->constln_ref and B->coconstln_ref if they point to a counter with value 0
    void clean_temp_refs_block(block_T* B)
    {
      if (B != nullptr) 
      {
         for (auto sit = B->marked_btm_states.begin(); sit != B->marked_btm_states.end(); ++sit) 
         {
            state_T* s = *sit;
            s->constln_cnt = nullptr;
            s->coconstln_cnt = nullptr;
            B->marked_btm_states.move_state_linked(s, B->btm_states, BTM_STATE, sit);
         }
         for (auto sit = B->marked_non_btm_states.begin(); sit != B->marked_non_btm_states.end(); ++sit) 
         {
            state_T* s = *sit;
            s->constln_cnt = nullptr;
            if (s->coconstln_cnt != nullptr) 
            {
              if (s->coconstln_cnt->counter_value() == 0) 
              {
                s->coconstln_cnt->delete_counter();
              }
            }
            s->coconstln_cnt = nullptr;
            B->marked_non_btm_states.move_state_linked(s, B->non_btm_states, NON_BTM_STATE, sit);
         }
         if (B->constln_ref != nullptr) 
         {
           if (size(B->constln_ref) == 0) 
           {
             // if the associated constellation is the one containing B, set inconstln_ref to NULL
             if (B->constln_ref->C() == B->constellation) 
             {
               B->inconstln_ref = nullptr;
             }
             B->to_constlns.remove_linked(B->constln_ref);
             B->constln_ref->delete_to_constlns_element();
             B->constln_ref = nullptr;
           }
           B->constln_ref = nullptr;
         }
         if (B->coconstln_ref != nullptr) 
         {
           if (size(B->coconstln_ref) == 0) 
           {
             if (B->coconstln_ref->C() == B->constellation) 
             {
                B->inconstln_ref = nullptr;
             }
             B->to_constlns.remove_linked(B->coconstln_ref);
             B->coconstln_ref->delete_to_constlns_element();
             B->coconstln_ref = nullptr;
           }
           B->coconstln_ref = nullptr;
         }
      }
    }
    
    // end auxiliary functions

#ifndef NDEBUG
    // print the Kripke structure
    void print_the_Kripke_structure()
    {
      for (const std::pair<Key, state_type>& p: extra_kripke_states) 
      {
        mCRL2log(log::verbose) << "Extra Kripke state " << p.second << " (" << p.first.first << "," << p.first.second << ")\n";
      }
      for (const state_T& s: states)
      {
        for (auto tit = s.Ttgt_begin; transitionpointers[tit]->source == &s; ++tit) 
        {
          transition_T* t = transitionpointers[tit];
          mCRL2log(log::verbose) << state_id(t->source) << " -> " << state_id(t->target) << "\n";
        }
        for (auto tit = s.Tsrc_begin; transitions[tit].source == &s; ++tit) 
        {
          transition_T* t = &transitions[tit];
          mCRL2log(log::verbose) << state_id(t->target) << " <- " << state_id(t->source) << "\n";
        }
      }
    }
#endif // NDEBUG

  public:
    /** \brief Creates a bisimulation partitioner for an LTS.
     *  \details This bisimulation partitioner applies the algorithm
     *  defined in J.F. Groote and A.J. Wijs.
     *
     *  If branching is true, then branching bisimulation is used, otherwise strong bisimulation is applied.
     *  If preserve_divergence is true, then branching must be true. In this case states with an internal tau loop
     *  are considered to be different from states without a tau loop. In this way the divergences are preserved.
     *
     *  The input transition system is not allowed to contain tau loops, except that if preserve_divergence is true
     *  tau loops on a single state are allowed as they indicate divergences. Using the scc partitioner the tau
     *  loops must first be removed before applying this algorithm.
     *  \warning Note that when compiled with optimisations, bisimulation partitioning
     *  is much faster than compiled without any optimisation. The difference can go up to a factor 10.
     *  \param[in] l Reference to the LTS. The LTS l is only changed if \ref replace_transitions is called. */
    bisim_partitioner_gw(LTS_TYPE& l,
                         const bool branching=false,
                         const bool preserve_divergence=false)
     : max_block_index(0),
       aut(l), 
       tau_label(determine_tau_label(l))
    {
      assert(branching || !preserve_divergence);
      // initialise variables
      nr_of_splits = 0;
      current_state_detect1 = nullptr;
      current_state_detect2 = nullptr;
      in_forward_check_detect2 = false;
          
      mCRL2log(log::verbose) << "O(m log n) " << (preserve_divergence?"Divergence preserving b":"B") <<
                  (branching?"ranching b":"") << "isimulation partitioner created for "
                  << l.num_states() << " states and " <<
                  l.num_transitions() << " transitions\n";
      create_initial_partition_gw(branching,preserve_divergence);
      refine_partition_until_it_becomes_stable_gw();
    }


    /** \brief Destroys this partitioner. */
    ~bisim_partitioner_gw()
    {}

    /** \brief Replaces the transition relation of the current lts by the transitions
     *         of the bisimulation reduced transition system.
     * \details Each transition (s,l,s') is replaced by a transition (t,l,t') where
     * t and t' are the equivalence classes to which classes of the LTS. If the label l is
     * internal, which is detected using the function is_tau, then it is only returned
     * if t!=t' or preserve_divergence=true. This effectively removes all inert transitions.
     * Duplicates are removed from the transitions in the new lts.
     * Note that the number of states nor the initial state are not adapted by this method.
     * These must be set separately.
     *
     * \pre The bisimulation equivalence classes have been computed.
     * \param[in] branching Causes non internal transitions to be removed. */
    void replace_transitions(const bool branching, const bool preserve_divergences)
    {
      std::unordered_map < state_type, Key > to_lts_map;
      // obtain a map from state to <action, state> pair from extra_kripke_states
      for (auto it = extra_kripke_states.begin(); it != extra_kripke_states.end(); ++it)
      {
        to_lts_map.insert(std::make_pair(it->second, it->first));
      }
      extra_kripke_states.clear();
      
      // Put all the non inert transitions in a set. A set is used to remove double occurrences of transitions.
      std::set < transition > resulting_transitions;

      // traverse the outgoing transitions of the original LTS states
      for (state_type snr = 0; snr < orig_nr_of_states; snr++)
      {
        state_T* s = &(states[snr]);
        size_t s_eq = get_eq_class(snr);
        for (trans_type tnr = s->Ttgt_begin; transitionpointers[tnr]->source == s; tnr++)
        {
          transition_T* t = transitionpointers[tnr];
          state_type tgt_id = t->target - &(states[0]);
          if (tgt_id < orig_nr_of_states)
          {
            // we have a tau transition
            assert(branching);
            if (branching && (s_eq != get_eq_class(tgt_id)||
                             (snr == tgt_id && preserve_divergences)))
            {
              assert(tau_label!=size_t(-1));
              resulting_transitions.insert(transition(s_eq, tau_label, get_eq_class(tgt_id)));
            }
          }
          else 
          {
            Key k = (to_lts_map.find(tgt_id))->second;
            resulting_transitions.insert(transition(s_eq, k.first, get_eq_class(k.second)));
          }
        }
      }

      // Copy the transitions from the set into the transition system.
      for (std::multiset < transition >::const_iterator i=resulting_transitions.begin();
           i!=resulting_transitions.end(); ++i)
      {
        aut.add_transition(*i);
      }
    }

    /** \brief Gives the number of bisimulation equivalence classes of the LTS.
     *  \return The number of bisimulation equivalence classes of the LTS.
     */
    size_t num_eq_classes() const
    {
      return max_block_index;
    }

    /** \brief Gives the bisimulation equivalence class number of a state.
     *  \param[in] s A state number.
     *  \return The number of the bisimulation equivalence class to which \e s belongs. */
    size_t get_eq_class(const size_t s) const
    {
      //assert(s<block_index_of_a_state.size());
      return states[s].block->block_id();
    }

    /** \brief Returns whether two states are in the same bisimulation equivalence class.
     *  \param[in] s A state number.
     *  \param[in] t A state number.
     *  \retval true if \e s and \e t are in the same bisimulation equivalence class;
     *  \retval false otherwise. */
    bool in_same_class(const size_t s, const size_t t) const
    {
      return get_eq_class(s)==get_eq_class(t);
    }

  private:
    std::string keypair_to_string(const std::string& action, state_type to)
    {
      return action + "_" + std::to_string((long long int) to);
    }

    // next_state functions to add states to detect1 and detect2 in the various phases of the algorithm
    // (1: splitting Bp, 2: splitting split(Bp, B), 3: stabilising blocks)
    // precondition: iter_state_added_detectx have been set to before the beginning of the relevant state lists
    inline bool has_next_state_detect1_1() 
    {
      auto next_it = iter_state_added_detect1;
      next_it++;
      if (next_it == block_detect->marked_btm_states.end() && !iterating_non_bottom) 
      {
        next_it = block_detect->marked_non_btm_states.begin();
      }
      return next_it != block_detect->marked_non_btm_states.end();
    }

    inline state_T* next_state_detect1_1() 
    {
      iter_state_added_detect1++;
      if (iter_state_added_detect1 == block_detect->marked_btm_states.end() && !iterating_non_bottom) 
      {
        iterating_non_bottom = true;
        iter_state_added_detect1 = block_detect->marked_non_btm_states.begin();
      }
      // is the state suitable?
      state_T* s = *iter_state_added_detect1;
      if (!s->is_in_L_detect1) 
      {
        return s;
      }
      else 
      {
        return nullptr;
      }
    }

    inline bool has_next_state_detect2_1() 
    {
      auto next_it = iter_state_added_detect2;
      next_it++;
      return next_it != block_detect->btm_states.end();
    }

    inline state_T* next_state_detect2_1() 
    {
      iter_state_added_detect2++;
      // Note: no need to check for membership in P. Current state is a bottom state, so cannot have been reached
      // via an incoming transition
      return *iter_state_added_detect2;
    }
    
    inline bool has_next_state_detect1_2() 
    {
      auto next_it = iter_trans_state_added_detect1;
      next_it++;
      return next_it != block_detect->coconstln_ref->trans_list.end();
    }

    inline state_T* next_state_detect1_2() 
    {
      iter_trans_state_added_detect1++;
      transition_T* t = *iter_trans_state_added_detect1;
      state_T* s = t->source;
      if (!s->is_in_L_detect1) 
      {
        return s;
      }
      else 
      {
        return nullptr;
      }
    }

    inline bool has_next_state_detect2_2() 
    {
      auto next_it = iter_state_added_detect2;
      next_it++;
      return next_it != block_detect->marked_btm_states.end();
    }

    inline state_T* next_state_detect2_2() 
    {
      iter_state_added_detect2++;
      state_T* s = *iter_state_added_detect2;
      if (s->coconstln_cnt == nullptr) 
      {
        return s;
      }
      else if (s->coconstln_cnt->counter_value() == 0) 
      {
        return s;
      }
      else 
      {
        return nullptr;
      }
    }

    inline bool has_next_state_detect1_3() 
    {
      auto next_it = iter_trans_state_added_detect1;
      next_it++;
      return next_it != e_detect->trans_list.end();
    }
    
    inline state_T* next_state_detect1_3() 
    {
      iter_trans_state_added_detect1++;
      transition_T* t = *iter_trans_state_added_detect1;
      state_T* s = t->source;
      if (!s->is_in_L_detect1) 
      {
        return s;
      }
      else 
      {
        return nullptr;
      }
    }

    inline bool has_next_state_detect2_3() {
         auto next_it = iter_state_added_detect2;
         next_it++;
         return next_it != block_detect->new_btm_states.end();
    }
    
    // compare SClist and new_btm_states. This depends on new bottom states having been added to
    // back of SClist in order of appearance in new_btm_states
    inline state_T* next_state_detect2_3() 
    {
         iter_state_added_detect2++;
         state_T* s = *iter_state_added_detect2;
         if (sclist_is_empty_detect2) {
              return s;
         }
         else if (iter_sclist_detect2 == e_detect->SClist->list.end()) {
              return s;
         }
         else if (s != *iter_sclist_detect2) {
              return s;
         }
         else {
              iter_sclist_detect2++;
              return nullptr;
         }
    }

    // step in detect1
    // returns false if not finished, true otherwise
    // function hasnext indicates whether there is at least one more state to be processed
    // function next provides new states to be considered in the search
    inline lockstep_search_mode detect1_step(bool (bisim_partitioner_gw<LTS_TYPE>::*hasnext)(), state_T* (bisim_partitioner_gw<LTS_TYPE>::*next)())
    {
      // stop if upperbound to size has been reached
      if (L.size() > maxsize_detect) 
      {
        //mCRL2log(log::verbose) << "detect1: STOPPED (" << L.size() << "," << maxsize_detect << ")\n";
        return STOPPED;
      }
      if (Q.size() > 0 || (this->*hasnext)() || current_state_detect1 != nullptr) 
      {
        if (current_state_detect1 == nullptr) 
        {
          if (Q.size() > 0) 
          {
            current_state_detect1 = Q.back();
            Q.pop_back();
          }
          else 
          {
            // there must still be a state obtainable through next
            current_state_detect1 = (this->*next)();
            if (current_state_detect1 == nullptr) {
              //mCRL2log(log::verbose) << "detect1: end\n";
              // skip this state in the current step
              return CONTINUE;
            }
            else 
            {
              L.insert(current_state_detect1);
              current_state_detect1->is_in_L_detect1 = true;
              //mCRL2log(log::verbose) << "detect1 base: adding state " << current_state_detect1->id << "\n";
            }
          }
          //mCRL2log(log::verbose) << "detect1: processing state " << current_state_detect1->id << "\n";
          current_trans_detect1 = current_state_detect1->Tsrc_begin;
          // no incoming transitions present
          if (transitions[current_trans_detect1].target != current_state_detect1)
          {
            current_state_detect1 = nullptr;
            //mCRL2log(log::verbose) << "detect1: end2\n";
            return CONTINUE;
          }
        }
        const transition_T* t = &(transitions[current_trans_detect1]);
        state_T* sp = t->source;
          
        if (sp->block == block_detect && !sp->is_in_L_detect1) 
        {
          L.insert(sp);
          sp->is_in_L_detect1 = true;
          //mCRL2log(log::verbose) << "detect1: adding state " << sp->id << "\n";
          Q.push_back(sp);
        }
        // increment transition counter
        current_trans_detect1++;
        if (transitions[current_trans_detect1].target != current_state_detect1)
        {
          current_state_detect1 = nullptr;
          //mCRL2log(log::verbose) << "detect1: end3\n";
        }
        //mCRL2log(log::verbose) << "detect1: continue\n";
        return CONTINUE;
      }
      else 
      {
        //mCRL2log(log::verbose) << "detect1: finished\n";
        return TERMINATED;
      }
    }

    // step in detect2
    // returns false if not finished, true otherwise
    // isnestedsplit indicates whether a block B' is being split, or a block split(B',B).
    inline lockstep_search_mode detect2_step(bool (bisim_partitioner_gw<LTS_TYPE>::*hasnext)(), state_T* (bisim_partitioner_gw<LTS_TYPE>::*next)(), bool isnestedsplit, bool forwardcheckrequired)
    {
      bool sp_newly_inserted;
      // this part of the procedure is performed when it needs to be determined whether a state has
      // a direct outgoing transition to Bp \ B.
      if (in_forward_check_detect2) 
      {
        if (transitionpointers[current_trans_detect2]->source != current_state_detect2)
        {
          // no direct transition to Bp \ B found. State is suitable for detect2
          in_forward_check_detect2 = false;
          Lp.insert(current_state_detect2);
          current_state_detect2->is_in_Lp_detect2 = true;
          //mCRL2log(log::verbose) << "detect2: adding state " << current_state_detect2->id << "\n";
          current_trans_detect2 = current_state_detect2->Tsrc_begin;
          // no incoming transitions present
          if (transitions[current_trans_detect2].target != current_state_detect2)
          {
            current_state_detect2 = nullptr;
          }
        }
        else 
        {
          transition_T* t = transitionpointers[current_trans_detect2];
          //mCRL2log(log::verbose) << "compare " << t->target->block->constellation->id << " " << constellation_splitter_detect->id << "\n";
          if (t->target->block->constellation == constellation_splitter_detect && t->source->block != t->target->block) 
          {
            // transition found. state is not suitable for detect2
            in_forward_check_detect2 = false;
            current_state_detect2 = nullptr;
          }
          else 
          {
            // go to next transition
            current_trans_detect2++;
          }
        }
        return CONTINUE;
      }
      // stop if upperbound to size has been reached
      if (Lp.size() > maxsize_detect) 
      {
        //mCRL2log(log::verbose) << "detect2: STOPPED (" << Lp.size() << "," << maxsize_detect << ")\n";
        return STOPPED;
      }
      if (P.size() > 0 || (this->*hasnext)() || current_state_detect2 != nullptr) 
      {
        if (current_state_detect2 == nullptr) 
        {
          if ((this->*hasnext)()) 
          {
            current_state_detect2 = (this->*next)();
            // check if state is suitable
            if (current_state_detect2 == nullptr) {
              return CONTINUE;
            }
            //mCRL2log(log::verbose) << "detect2: processing state " << current_state_detect2->id << "\n";
          }
          else 
          {
            auto Pit = P.begin();
            current_state_detect2 = *Pit;
            P.erase(Pit);
            current_state_detect2->is_in_P_detect2 = false;
            //mCRL2log(log::verbose) << "detect2: from queue: processing state " << current_state_detect2->id << " with priority " << current_state_detect2->priority << "\n";
            // check if this element still has priority 0
            if (current_state_detect2->priority != 0) 
            {
              //mCRL2log(log::verbose) << "detect2: finished\n";
              return TERMINATED;
            }
            else if (forwardcheckrequired) 
            {
              // skip if state has been reached by detect1
              if (current_state_detect2->is_in_L_detect1) 
              {
                current_state_detect2 = nullptr;
              }
              // else check if state can reach Bp \ B directly
              else 
              {
                in_forward_check_detect2 = true;
                current_trans_detect2 = current_state_detect2->Ttgt_begin;
              }
              return CONTINUE;
            }
          }
          current_trans_detect2 = current_state_detect2->Tsrc_begin;
          Lp.insert(current_state_detect2);
          current_state_detect2->is_in_Lp_detect2 = true;
          //mCRL2log(log::verbose) << "detect2 base: adding state " << current_state_detect2->id << "\n";
          // no incoming transitions present
          if (transitions[current_trans_detect2].target != current_state_detect2)
          {
            current_state_detect2 = nullptr;
            return CONTINUE;
          }
        }
        const transition_T* t = &(transitions[current_trans_detect2]);
        state_T* sp = t->source;
        sp_newly_inserted = false;
        //mCRL2log(log::verbose) << "detect2: walking back to state " << sp->id << "\n";
        //mCRL2log(log::verbose) << (sp->block == block_detect) << " " << sp->is_in_P_detect2 << " " << sp->is_in_Lp_detect2 << "\n";
        if (sp->block == block_detect && !sp->is_in_P_detect2 && !sp->is_in_Lp_detect2) 
        {
          // depending on isnestedsplit, check condition
          // For the nested split, we check whether the state is a member of L.
          // This seems dangerous, since L is maintained by detect1, but we are interested in the states that
          // reach Bp \ B, and it is problematic to determine which states can do so directly. Some states
          // in L are likely to reach Bp \ B directly (at least indirectly), so a state in L should be
          // ignored. Another condition involves the case that a state is marked: if it is, then
          // coconstln_cnt has been initialised. If it is higher than 0, the state can reach Bp \ B
          // directly.
          bool skip = false;
          //mCRL2log(log::verbose) << "checking\n";
          if (isnestedsplit && sp->type == MARKED_NON_BTM_STATE) 
          {
            //mCRL2log(log::verbose) << "here " << "\n";
            //if (sp->coconstln_cnt != NULL) {
            //  mCRL2log(log::verbose) << "count: " << sp->coconstln_cnt->cnt << "\n";
            //}
            //if (sp->coconstln_cnt == NULL) {
            //  skip = true;
            //}
            // Take into account that inert transitions are counted as well (they should be ignored here)
            // Note that the case that sp is in the splitter is handled correctly, since the splitter has already
            // been moved to a new constellation, which therefore cannot be equal to its old one.
            // (inert transitions are counted by constln_cnt, hence should not be taken into account here).
            if (block_detect->constellation == constellation_splitter_detect) 
            {
              if (sp->coconstln_cnt->counter_value() - sp->inert_cnt > 0) 
              {
                skip = true;
              }
            }
            else if (sp->coconstln_cnt->counter_value() > 0) 
            {
              skip = true;
            }
          }
          if (!skip) 
          {
            if (forwardcheckrequired ? (!sp->is_in_L_detect1) : (sp->type != MARKED_NON_BTM_STATE)) 
            {
              sp->priority = sp->inert_cnt;
              if (sp->priority > 0) 
              {
                sp->priority--;
              }
              //mCRL2log(log::verbose) << "detect2: pushing " << sp->id << " on P with prio " << sp->priority << "\n";
              sp->pos_in_P_detect2 = P.insert(sp);
              //mCRL2log(log::verbose) << "begin: " << (*(P.begin()))->id << "\n";
              sp->is_in_P_detect2 = true;
              sp_newly_inserted = true;
            }
          }
        }
        // priority of sp is only decremented if sp is in P.
        if (sp->is_in_P_detect2 && !sp_newly_inserted) 
        {
          if (sp->priority > 0) 
          {
            sp->priority--;
            // replace element in P
            //mCRL2log(log::verbose) << "updating state " << sp->id << "\n";
            P.erase(sp->pos_in_P_detect2);
            sp->pos_in_P_detect2 = P.insert(sp);
            //mCRL2log(log::verbose) << "detect2: new prio of " << sp->id << " is " << sp->priority << "\n";
          }
        }
        // increment transition counter
        current_trans_detect2++;
        if (transitions[current_trans_detect2].target != current_state_detect2)
        {
          current_state_detect2 = nullptr;
        }
        return CONTINUE;
      }
      else 
      {
        //mCRL2log(log::verbose) << "detect2: finished\n";
        return TERMINATED;
      }
    }

    // end structures and functions for lockstep search

    // method to print current partition
#ifndef NDEBUG
    void print_partition ()
    {
      mCRL2log(log::verbose) << "PARTITION ";
      for (auto cit = non_trivial_constlns.begin(); cit != non_trivial_constlns.end(); ++cit) 
      {
        constellation_T* C = *cit;
        for (auto bit = C->blocks.begin(); bit != C->blocks.end(); ++bit) 
        {
          block_T* B = *bit;
          mCRL2log(log::verbose) << "[";
          for (auto sit = unmarked_states_begin(B); sit != unmarked_states_end(B); sit = unmarked_states_next(B, sit)) 
          {
            state_T* s = *sit;
            mCRL2log(log::verbose) << state_id(s) << ",";
          }
          for (auto sit = marked_states_begin(B); sit != marked_states_end(B); sit = marked_states_next(B, sit)) 
          {
            state_T* s = *sit;
            mCRL2log(log::verbose) << "*" << state_id(s) << "*,";
          }
          mCRL2log(log::verbose) << "], ";
        }
      }
      mCRL2log(log::verbose) << "| ";
      for (auto cit = trivial_constlns.begin(); cit != trivial_constlns.end(); ++cit) 
      {
        constellation_T* C = *cit;
        for (auto bit = C->blocks.begin(); bit != C->blocks.end(); ++bit) 
        {
          block_T* B = *bit;
          mCRL2log(log::verbose) << "[";
          for (auto sit = unmarked_states_begin(B); sit != unmarked_states_end(B); sit = unmarked_states_next(B, sit)) 
          {
            state_T* s = *sit;
            mCRL2log(log::verbose) << state_id(s) << ",";
          }
          for (auto sit = marked_states_begin(B); sit != marked_states_end(B); sit = marked_states_next(B, sit)) 
          {
            state_T* s = *sit;
            mCRL2log(log::verbose) << "*" << state_id(s) << "*,";
          }
          mCRL2log(log::verbose) << "], ";
        }
      }
      mCRL2log(log::verbose) << "\n";
    }
#endif // NDEBUG

    void create_initial_partition_gw(const bool branching,
                                     const bool preserve_divergence)
    {
      using namespace std;
      // number of blocks
      size_t nr_of_blocks;
      // the number of states
      nr_of_states = aut.num_states();
      // original size of input
      orig_nr_of_states = nr_of_states;
      // temporary map to keep track of blocks (maps transition labels (unequal to tau) to blocks
      unordered_map < label_type, size_t > action_block_map;

      // create single initial non-trivial constellation
      constellations.emplace_back();
      constellation_T* C = &(constellations.back());
      
      // create first block in C
      // blocks.emplace_back(block_T(STD_BLOCK, max_block_index));
      blocks.emplace_back(STD_BLOCK, max_block_index);
      block_T* B1 = &(blocks.back());
      // create associated list of transitions from block to constellation C
      to_constlns_element_T* e = to_constlns_element_T::new_to_constlns_element(C);
      blocks.back().to_constlns.insert_linked(e);
      blocks.back().inconstln_ref = e;
      blocks.back().constellation = C;
      C->blocks.insert_linked(&(blocks.back()));
      nr_of_blocks = 1;

      // Iterate over the transitions and collect new states
      // and sort by target state.
      for (const transition& t: aut.get_transitions())
      {
        if (!aut.is_tau(t.label(aut.hidden_label_map())) || 
            !branching ||
            (preserve_divergence && t.from() == t.to())
           )
        {
          // create new state
          Key k(t.label(aut.hidden_label_map()),t.to());
          if ((extra_kripke_states.insert(make_pair(k, nr_of_states))).second) 
          {
            nr_of_states++;
          }
          // (possibly) create new block
          if (action_block_map.insert(make_pair(t.label(aut.hidden_label_map()), nr_of_blocks)).second) 
          {
            nr_of_blocks++;
          }
        }
      }
      mCRL2log(log::verbose) << "Number of extra states: " << extra_kripke_states.size() << "\n";
      // knowing the number of blocks,
      // create blocks, and add one entry in their to_constlns list for the single constellation C
      // each of these new blocks will contain extra Kripke states, therefore we increment nr_of_extra_kripke_blocks
      // each time we create a block
      
      for (size_t i = 0; i < nr_of_blocks-1; i++) 
      {
        blocks.emplace_back(EXTRA_KRIPKE_BLOCK, max_block_index);
        to_constlns_element_T* e = to_constlns_element_T::new_to_constlns_element(C);
        blocks.back().to_constlns.insert_linked(e);
        blocks.back().inconstln_ref = e;
        // add block to constellation
        blocks.back().constellation = C;
        C->blocks.insert_linked(&(blocks.back()));
      }

      // create state entries in states list
      states.reserve(nr_of_states);
      for (size_t i = 0; i < nr_of_states; i++)
      {
        states.emplace_back(state_T());
    
        // add state to first block, if applicable
        if (i < orig_nr_of_states) 
        {
          states.back().block = B1;
        }
      }
      // add the new states to their respective blocks
      for (auto it=extra_kripke_states.begin(); it != extra_kripke_states.end(); ++it) 
      {
        (states[it->second]).block = &(blocks[(action_block_map.find((it->first).first))->second]);
      }
      // create transitions (we need the original number in input + one for each extra kripke state,
      // and one transition at the end indicating the end of the list)
      transitions.reserve(aut.get_transitions().size()+extra_kripke_states.size()+1);
      // first add the original LTS transitions
      for (const transition& t: aut.get_transitions())
      {
        // create transition entry
        transitions.emplace_back();
        transition_T& t_entry = transitions.back();
        // fill in info
        t_entry.source = &states[t.from()];
        // target depends on transition label
        if (aut.is_tau(t.label(aut.hidden_label_map())) && branching && (!preserve_divergence || t.from()!=t.to())) 
        {
          t_entry.target = &states[t.to()];
          // initially, all tau-transitions are inert
          // number of inert transitions of source needs to be incremented
          states[t.from()].inert_cnt++;
        }
        else 
        {
          Key k(t.label(aut.hidden_label_map()),t.to());
          t_entry.target = &states[(extra_kripke_states.find(k))->second];
        }
      }
      // now add transitions <a,t> -> t
      for (auto sit = extra_kripke_states.begin(); sit != extra_kripke_states.end(); ++sit)
      {
        transitions.emplace_back();
        std::pair<Key, state_type> e = *sit;
        state_type sid = e.second;
        state_type tid = e.first.second;

        transition_T& t_entry = transitions.back();
        // fill in info
        t_entry.source = &states[sid];
        t_entry.target = &states[tid];
      }
      // sort the transitions by target
      std::sort(transitions.begin(), transitions.end(), compare_targets_of_transitions);
      // we no longer need original transitions
      aut.clear_transitions();
      // in one loop, create a vector of pointers for forward traversal, and set the Tsrc_begin pointers
      state_T* current_state = nullptr;
      // create vector of transition pointers for forward traversals
      // again, add a dummy transition at the end
      transitionpointers.reserve(aut.get_transitions().size()+extra_kripke_states.size()+1);
      for (trans_type tit = 0; tit < transitions.size(); tit++)
      {
        // add pointer to transitionpointers
        transition_T* t = &(transitions[tit]);
        transitionpointers.emplace_back(t);

        // if we see a new target state, set begin of Tsrc to this transition
        if (t->target != current_state)
        {
          current_state = t->target;
          current_state->Tsrc_begin = tit;
        }
        
        // do the following only if transition is non-inert
        if ((t->source->block != t->target->block) || !branching)
        {
          // set pointer to C entry in to_constlns list of B.
          t->to_constln_ref = t->source->block->to_constlns.front();
          // add transition to transition list of source block
          t->to_constln_ref->trans_list.insert_linked(t);
        }
      }
      // sort transition pointers by source state
      std::sort(transitionpointers.begin(), transitionpointers.end(), compare_sources_of_transition_pointers);
      // walk over the transition pointers, add counters and set Ttgt_begin entries
      current_state = nullptr;
      counter_T* counter_it;
      for (trans_type tit = 0; tit < transitionpointers.size(); tit++)
      {
        transition_T* t = transitionpointers[tit];
        
        if (t->source != current_state)
        {
          current_state = t->source;
          current_state->Ttgt_begin = tit;
          // create a new counter
          counter_it=counter_T::new_counter();
        }
        // add pointer to counter
        t->to_constln_cnt = counter_it;
        // increment the counter
        t->to_constln_cnt->increment();
      }
      // add the dummy 'end of the list' transition
      transitions.emplace_back();
      // add dummy 'end of the list' transition pointer
      transitionpointers.emplace_back(&(transitions.back()));

      // print_the_Kripke_structure();
      
      // Add all states to their appropriate list in the block they reside in
      for (state_T& s: states)
      {
        if (s.inert_cnt == 0) 
        {
          // state is bottom
          s.block->btm_states.insert_state_linked(&s, BTM_STATE);
        }
        else 
        {
          // state is not bottom
          s.block->non_btm_states.insert_state_linked(&s, NON_BTM_STATE);
        }
      }
      
      // set size of constellation C
      C->size = nr_of_states;
      // add C to appropriate list
      if (C->blocks.size() > 1) 
      {
        assert(C->type() == NONTRIVIAL);
        non_trivial_constlns.insert_linked(C);
      }
      else 
      {
        trivial_constlns.insert_linked(C);
      }
      mCRL2log(log::verbose) << "Size of the resulting Kripke structure: " << states.size() << " states and " << transitions.size() << " transitions.\n";

    }; // end create_initial_partition

// Refine the partition until the partition has become stable
    void refine_partition_until_it_becomes_stable_gw()
    {
      while (non_trivial_constlns.size() > 0) 
      {
        // list of splittable blocks
        sized_forward_list < block_T > splittable_blocks;
#ifndef NDEBUG
        // print_partition();
        //check_internal_consistency_of_the_partitioning_data_structure_gw(branching);
        check_consistency_blocks();
        check_consistency_transitions();
        check_non_empty_blocks();
#endif
        // walk over the constellations
        bool const_block_found = false;
        constellation_T* setB;
        block_T* B;
        for (auto const_it = non_trivial_constlns.begin(); const_it != non_trivial_constlns.end(); ++const_it) 
        {
          setB = *const_it;
          for (auto bit = setB->blocks.begin(); bit != setB->blocks.end(); ++bit) 
          {
            B = *bit;
            if (B->btm_states.size()+B->non_btm_states.size() <= (setB->size)/2) 
            {
              // 5.2.1
              // 5.2.1.a
              constellations.emplace_back();
              constellation_T* setC = &(constellations.back());
              setB->blocks.move_linked(B, setC->blocks, bit);
              size_t Bsize = B->btm_states.size() + B->non_btm_states.size() + B->marked_btm_states.size() + B->marked_non_btm_states.size();
              setB->size -= Bsize;
              setC->size += Bsize;
              B->constellation = setC;
              B->inconstln_ref = nullptr;
              // 5.2.1.b
              trivial_constlns.insert_linked(setC);
              if (setB->blocks.size() == 1) 
              {
                assert(setB->type() == TRIVIAL);
                non_trivial_constlns.move_linked(setB, trivial_constlns, const_it);
              }
              // 5.2.2 walk through B.btm_states and B.non_btm_states, and check incoming transitions
              //check_consistency_trans_lists_2(setB);
              for (auto sit = unmarked_states_begin(B); sit != unmarked_states_end(B); sit = unmarked_states_next(B, sit)) 
              {
                state_T* s = *sit;
                
                for (auto tit = s->Tsrc_begin; transitions[tit].target == s; ++tit)
                {
                  transition_T* t = &(transitions[tit]);
                  state_T* sp = t->source;
                  block_T* Bp = sp->block;
                  // Ignore if Bp=B. We will revisit this transition when considering
                  // the outgoing transitions of each s in B
                  if (Bp != B) 
                  {
                    // 5.2.2.a
                    if (Bp->marked_btm_states.size() == 0 && Bp->marked_non_btm_states.size() == 0) 
                    {
                      // 5.2.2.a.i
                      splittable_blocks.insert(Bp);
                      // 5.2.2.a.ii
                      Bp->coconstln_ref = t->to_constln_ref;
                      Bp->constln_ref = to_constlns_element_T::new_to_constlns_element(setC);
                      Bp->to_constlns.insert_linked(Bp->constln_ref);
                    }
                    // 5.2.2.b
                    // we split the counter into two counters, respecting
                    // the pointers to counters of the individual transitions
                    if (sp->coconstln_cnt == nullptr) 
                    {
                      sp->constln_cnt = counter_T::new_counter();
                      sp->coconstln_cnt = t->to_constln_cnt;
                    }
                    // 5.2.2.c
                    if (sp->type != MARKED_BTM_STATE && sp->type != MARKED_NON_BTM_STATE) 
                    {
                      // 5.2.2.c.i
                      if (sp->type == BTM_STATE) 
                      {
                        Bp->btm_states.move_state_linked(sp, Bp->marked_btm_states, MARKED_BTM_STATE);
                      }
                      // 5.2.2.c.ii
                      else 
                      {
                        Bp->non_btm_states.move_state_linked(sp, Bp->marked_non_btm_states, MARKED_NON_BTM_STATE);
                      }
                    }
                    // 5.2.2.d
                    sp->constln_cnt->increment();
                    t->to_constln_cnt = sp->constln_cnt;
                    sp->coconstln_cnt->decrement();
                    // move t
                    Bp->coconstln_ref->trans_list.move_linked(t, Bp->constln_ref->trans_list);
                    //assert(t->target->block->constellation == s->block->constellation);
                    t->to_constln_ref = Bp->constln_ref;
                  }
                }
              }
              //check_consistency_trans_lists(B, setB);
              // Consider the case that B itself can be split.
              // ALL states should be marked first
              // 5.2.3
              B->btm_states.swap(B->marked_btm_states);
              B->non_btm_states.swap(B->marked_non_btm_states);
              splittable_blocks.insert(B);
              B->constln_ref = nullptr;
              B->coconstln_ref = nullptr;
              for (auto sit = marked_states_begin(B); sit != marked_states_end(B); sit = marked_states_next(B, sit)) 
              {
                state_T* s = *sit;
                // actually mark the state
                if (s->type == BTM_STATE) 
                {
                  s->type = MARKED_BTM_STATE;
                }
                else 
                {
                  s->type = MARKED_NON_BTM_STATE;
                }
                // consider the outgoing transitions
                for (trans_type tit = s->Ttgt_begin; transitionpointers[tit]->source == s; ++tit)
                {
                  transition_T* t = transitionpointers[tit];
                  block_T* Bp = t->target->block;
                  constellation_T* setBp = Bp->constellation;
                  
                  if (setBp == setB || setBp == setC) 
                  {
                    // 5.2.3.a
                    if (setBp == setB && B->constln_ref == nullptr) 
                    {
                      B->coconstln_ref = t->to_constln_ref;
                      B->constln_ref = to_constlns_element_T::new_to_constlns_element(setC);
                      B->to_constlns.insert_linked(B->constln_ref);
                      B->inconstln_ref = B->constln_ref;
                    }
                    // 5.2.3.b
                    if (s->coconstln_cnt == nullptr) 
                    {
                      s->constln_cnt = counter_T::new_counter();
                      s->coconstln_cnt = t->to_constln_cnt;
                    }
                    // 5.2.3.c
                    if (Bp == B) 
                    {
                      s->constln_cnt->increment();
                      t->to_constln_cnt = s->constln_cnt;
                      s->coconstln_cnt->decrement();
                    }
                  }
                }
              }
              // 5.2.4
              //check_consistency_trans_lists(B, setB);
              auto prev_sbit = splittable_blocks.before_begin();
              for (auto sbit = splittable_blocks.begin(); sbit != splittable_blocks.end(); ++sbit) 
              {
                block_T* Bp = *sbit;
                
                // 5.2.4.a/b
                bool split = true;
                if (Bp->btm_states.size() == 0) 
                {
                  split = false;
                  if (size(Bp->coconstln_ref) > 0) 
                  {
                    // find a state in marked_btm_states with s->coconstln_cnt->cnt == 0
                    for (auto sit = Bp->marked_btm_states.begin(); sit != Bp->marked_btm_states.end(); ++sit) 
                    {
                      state_T* s = *sit;
                      
                      if (s->coconstln_cnt == nullptr) 
                      {
                        split = true;
                        break;
                      }
                      if ((s->coconstln_cnt)->counter_value() == 0) 
                      {
                        split = true;
                        break;
                      }
                    }
                  }
                }
                // 5.2.4.c
                if (!split) 
                {
                  splittable_blocks.remove_after(prev_sbit, sbit);
                  // move states back (unmark)
                  clean_temp_refs_block(Bp);
                }
                prev_sbit = sbit;
              }
              // 5.2.5
              if (splittable_blocks.size() > 0) 
              {
                const_block_found = true;
              }
              // end of considering block B. Go on to next constellation
              break;
            } // end if block suitable
          } // end loop over blocks in constellation
          if (const_block_found) 
          {
            break;
          }
        } // end loop over constellations
        if (const_block_found) 
        {
          // constellation / block for splitting found. Now split
          //check_consistency_trans_lists(B, setB);
          for (auto bit = splittable_blocks.begin(); bit != splittable_blocks.end(); ++bit) 
          {
            block_T* Bp = *bit;
            const constellation_marker old_constellation_type_of_Bp =Bp->constellation->type();

            // pointer to Bp to be used when doing the nested split
            block_T* splitBpB = Bp;
            block_T* cosplitBpB = nullptr;
            block_T* splitsplitBpB = nullptr;
            size_t detect1_finished = CONTINUE;
            size_t detect2_finished = CONTINUE;
            block_T* Bpp = nullptr;
            block_T* Bp3 = nullptr;
            // lists for new btm states
            sized_forward_list<state_T> XBp;
            sized_forward_list<state_T> XBpp;
            sized_forward_list<state_T> XBp3;
            sized_forward_list<state_T> XBp4;
            // check if we can jump to step 5.3.3
            //check_consistency_trans_lists(B, setB);
            //check_consistency_blocks();
            if (Bp->btm_states.size() > 0) 
            {
              // 5.3.1. Perform detect1 and detect2 in lockstep
              Q.clear(); 
              L.clear();
              //P = new std::priority_queue < state_T*, std::vector< state_T* >, LessThanByInert>;
              assert(P.empty());
              Lp.clear();
              maxsize_detect = (Bp->btm_states.size() + Bp->non_btm_states.size() + Bp->marked_btm_states.size() + Bp->marked_non_btm_states.size()) / 2;
              block_detect = Bp;
              current_state_detect1 = nullptr;
              current_state_detect2 = nullptr;
              in_forward_check_detect2 = false;
              if (Bp->marked_btm_states.size() > 0) 
              {
                iter_state_added_detect1 = Bp->marked_btm_states.before_begin();
                iterating_non_bottom = false;
              }
              else 
              {
                iter_state_added_detect1 = Bp->marked_non_btm_states.before_begin();
                iterating_non_bottom = true;
              }
              iter_state_added_detect2 = Bp->btm_states.before_begin();
              
              while (detect1_finished != TERMINATED && detect2_finished != TERMINATED) 
              {
                if (detect1_finished != STOPPED) 
                {
                  detect1_finished = detect1_step(&bisim_partitioner_gw<LTS_TYPE>::has_next_state_detect1_1, &bisim_partitioner_gw<LTS_TYPE>::next_state_detect1_1);
                }
                if (detect1_finished != TERMINATED && detect2_finished != STOPPED) 
                {
                  detect2_finished = detect2_step(&bisim_partitioner_gw<LTS_TYPE>::has_next_state_detect2_1, &bisim_partitioner_gw<LTS_TYPE>::next_state_detect2_1, false, false);
                }
              }
              // 5.3.2
              blocks.emplace_back(Bp->type(), max_block_index);
              Bpp = &(blocks.back());
              Bpp->constellation = Bp->constellation;
              Bpp->constellation->blocks.insert_linked(Bpp);
              // Let N point to correct list
              sized_forward_list <state_T>* N;
              if (detect1_finished == TERMINATED) 
              {
                N = &L;
              }
              else 
              {
                N = &Lp;
              }
#ifndef NDEBUG
              // mCRL2log(log::verbose) << "splitting off: [";
              // for (auto sit = N->begin(); sit != N->end(); ++sit) 
              // {
              //   state_T* s = *sit;
                // mCRL2log(log::verbose) << " " << state_id(s);
              // }
              // mCRL2log(log::verbose) << "]\n";
              check_consistency_blocks();
              check_consistency_transitions();
              // mCRL2log(log::verbose) << "to_constlns list: \n";
              // for (auto it = Bp->to_constlns.begin(); it != Bp->to_constlns.end(); ++it) 
              // {
                // to_constlns_element_T* l = *it;
                // mCRL2log(log::verbose) << l << " " << l->new_element() << " " << l->C() << "\n";
              // }
              // mCRL2log(log::verbose) << "---\n";
#endif
              for (auto sit = N->begin(); sit != N->end(); ++sit) 
              {
                state_T* s = *sit;
                // 5.3.2.a
                move_state_to_block(s, Bpp);
                // 5.3.2.b
                for (auto tit = s->Ttgt_begin; transitionpointers[tit]->source == s; ++tit)
                {
                  transition_T* t = transitionpointers[tit];

                  // 5.3.2.b.i
                  if (t->to_constln_ref != nullptr) 
                  {
                    to_constlns_element_T* l = t->to_constln_ref;
                    to_constlns_element_T* lp;
                    if (l->new_element() != nullptr) 
                    {
                      lp = l->new_element();
                    }
                    else 
                    {
                      lp = to_constlns_element_T::new_to_constlns_element(l->C());
                      l->set_new_element(lp);
                      // add lp to Bpp.to_constlns
                      Bpp->to_constlns.insert_linked(lp);
                      // possibly set Bpp.inconstln_ref
                      if (l->C() == Bpp->constellation) 
                      {
                        Bpp->inconstln_ref = lp;
                      }
                      // possibly set Bpp.constln_ref and Bpp.coconstln_ref
                      if (l == Bp->constln_ref) 
                      {
                        Bpp->constln_ref = lp;
                      }
                      else if (l == Bp->coconstln_ref) 
                      {
                        Bpp->coconstln_ref = lp;
                      }
                      // let lp point to l, to be able to efficiently reset new_element pointers later on
                      lp->set_new_element(l);
                    }
                    l->trans_list.move_linked(t, lp->trans_list);
                    t->to_constln_ref = lp;
                  }
                  // 5.3.2.b.ii
                  else 
                  {
                    state_T* sp = t->target;
                  
                    if (sp->block == Bp && (N == &L ? !sp->is_in_L_detect1 : !sp->is_in_Lp_detect2)) 
                    {
                      // an inert transition becomes non-inert
											// 5.3.2.b.ii.A
                      s->inert_cnt--;
											// 5.3.2.b.ii.B
                      if (s->inert_cnt == 0)
                      {
                        XBpp.insert(s);
                        if (s->type == NON_BTM_STATE) 
                        {
                          Bpp->non_btm_states.move_state_linked(s, Bpp->btm_states, BTM_STATE);
                        }
                        else if (s->type == MARKED_NON_BTM_STATE) 
                        {
                          Bpp->marked_non_btm_states.move_state_linked(s, Bpp->marked_btm_states, MARKED_BTM_STATE);
                        }
                      }
                      // add the transition to the corresponding trans_list
                      if (Bpp->inconstln_ref == nullptr) 
                      {
                        Bpp->inconstln_ref = to_constlns_element_T::new_to_constlns_element(Bpp->constellation);
                        Bpp->to_constlns.insert_linked(Bpp->inconstln_ref);
                        if (Bp->inconstln_ref != nullptr) 
                        {
                          Bp->inconstln_ref->set_new_element(Bpp->inconstln_ref);
                          Bpp->inconstln_ref->set_new_element(Bp->inconstln_ref);
                        }
                      }
                      Bpp->inconstln_ref->trans_list.insert_linked(t);
                      t->to_constln_ref = Bpp->inconstln_ref;
                    }
                  }
                }
                // 5.3.2.c
                //check_consistency_blocks();
                //check_consistency_trans_lists(B, setB);
                for (auto tit = s->Tsrc_begin; transitions[tit].target == s; ++tit)
                {
                  transition_T* t = &(transitions[tit]);
                  state_T* sp = t->source;
                  
                  if (sp->block == Bp && (N == &L ? !sp->is_in_L_detect1 : !sp->is_in_Lp_detect2)) 
                  {
                    // an inert transition becomes non-inert
										// 5.3.2.c.i
                    sp->inert_cnt--;
										// 5.3.2.c.ii
                    if (sp->inert_cnt == 0)
                    {
                      XBp.insert(sp);
                      if (sp->type == NON_BTM_STATE) 
                      {
                        Bp->non_btm_states.move_state_linked(sp, Bp->btm_states, BTM_STATE);
                      }
                      else if (sp->type == MARKED_NON_BTM_STATE) 
                      {
                        Bp->marked_non_btm_states.move_state_linked(sp, Bp->marked_btm_states, MARKED_BTM_STATE);
                      }
                    }
                    // add the transition to the corresponding trans_list
                    if (Bp->inconstln_ref == nullptr) 
                    {
                      Bp->inconstln_ref = to_constlns_element_T::new_to_constlns_element(Bp->constellation);
                      Bp->to_constlns.insert_linked(Bp->inconstln_ref);
                      if (Bpp->inconstln_ref != nullptr) 
                      {
                        Bpp->inconstln_ref->set_new_element(Bp->inconstln_ref);
                        Bp->inconstln_ref->set_new_element(Bpp->inconstln_ref);
                      }
                    }
                    Bp->inconstln_ref->trans_list.insert_linked(t);
                    t->to_constln_ref = Bp->inconstln_ref;
                  }
                }
              }
              // reset temporary pointers of states
              for (auto it = L.begin(); it != L.end(); ++it) 
              {
                state_T* s = *it;
                s->is_in_L_detect1 = false;
              }
              for (auto it = Lp.begin(); it != Lp.end(); ++it) 
              {
                state_T* s = *it;
                s->is_in_Lp_detect2 = false;
              }
              while (!P.empty()) 
              {
                state_T* s = *(P.begin());
                P.erase(P.begin());
                s->is_in_P_detect2 = false;
              }
              // check
              //check_consistency_trans_lists(B, setB);
#ifndef NDEBUG
              // mCRL2log(log::verbose) << "---\n";
              // mCRL2log(log::verbose) << Bp->block_id() << ": \n";
              // for (auto it = Bp->to_constlns.begin(); it != Bp->to_constlns.end(); ++it) 
              // {
              //   to_constlns_element_T* l = *it;
              //   mCRL2log(log::verbose) << l << " " << l->new_element() << "\n";
              // }
              // mCRL2log(log::verbose) << "inconstln: " << Bp->inconstln_ref << "\n";
              // mCRL2log(log::verbose) << "---\n";
              // mCRL2log(log::verbose) << Bpp->block_id() << ": \n";
              // for (auto it = Bpp->to_constlns.begin(); it != Bpp->to_constlns.end(); ++it) 
              // {
                // to_constlns_element_T* l = *it;
                // mCRL2log(log::verbose) << l << " " << l->new_element() << "\n";
              // }
              // mCRL2log(log::verbose) << "inconstln: " << Bpp->inconstln_ref << "\n";
              // mCRL2log(log::verbose) << "---\n";
#endif
              // Reset temporary pointers (new elements) of blocks.
              // Remove the associated element if the transition list is empty, UNLESS the element is pointed to
              // by either Bp->constln_ref or Bp->coconstln_ref
              // 5.3.3
              for (auto it = Bpp->to_constlns.begin(); it != Bpp->to_constlns.end(); ++it) 
              {
                to_constlns_element_T* l = *it;
                if (l->new_element() != nullptr) 
                {
                  if (size(l->new_element()) == 0 && l->new_element() != Bp->constln_ref && l->new_element() != Bp->coconstln_ref) 
                  {
                    if (l->new_element()->C() == Bp->constellation) 
                    {
                      Bp->inconstln_ref = nullptr;
                    }
                    Bp->to_constlns.remove_linked(l->new_element());
                    l->new_element()->delete_to_constlns_element();
                    l->set_new_element(nullptr);
                  }
                  else 
                  {
                    l->new_element()->set_new_element(nullptr);
                  }
                  l->set_new_element(nullptr);
                }
              }
            }
#ifndef NDEBUG
            check_consistency_blocks();
            check_consistency_transitions();
#endif
            // 5.3.4
            //check_consistency_trans_lists(B, setB);
            // consider splitting split(B',B) under setB \ B
            // set pointer to split(B',B) if it moved to new block
            if (detect1_finished == TERMINATED) 
            {
              splitBpB = Bpp;
              cosplitBpB = Bp;
            }
            else 
            {
              cosplitBpB = Bpp;
            }
            // point to correct XB list
            sized_forward_list<state_T> *XsplitBpB = nullptr;
            sized_forward_list<state_T> *XcosplitBpB = nullptr;
            sized_forward_list<state_T> *XsplitsplitBpB = nullptr;
            //check_consistency_trans_lists(B, setB);
            if (detect1_finished == TERMINATED) 
            {
              XsplitBpB = &XBpp;
              XcosplitBpB = &XBp;
            }
            else 
            {
              XsplitBpB = &XBp;
              XcosplitBpB = &XBpp;
            }
            // check if splitBpB is stable, and if not, do a nested splitting
            splitsplitBpB = nullptr;
            bool is_stable = false;
            if (splitBpB->coconstln_ref == nullptr) 
            {
              is_stable = true;
            }
            else if (size(splitBpB->coconstln_ref) == 0) 
            {
              is_stable = true;
            }
            //check_consistency_trans_lists(B, setB);
            if (!is_stable) 
            {
              is_stable = true;
              for (auto sit = splitBpB->marked_btm_states.begin(); sit != splitBpB->marked_btm_states.end(); ++sit) 
              {
                state_T* s = *sit;
                if (s->coconstln_cnt == nullptr) 
                {
                  is_stable = false;
                  break;
                }
                if (s->coconstln_cnt->counter_value() == 0) 
                {
                  is_stable = false;
                  break;
                }
              }
            }
            //check_consistency_trans_lists(B, setB);
            if (!is_stable) 
            {
              nr_of_splits++;
              Q.clear();
              L.clear();
              assert(P.empty());
              Lp.clear();
              XBp3.clear();
              XBp4.clear();
              // prepare for lockstep search
              maxsize_detect = (splitBpB->btm_states.size() + splitBpB->non_btm_states.size() + splitBpB->marked_btm_states.size() + splitBpB->marked_non_btm_states.size()) / 2;
              block_detect = splitBpB;
              constellation_splitter_detect = setB;
              detect1_finished = CONTINUE;
              detect2_finished = CONTINUE;
              // lockstep search
#ifndef NDEBUG
              //print_partition();
#endif
              current_state_detect1 = nullptr;
              current_state_detect2 = nullptr;
              in_forward_check_detect2 = false;
              iter_trans_state_added_detect1 = splitBpB->coconstln_ref->trans_list.before_begin();
              iter_state_added_detect2 = splitBpB->marked_btm_states.before_begin();
              
              while (detect1_finished != TERMINATED && detect2_finished != TERMINATED) 
              {
                if (detect1_finished != STOPPED) 
                {
                  detect1_finished = detect1_step(&bisim_partitioner_gw<LTS_TYPE>::has_next_state_detect1_2, &bisim_partitioner_gw<LTS_TYPE>::next_state_detect1_2);
                }
                if (detect1_finished != TERMINATED && detect2_finished != STOPPED) 
                {
                  detect2_finished = detect2_step(&bisim_partitioner_gw<LTS_TYPE>::has_next_state_detect2_2, &bisim_partitioner_gw<LTS_TYPE>::next_state_detect2_2, true, true);
                }
                // DEBUG EXIT
                //if (detect1_finished == STOPPED && detect2_finished == STOPPED) {
                //  exit(1);
                //}
              }
#ifndef NDEBUG
              check_consistency_blocks();
              check_consistency_transitions();
#endif
              // split
              blocks.emplace_back(splitBpB->type(), max_block_index);
              Bp3 = &(blocks.back());
              Bp3->constellation = splitBpB->constellation;
              Bp3->constellation->blocks.insert_linked(Bp3);
              // Let N point to correct list
              sized_forward_list <state_T>* N;
              if (detect1_finished == TERMINATED) 
              {
                N = &L;
              }
              else 
              {
                N = &Lp;
              }
              for (auto sit = N->begin(); sit != N->end(); ++sit) 
              {
                state_T* s = *sit;
                
                // 5.3.4 (5.3.2.a)
                move_state_to_block(s, Bp3);
#ifndef NDEBUG
                //print_partition();
#endif
                // 5.3.4 (5.3.2.b)
                for (auto tit = s->Ttgt_begin; transitionpointers[tit]->source == s; ++tit)
                {
                  transition_T* t = transitionpointers[tit];
                  
                  // 5.3.4 (5.3.2.b.i)
                  if (t->to_constln_ref != nullptr) 
                  {
                    to_constlns_element_T* l = t->to_constln_ref;
                    to_constlns_element_T* lp;
                    if (l->new_element() != nullptr) 
                    {
                      lp = l->new_element();
                    }
                    else 
                    {
                      //assert(Bp3->inconstln_ref == NULL || l->C() != Bp3->constellation);
                      lp = to_constlns_element_T::new_to_constlns_element(l->C());
                      l->set_new_element(lp);
                      // add lp to Bp3.to_constlns (IMPLIED IN PSEUDO-CODE)
                      Bp3->to_constlns.insert_linked(lp);
                      // possibly set Bp3.inconstln_ref
                      if (l->C() == Bp3->constellation) 
                      {
                        Bp3->inconstln_ref = lp;
                      }
                      // point lp to l, to be able to efficiently reset new_element pointers later on
                      lp->set_new_element(l);
                    }
                    l->trans_list.move_linked(t, lp->trans_list);
                    t->to_constln_ref = lp;
                    // postpone deleting element l until cleaning up new_element pointers
                  }
                  // 5.3.4 (5.3.2.b.ii)
                  else 
                  {
                    state_T* sp = t->target;
                  
                    if (sp->block == splitBpB && (N == &L ? !sp->is_in_L_detect1 : !sp->is_in_Lp_detect2)) 
                    {
                      // an inert transition becomes non-inert
                      s->inert_cnt--;
                      if (s->inert_cnt == 0) 
                      {
                        XBp3.insert(s);
                        if (s->type == NON_BTM_STATE) 
                        {
                          Bp3->non_btm_states.move_state_linked(s, Bp3->btm_states, BTM_STATE);
                        }
                        else if (s->type == MARKED_NON_BTM_STATE) 
                        {
                          Bp3->marked_non_btm_states.move_state_linked(s, Bp3->marked_btm_states, MARKED_BTM_STATE);
                        }
                      }
                      // add the transition to the corresponding trans_list
                      if (Bp3->inconstln_ref == nullptr) 
                      {
                        Bp3->inconstln_ref = to_constlns_element_T::new_to_constlns_element(Bp3->constellation);
                        Bp3->to_constlns.insert_linked(Bp3->inconstln_ref);
                        if (splitBpB->inconstln_ref != nullptr) 
                        {
                          splitBpB->inconstln_ref->set_new_element(Bp3->inconstln_ref);
                          Bp3->inconstln_ref->set_new_element(splitBpB->inconstln_ref);
                        }
                      }
                      Bp3->inconstln_ref->trans_list.insert_linked(t);
                      t->to_constln_ref = Bp3->inconstln_ref;
                    }
                  }
                }
                // 5.3.4 (5.3.2.c)
                for (auto tit = s->Tsrc_begin; transitions[tit].target == s; ++tit)
                {
                  transition_T* t = &(transitions[tit]);
                  state_T* sp = t->source;
                  
                  if (sp->block == splitBpB && (N == &L ? !sp->is_in_L_detect1 : !sp->is_in_Lp_detect2)) 
                  {
                    // an inert transition becomes non-inert
                    sp->inert_cnt--;
                    if (sp->inert_cnt == 0) 
                    {
                      XBp4.insert(sp);
                      if (sp->type == NON_BTM_STATE) 
                      {
                        splitBpB->non_btm_states.move_state_linked(sp, splitBpB->btm_states, BTM_STATE);
                      }
                      else if (sp->type == MARKED_NON_BTM_STATE) 
                      {
                        splitBpB->marked_non_btm_states.move_state_linked(sp, splitBpB->marked_btm_states, MARKED_BTM_STATE);
                      }
                    }
                    // add the transition to the corresponding trans_list
                    if (splitBpB->inconstln_ref == nullptr) 
                    {
                      splitBpB->inconstln_ref = to_constlns_element_T::new_to_constlns_element(splitBpB->constellation);
                      splitBpB->to_constlns.insert_linked(splitBpB->inconstln_ref);
                      if (Bp3->inconstln_ref != nullptr) 
                      {
                        Bp3->inconstln_ref->set_new_element(splitBpB->inconstln_ref);
                        splitBpB->inconstln_ref->set_new_element(Bp3->inconstln_ref);
                      }
                    }
                    splitBpB->inconstln_ref->trans_list.insert_linked(t);
                    t->to_constln_ref = splitBpB->inconstln_ref;
                  }
                }
              }
              // reset temporary pointers of states
              for (auto it = L.begin(); it != L.end(); ++it) 
              {
                state_T* s = *it;
                s->is_in_L_detect1 = false;
              }
              for (auto it = Lp.begin(); it != Lp.end(); ++it) 
              {
                state_T* s = *it;
                s->is_in_Lp_detect2 = false;
              }
              while (!P.empty()) 
              {
                state_T* s = *(P.begin());
                P.erase(P.begin());
                s->is_in_P_detect2 = false;
              }
              // Reset temporary pointers (new elements) of blocks.
              // Remove the associated element if the transition list is empty, UNLESS the element is pointed to
              // by either splitBpB->constln_ref or splitBpB->coconstln_ref
              // 5.3.4 (5.3.3)
              for (auto it = Bp3->to_constlns.begin(); it != Bp3->to_constlns.end(); ++it) 
              {
                to_constlns_element_T* l = *it;
                if (l->new_element() != nullptr) 
                {
                  if (size(l->new_element()) == 0 && l->new_element() != splitBpB->constln_ref && l->new_element() != splitBpB->coconstln_ref) 
                  {
                    if (l->new_element()->C() == splitBpB->constellation) 
                    {
                      splitBpB->inconstln_ref = nullptr;
                    }
                    splitBpB->to_constlns.remove_linked(l->new_element());
                    l->new_element()->delete_to_constlns_element();
                    l->set_new_element(nullptr);
                  }
                  else 
                  {
                    l->new_element()->set_new_element(nullptr);
                  }
                  l->set_new_element(nullptr);
                }
              }
              // set splitsplitBpB
              if (detect1_finished == TERMINATED) 
              {
                splitsplitBpB = Bp3;
              }
              else if (detect2_finished == TERMINATED) 
              {
                splitsplitBpB = splitBpB;
                splitBpB = Bp3;
              }
              // determine the XB sets
              sized_forward_list<state_T> *X2 = nullptr;
              if (detect1_finished == TERMINATED) 
              {
                XsplitsplitBpB = &XBp3;
                X2 = &XBp4;
              }
              else 
              {
                XsplitsplitBpB = &XBp4;
                X2 = &XBp3;
              }
              // redistribute content of XsplitBpB
              for (auto sit = XsplitBpB->begin(); sit != XsplitBpB->end(); ++sit) 
              {
                state_T* s = *sit;
                if (s->block == splitsplitBpB) 
                {
                  XsplitsplitBpB->insert(s);
                }
                else 
                {
                  X2->insert(s);
                }
              }
              XsplitBpB = X2;
            } // end if stable (5.3.4)
            // remove markings and reset constln_ref and coconstln_ref
            // 5.3.5
            clean_temp_refs_block(splitBpB);
            clean_temp_refs_block(cosplitBpB);
            clean_temp_refs_block(splitsplitBpB);

            // 5.3.6
            block_T* Bhat = splitBpB;
            block_T* Bhatp = nullptr;
            sized_forward_list<state_T> *XBhat = XsplitBpB;
            while (Bhat != nullptr) 
            {
              for (auto sit = XBhat->begin(); sit != XBhat->end(); ++sit) 
              {
                state_T* s = *sit;
                // 5.3.6.a
                for (auto tit = s->Ttgt_begin; transitionpointers[tit]->source == s; ++tit)
                {
                  transition_T* t = transitionpointers[tit];
                  to_constlns_element_T* setBp_entry = t->to_constln_ref;
                  // 5.3.6.a.i
                  if (setBp_entry->SClist == nullptr) 
                  {
                    setBp_entry->SClist = SClists.get_element();
                    // move the to_constln entry to the front of the list
                    Bhat->to_constlns.move_to_front_linked(setBp_entry);
                  }
                  // 5.3.6.a.ii
                  if (setBp_entry->SClist->list.size() == 0)
                  {
                    setBp_entry->SClist->list.insert(s);
                  }
                  else if (setBp_entry->SClist->list.front() != s)
                  {
                    setBp_entry->SClist->list.insert(s);
                  }
                }
                // 5.3.6.b
                Bhat->new_btm_states.insert(s);
              }
            
              // consider next block
              if (Bhat == splitsplitBpB) 
              {
                break;
              }
              if (Bhat == cosplitBpB) 
              {
                Bhat = splitsplitBpB;
                XBhat = XsplitsplitBpB;
              }
              else if (cosplitBpB != nullptr) 
              {
                Bhat = cosplitBpB;
                XBhat = XcosplitBpB;
              }
              else 
              {
                Bhat = splitsplitBpB;
                XBhat = XsplitsplitBpB;
              }
            }
#ifndef NDEBUG
            //print_partition();
            check_consistency_blocks();
#endif
            // 5.3.7
            // We use a stack blocks_to_process to keep track of which blocks still need to be checked for stability
            std::stack < block_T* > blocks_to_process;
            // push blocks on the stack if they have new bottom states
            if (splitBpB != nullptr) 
            {
              if (splitBpB->new_btm_states.size() > 0) 
              {
                blocks_to_process.push(splitBpB);
              }
            }
            if (cosplitBpB != nullptr) 
            {
              if (cosplitBpB->new_btm_states.size() > 0) 
              {
                blocks_to_process.push(cosplitBpB);
              }
            }
            if (splitsplitBpB != nullptr) 
            {
              if (splitsplitBpB->new_btm_states.size() > 0) 
              {
                blocks_to_process.push(splitsplitBpB);
              }
            }
            while (blocks_to_process.size() > 0) 
            {
              // 5.3.7.a
              Bhat = blocks_to_process.top();
              blocks_to_process.pop();
              bool split = false;
              // 5.3.7.b
              // Find a constellation under which the block is not stable
              for (auto cit = Bhat->to_constlns.begin(); cit != Bhat->to_constlns.end(); ++cit) 
              {
                to_constlns_element_T* e = *cit;
                bool splitcrit_met = false;
                if (e->SClist == nullptr) 
                {
                  splitcrit_met = true;
                  split = true;
                }
                else if (e->SClist->list.size() < Bhat->new_btm_states.size())
                {
                  splitcrit_met = true;
                  split = true;
                }
                if (splitcrit_met) 
                {
                  nr_of_splits++;
                  // further splitting is required
                  XBp.clear();
                  XBpp.clear();
                  // Prepare detect1 and detect2 for lockstep
                  Q.clear();
                  L.clear();
                  assert(P.empty());
                  Lp.clear();
                  // 5.3.7.b.i.A / B
                  // we prepare detect1 by walking over the transitions from Bhat to constellation pointed
                  // to by *cit. To optimise this, we should probably maintain a list of states that have a direct transition from Bhat to constellation
                  // of *cit, and instead walk over that list
                  maxsize_detect = (Bhat->btm_states.size() + Bhat->non_btm_states.size()) / 2;
                  block_detect = Bhat;
                  e_detect = e;
                  constellation_splitter_detect = e->C();
                  detect1_finished = CONTINUE;
                  detect2_finished = CONTINUE;
                  current_state_detect1 = nullptr;
                  current_state_detect2 = nullptr;
                  in_forward_check_detect2 = false;
                  iter_trans_state_added_detect1 = e->trans_list.before_begin();
                  iter_state_added_detect2 = Bhat->new_btm_states.before_begin();
                  if (e->SClist == nullptr) 
                  {
                    sclist_is_empty_detect2 = true;
                  }
                  else 
                  {
                    sclist_is_empty_detect2 = false;
                    iter_sclist_detect2 = e->SClist->list.begin();
                  }
                  while (detect1_finished != TERMINATED && detect2_finished != TERMINATED) 
                  {
                    if (detect1_finished != STOPPED) 
                    {
                      detect1_finished = detect1_step(&bisim_partitioner_gw<LTS_TYPE>::has_next_state_detect1_3, &bisim_partitioner_gw<LTS_TYPE>::next_state_detect1_3);
                    }
                    if (detect1_finished != TERMINATED && detect2_finished != STOPPED) 
                    {
                      detect2_finished = detect2_step(&bisim_partitioner_gw<LTS_TYPE>::has_next_state_detect2_3, &bisim_partitioner_gw<LTS_TYPE>::next_state_detect2_3, false, true);
                    }
                  }
                  // 5.3.7.b.ii (5.3.2)
                  blocks.emplace_back(Bhat->type(), max_block_index);
                  Bhatp = &(blocks.back());
                  Bhatp->constellation = Bhat->constellation;
                  Bhatp->constellation->blocks.insert_linked(Bhatp);
                  // Let N point to correct list
                  sized_forward_list <state_T> *N;
                  if (detect1_finished == TERMINATED) 
                  {
                    N = &L;
                  }
                  else 
                  {
                    N = &Lp;
                  }
                  for (auto sit = N->begin(); sit != N->end(); ++sit) 
                  {
                    state_T* s = *sit;
                    move_state_to_block(s, Bhatp);
                    for (auto tit = s->Ttgt_begin; transitionpointers[tit]->source == s; ++tit)
                    {
                      transition_T* t = transitionpointers[tit];

                      if (t->to_constln_ref != nullptr)
                      {
                        to_constlns_element_T* l = t->to_constln_ref;
                        to_constlns_element_T* lp;
                        if (l->new_element() != nullptr) 
                        {
                          lp = l->new_element();
                        }
                        else 
                        {
                          lp = to_constlns_element_T::new_to_constlns_element(l->C());
                          l->set_new_element(lp);
                          // point lp->new_element back to l, to efficiently reset
                          // new_element pointers of 'old' elements later
                          lp->set_new_element(l);
                          // add lp to Bhatp.to_constlns
                          Bhatp->to_constlns.insert_linked(lp);
                          // possibly set Bhatp.inconstln_ref
                          if (l->C() == Bhatp->constellation) 
                          {
                            Bhatp->inconstln_ref = lp;
                          }
                        }
                        l->trans_list.move_linked(t, lp->trans_list);
                        t->to_constln_ref = lp;
                        // postpone deleting element l until cleaning up new_element pointers
                      }
                      else 
                      {
                        state_T* sp = t->target;
                
                        if (sp->block == Bhat && (N == &L ? !sp->is_in_L_detect1 : !sp->is_in_Lp_detect2)) 
                        {
                          // an inert transition becomes non-inert
                          s->inert_cnt--;
                          if (s->inert_cnt == 0) 
                          {
                            XBpp.insert(s);
                            Bhatp->non_btm_states.move_state_linked(s, Bhatp->btm_states, BTM_STATE);
                          }
                          // add the transition to the corresponding trans_list
                          if (Bhatp->inconstln_ref == nullptr) 
                          {
                            Bhatp->inconstln_ref = to_constlns_element_T::new_to_constlns_element(Bhatp->constellation);
                            Bhatp->to_constlns.insert_linked(Bhatp->inconstln_ref);
                            if (Bhat->inconstln_ref != nullptr) 
                            {
                              Bhat->inconstln_ref->set_new_element(Bhatp->inconstln_ref);
                              Bhatp->inconstln_ref->set_new_element(Bhat->inconstln_ref);
                            }
                          }
                          Bhatp->inconstln_ref->trans_list.insert_linked(t);
                          t->to_constln_ref = Bhatp->inconstln_ref;
                        }
                      }
                    }
                    for (auto tit = s->Tsrc_begin; transitions[tit].target == s; ++tit)
                    {
                      transition_T* t = &(transitions[tit]);
                      state_T* sp = t->source;
                
                      if (sp->block == Bhat && (N == &L ? !sp->is_in_L_detect1 : !sp->is_in_Lp_detect2)) 
                      {
                        // an inert transition becomes non-inert
                        sp->inert_cnt--;
                        if (sp->inert_cnt == 0) 
                        {
                          XBp.insert(sp);
                          Bhat->non_btm_states.move_state_linked(sp, Bhat->btm_states, BTM_STATE);
                        }
                        // add the transition to the corresponding trans_list
                        if (Bhat->inconstln_ref == nullptr) 
                        {
                          Bhat->inconstln_ref = to_constlns_element_T::new_to_constlns_element(Bhat->constellation);
                          Bhat->to_constlns.insert_linked(Bhat->inconstln_ref);
                          if (Bhatp->inconstln_ref != nullptr) 
                          {
                            Bhatp->inconstln_ref->set_new_element(Bhat->inconstln_ref);
                            Bhat->inconstln_ref->set_new_element(Bhatp->inconstln_ref);
                          }
                        }
                        Bhat->inconstln_ref->trans_list.insert_linked(t);
                        t->to_constln_ref = Bhat->inconstln_ref;
                      }
                    }
                  }
                  sized_forward_list < state_T >* SinSC;
                  sized_forward_list < state_T >* SnotinSC;
                  if (detect1_finished == TERMINATED) 
                  {
                    // keep temp value in SinSC for swapping
                    Bhatp->new_btm_states.swap(Bhat->new_btm_states);
                    SinSC = &(Bhatp->new_btm_states);
                    SnotinSC = &(Bhat->new_btm_states);
                  }
                  else 
                  {
                    SinSC = &(Bhat->new_btm_states);
                    SnotinSC = &(Bhatp->new_btm_states);
                  }
                  // Walk through new states and add them to SnotinSC if they are not in SClist
                  auto prev_sit = SinSC->before_begin();
                  typename sized_forward_list <state_T>::iterator it_SClist;
                  if (!sclist_is_empty_detect2) 
                  {
                    it_SClist = e->SClist->list.begin();
                  }
                  for (auto sit = SinSC->begin(); sit != SinSC->end(); ++sit) 
                  {
                    state_T* s = *sit;
                    if (sclist_is_empty_detect2) 
                    {
                      SinSC->move_from_after_to_back(prev_sit, *SnotinSC, sit);
                    }
                    else if (it_SClist == e->SClist->list.end())
                    {
                      SinSC->move_from_after_to_back(prev_sit, *SnotinSC, sit);
                    }
                    else if (s != *it_SClist) 
                    {
                      SinSC->move_from_after_to_back(prev_sit, *SnotinSC, sit);
                    }
                    else 
                    {
                      it_SClist++;
                    }
                    prev_sit = sit;
                  }
									// 5.3.7.b.ii
                  for (auto bit = Bhat->to_constlns.begin(); bit != Bhat->to_constlns.end(); ++bit) 
                  {
                    to_constlns_element_T* l = *bit;
                    // if SClist is empty, we can stop, since all subsequent elements will have empty SClists
                    if (l->SClist == nullptr) 
                    {
                      break;
                    }
                    auto prev_sit = l->SClist->list.before_begin();
                    // 5.3.7.b.ii.A
                    for (auto sit = l->SClist->list.begin(); sit != l->SClist->list.end(); ++sit)
                    {
                      state_T* s = *sit;
                      if (s->block == Bhatp) 
                      {
                        // new_element still points to corresponding element of Bhatp
                        // Note that we cannot have l->new_element() == NULL, since then, l would be a new entry with an empty SClist
                        if (l->new_element()->SClist == nullptr) 
                        {
                          l->new_element()->SClist = SClists.get_element();
                          // move to front in list
                          Bhatp->to_constlns.move_to_front_linked(l->new_element());
                        }
                        l->SClist->list.move_from_after_to_back(prev_sit, l->new_element()->SClist->list, sit);
                      }
                      prev_sit = sit;
                    }
                    // 5.3.7.b.ii.B
                    if (l->SClist->list.size() == 0)
                    {
                      SClists.remove_element(l->SClist);
                      l->SClist = nullptr;
                      // move the l entry to the back of the list. We do not need to worry about keeping iterator e valid (in while loop), since we will not use it on the current list anymore.
                      Bhat->to_constlns.move_to_back_linked(l, bit);
                    }
                  }
                  // reset temporary pointers of states
                  for (auto it = L.begin(); it != L.end(); ++it) 
                  {
                    state_T* s = *it;
                    s->is_in_L_detect1 = false;
                  }
                  for (auto it = Lp.begin(); it != Lp.end(); ++it) 
                  {
                    state_T* s = *it;
                    s->is_in_Lp_detect2 = false;
                  }
                  while (!P.empty()) 
                  {
                    state_T* s = *(P.begin());
                    P.erase(P.begin());
                    s->is_in_P_detect2 = false;
                  }
                  // reset temporary pointers (new elements) of blocks
                  // 5.3.7.b.iii (5.3.3)
                  for (auto it = Bhatp->to_constlns.begin(); it != Bhatp->to_constlns.end(); ++it) 
                  {
                    to_constlns_element_T* l = *it;
                    if (l->new_element() != nullptr) 
                    {
                      if (size(l->new_element()) == 0) 
                      {
                        if (l->new_element()->C() == Bhat->constellation) 
                        {
                          Bhat->inconstln_ref = nullptr;
                        }
                        Bhat->to_constlns.remove_linked(l->new_element());
                        l->new_element()->delete_to_constlns_element();
                        l->set_new_element(nullptr);
                      }
                      else 
                      {
                        l->new_element()->set_new_element(nullptr);
                      }
                      l->set_new_element(nullptr);
                    }
                  }
                  // 5.3.7.b.iii (5.3.6)
                  block_T* Btmp = Bhat;
                  sized_forward_list<state_T> *XBtmp = &XBp;
                  //check_consistency_transitions();
                  //check_consistency_blocks();
                  while (Btmp != nullptr) 
                  {
                    for (auto sit = XBtmp->begin(); sit != XBtmp->end(); ++sit) 
                    {
                      state_T* s = *sit;
                      for (auto tit = s->Ttgt_begin; transitionpointers[tit]->source == s; ++tit)
                      {
                        transition_T* t = transitionpointers[tit];
                        to_constlns_element_T* setBp_entry = t->to_constln_ref;
                        // 5.3.4.a.i
                        if (setBp_entry->SClist == nullptr) 
                        {
                          setBp_entry->SClist = SClists.get_element();
                          // move the to_constln entry to the front of the list
                          Btmp->to_constlns.move_to_front_linked(setBp_entry);
                        }
                        // 5.3.4.a.ii
                        if (setBp_entry->SClist->list.size() == 0)
                        {
                          setBp_entry->SClist->list.insert(s);
                        }
                        else if (setBp_entry->SClist->list.front() != s)
                        {
                          setBp_entry->SClist->list.insert(s);
                        }
                      }
                      // 5.3.4.a.iii
                      Btmp->new_btm_states.insert(s);
                    }
              
                    // consider next block
                    if (Btmp == Bhatp) 
                    {
                      break;
                    }
                    if (Btmp == Bhat) 
                    {
                      Btmp = Bhatp;
                      XBtmp = &XBpp;
                    }
                  }
                  // push resulting blocks on the work stack if they have new states
                  // 5.3.7.b.iv
                  if (Bhatp->new_btm_states.size() > 0) 
                  {
                    blocks_to_process.push(Bhatp);
                  }
                  if (Bhat->new_btm_states.size() > 0) 
                  {
                    blocks_to_process.push(Bhat);
                  }
                  break;
                } // end if splitcrit_met
                // we do not clean e->SClist and move e to the back.
                // We reset the list only later. This, since the original moving would lead to the list
                // of e representing a reason to split Bhat (an empty list is sufficient for that).
//                else {
//                  // 5.3.5.b
//                  deleteobject (e->SClist);
//                  Bhat->to_constlns.remove_linked(e, cit);
//                  Bhat->to_constlns.insert_linked_back(e);
//                }
              } // end of walk over to_constln of block
              // if block is stable, reset new_btm_states, remove remaining SClists
              // 5.3.7.c
              if (!split) 
              {
                Bhat->new_btm_states.clear();
                for (auto it = Bhat->to_constlns.begin(); it != Bhat->to_constlns.end(); ++it) 
                {
                  to_constlns_element_T* l = *it;
                  if (l->SClist != nullptr) 
                  {
                    SClists.remove_element(l->SClist);
                    l->SClist = nullptr;
                  }
                  else 
                  {
                    break;
                  }
                }
              }
            } // end while loop for stabilising blocks
            // 5.3.8
            if (old_constellation_type_of_Bp == TRIVIAL) 
            {
              assert(Bp->constellation->type() == NONTRIVIAL);
              trivial_constlns.move_linked(Bp->constellation, non_trivial_constlns);
            }
          } // end walking over splittable blocks
        } // end walk over non-trivial constellations
      } // end while there are non-trivial constellations

      mCRL2log(log::verbose) << "number of splits performed: " << nr_of_splits << "\n";

      // clear what is no longer needed
      constellations.clear();
    }

#ifndef NDEBUG
    // The methods below are intended to check the consistency of the internal data
    // structure. Their sole purpose is to detect programming errors. They have no
    // side effects on the data structure. If a problem occurs, execution halts with
    // an assert.

    void check_non_empty_blocks() 
    {
      sized_forward_list < constellation_T >* L = &trivial_constlns;
      while (L != nullptr) 
      {
        for (auto cit = L->begin(); cit != L->end(); ++cit) 
        {
          constellation_T* C = *cit;
          for (auto bit = C->blocks.begin(); bit != C->blocks.end(); ++bit) 
          {
            block_T* B = *bit;
            assert(B->btm_states.size() + B->non_btm_states.size() + B->marked_btm_states.size() + B->marked_non_btm_states.size() > 0);
          }
        }
      
        if (L == &trivial_constlns) 
        {
          L = &non_trivial_constlns;
        }
        else 
        {
          L = nullptr;
        }
      }
    }

    void check_consistency_transitions() 
    {
      bool found = false;
      for (auto sit = states.begin(); sit != states.end(); ++sit) 
      {
        const state_T& s = *sit;
        for (auto tit = s.Ttgt_begin; transitionpointers[tit]->source == &s; ++tit)
        {
          transition_T* t = transitionpointers[tit];
          block_T* B = t->source->block;
          found = false;
          if (t->to_constln_ref != nullptr) 
          {
            assert(t->target->block->constellation == t->to_constln_ref->C());
            for (auto eit = B->to_constlns.begin(); eit != B->to_constlns.end(); ++eit) 
            {
              to_constlns_element_T* e = *eit;
              if (e == t->to_constln_ref) 
              {
                found = true;
              }
            }
            assert(found);
          }
        }
      }
    }

    void check_consistency_blocks()
    {
      sized_forward_list < constellation_T >* L = &trivial_constlns;
      size_t count;
      while (L != nullptr) 
      {
        for (auto cit = L->begin(); cit != L->end(); ++cit) 
        {
          constellation_T* C = *cit;
          for (auto bit = C->blocks.begin(); bit != C->blocks.end(); ++bit) 
          {
            block_T* B = *bit;
            // walk over elements in to_constlns
            sized_forward_list < to_constlns_element_T>::iterator prev_eit = B->to_constlns.before_begin();
            for (auto eit = B->to_constlns.begin(); eit != B->to_constlns.end(); ++eit) 
            {
              to_constlns_element_T* e = *eit;
              assert(e->new_element() == nullptr);
              sized_forward_list < transition_T >::iterator prev_tit = e->trans_list.before_begin();
              for (auto tit = e->trans_list.begin(); tit != e->trans_list.end(); ++tit) 
              {
                transition_T* t = *tit;
                assert(t->ptr_in_list == prev_tit);
                assert(t->to_constln_ref == e);
                assert(t->source->block == B);
                assert(t->target->block->constellation == e->C());
                // assert that t->to_constln_ref is valid
                count = 0;
                for (auto eit2 = B->to_constlns.begin(); eit2 != B->to_constlns.end(); ++eit2) 
                {
                  to_constlns_element_T* l = *eit2;
                  if (l == t->to_constln_ref) 
                  {
                    count = 1;
                    break;
                  }
                }
                assert(count == 1);
                prev_tit = tit;
              }
              assert(e->new_element() == nullptr);  
              assert(e->ptr_in_list == prev_eit); 
              
              prev_eit = eit;
            }
          }
        }
        if (L == &trivial_constlns) 
        {
          L = &non_trivial_constlns;
        }
        else 
        {
          L = nullptr;
        }
      }
    }

    void check_consistency_state_list(block_T* B, state_type t)
    {
      sized_forward_list < state_T >* list;
      switch (t) 
      {
        case BTM_STATE:
          list = &(B->btm_states);
          break;
        case NON_BTM_STATE:
          list = &(B->non_btm_states);
          break;
        case MARKED_BTM_STATE:
          list = &(B->marked_btm_states);
          break;
        case MARKED_NON_BTM_STATE:
          list = &(B->marked_non_btm_states);
          break;
      }
      sized_forward_list < state_T >::iterator prev_sit = list->before_begin();
      for (auto sit = list->begin(); sit != list->end(); ++sit) 
      {
        state_T* s = *sit;
        assert(s->block == B);
        assert(prev_sit == s->ptr_in_list);
        assert(s->type == t);
        assert(s->constln_cnt == nullptr);
        assert(s->coconstln_cnt == nullptr);
        assert(s->is_in_L_detect1 == false);
        assert(s->is_in_Lp_detect2 == false);
        assert(s->is_in_P_detect2 == false);
        size_t local_inert_cnt = 0;
        // check outgoing transitions
        for (auto tit = s->Ttgt_begin; transitionpointers[tit]->source == s; ++tit)
        {
          transition_T* t = transitionpointers[tit];
          assert(t->source == s);
          
          if (t->target->block == B) 
          {
            local_inert_cnt++;
          }
        }
        assert(s->inert_cnt == local_inert_cnt);
        // check incoming transitions
        for (auto tit = s->Tsrc_begin; transitions[tit].target == s; ++tit)
        {
          transition_T* t = &(transitions[tit]);
          assert(t->target == s);
          assert(t->to_constln_ref->C() == B->constellation);
        }
        
        prev_sit = sit;
      }
    }

    void check_internal_consistency_of_constellations(
               const size_t consttype,
               const bool /* branching [Intended to use if strong bisimulation can also be checked] */ )
    {
      // check the constellations of given type and blocks
      sized_forward_list < constellation_T >* Clist;
      if (consttype == TRIVIAL) 
      {
        Clist = &trivial_constlns;
      }
      else 
      {
        Clist = &non_trivial_constlns;
      }
      sized_forward_list < constellation_T >::iterator prev_cit = Clist->before_begin();
      for (auto cit = Clist->begin(); cit != Clist->end(); ++cit) 
      {
        constellation_T* C = *cit;
        // check consistency of ptr_in_list
        assert(prev_cit == C->ptr_in_list);
        size_t const_size = 0;
        sized_forward_list < block_T >::iterator prev_bit = C->blocks.before_begin();
        for (auto bit = C->blocks.begin(); bit != C->blocks.end(); ++bit) 
        {
          block_T* B = *bit;
          const_size += B->btm_states.size() + B->non_btm_states.size() + B-> marked_btm_states.size() + B->marked_non_btm_states.size();
          assert(B->type() == STD_BLOCK || B->type() == EXTRA_KRIPKE_BLOCK);
          assert(B->constellation == C);
          assert(B->constellation->type() == consttype);
          // check consistency of ptr_in_list
          assert(prev_bit == B->ptr_in_list);
          // empty state lists
          assert(B->marked_btm_states.size() == 0);
          assert(B->marked_non_btm_states.size() == 0);
          assert(B->new_btm_states.size() == 0);
          assert(B->constln_ref == nullptr);
          assert(B->coconstln_ref == nullptr);
          // check states
          check_consistency_state_list(B, BTM_STATE);
          check_consistency_state_list(B, NON_BTM_STATE);
          
          prev_bit = bit;
        }
        // check constellation size
        assert(const_size == C->size);
        
        prev_cit = cit;
      }
    }

    void check_internal_consistency_of_the_partitioning_data_structure_gw(const bool branching)
    {
      // check the constellations and blocks
      check_internal_consistency_of_constellations(TRIVIAL, branching);
      check_internal_consistency_of_constellations(NONTRIVIAL, branching);
    }
     
    void check_consistency_trans_lists(block_T* /* split_block */, constellation_T* split_const)
    {
      // check consistency of the block to constellation transition lists
      sized_forward_list < constellation_T >* L = &trivial_constlns;
      while (L != nullptr) 
      {
        for (auto cit = L->begin(); cit != L->end(); ++cit) 
        {
          constellation_T* C = *cit;
          for (auto bit = C->blocks.begin(); bit != C->blocks.end(); ++bit) 
          {
            block_T* B = *bit;
            // traverse the lists
            for (auto eit = B->to_constlns.begin(); eit != B->to_constlns.end(); ++eit) 
            {
              to_constlns_element_T* e = *eit;
              if (e->C() == split_const) 
              {
                for (auto tit = e->trans_list.begin(); tit != e->trans_list.end(); ++tit) 
                {
                  transition_T* t = *tit;
                  //assert(t->target->block->constellation == e->C);
                  if (t->source->coconstln_cnt != nullptr) 
                  {
                      assert(t->source->coconstln_cnt->counter_value() > 0);
                  }
                }
              }
            }
          }
        }
      
      
        if (L == &trivial_constlns) 
        {
          L = &non_trivial_constlns;
        }
        else 
        {
          L = nullptr;
        }
      }
    }
  
    void check_consistency_trans_lists_2(constellation_T* split_const)
    {
      // check consistency of the block to constellation transition lists
      sized_forward_list < constellation_T >* L = &trivial_constlns;
      while (L != nullptr) 
      {
        for (auto cit = L->begin(); cit != L->end(); ++cit) 
        {
          constellation_T* C = *cit;
          for (auto bit = C->blocks.begin(); bit != C->blocks.end(); ++bit) 
          {
            block_T* B = *bit;
            // traverse the lists
            for (auto eit = B->to_constlns.begin(); eit != B->to_constlns.end(); ++eit) 
            {
              to_constlns_element_T* e = *eit;
              if (e->C() == split_const) 
              {
                for (auto tit = e->trans_list.begin(); tit != e->trans_list.end(); ++tit) 
                {
                  transition_T* t = *tit;
                  assert(t->source->coconstln_cnt == nullptr);
                }
              }
            }
          }
        }
      
      
        if (L == &trivial_constlns) 
        {
          L = &non_trivial_constlns;
        }
        else 
        {
          L = nullptr;
        }
      }
    }
#endif // not NDEBUG

};


/** \brief Reduce transition system l with respect to strong or (divergence preserving) branching bisimulation.
 * \param[in/out] l The transition system that is reduced.
 * \param[in] branching If true branching bisimulation is applied, otherwise strong bisimulation.
 * \param[in] preserve_divergence Indicates whether loops of internal actions on states must be preserved. If false
 *            these are removed. If true these are preserved.  */
template < class LTS_TYPE>
void bisimulation_reduce_gw(
  LTS_TYPE& l,
  const bool branching = false,
  const bool preserve_divergence = false);


/** \brief Checks whether the two initial states of two lts's are strong or branching bisimilar.
 * \details This lts and the lts l2 are not usable anymore after this call.
 *          The space consumption is O(n) and time is O(m log n). It uses the branching bisimulation
 *          algorithm by Groote and Wijs from 2015.
 * \param[in/out] l1 A first transition system.
 * \param[in/out] l2 A second transistion system.
 * \param[branching] If true branching bisimulation is used, otherwise strong bisimulation is applied.
 * \param[preserve_divergence] If true and branching is true, preserve tau loops on states.
 * \retval True iff the initial states of the current transition system and l2 are (divergence preserving) (branching) bisimilar */
template < class LTS_TYPE>
bool destructive_bisimulation_compare_gw(
  LTS_TYPE& l1,
  LTS_TYPE& l2,
  const bool branching=false,
  const bool preserve_divergence=false,
  const bool generate_counter_examples = false);


/** \brief Checks whether the two initial states of two lts's are strong or branching bisimilar.
 *  \details The current transitions system and the lts l2 are first duplicated and subsequently
 *           reduced modulo bisimulation. If memory space is a concern, one could consider to
 *           use destructive_bisimulation_compare. This routine uses the m log n
 *           branching bisimulation routine. It runs in O(m log n) and uses O(n) memory where n is the
 *           number of states and m is the number of transitions.
 * \param[in/out] l1 A first transition system.
 * \param[in/out] l2 A second transistion system.
 * \param[branching] If true branching bisimulation is used, otherwise strong bisimulation is applied.
 * \param[preserve_divergence] If true and branching is true, preserve tau loops on states.
 * \retval True iff the initial states of the current transition system and l2 are (divergence preserving) (branching) bisimilar */
template < class LTS_TYPE>
bool bisimulation_compare_gw(
  const LTS_TYPE& l1,
  const LTS_TYPE& l2,
  const bool branching=false);


template < class LTS_TYPE>
void bisimulation_reduce_gw(LTS_TYPE& l,
                            const bool branching/*=false */,
                            const bool preserve_divergence/*=false */) 
{
  // First, remove tau loops in case of branching bisimulation.
  if (branching)
  {
    scc_reduce(l,preserve_divergence);
  }

  // Secondly, apply the branching bisimulation reduction algorithm. If there are no tau's,
  // this will automatically yield strong bisimulation.
  detail::bisim_partitioner_gw<LTS_TYPE> bisim_part(l, branching, preserve_divergence);

  // Clear the state labels of the LTS l
  l.clear_state_labels();

  // Assign the reduced LTS
  l.set_num_states(bisim_part.num_eq_classes());
  l.set_initial_state(bisim_part.get_eq_class(l.initial_state()));
  bisim_part.replace_transitions(branching,preserve_divergence);
}

template < class LTS_TYPE>
bool bisimulation_compare_gw(
  const LTS_TYPE& l1,
  const LTS_TYPE& l2,
  const bool branching, /* =false*/
  const bool preserve_divergence /* =false*/)
{
  LTS_TYPE l1_copy(l1);
  LTS_TYPE l2_copy(l2);
  return destructive_bisimulation_compare_gw(l1_copy,l2_copy,branching,preserve_divergence);
}

template < class LTS_TYPE>
bool destructive_bisimulation_compare_gw(
  LTS_TYPE& l1,
  LTS_TYPE& l2,
  const bool branching /* =false*/,
  const bool preserve_divergence /* = false*/,
  const bool generate_counter_examples /* = false*/)
{
  if (generate_counter_examples)
  {
    mCRL2log(log::warning) << "The GW branching bisimulation algorithm does not generate counterexamples.\n";
  }
  size_t init_l2 = l2.initial_state() + l1.num_states();
  mcrl2::lts::detail::merge(l1,l2);
  l2.clear(); // No use for l2 anymore.


  // First remove tau loops in case of branching bisimulation.
  if (branching)
  {
    detail::scc_partitioner<LTS_TYPE> scc_part(l1);
    scc_part.replace_transitions(preserve_divergence);
    l1.set_num_states(scc_part.num_eq_classes());
    l1.set_initial_state(scc_part.get_eq_class(l1.initial_state()));
    init_l2 = scc_part.get_eq_class(init_l2);
  }

  detail::bisim_partitioner_gw<LTS_TYPE> bisim_part(l1, branching, preserve_divergence);
  return bisim_part.in_same_class(l1.initial_state(),init_l2);
}

}
}
}
#endif
