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
#include <forward_list>
#include <unordered_map>
#include <string>
#include <tuple>
#include <stack>
#include <queue>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/trace/trace.h"
#include "mcrl2/lts/lts_utilities.h"
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_merge.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_dot.h"

#define BTM_STATE                                   0
#define NON_BTM_STATE                         1
#define MARKED_BTM_STATE               2
#define MARKED_NON_BTM_STATE     3
#define STATE                                             4
#define NEW_BTM_STATE                         5

// definition to distinguish two types of blocks
#define STD_BLOCK                                   0
#define EXTRA_KRIPKE_BLOCK          1

// definition to distinguish two types of constellations
#define TRIVIAL                                        0
#define NONTRIVIAL                              1

// return values for procedures in lockstep search
#define CONTINUE                                   0
#define TERMINATED                              1
#define     STOPPED                                        2

namespace mcrl2
{
namespace lts
{
namespace detail
{
typedef size_t state_type;
typedef size_t statemode_type;
typedef size_t block_type;
typedef size_t label_type;
typedef size_t constellation_type;

typedef struct state_T state_T;
typedef struct block_T block_T;
typedef struct transition_T transition_T;
typedef struct constellation_T constellation_T;
// to constellation counter (unique for a (state, constellation) combination)
typedef struct to_constlns_element_T to_constlns_element_T;
typedef struct counter_T counter_T;

// delete object and set pointer to NULL
template<typename T>
void deleteobject(T*& obj)
{
     if (obj != NULL) {
          delete (obj);
          obj = NULL;
     }
}

// forward_list with size counter and pointer to last element
template < class T>
class sized_forward_list
{
     public:
          sized_forward_list()
          {
               listsize = 0;
               fl = new std::forward_list < T* >;
               last = fl->before_begin();
          }

          ~sized_forward_list()
          {
               delete(fl);
          }
     
          typename std::forward_list<T*>::iterator before_begin()
          {
               return fl->before_begin();
          }
     
          typename std::forward_list<T*>::iterator begin()
          {
               return fl->begin();
          }

          typename std::forward_list<T*>::iterator end()
          {
               return fl->end();
          }

          void insert(T* obj)
          {
               auto it = fl->insert_after(fl->before_begin(), obj);
               listsize++;
               if (listsize == 1) {
                    last = it;
               }
          }

          void insert_back(T* obj)
          {
               auto it = fl->insert_after(last, obj);
               last = it;
               listsize++;
          }
     
          // in 'linked' methods, the object to be inserted has an iterator 'ptr_in_list' which needs to refer to the position
          // preceding the one where obj is inserted. This allows constant time lookup of objects in lists.
          void insert_linked(T* obj)
          {
               obj->ptr_in_list = fl->before_begin();
               auto it = fl->insert_after(obj->ptr_in_list, obj);
               listsize++;
               if (listsize == 1) {
                    last = it;
               }
               else {
                    auto itnext = it;
                    itnext++;
                    (*itnext)->ptr_in_list = it;
               }
          }
     
          // insert element at the back of the list
          void insert_linked_back(T* obj) {
               obj->ptr_in_list = last;
               auto it = fl->insert_after(last, obj);
               last = it;
               listsize++;
          }

          // special method for inserting states
          void insert_state_linked(T* obj, statemode_type t)
          {
               // make sure that element pointing before current first element will point to the newly inserted element
               // (the new position before its element)
               obj->ptr_in_list = fl->before_begin();
               auto it = fl->insert_after(obj->ptr_in_list, obj);
               listsize++;
               if (listsize == 1) {
                    last = it;
               }
               else {
                    auto itnext = it;
                    itnext++;
                    (*itnext)->ptr_in_list = it;
               }
               obj->type = t;
          }
     
          typename std::forward_list<T*>::iterator remove(typename std::forward_list<T*>::iterator position)
          {
               auto posnext = position;
               ++posnext;
               if (posnext == last) {
                    last = position;
               }
               listsize--;
               return fl->erase_after(position);
          }

          typename std::forward_list<T*>::iterator remove(typename std::forward_list<T*>::iterator position, typename std::forward_list<T*>::iterator& current)
          {
               auto posnext = position;
               ++posnext;
               if (posnext == last) {
                    last = position;
               }
               listsize--;
               if (current == posnext) {
                    current = position;
               }
               return fl->erase_after(position);
          }

          void remove_linked(T* obj)
          {
               auto objptrnext = obj->ptr_in_list;
               ++objptrnext;
               if (objptrnext == last) {
                    last = obj->ptr_in_list;
               }
               // redirect pointer of next element in list
               ++objptrnext;
               if (objptrnext != fl->end()) {
                    (*objptrnext)->ptr_in_list = obj->ptr_in_list;
               }
               listsize--;
               fl->erase_after(obj->ptr_in_list);
          }

          void remove_linked(T* obj, typename std::forward_list<T*>::iterator& current)
          {
               auto objptrnext = obj->ptr_in_list;
               ++objptrnext;
               if (objptrnext == last) {
                    last = obj->ptr_in_list;
               }
               // redirect pointer of next element in list
               auto objptrnextnext = objptrnext;
               ++objptrnextnext;
               if (objptrnextnext != fl->end()) {
                    (*objptrnextnext)->ptr_in_list = obj->ptr_in_list;
               }
               listsize--;
               if (current == objptrnext) {
                    current = obj->ptr_in_list;
               }
               fl->erase_after(obj->ptr_in_list);
          }
     
          typename std::forward_list < T* >* getlist ()
          {
               return fl;
          }
     
          // concatenate two lists, where the elements have pointers (iterators) to the
          // positions preceding the ones where they are stored
          void concat_linked(sized_forward_list<T>& obj1)
          {
               // the first position from which ptr_in_list updates are no longer required
               typename std::forward_list < T* >::iterator start_correct = this->begin();
               if (start_correct != this->end()) {
                    ++start_correct;
               }
               fl->splice_after(this->before_begin(), *(obj1.getlist()));
               listsize += obj1.size();
               // update ptr_in_list of elements
               typename std::forward_list < T* >::iterator prev_it = this->before_begin();
               for (auto it = this->begin(); it != start_correct; ++it) {
                    T* o = *it;
                    o->ptr_in_list = prev_it;
                    prev_it = it;
               }
               obj1.constant_clear();
          }

          size_t size()
          {
               return listsize;
          }
     
          T* front()
          {
               return fl->front();
          }
     
          T* back()
          {
               return *last;
          }

          // clear the list in constant time
          void constant_clear() {
               std::forward_list < T* > empty;
               fl->swap(empty);
               listsize = 0;
               last = fl->before_begin();
          }

     private:
          std::forward_list < T* >* fl;
          size_t listsize;
          // pointer to last element
          typename std::forward_list < T* >::iterator last;
};

struct counter_T
{
     // the counter
     size_t cnt;
     // the number of references to the counter
     //size_t refs = 0;
     counter_T(): cnt(0) {}
};

struct state_T
{
public:
     // ID (for debugging)
     state_type id;
     // block B' containing state s
     block_T *block;
     // static list of transitions from s (s -> s')
     std::vector <transition_T*>* Ttgt;
     // static list of transitions to s (s <- s')
     std::vector <transition_T*>* Tsrc;
     // number of inert transitions from s to a state in B'
     size_t inert_cnt;
     // priority used for priority queue in detect2 when splitting
     size_t priority;
     // reference to counter of number of transitions from B' to block B in constellation C (splitting is done under C)
     counter_T *constln_cnt;
     // reference to counter of number of transitions from B' to constellation C without block B (splitting is done under C)
     counter_T *coconstln_cnt;

     // ADDITIONAL INFO to keep track of where the state is listed into which list (pointers point to positions preceding the ones for the state)
     statemode_type type;
     // typename
     std::forward_list<state_T*>::iterator ptr_in_list;
     // is the state in stack of detect1?
     bool is_in_L_detect1;
     // is the state in priority queue of detect2?
     bool is_in_P_detect2;
     // iterator pointing to the position of the state in P
     // typename
     std::multiset<state_T*>::iterator pos_in_P_detect2;
     // is the state in Lp of detect2?
     bool is_in_Lp_detect2;
     
     // constructor
     state_T(size_t& state_index)
      : id(state_index++), 
        block(NULL), 
        inert_cnt(0), 
        priority(0),  
        constln_cnt(NULL), 
        coconstln_cnt(NULL), 
        type(STATE), 
        is_in_L_detect1(false), 
        is_in_P_detect2(false), 
        is_in_Lp_detect2(false) 
     { 
       Ttgt = new std::vector <transition_T*>; 
       Tsrc = new std::vector <transition_T*>;
     }

     // destructor
     ~state_T() 
     {
       deleteobject(Ttgt); 
       deleteobject(Tsrc);
     }
};

struct transition_T
{
     // Struct containing information about transition s->s'. Let B be block containing s, C constellation containing s'
     // source of transition (s)
     state_T *source;
     // target of transition (s')
     state_T *target;
     // pointer to constellation counter for corresponding (s, C) combination
     counter_T *to_constln_cnt;
     // pointer to list of transitions from B to C containing this transition.
     // Different from pseudo-code: redundant, since C entry (below) has a reference to this list as well
     //sized_forward_list < transition_T > *block_constln_list = NULL;
     // pointer to C entry in to_constlns list of B
     to_constlns_element_T *to_constln_ref;
     
     // ADDITIONAL INFO to keep track of where the transition is listed in a (block)
     // constln_transitions list
     // typename
     std::forward_list<transition_T* >::iterator ptr_in_list;
     
     // constructor
     transition_T(): source(NULL), target(NULL), to_constln_cnt(NULL), to_constln_ref(NULL) {}
};

struct constellation_T
{
     // id
     size_t id;
     // size in number of states
     size_t size;
     // list of blocks
     sized_forward_list < block_T >* blocks;
     // type of constellation
     size_t type;
     
     // ADDITIONAL INFO to keep track of where the constellation is listed
     // typename
     std::forward_list<constellation_T* >::iterator ptr_in_list;
     
     // constructor
     constellation_T(size_t& max_const_index)
      : id(max_const_index++), 
        size(0), 
        type(TRIVIAL) 
     { 
       blocks = new sized_forward_list < block_T >;
     }
     // destructor
     ~constellation_T() {deleteobject(blocks);}
};

struct to_constlns_element_T
{
     constellation_T *C;
     // !!! Not present in pseudo-code: a pointer to the list of transitions from block containing this element (in to_constlns list) to constellation C,
     // EXCLUDING the inert transitions, i.e., the transitions must leave the source block;
     // This is needed to efficiently find these transitions when preparing for lockstep search detect1 when stabilising blocks
     sized_forward_list < transition_T > *trans_list;
     // !!! Different from pseudo-code: nr_trans_block_to_C is not needed: trans_list points to the list of transitions, and its size equals nr_trans_block_to_C
     //size_t nr_trans_block_to_C = 0;
     struct to_constlns_element_T *new_element;
     // A list S_C for constellation C
     sized_forward_list < state_T >* SClist;

     // ADDITIONAL INFO to keep track of where the element is listed in a (block)
     // to_constlns list
     // typename
     std::forward_list<to_constlns_element_T* >::iterator ptr_in_list;
     
     // constructor
     to_constlns_element_T(constellation_T *CC): C(CC), new_element(NULL), SClist(NULL) {trans_list = new sized_forward_list < transition_T >;}
     // destructor
     ~to_constlns_element_T() {delete(trans_list);}
};

struct block_T
{
     // ID of block
     block_type id;
     // type of block
     block_type type;
     // constellation C containing B
     constellation_T *constellation;
     // list of bottom states
     sized_forward_list < state_T >* btm_states;
     // list of non-bottom states
     sized_forward_list < state_T >* non_btm_states;
     // list of constellations reachable from block
     sized_forward_list < to_constlns_element_T >* to_constlns;
     // marked bottom states
     sized_forward_list < state_T >* marked_btm_states;
     // marked non-bottom states
     sized_forward_list < state_T >* marked_non_btm_states;
     // different from pseudo-code: instead of list of transitions from state in block to constellation on which splitting is done, we have a pointer to a to_constlns object containing such a list
     to_constlns_element_T* constln_ref;
     // different from pseudo-code: as above, now for list of transitions from state in block to (constellation \ block) on which splitting is done
     to_constlns_element_T* coconstln_ref;
     // different from pseudo-code: this pointer is used to find in constant time the element in to_constlns that belongs to the constellation
     // of which the current block is a member. This is needed to allow inert transitions when becoming non-inert to be added to the associated
     // trans_list in constant time.
     to_constlns_element_T* inconstln_ref;
     // list of new bottom states
     sized_forward_list < state_T >* new_btm_states;
     
     // ADDITIONAL INFO to keep track of where the block is listed in a constellation.blocks
     // list
     // typename
     std::forward_list<block_T*>::iterator ptr_in_list;
     
     // constructor
     block_T(): constellation(NULL), constln_ref(NULL), coconstln_ref(NULL), inconstln_ref(NULL) {btm_states = new sized_forward_list < state_T >; non_btm_states = new sized_forward_list < state_T >; to_constlns = new sized_forward_list < to_constlns_element_T >; marked_btm_states = new sized_forward_list < state_T >; marked_non_btm_states = new sized_forward_list < state_T >; new_btm_states = new sized_forward_list < state_T >;}
     // destructor
     ~block_T() {deleteobject(btm_states); deleteobject(non_btm_states); deleteobject(to_constlns); deleteobject(marked_btm_states); deleteobject(marked_non_btm_states); deleteobject(new_btm_states);}
};

template < class LTS_TYPE>
class bisim_partitioner_gw
{
  // Local class variables

  size_t max_block_index;
  size_t max_const_index;
  size_t state_index;

// begin auxiliary functions

// create a new block of given type
block_T* create_new_block(block_type t)
{
     block_T* b_new = new block_T;
     b_new->type = t;
     if (b_new->type == STD_BLOCK) {
          b_new->id = max_block_index++;
     }
     else {
          b_new->id = -1;
     }
     return b_new;
}

// move state to block
void move_state_to_block(state_T* s, block_T* B)
{
     switch (s->type) {
          case BTM_STATE:
               s->block->btm_states->remove_linked(s);
               B->btm_states->insert_linked(s);
               break;
          case NON_BTM_STATE:
               s->block->non_btm_states->remove_linked(s);
               B->non_btm_states->insert_linked(s);
               break;
          case MARKED_BTM_STATE:
               s->block->marked_btm_states->remove_linked(s);
               B->marked_btm_states->insert_linked(s);
               break;
          case MARKED_NON_BTM_STATE:
               s->block->marked_non_btm_states->remove_linked(s);
               B->marked_non_btm_states->insert_linked(s);
     }
     s->block = B;
}

// function to check the size of a to_constln_element_T object. This size is determined by the size of the
// associated trans_list. The function can handle NULL pointers.
size_t size(to_constlns_element_T* l)
{
     if (l == NULL) {
          return 0;
     }
     else {
          return l->trans_list->size();
     }
}

// iterator functions to iterate over all unmarked states of a given block
bool iterating_non_bottom;
bool iterating_non_bottom2;

std::forward_list < state_T* >::iterator unmarked_states_begin(block_T* B)
{
     if (B->btm_states->size() > 0) {
          iterating_non_bottom2 = false;
          return B->btm_states->begin();
     }
     else {
          iterating_non_bottom2 = true;
          return B->non_btm_states->begin();
     }
}

std::forward_list < state_T* >::iterator unmarked_states_end(block_T* B)
{
     return B->non_btm_states->end();
}

std::forward_list < state_T* >::iterator unmarked_states_next(block_T* B, std::forward_list < state_T* >::iterator it)
{
     auto tmpit = it;
     ++tmpit;
     if (!iterating_non_bottom2 && tmpit == B->btm_states->end()) {
          iterating_non_bottom2 = true;
          tmpit = B->non_btm_states->begin();
     }
     return tmpit;
}

// iterator functions to iterate over all marked states of a given block
std::forward_list < state_T* >::iterator marked_states_begin(block_T* B)
{
     if (B->marked_btm_states->size() > 0) {
          iterating_non_bottom2 = false;
          return B->marked_btm_states->begin();
     }
     else {
          iterating_non_bottom2 = true;
          return B->marked_non_btm_states->begin();
     }
}

std::forward_list < state_T* >::iterator marked_states_end(block_T* B)
{
     return B->marked_non_btm_states->end();
}

std::forward_list < state_T* >::iterator marked_states_next(block_T* B, std::forward_list < state_T* >::iterator it)
{
     auto tmpit = it;
     ++tmpit;
     if (!iterating_non_bottom2 && tmpit == B->marked_btm_states->end()) {
          iterating_non_bottom2 = true;
          tmpit = B->marked_non_btm_states->begin();
     }
     return tmpit;
}

// unmark all marked states in the given block, reset s->constln_cnt and s->coconstln_cnt for all s in B, and reset
// B->constln_ref and B->coconstln_ref if they point to a counter with value 0
void clean_temp_refs_block(block_T* B)
{
     if (B != NULL) {
          //mCRL2log(log::verbose) << "CLEANING " << B->id << "\n";
          for (auto sit = B->marked_btm_states->begin(); sit != B->marked_btm_states->end(); ++sit) {
               state_T* s = *sit;
               s->constln_cnt = NULL;
               s->coconstln_cnt = NULL;
               B->btm_states->insert_state_linked(s, BTM_STATE);
          }
          for (auto sit = B->marked_non_btm_states->begin(); sit != B->marked_non_btm_states->end(); ++sit) {
               state_T* s = *sit;
               s->constln_cnt = NULL;
               if (s->coconstln_cnt != NULL) {
                    if (s->coconstln_cnt->cnt == 0) {
                         deleteobject (s->coconstln_cnt);
                    }
               }
               s->coconstln_cnt = NULL;
               B->non_btm_states->insert_state_linked(s, NON_BTM_STATE);
          }
          B->marked_btm_states->constant_clear();
          B->marked_non_btm_states->constant_clear();
          if (B->constln_ref != NULL) {
               if (size(B->constln_ref) == 0) {
                    // if the associated constellation is the one containing B, set inconstln_ref to NULL
                    if (B->constln_ref->C == B->constellation) {
                         B->inconstln_ref = NULL;
                    }
                    B->to_constlns->remove_linked(B->constln_ref);
                    deleteobject (B->constln_ref);
               }
               B->constln_ref = NULL;
          }
          if (B->coconstln_ref != NULL) {
               if (size(B->coconstln_ref) == 0) {
                    if (B->coconstln_ref->C == B->constellation) {
                         B->inconstln_ref = NULL;
                    }
                    B->to_constlns->remove_linked(B->coconstln_ref);
                    deleteobject (B->coconstln_ref);
               }
               B->coconstln_ref = NULL;
          }
     }
}

// end auxiliary functions


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
                         const bool branching=false)
     : max_block_index(0),
       max_const_index(0),
       state_index(0),
       aut(l), 
       tau_label(determine_tau_label(l))
    {
               // initialise variables
               nr_of_splits = 0;
               Q = NULL;
               current_state_detect1 = NULL;
               P = NULL;
               current_state_detect2 = NULL;
               in_forward_check_detect2 = false;
          
      assert(branching);
      mCRL2log(log::verbose) << "O(m log n) " <<
                  (branching?"branching ":"") << "bisimulation partitioner created for "
                  << l.num_states() << " states and " <<
                  l.num_transitions() << " transitions\n";
      create_initial_partition_gw(branching);
      refine_partition_until_it_becomes_stable_gw(branching);
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
    void replace_transitions(const bool branching)
    {
      // Put all the non inert transitions in a set. A set is used to remove double occurrences of transitions.
      std::set < transition > resulting_transitions;

      const std::vector<transition> & trans=aut.get_transitions();
      for (std::vector<transition>::const_iterator t=trans.begin(); t!=trans.end(); ++t)
      {
        const transition i=*t;
        if (!branching ||
            !aut.is_tau(i.label()) ||
            get_eq_class(i.from())!=get_eq_class(i.to()))
        {
          resulting_transitions.insert(
            transition(
              get_eq_class(i.from()),
              i.label(),
              get_eq_class(i.to())));
        }
      }
      // Remove the old transitions
      aut.clear_transitions();

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
      return states[s]->block->id;
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
          size_t nr_of_splits;
     
          LTS_TYPE& aut;

    // current partition
    //sized_forward_list < block_T* > pi;
    // trivial and non-trivial constellations
    sized_forward_list < constellation_T > non_trivial_constlns;
    sized_forward_list < constellation_T > trivial_constlns;
          // the number of states
          size_t nr_of_states;
          // the list of states
          std::vector < state_T* > states;
     
          const label_type tau_label;

    std::string keypair_to_string(std::string action, state_type to)
    {
      return action + "_" + std::to_string((long long int) to);
    }
     
          // key and hash function for (action, target state) pair. Required since unordered_map does not
          // directly allow to use pair keys
          struct Key {
               label_type first;
               state_type second;

               bool operator==(const Key &other) const {
                    return (first == other.first
            && second == other.second);
               }
          };

          struct KeyHasher {
               std::size_t operator()(const Key& k) const {
                    using std::size_t;
                    using std::hash;
                    //using std::string;

                    return (hash<label_type>()(k.first)
             ^ (hash<state_type>()(k.second) << 1));
               }
          };

          // start structures and functions for lockstep search

          // A Comparator class to compare states in the priority queue
          struct LessThanByInert
          {
               bool operator()(const state_T* lhs, const state_T* rhs) const {
                    return lhs->priority < rhs->priority;
               }
          };

          // detect1
          std::stack < state_T* >* Q;
          sized_forward_list < state_T > L;
          // to keep track of state of detect 1
          state_T *current_state_detect1;
          typename std::vector<transition_T*>::iterator current_trans_detect1;
          // to keep track of adding states to detect1
          typename std::forward_list<state_T*>::iterator iter_state_added_detect1;
          typename std::forward_list<transition_T*>::iterator iter_trans_state_added_detect1;
          // detect2
          //std::priority_queue < state_T*, std::vector< state_T* >, LessThanByInert>* P;
          std::multiset< state_T*, LessThanByInert>* P;
          sized_forward_list < state_T > Lp;
          // to keep track of state of detect 2
          state_T *current_state_detect2;
          typename std::vector<transition_T*>::iterator current_trans_detect2;
          // to keep track of adding states to detect 2
          typename std::forward_list<state_T*>::iterator iter_state_added_detect2;
          typename std::forward_list<state_T*>::iterator iter_sclist_detect2;
          bool sclist_is_empty_detect2;
          // required for forward check in detect2 in nested split
          bool in_forward_check_detect2;
          // required in both procedures
          block_T* block_detect;
          to_constlns_element_T* e_detect;
          size_t maxsize_detect;
          // old constellation of the splitter
          constellation_T* constellation_splitter_detect;
     
          // next_state functions to add states to detect1 and detect2 in the various phases of the algorithm
          // (1: splitting Bp, 2: splitting split(Bp, B), 3: stabilising blocks)
          // precondition: iter_state_added_detectx have been set to before the beginning of the relevant state lists
          inline bool has_next_state_detect1_1() {
               auto next_it = iter_state_added_detect1;
               next_it++;
               if (next_it == block_detect->marked_btm_states->end() && !iterating_non_bottom) {
                    next_it = block_detect->marked_non_btm_states->begin();
                    //mCRL2log(log::verbose) << "non bottom\n";
               }
               //mCRL2log(log::verbose) << (next_it != block_detect->marked_non_btm_states->end()) << "number of non bottom states: " << block_detect->marked_non_btm_states->size() << "\n";
               return next_it != block_detect->marked_non_btm_states->end();
           }

          inline state_T* next_state_detect1_1() {
               iter_state_added_detect1++;
               //mCRL2log(log::verbose) << "get next state\n";
               if (iter_state_added_detect1 == block_detect->marked_btm_states->end() && !iterating_non_bottom) {
                    iterating_non_bottom = true;
                    iter_state_added_detect1 = block_detect->marked_non_btm_states->begin();
               }
               // is the state suitable?
               state_T* s = *iter_state_added_detect1;
               if (!s->is_in_L_detect1) {
               //mCRL2log(log::verbose) << "state " << s->id << " is not in L\n";
                    return s;
               }
               else {
                    //mCRL2log(log::verbose) << "state " << s->id << " is in L\n";
                    return NULL;
               }
          }

          inline bool has_next_state_detect2_1() {
               auto next_it = iter_state_added_detect2;
               next_it++;
               return next_it != block_detect->btm_states->end();
           }

          inline state_T* next_state_detect2_1() {
               iter_state_added_detect2++;
               // Note: no need to check for membership in P. Current state is a bottom state, so cannot have been reached
               // via an incoming transition
               return *iter_state_added_detect2;
          }
     
          inline bool has_next_state_detect1_2() {
               auto next_it = iter_trans_state_added_detect1;
               next_it++;
               return next_it != block_detect->coconstln_ref->trans_list->end();
          }

          inline state_T* next_state_detect1_2() {
               iter_trans_state_added_detect1++;
               transition_T* t = *iter_trans_state_added_detect1;
               state_T* s = t->source;
               if (!s->is_in_L_detect1) {
                    //mCRL2log(log::verbose) << "suitable trans from " << s->id << " to " << t->target->id << " (" << t->target->block->constellation << ")\n";
                    return s;
               }
               else {
                    return NULL;
               }
          }

          inline bool has_next_state_detect2_2() {
               auto next_it = iter_state_added_detect2;
               next_it++;
               return next_it != block_detect->marked_btm_states->end();
          }

          inline state_T* next_state_detect2_2() {
               iter_state_added_detect2++;
               state_T* s = *iter_state_added_detect2;
               if (s->coconstln_cnt == NULL) {
                    //mCRL2log(log::verbose) << s->id << ": NULL counter\n";
                    return s;
               }
               else if (s->coconstln_cnt->cnt == 0) {
                    //mCRL2log(log::verbose) << s->id << ": 0 counter\n";
                    return s;
               }
               else {
                    return NULL;
               }
          }

          inline bool has_next_state_detect1_3() {
               auto next_it = iter_trans_state_added_detect1;
               next_it++;
               return next_it != e_detect->trans_list->end();
          }
     
          inline state_T* next_state_detect1_3() {
               iter_trans_state_added_detect1++;
               transition_T* t = *iter_trans_state_added_detect1;
               state_T* s = t->source;
               if (!s->is_in_L_detect1) {
                    return s;
               }
               else {
                    return NULL;
               }
          }

          inline bool has_next_state_detect2_3() {
               auto next_it = iter_state_added_detect2;
               next_it++;
               return next_it != block_detect->new_btm_states->end();
          }
     
          // compare SClist and new_btm_states. This depends on new bottom states having been added to
          // back of SClist in order of appearance in new_btm_states
          inline state_T* next_state_detect2_3() {
               iter_state_added_detect2++;
               state_T* s = *iter_state_added_detect2;
               if (sclist_is_empty_detect2) {
                    return s;
               }
               else if (iter_sclist_detect2 == e_detect->SClist->end()) {
                    return s;
               }
               else if (s != *iter_sclist_detect2) {
                    return s;
               }
               else {
                    iter_sclist_detect2++;
                    return NULL;
               }
          }

          // step in detect1
          // returns false if not finished, true otherwise
          // function hasnext indicates whether there is at least one more state to be processed
          // function next provides new states to be considered in the search
          inline size_t detect1_step(bool (bisim_partitioner_gw<LTS_TYPE>::*hasnext)(), state_T* (bisim_partitioner_gw<LTS_TYPE>::*next)()) {
               // stop if upperbound to size has been reached
               if (L.size() > maxsize_detect) {
                    //mCRL2log(log::verbose) << "detect1: STOPPED (" << L.size() << "," << maxsize_detect << ")\n";
                    return STOPPED;
               }
               if (Q->size() > 0 || (this->*hasnext)() || current_state_detect1 != NULL) {
                    if (current_state_detect1 == NULL) {
                         if (Q->size() > 0) {
                              current_state_detect1 = Q->top();
                              Q->pop();
                         }
                         else {
                              // there must still be a state obtainable through next
                              current_state_detect1 = (this->*next)();
                              if (current_state_detect1 == NULL) {
                                   //mCRL2log(log::verbose) << "detect1: end\n";
                                   // skip this state in the current step
                                   return CONTINUE;
                              }
                              else {
                                   L.insert(current_state_detect1);
                                   current_state_detect1->is_in_L_detect1 = true;
                                   //mCRL2log(log::verbose) << "detect1 base: adding state " << current_state_detect1->id << "\n";
                              }
                         }
                         //mCRL2log(log::verbose) << "detect1: processing state " << current_state_detect1->id << "\n";
                         current_trans_detect1 = current_state_detect1->Tsrc->begin();
                         // no incoming transitions present
                         if (current_trans_detect1 == current_state_detect1->Tsrc->end()) {
                              current_state_detect1 = NULL;
                              //mCRL2log(log::verbose) << "detect1: end2\n";
                              return CONTINUE;
                         }
                    }
                    const transition_T* t = *current_trans_detect1;
                    state_T* sp = t->source;
                         
                    if (sp->block == block_detect && !sp->is_in_L_detect1) {
                         L.insert(sp);
                         sp->is_in_L_detect1 = true;
                         //mCRL2log(log::verbose) << "detect1: adding state " << sp->id << "\n";
                         Q->push(sp);
                    }
                    // increment transition counter
                    current_trans_detect1++;
                    if (current_trans_detect1 == current_state_detect1->Tsrc->end()) {
                         current_state_detect1 = NULL;
                         //mCRL2log(log::verbose) << "detect1: end3\n";
                    }
                    //mCRL2log(log::verbose) << "detect1: continue\n";
                    return CONTINUE;
               }
               else {
                    //mCRL2log(log::verbose) << "detect1: finished\n";
                    return TERMINATED;
               }
          }

          // step in detect2
          // returns false if not finished, true otherwise
          // isnestedsplit indicates whether a block B' is being split, or a block split(B',B).
          inline size_t detect2_step(bool (bisim_partitioner_gw<LTS_TYPE>::*hasnext)(), state_T* (bisim_partitioner_gw<LTS_TYPE>::*next)(), bool isnestedsplit, bool forwardcheckrequired) {
               bool sp_newly_inserted;
               // this part of the procedure is performed when it needs to be determined whether a state has
               // a direct outgoing transition to Bp \ B.
               if (in_forward_check_detect2) {
                    if (current_trans_detect2 == current_state_detect2->Ttgt->end()) {
                         // no direct transition to Bp \ B found. State is suitable for detect2
                         in_forward_check_detect2 = false;
                         Lp.insert(current_state_detect2);
                         current_state_detect2->is_in_Lp_detect2 = true;
                         //mCRL2log(log::verbose) << "detect2: adding state " << current_state_detect2->id << "\n";
                         current_trans_detect2 = current_state_detect2->Tsrc->begin();
                         // no incoming transitions present
                         if (current_trans_detect2 == current_state_detect2->Tsrc->end()) {
                              current_state_detect2 = NULL;
                         }
                    }
                    else {
                         transition_T* t = *current_trans_detect2;
                         //mCRL2log(log::verbose) << "compare " << t->target->block->constellation->id << " " << constellation_splitter_detect->id << "\n";
                         if (t->target->block->constellation == constellation_splitter_detect && t->source->block != t->target->block) {
                              // transition found. state is not suitable for detect2
                              in_forward_check_detect2 = false;
                              current_state_detect2 = NULL;
                         }
                         else {
                              // go to next transition
                              current_trans_detect2++;
                         }
                    }
                    return CONTINUE;
               }
               // stop if upperbound to size has been reached
               if (Lp.size() > maxsize_detect) {
                    //mCRL2log(log::verbose) << "detect2: STOPPED (" << Lp.size() << "," << maxsize_detect << ")\n";
                    return STOPPED;
               }
               if (P->size() > 0 || (this->*hasnext)() || current_state_detect2 != NULL) {
                    if (current_state_detect2 == NULL) {
                         if ((this->*hasnext)()) {
                              current_state_detect2 = (this->*next)();
                              // check if state is suitable
                              if (current_state_detect2 == NULL) {
                                   return CONTINUE;
                              }
                              //mCRL2log(log::verbose) << "detect2: processing state " << current_state_detect2->id << "\n";
                         }
                         else {
                              auto Pit = P->begin();
                              current_state_detect2 = *Pit;
                              P->erase(Pit);
                              current_state_detect2->is_in_P_detect2 = false;
                              //mCRL2log(log::verbose) << "detect2: from queue: processing state " << current_state_detect2->id << " with priority " << current_state_detect2->priority << "\n";
                              // check if this element still has priority 0
                              if (current_state_detect2->priority != 0) {
                                   //mCRL2log(log::verbose) << "detect2: finished\n";
                                   return TERMINATED;
                              }
                              else if (forwardcheckrequired) {
                                   // skip if state has been reached by detect1
                                   if (current_state_detect2->is_in_L_detect1) {
                                        current_state_detect2 = NULL;
                                   }
                                   // else check if state can reach Bp \ B directly
                                   else {
                                        in_forward_check_detect2 = true;
                                        current_trans_detect2 = current_state_detect2->Ttgt->begin();
                                   }
                                   return CONTINUE;
                              }
                         }
                         current_trans_detect2 = current_state_detect2->Tsrc->begin();
                         Lp.insert(current_state_detect2);
                         current_state_detect2->is_in_Lp_detect2 = true;
                         //mCRL2log(log::verbose) << "detect2 base: adding state " << current_state_detect2->id << "\n";
                         // no incoming transitions present
                         if (current_trans_detect2 == current_state_detect2->Tsrc->end()) {
                              current_state_detect2 = NULL;
                              return CONTINUE;
                         }
                    }
                    const transition_T* t = *current_trans_detect2;
                    state_T* sp = t->source;
                    sp_newly_inserted = false;
                    //mCRL2log(log::verbose) << "detect2: walking back to state " << sp->id << "\n";
                    //mCRL2log(log::verbose) << (sp->block == block_detect) << " " << sp->is_in_P_detect2 << " " << sp->is_in_Lp_detect2 << "\n";
                    if (sp->block == block_detect && !sp->is_in_P_detect2 && !sp->is_in_Lp_detect2) {
                         // depending on isnestedsplit, check condition
                         // Different from pseudo-code: for the nested split, we check whether the state is a member of L.
                         // This seems dangerous, since L is maintained by detect1, but we are interested in the states that
                         // reach Bp \ B, and it is problematic to determine which states can do so directly. Some states
                         // in L are likely to reach Bp \ B directly (at least indirectly), so a state in L should be
                         // ignored. Another condition involves the case that a state is marked: if it is, then
                         // coconstln_cnt has been initialised. If it is higher than 0, the state can reach Bp \ B
                         // directly.
                         bool skip = false;
                         //mCRL2log(log::verbose) << "checking\n";
                         if (isnestedsplit && sp->type == MARKED_NON_BTM_STATE) {
                              //mCRL2log(log::verbose) << "here " << "\n";
                              //if (sp->coconstln_cnt != NULL) {
                              //     mCRL2log(log::verbose) << "count: " << sp->coconstln_cnt->cnt << "\n";
                              //}
                              //if (sp->coconstln_cnt == NULL) {
                              //     skip = true;
                              //}
                              // Take into account that inert transitions are counted as well (they should be ignored here)
                              // Note that the case that sp is in the splitter is handled correctly, since the splitter has already
                              // been moved to a new constellation, which therefore cannot be equal to its old one.
                              // (inert transitions are counted by constln_cnt, hence should not be taken into account here).
                              if (block_detect->constellation == constellation_splitter_detect) {
                                   if (sp->coconstln_cnt->cnt - sp->inert_cnt > 0) {
                                        skip = true;
                                   }
                              }
                              else if (sp->coconstln_cnt->cnt > 0) {
                                   skip = true;
                              }
                         }
                         if (!skip) {
                              if (forwardcheckrequired ? (!sp->is_in_L_detect1) : (sp->type != MARKED_NON_BTM_STATE)) {
                                   sp->priority = sp->inert_cnt;
                                   if (sp->priority > 0) {
                                        sp->priority--;
                                   }
                                   //mCRL2log(log::verbose) << "detect2: pushing " << sp->id << " on P with prio " << sp->priority << "\n";
                                   sp->pos_in_P_detect2 = P->insert(sp);
                                   //mCRL2log(log::verbose) << "begin: " << (*(P->begin()))->id << "\n";
                                   sp->is_in_P_detect2 = true;
                                   sp_newly_inserted = true;
                              }
                         }
                    }
                    // Different from pseudo-code: priority of sp is only decremented if sp is in P.
                    if (sp->is_in_P_detect2 && !sp_newly_inserted) {
                         if (sp->priority > 0) {
                              sp->priority--;
                              // replace element in P
                              //mCRL2log(log::verbose) << "updating state " << sp->id << "\n";
                              P->erase(sp->pos_in_P_detect2);
                              sp->pos_in_P_detect2 = P->insert(sp);
                              //mCRL2log(log::verbose) << "detect2: new prio of " << sp->id << " is " << sp->priority << "\n";
                         }
                    }
                    // increment transition counter
                    current_trans_detect2++;
                    if (current_trans_detect2 == current_state_detect2->Tsrc->end()) {
                         current_state_detect2 = NULL;
                    }
                    return CONTINUE;
               }
               else {
                    //mCRL2log(log::verbose) << "detect2: finished\n";
                    return TERMINATED;
               }
          }

          // end structures and functions for lockstep search

          // method to print current partition
          void print_partition ()
          {
               for (auto cit = non_trivial_constlns.begin(); cit != non_trivial_constlns.end(); ++cit) {
                    constellation_T* C = *cit;
                    for (auto bit = C->blocks->begin(); bit != C->blocks->end(); ++bit) {
                         block_T* B = *bit;
                         mCRL2log(log::verbose) << "[";
                         for (auto sit = unmarked_states_begin(B); sit != unmarked_states_end(B); sit = unmarked_states_next(B, sit)) {
                              state_T* s = *sit;
                              mCRL2log(log::verbose) << s->id << ",";
                         }
                         for (auto sit = marked_states_begin(B); sit != marked_states_end(B); sit = marked_states_next(B, sit)) {
                              state_T* s = *sit;
                              mCRL2log(log::verbose) << "*" << s->id << "*,";
                         }
                         mCRL2log(log::verbose) << "], ";
                    }
               }
               mCRL2log(log::verbose) << "| ";
               for (auto cit = trivial_constlns.begin(); cit != trivial_constlns.end(); ++cit) {
                    constellation_T* C = *cit;
                    for (auto bit = C->blocks->begin(); bit != C->blocks->end(); ++bit) {
                         block_T* B = *bit;
                         mCRL2log(log::verbose) << "[";
                         for (auto sit = unmarked_states_begin(B); sit != unmarked_states_end(B); sit = unmarked_states_next(B, sit)) {
                              state_T* s = *sit;
                              mCRL2log(log::verbose) << s->id << ",";
                         }
                         for (auto sit = marked_states_begin(B); sit != marked_states_end(B); sit = marked_states_next(B, sit)) {
                              state_T* s = *sit;
                              mCRL2log(log::verbose) << "*" << s->id << "*,";
                         }
                         mCRL2log(log::verbose) << "], ";
                    }
               }
               mCRL2log(log::verbose) << "\n";
          }

    void create_initial_partition_gw(const bool branching)

    {
      using namespace std;

               // number of blocks
               block_type nr_of_blocks;
               // the list of initial blocks (for fast access)
               vector < block_T* > blocks;
               // the number of states
               nr_of_states = aut.num_states();
               // original size of input
               size_t orig_nr_of_states = nr_of_states;
      // temporary map to keep track of states to be added when converting LTS to Kripke structure
      unordered_map < Key, state_type, KeyHasher > extra_kripke_states;
               // temporary map to keep track of blocks (maps transition labels (unequal to tau) to blocks
               unordered_map < label_type, block_type > action_block_map;
               // temporary list to keep track of lists of transitions, one for each block
               //vector < sized_forward_list < transition_T* >* > block_trans_list;

               // create single initial non-trivial constellation
               constellation_T* C = new constellation_T(max_const_index);
               
               // create first block in C
               block_T* B1 = create_new_block(STD_BLOCK);
               // create associated list of transitions from block to constellation C
               to_constlns_element_T* e = new to_constlns_element_T(C);
               B1->to_constlns->insert_linked(e);
               B1->inconstln_ref = e;
               B1->constellation = C;
               C->blocks->insert_linked(B1);
               // add block to initial list of blocks
               blocks.insert(blocks.end(), B1);
               nr_of_blocks = 1;

      // iterate over the transitions and collect new states
      aut.sort_transitions(mcrl2::lts::src_lbl_tgt);
      const std::vector<transition> &trans = aut.get_transitions();
               for (auto r=trans.begin(); r != trans.end(); ++r)
      {
        const transition t = *r;
                    
        if (!aut.is_tau(t.label()) || !branching)
        {
                         // create new state
                         Key* k = new(Key);
                         k->first = t.label();
                         k->second = t.to();
          if ((extra_kripke_states.insert(make_pair(*k, nr_of_states))).second) {
                              nr_of_states++;
                         }
                         // (possibly) create new block
                         if (action_block_map.insert(make_pair(t.label(), nr_of_blocks)).second) {
                              nr_of_blocks++;
                         }
        }
      }
               mCRL2log(log::verbose) << "number of extra states: " << extra_kripke_states.size() << "\n";
               // knowing the number of blocks,
               // create blocks, and add one entry in their to_constlns list for the single constellation C
               // each of these new blocks will contain extra Kripke states, therefore we increment nr_of_extra_kripke_blocks
               // each time we create a block
               for (size_t i = 0; i < nr_of_blocks-1; i++) {
                    block_T* B = create_new_block(EXTRA_KRIPKE_BLOCK);
                    to_constlns_element_T* e = new to_constlns_element_T(C);
                    B->to_constlns->insert_linked(e);
                    B->inconstln_ref = e;
                    // add block to constellation
                    B->constellation = C;
                    C->blocks->insert_linked(B);
                    // add block to initial list of blocks
                    blocks.insert(blocks.end(), B);
               }

               // create state entries in states list
      for (size_t i = 0; i < nr_of_states; i++)
      {
        state_T* s = new state_T(state_index);
        this->states.insert(this->states.end(), s);
                    // add state to first block, if applicable
                    if (i < orig_nr_of_states) {
                         s->block = B1;
                    }
      }
               // add the new states to their respective blocks
               for (auto it=extra_kripke_states.begin(); it != extra_kripke_states.end(); ++it) {
                    (states[it->second])->block = blocks[(action_block_map.find((it->first).first))->second];
               }
               // add transitions
               state_type current_src_state = -1;
               counter_T* counter;
      for (auto r=trans.begin(); r != trans.end(); ++r)
      {
        const transition t = *r;
                    
                    // if we see a new source state, create a new counter for it
                    if (t.from() != current_src_state) {
                         current_src_state = t.from();
                         counter = new counter_T;
                    }
                    // create transition entry
                    transition_T* t_entry = new transition_T;
                    // fill in info
                    t_entry->source = states[t.from()];
                    // target depends on transition label
                    if (aut.is_tau(t.label()) && branching) {
                         t_entry->target = states[t.to()];
                         // initially, all tau-transitions are inert
                         // number of inert transitions of source needs to be incremented
                         states[t.from()]->inert_cnt++;
                    }
                    else {
                         Key* k = new Key;
                         k->first = t.label();
                         k->second = t.to();
                         t_entry->target = states[(extra_kripke_states.find(*k))->second];
                    }
                    // connect transition to its states
                    t_entry->target->Tsrc->insert(t_entry->target->Tsrc->end(), t_entry);
                    t_entry->source->Ttgt->insert(t_entry->source->Ttgt->end(), t_entry);
                    // add pointer to counter
                    t_entry->to_constln_cnt = counter;
                    // increment the counter
                    t_entry->to_constln_cnt->cnt++;
                    // Different from pseudo-code: ONLY if transition is non-inert
                    if (!aut.is_tau(t.label()) || !branching) {
                         // set pointer to C entry in to_constlns list of B.
                         t_entry->to_constln_ref = t_entry->source->block->to_constlns->front();
                         // add transition to transition list of source block
                         t_entry->to_constln_ref->trans_list->insert_linked(t_entry);
                    }
                    // !!! Not in pseudo-code: refer to block transition list (pointed to by t_entry.block_constln_list) from the C entry
                    //t_entry.to_constln_ref->trans_list = t_entry.block_constln_list;
      }
               // add transitions <a,t> -> t
               for (auto sit = extra_kripke_states.begin(); sit != extra_kripke_states.end(); ++sit) {
                    std::pair<Key, state_type> e = *sit;
                    state_type sid = e.second;
                    state_type tid = e.first.second;

                    transition_T* t_entry2 = new transition_T;
                    // fill in info
                    t_entry2->source = states[sid];
                    t_entry2->target = states[tid];
                    // connect transition to its states
                    t_entry2->target->Tsrc->insert(t_entry2->target->Tsrc->end(), t_entry2);
                    t_entry2->source->Ttgt->insert(t_entry2->source->Ttgt->end(), t_entry2);
                    // add pointer to counter object of source state
                    t_entry2->to_constln_cnt = new counter_T;
                    // increment the counter
                    t_entry2->to_constln_cnt->cnt++;
                    // set pointer to C entry in to_constlns list of B. Increment the associated counter
                    t_entry2->to_constln_ref = t_entry2->source->block->to_constlns->front();
                    // add transition to transition list of source block
                    t_entry2->to_constln_ref->trans_list->insert_linked(t_entry2);
                    // !!! Not in pseudo-code: refer to block transition list (pointed to by t_entry.block_constln_list) from the C entry
                    //t_entry2.to_constln_ref->trans_list = t_entry2.block_constln_list;
               }
               // print the Kripke structure
#ifndef NDEBUG
               for (auto sit = extra_kripke_states.begin(); sit != extra_kripke_states.end(); ++sit) {
                    std::pair<Key, state_type> p = *sit;
                    mCRL2log(log::verbose) << p.second << " (" << p.first.first << "," << p.first.second << ")\n";
               }
               for (auto sit = states.begin(); sit != states.end(); ++sit) {
                    state_T* s = *sit;
                    for (auto tit = s->Ttgt->begin(); tit != s->Ttgt->end(); ++tit) {
                         transition_T* t = *tit;
                         mCRL2log(log::verbose) << t->source->id << " -> " << t->target->id << "\n";
                    }
                    for (auto tit = s->Tsrc->begin(); tit != s->Tsrc->end(); ++tit) {
                         transition_T* t = *tit;
                         mCRL2log(log::verbose) << t->target->id << " <- " << t->source->id << "\n";
                    }
               }
#endif
               
               // Add all states to their appropriate list in the block they reside in
               for (auto it=states.begin(); it != states.end(); ++it) {
                    state_T* s = *it;
                    
                    if (s->inert_cnt == 0) {
                         // state is bottom
                         s->block->btm_states->insert_state_linked(s, BTM_STATE);
                    }
                    else {
                         // state is not bottom
                         s->block->non_btm_states->insert_state_linked(s, NON_BTM_STATE);
                    }
               }
               
               // set size of constellation C
               C->size = nr_of_states;
               // add C to appropriate list
               if (C->blocks->size() > 1) {
                    C->type = NONTRIVIAL;
                    non_trivial_constlns.insert_linked(C);
               }
               else {
                    trivial_constlns.insert_linked(C);
               }
               
               // create a priority queue for use in the algorithm
               P = new std::multiset < state_T*, LessThanByInert>;
          }; // end create_initial_partition

// Refine the partition until the partition has become stable
    void refine_partition_until_it_becomes_stable_gw(const bool branching)
    {
               while (non_trivial_constlns.size() > 0) {
                    // list of splittable blocks
                    sized_forward_list < block_T > splittable_blocks;
#ifndef NDEBUG
                    //print_partition();
                    //check_internal_consistency_of_the_partitioning_data_structure_gw(branching);
                    check_consistency_blocks();
                    check_consistency_transitions();
                    check_non_empty_blocks();
#endif
                    // walk over the constellations
                    bool const_block_found = false;
                    constellation_T* setB;
                    block_T* B;
                    for (auto const_it = non_trivial_constlns.begin(); const_it != non_trivial_constlns.end(); ++const_it) {
                         setB = *const_it;
                         for (auto bit = setB->blocks->begin(); bit != setB->blocks->end(); ++bit) {
                              B = *bit;
                              if (B->btm_states->size()+B->non_btm_states->size() <= (setB->size)/2) {
                                   //mCRL2log(log::verbose) << B->btm_states->size() << " " << B->non_btm_states->size() << " " << B->marked_btm_states->size() << " " << B->marked_non_btm_states->size() << "\n";
                                   //mCRL2log(log::verbose) << "splitter: " << B->id << " " << B->btm_states->front()->id << "\n";
                                   //for (auto it = B->to_constlns->begin(); it != B->to_constlns->end(); ++it) {
                                   //     to_constlns_element_T* l = *it;
                                   //     mCRL2log(log::verbose) << l << " " << l->new_element << " " << l->C->id << "\n";
                                   //}
                                   //mCRL2log(log::verbose) << "inconstln_ref: " << B->inconstln_ref << "\n";
                                   //mCRL2log(log::verbose) << "---\n";
                                   // 5.2.1
                                   // 5.2.1.a
                                   constellation_T* setC = new constellation_T(max_const_index);
                                   setB->blocks->remove_linked(B, bit);
                                   size_t Bsize = B->btm_states->size() + B->non_btm_states->size() + B->marked_btm_states->size() + B->marked_non_btm_states->size();
                                   setB->size -= Bsize;
                                   setC->blocks->insert_linked(B);
                                   setC->size += Bsize;
                                   B->constellation = setC;
                                   //mCRL2log(log::verbose) << "new constln of " << B->id << " is " << setC->id << "\n";
                                   B->inconstln_ref = NULL;
                                   // 5.2.1.b
                                   trivial_constlns.insert_linked(setC);
                                   if (setB->blocks->size() == 1) {
                                        setB->type = TRIVIAL;
                                        non_trivial_constlns.remove_linked(setB, const_it);
                                        trivial_constlns.insert_linked(setB);
                                   }
                                   // 5.2.2 walk through B.btm_states and B.non_btm_states, and check incoming transitions
                                   //check_consistency_trans_lists_2(setB);
                                   for (auto sit = unmarked_states_begin(B); sit != unmarked_states_end(B); sit = unmarked_states_next(B, sit)) {
                                        state_T* s = *sit;
                                        
                                        for (auto tit = s->Tsrc->begin(); tit != s->Tsrc->end(); ++tit) {
                                             transition_T* t = *tit;
                                             state_T* sp = t->source;
                                             block_T* Bp = sp->block;
                                             // Different from pseudo-code: ignore if Bp=B. We will revisit this transition when considering
                                             // the outgoing transitions of each s in B
                                             if (Bp != B) {
                                                  // 5.2.2.a
                                                  if (Bp->marked_btm_states->size() == 0 && Bp->marked_non_btm_states->size() == 0) {
                                                       // 5.2.2.a.i
                                                       splittable_blocks.insert(Bp);
                                                       //if (Bp->id == 955) {
                                                            //mCRL2log(log::verbose) << "ADDING BLOCK " << Bp->id << " with states " << Bp->btm_states->size() << " " << Bp->non_btm_states->size() << " " << Bp->marked_btm_states->size() << " " << Bp->marked_non_btm_states->size() << "\n";
                                                       //}
                                                       // 5.2.2.a.ii
                                                       //mCRL2log(log::verbose) << "ref: " << t->to_constln_ref << "\n";
                                                       Bp->coconstln_ref = t->to_constln_ref;
                                                       Bp->constln_ref = new to_constlns_element_T(setC);
                                                       Bp->to_constlns->insert_linked(Bp->constln_ref);
                                                  }
                                                  // 5.2.2.b
                                                  // Different from pseudo-code: we split the counter into two counters differently, this one respects
                                                  // the pointers to counters of the individual transitions
                                                  if (sp->coconstln_cnt == NULL) {
                                                       sp->constln_cnt = new counter_T;
                                                       sp->coconstln_cnt = t->to_constln_cnt;
                                                  }
                                                  // 5.2.2.c
                                                  if (sp->type != MARKED_BTM_STATE && sp->type != MARKED_NON_BTM_STATE) {
                                                       // 5.2.2.c.i
                                                       if (sp->type == BTM_STATE) {
                                                            Bp->btm_states->remove_linked(sp);
                                                            //if (Bp->id == 955) {
                                                                 //mCRL2log(log::verbose) << "ADDING BLOCK " << Bp->id << " with states " << Bp->btm_states->size() << " " << Bp->non_btm_states->size() << " " << Bp->marked_btm_states->size() << " " << Bp->marked_non_btm_states->size() << "\n";
                                                            //}
                                                            Bp->marked_btm_states->insert_state_linked(sp, MARKED_BTM_STATE);
                                                            //if (Bp->id == 955) {
                                                                 //mCRL2log(log::verbose) << "ADDING BLOCK " << Bp->id << " with states " << Bp->btm_states->size() << " " << Bp->non_btm_states->size() << " " << Bp->marked_btm_states->size() << " " << Bp->marked_non_btm_states->size() << "\n";
                                                            //}
                                                       }
                                                       // 5.2.2.c.ii
                                                       else {
                                                            Bp->non_btm_states->remove_linked(sp);
                                                            Bp->marked_non_btm_states->insert_state_linked(sp, MARKED_NON_BTM_STATE);
                                                       }
                                                  }
                                                  // 5.2.2.d
                                                  sp->constln_cnt->cnt++;
                                                  t->to_constln_cnt = sp->constln_cnt;
                                                  sp->coconstln_cnt->cnt--;
                                                  // move t
                                                  Bp->coconstln_ref->trans_list->remove_linked(t);
                                                  Bp->constln_ref->trans_list->insert_linked(t);
                                                  //assert(t->target->block->constellation == s->block->constellation);
                                                  // Different from pseudo-code: no longer needed, since direct link to transitions from block to constellation has been removed
                                                  // (now goes via to_constln_ref, which is updated at 5.2.2.b.
                                                  //t.block_constln_list = Bp.constln_transitions;
                                                  t->to_constln_ref = Bp->constln_ref;
                                                  //if (Bp->id == 355) {
                                                  //     mCRL2log(log::verbose) << "1518: setting t->to_constln_ref to " << Bp->constln_ref << "\n";
                                                  //}
                                             }
                                             //if (Bp->id == 955) {
                                             //mCRL2log(log::verbose) << "BLOCK " << Bp->id << " with states " << Bp->btm_states->size() << " " << Bp->non_btm_states->size() << " " << Bp->marked_btm_states->size() << " " << Bp->marked_non_btm_states->size() << "\n";
                                             //}
                                        }
                                   }
                                   //check_consistency_trans_lists(B, setB);
                                   // Different from pseudo-code: consider the case that B itself can be split
                                   // ALL states should be marked
                                   // 5.2.3
                                   auto tmpptr = B->btm_states;
                                   B->btm_states = B->marked_btm_states;
                                   B->marked_btm_states = tmpptr;
                                   tmpptr = B->non_btm_states;
                                   B->non_btm_states = B->marked_non_btm_states;
                                   B->marked_non_btm_states = tmpptr;
                                   splittable_blocks.insert(B);
                                   B->constln_ref = NULL;
                                   B->coconstln_ref = NULL;
                                   for (auto sit = marked_states_begin(B); sit != marked_states_end(B); sit = marked_states_next(B, sit)) {
                                        state_T* s = *sit;
                                        // actually mark the state
                                        if (s->type == BTM_STATE) {
                                             s->type = MARKED_BTM_STATE;
                                        }
                                        else {
                                             s->type = MARKED_NON_BTM_STATE;
                                        }
                                        // consider the outgoing transitions
                                        for (auto tit = s->Ttgt->begin(); tit != s->Ttgt->end(); ++tit) {
                                             transition_T* t = *tit;
                                             block_T* Bp = t->target->block;
                                             constellation_T* setBp = Bp->constellation;
                                             
                                             if (setBp == setB || setBp == setC) {
                                                  // 5.2.3.a
                                                  if (setBp == setB && B->constln_ref == NULL) {
                                                       B->coconstln_ref = t->to_constln_ref;
                                                       B->constln_ref = new to_constlns_element_T(setC);
                                                       B->to_constlns->insert_linked(B->constln_ref);
                                                       B->inconstln_ref = B->constln_ref;
                                                  }
                                                  // 5.2.3.b
                                                  if (s->coconstln_cnt == NULL) {
                                                       s->constln_cnt = new counter_T;
                                                       s->coconstln_cnt = t->to_constln_cnt;
                                                  }
                                                  // 5.2.3.c
                                                  if (Bp == B) {
                                                       s->constln_cnt->cnt++;
                                                       t->to_constln_cnt = s->constln_cnt;
                                                       s->coconstln_cnt->cnt--;
                                                       // move t
                                                       // Next lines are commented out, since these transitions are inert, which are NOT in the trans_list
                                                       //B->coconstln_ref->trans_list->remove_linked(t);
                                                       //B->constln_ref->trans_list->insert_linked(t);
                                                       //t->to_constln_ref = B->constln_ref;
                                                  }
                                             }
                                        }
                                   }
                                   // 5.2.4
                                   //check_consistency_trans_lists(B, setB);
                                   auto prev_sbit = splittable_blocks.before_begin();
                                   for (auto sbit = splittable_blocks.begin(); sbit != splittable_blocks.end(); ++sbit) {
                                        block_T* Bp = *sbit;
                                        //if (Bp->id == 955) {
                                             //mCRL2log(log::verbose) << "BLOCK " << Bp->id << " with states " << Bp->btm_states->size() << " " << Bp->non_btm_states->size() << " " << Bp->marked_btm_states->size() << " " << Bp->marked_non_btm_states->size() << "\n";
                                        //}
                                        
                                        // 5.2.3.a/b
                                        bool split = true;
                                        //mCRL2log(log::verbose) << "check " << Bp->id << " " << Bp->btm_states->size() << "\n";
                                        if (Bp->btm_states->size() == 0) {
                                             split = false;
                                             if (size(Bp->coconstln_ref) > 0) {
                                                  // find a state in marked_btm_states with s->coconstln_cnt->cnt == 0
                                                  for (auto sit = Bp->marked_btm_states->begin(); sit != Bp->marked_btm_states->end(); ++sit) {
                                                       state_T* s = *sit;
                                                       
                                                       if (s->coconstln_cnt == NULL) {
                                                            split = true;
                                                            break;
                                                       }
                                                       if ((s->coconstln_cnt)->cnt == 0) {
                                                            split = true;
                                                            break;
                                                       }
                                                  }
                                             }
                                        }
                                        // 5.2.3.c
                                        if (!split) {
                                             splittable_blocks.remove(prev_sbit, sbit);
                                             // move states back (unmark)
                                             clean_temp_refs_block(Bp);
                                        }
                                        prev_sbit = sbit;
                                   }
                                   // 5.2.4
                                   if (splittable_blocks.size() > 0) {
                                        const_block_found = true;
                                   }
                                   // end of considering block B. Go on to next constellation
                                   break;
                              } // end if block suitable
                         } // end loop over blocks in constellation
                         if (const_block_found) {
                              break;
                         }
                    } // end loop over constellations
                    if (const_block_found) {
                         // constellation / block for splitting found. Now split
                         //check_consistency_trans_lists(B, setB);
                         for (auto bit = splittable_blocks.begin(); bit != splittable_blocks.end(); ++bit) {
                              block_T* Bp = *bit;

                              //mCRL2log(log::verbose) << "splitting block " << Bp->id << " with size " << Bp->btm_states->size() + Bp->non_btm_states->size() + Bp->marked_btm_states->size() + Bp->marked_non_btm_states->size() << "\n";
                              //mCRL2log(log::verbose) << Bp->id << ": \n";
                              //for (auto it = Bp->to_constlns->begin(); it != Bp->to_constlns->end(); ++it) {
                              //     to_constlns_element_T* l = *it;
                              //     mCRL2log(log::verbose) << l << " " << l->new_element << " " << l->C->id << "\n";
                              //}
                              //mCRL2log(log::verbose) << "inconstln_ref: " << Bp->inconstln_ref << "\n";
                              //mCRL2log(log::verbose) << "---\n";
                              // pointer to Bp to be used when doing the nested split
                              block_T* splitBpB = Bp;
                              block_T* cosplitBpB = NULL;
                              block_T* splitsplitBpB = NULL;
                              size_t detect1_finished = CONTINUE;
                              size_t detect2_finished = CONTINUE;
                              block_T* Bpp = NULL;
                              block_T* Bp3 = NULL;
                              // lists for new btm states
                              sized_forward_list<state_T> XBp;
                              sized_forward_list<state_T> XBpp;
                              sized_forward_list<state_T> XBp3;
                              sized_forward_list<state_T> XBp4;
                              // check if we can jump to step 5.3.3
                              //check_consistency_trans_lists(B, setB);
                              //check_consistency_blocks();
                              if (Bp->btm_states->size() > 0) {
                                   nr_of_splits++;
                                   // 5.3.1. Perform detect1 and detect2 in lockstep
                                   Q = new std::stack< state_T*>;
                                   L.constant_clear();
                                   //P = new std::priority_queue < state_T*, std::vector< state_T* >, LessThanByInert>;
                                   Lp.constant_clear();
                                   maxsize_detect = (Bp->btm_states->size() + Bp->non_btm_states->size() + Bp->marked_btm_states->size() + Bp->marked_non_btm_states->size()) / 2;
                                   block_detect = Bp;
                                   current_state_detect1 = NULL;
                                   current_state_detect2 = NULL;
                                   in_forward_check_detect2 = false;
                                   //for (auto sit = Bp->marked_btm_states->begin(); sit != Bp->marked_btm_states->end(); ++sit) {
                                   //     state_T* s = *sit;
                                   //     mCRL2log(log::verbose) << "marked bottom state " << s->id << "\n";
                                   //}
                                   if (Bp->marked_btm_states->size() > 0) {
                                        iter_state_added_detect1 = Bp->marked_btm_states->before_begin();
                                        iterating_non_bottom = false;
                                   }
                                   else {
                                        iter_state_added_detect1 = Bp->marked_non_btm_states->before_begin();
                                        iterating_non_bottom = true;
                                   }
                                   iter_state_added_detect2 = Bp->btm_states->before_begin();
                                   //mCRL2log(log::verbose) << "states in " << Bp->id << ": " << Bp->btm_states->size() << " " << Bp->non_btm_states->size() << " " << Bp->marked_btm_states->size() << " " << Bp->marked_non_btm_states->size() << "\n";
                                   //mCRL2log(log::verbose) << Bp->marked_non_btm_states->size() << "\n";
                                   //print_partition();
                                   //mCRL2log(log::verbose) << "Launching lockstep search 1\n";
                                   //for (auto sit = Bp->marked_non_btm_states->begin(); sit != Bp->marked_non_btm_states->end(); ++sit) {
                                   //     state_T* s = *sit;
                                   //     mCRL2log(log::verbose) << s->id << "\n";
                                   //}
                                   while (detect1_finished != TERMINATED && detect2_finished != TERMINATED) {
                                        if (detect1_finished != STOPPED) {
                                             detect1_finished = detect1_step(&bisim_partitioner_gw<LTS_TYPE>::has_next_state_detect1_1, &bisim_partitioner_gw<LTS_TYPE>::next_state_detect1_1);
                                        }
                                        if (detect1_finished != TERMINATED && detect2_finished != STOPPED) {
                                             detect2_finished = detect2_step(&bisim_partitioner_gw<LTS_TYPE>::has_next_state_detect2_1, &bisim_partitioner_gw<LTS_TYPE>::next_state_detect2_1, false, false);
                                        }
                                   }
                                   // 5.3.2
                                   // Pseudo-code: 'add it to the list of blocks' could possibly be removed.
                                   Bpp = create_new_block(Bp->type);
                                   Bpp->constellation = Bp->constellation;
                                   Bpp->constellation->blocks->insert_linked(Bpp);
                                   //mCRL2log(log::verbose) << "creating new block " << Bpp->id << "\n";
                                   // Let N point to correct list
                                   sized_forward_list <state_T>* N;
                                   if (detect1_finished == TERMINATED) {
                                        N = &L;
                                   }
                                   else {
                                        N = &Lp;
                                   }
#ifndef NDEBUG
                                   mCRL2log(log::verbose) << "splitting off: [";
                                   for (auto sit = N->begin(); sit != N->end(); ++sit) {
                                        state_T* s = *sit;
                                        mCRL2log(log::verbose) << " " << s->id;
                                   }
                                   mCRL2log(log::verbose) << "]\n";
                                   check_consistency_blocks();
                                   check_consistency_transitions();
                                   for (auto it = Bp->to_constlns->begin(); it != Bp->to_constlns->end(); ++it) {
                                        to_constlns_element_T* l = *it;
                                        mCRL2log(log::verbose) << l << " " << l->new_element << " " << l->C->id << "\n";
                                   }
                                   mCRL2log(log::verbose) << "---\n";
#endif
                                   //check_consistency_trans_lists(B, setB);
                                   //if (Bpp->id == 355) {
                                   //     mCRL2log(log::verbose) << "1727: splitting " << Bp->id << " into " << Bpp->id << "\n";
                                   //}
                                   for (auto sit = N->begin(); sit != N->end(); ++sit) {
                                        state_T* s = *sit;
                                        
                                        //mCRL2log(log::verbose) << "State " << s->id << "\n";
                                        
                                        // 5.3.2.a
                                        move_state_to_block(s, Bpp);
                                        // 5.3.2.b
                                        // Different from pseudo-code: we need to consider all constellations that can be reached from
                                        // s, not just setB \ B and B.
                                        for (auto tit = s->Ttgt->begin(); tit != s->Ttgt->end(); ++tit) {
                                             transition_T* t = *tit;
                                             
                                             // Pseudo-code: How can s be in B'? It was removed at step 5.3.2.a
                                             // Different from pseudo-code: only do this if transition is non-inert
                                             // 5.3.2.b.i
                                             if (t->to_constln_ref != NULL) {
                                                  to_constlns_element_T* l = t->to_constln_ref;
                                                  //mCRL2log(log::verbose) << "t->to_constln_ref: " << t->to_constln_ref << " " << t->to_constln_ref->C->id << "\n";
                                                  to_constlns_element_T* lp;
                                                  if (l->new_element != NULL) {
                                                       //mCRL2log(log::verbose) << "point to new element " << l->new_element << " " << l->new_element->C->id << "\n";
                                                       lp = l->new_element;
                                                  }
                                                  else {
                                                       lp = new to_constlns_element_T(l->C);
                                                       l->new_element = lp;
                                                       //mCRL2log(log::verbose) << "create new element " << l->new_element << " " << l->new_element->C->id << "\n";
                                                       // add lp to Bpp.to_constlns (IMPLIED IN PSEUDO-CODE)
                                                       Bpp->to_constlns->insert_linked(lp);
                                                       // possibly set Bpp.inconstln_ref
                                                       if (l->C == Bpp->constellation) {
                                                            Bpp->inconstln_ref = lp;
                                                            //mCRL2log(log::verbose) << "setting inconstln " << lp << " " << lp->C->id << "\n";
                                                       }
                                                       // possibly set Bpp.constln_ref and Bpp.coconstln_ref
                                                       if (l == Bp->constln_ref) {
                                                            //mCRL2log(log::verbose) << "setting constln_ref " << lp << " " << lp->C->id << "\n";
                                                            Bpp->constln_ref = lp;
                                                       }
                                                       else if (l == Bp->coconstln_ref) {
                                                            //mCRL2log(log::verbose) << "setting coconstln_ref " << lp << " " << lp->C->id << "\n";
                                                            Bpp->coconstln_ref = lp;
                                                       }
                                                       // let lp point to l, to be able to efficiently reset new_element pointers later on
                                                       lp->new_element = l;
                                                       //mCRL2log(log::verbose) << "pointing " << lp << "back to " << l << "\n";
                                                  }
                                                  //mCRL2log(log::verbose) << "removing transition\n";
                                                  l->trans_list->remove_linked(t);
                                                  lp->trans_list->insert_linked(t);
                                                  t->to_constln_ref = lp;
                                                  //mCRL2log(log::verbose) << "setting t->to_constln_ref to " << lp << "\n";
                                             }
                                             // 5.3.2.b.ii
                                             else {
                                                  state_T* sp = t->target;
                                             
                                                  if (sp->block == Bp && (N == &L ? !sp->is_in_L_detect1 : !sp->is_in_Lp_detect2)) {
                                                       // an inert transition becomes non-inert
                                                       s->inert_cnt--;
                                                       if (s->inert_cnt == 0) {
                                                            XBpp.insert(s);
                                                            if (s->type == NON_BTM_STATE) {
                                                                 Bpp->non_btm_states->remove_linked(s);
                                                                 Bpp->btm_states->insert_state_linked(s, BTM_STATE);
                                                            }
                                                            else if (s->type == MARKED_NON_BTM_STATE) {
                                                                 Bpp->marked_non_btm_states->remove_linked(s);
                                                                 Bpp->marked_btm_states->insert_state_linked(s, MARKED_BTM_STATE);
                                                            }
                                                       }
                                                       // Different from pseudo-code: add the transition to the corresponding trans_list
                                                       if (Bpp->inconstln_ref == NULL) {
                                                            Bpp->inconstln_ref = new to_constlns_element_T(Bpp->constellation);
                                                            Bpp->to_constlns->insert_linked(Bpp->inconstln_ref);
                                                            if (Bp->inconstln_ref != NULL) {
                                                                 Bp->inconstln_ref->new_element = Bpp->inconstln_ref;
                                                                 Bpp->inconstln_ref->new_element = Bp->inconstln_ref;
                                                            }
                                                       }
                                                       Bpp->inconstln_ref->trans_list->insert_linked(t);
                                                       t->to_constln_ref = Bpp->inconstln_ref;
                                                       //if (Bpp->id == 355) {
                                                       //     mCRL2log(log::verbose) << "1830: setting t->to_constln_ref to " << Bpp->inconstln_ref << "\n";
                                                       //}
                                                  }
                                             }
                                        }
                                        // 5.3.2.c
                                        //check_consistency_blocks();
                                        //check_consistency_trans_lists(B, setB);
                                        for (auto tit = s->Tsrc->begin(); tit != s->Tsrc->end(); ++tit) {
                                             transition_T* t = *tit;
                                             state_T* sp = t->source;
                                             
                                             if (sp->block == Bp && (N == &L ? !sp->is_in_L_detect1 : !sp->is_in_Lp_detect2)) {
                                                  // an inert transition becomes non-inert
                                                  sp->inert_cnt--;
                                                  if (sp->inert_cnt == 0) {
                                                       XBp.insert(sp);
                                                       if (sp->type == NON_BTM_STATE) {
                                                            Bp->non_btm_states->remove_linked(sp);
                                                            Bp->btm_states->insert_state_linked(sp, BTM_STATE);
                                                       }
                                                       else if (sp->type == MARKED_NON_BTM_STATE) {
                                                            Bp->marked_non_btm_states->remove_linked(sp);
                                                            Bp->marked_btm_states->insert_state_linked(sp, MARKED_BTM_STATE);
                                                       }
                                                  }
                                                  // Different from pseudo-code: add the transition to the corresponding trans_list
                                                  if (Bp->inconstln_ref == NULL) {
                                                       Bp->inconstln_ref = new to_constlns_element_T(Bp->constellation);
                                                       Bp->to_constlns->insert_linked(Bp->inconstln_ref);
                                                       if (Bpp->inconstln_ref != NULL) {
                                                            Bpp->inconstln_ref->new_element = Bp->inconstln_ref;
                                                            Bp->inconstln_ref->new_element = Bpp->inconstln_ref;
                                                       }
                                                  }
                                                  Bp->inconstln_ref->trans_list->insert_linked(t);
                                                  t->to_constln_ref = Bp->inconstln_ref;
                                                  //if (Bp->id == 355) {
                                                  //     mCRL2log(log::verbose) << "1866: setting t->to_constln_ref to " << Bp->inconstln_ref << "\n";
                                                  //}
                                             }
                                        }
                                   }
                                   //check_consistency_blocks();
                                   //mCRL2log(log::verbose) << Bp->id << ": \n";
                                   //for (auto it = Bp->to_constlns->begin(); it != Bp->to_constlns->end(); ++it) {
                                   //     to_constlns_element_T* l = *it;
                                   //     mCRL2log(log::verbose) << l << " " << l->new_element << " " << l->C->id << "\n";
                                   //}
                                   //mCRL2log(log::verbose) << Bpp->id << ": \n";
                                   //for (auto it = Bpp->to_constlns->begin(); it != Bpp->to_constlns->end(); ++it) {
                                   //     to_constlns_element_T* l = *it;
                                   //     mCRL2log(log::verbose) << l << " " << l->new_element << " " << l->C->id << "\n";
                                   //}
                                   //mCRL2log(log::verbose) << "3---\n";
                                   //check_consistency_trans_lists(B, setB);
                                   // reset temporary pointers of states
                                   for (auto it = L.begin(); it != L.end(); ++it) {
                                        state_T* s = *it;
                                        s->is_in_L_detect1 = false;
                                   }
                                   for (auto it = Lp.begin(); it != Lp.end(); ++it) {
                                        state_T* s = *it;
                                        s->is_in_Lp_detect2 = false;
                                   }
                                   while (!P->empty()) {
                                        state_T* s = *(P->begin());
                                        P->erase(P->begin());
                                        s->is_in_P_detect2 = false;
                                        //mCRL2log(log::verbose) << "remove from P: " << s->id << "\n";
                                   }
                                   // check
                                   //check_consistency_trans_lists(B, setB);
#ifndef NDEBUG
                                   mCRL2log(log::verbose) << "---\n";
                                   mCRL2log(log::verbose) << Bp->id << ": \n";
                                   for (auto it = Bp->to_constlns->begin(); it != Bp->to_constlns->end(); ++it) {
                                        to_constlns_element_T* l = *it;
                                        mCRL2log(log::verbose) << l << " " << l->new_element << "\n";
                                   }
                                   mCRL2log(log::verbose) << "inconstln: " << Bp->inconstln_ref << "\n";
                                   mCRL2log(log::verbose) << "---\n";
                                   mCRL2log(log::verbose) << Bpp->id << ": \n";
                                   for (auto it = Bpp->to_constlns->begin(); it != Bpp->to_constlns->end(); ++it) {
                                        to_constlns_element_T* l = *it;
                                        mCRL2log(log::verbose) << l << " " << l->new_element << "\n";
                                   }
                                   mCRL2log(log::verbose) << "inconstln: " << Bpp->inconstln_ref << "\n";
                                   mCRL2log(log::verbose) << "---\n";
#endif
                                // Different from pseudo-code: reset temporary pointers (new elements) of blocks
                                   // Remove the associated element if the transition list is empty, UNLESS the element is pointed to
                                   // by either Bp->constln_ref or Bpp->coconstln_ref
                                   // 5.3.3
                                   for (auto it = Bpp->to_constlns->begin(); it != Bpp->to_constlns->end(); ++it) {
                                        to_constlns_element_T* l = *it;
                                        if (l->new_element != NULL) {
                                             if (size(l->new_element) == 0 && l->new_element != Bp->constln_ref && l->new_element != Bp->coconstln_ref) {
                                                  if (l->new_element->C == Bp->constellation) {
                                                       Bp->inconstln_ref = NULL;
                                                  }
                                                  Bp->to_constlns->remove_linked(l->new_element);
                                                  deleteobject (l->new_element);
                                             }
                                             else {
                                                  l->new_element->new_element = NULL;
                                             }
                                             l->new_element = NULL;
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
                              if (detect1_finished == TERMINATED) {
                                   splitBpB = Bpp;
                                   cosplitBpB = Bp;
                              }
                              else {
                                   cosplitBpB = Bpp;
                              }
                              // point to correct XB list
                              sized_forward_list<state_T> *XsplitBpB = NULL;
                              sized_forward_list<state_T> *XcosplitBpB = NULL;
                              sized_forward_list<state_T> *XsplitsplitBpB = NULL;
                              //check_consistency_trans_lists(B, setB);
                              if (detect1_finished == TERMINATED) {
                                   XsplitBpB = &XBpp;
                                   XcosplitBpB = &XBp;
                              }
                              else {
                                   XsplitBpB = &XBp;
                                   XcosplitBpB = &XBpp;
                              }
                              // check if splitBpB is stable, and if not, do a nested splitting
                              splitsplitBpB = NULL;
                              bool is_stable = false;
                              if (splitBpB->coconstln_ref == NULL) {
                                   is_stable = true;
                              }
                              else if (size(splitBpB->coconstln_ref) == 0) {
                                   is_stable = true;
                              }
                              //check_consistency_trans_lists(B, setB);
                              if (!is_stable) {
                                   is_stable = true;
                                   for (auto sit = splitBpB->marked_btm_states->begin(); sit != splitBpB->marked_btm_states->end(); ++sit) {
                                        state_T* s = *sit;
                                        if (s->coconstln_cnt == NULL) {
                                             is_stable = false;
                                             break;
                                        }
                                        if (s->coconstln_cnt->cnt == 0) {
                                             is_stable = false;
                                             break;
                                        }
                                   }
                              }
                              //check_consistency_trans_lists(B, setB);
                              if (!is_stable) {
                                   nr_of_splits++;
                                   //mCRL2log(log::verbose) << "not stable\n";
                                   deleteobject (Q);
                                   Q = new std::stack< state_T*>;
                                   L.constant_clear();
                                   //std::priority_queue < state_T*, std::vector< state_T* >, LessThanByInert> P;
                                   Lp.constant_clear();
                                   XBp3.constant_clear();
                                   XBp4.constant_clear();
                                   // prepare for lockstep search
                                   maxsize_detect = (splitBpB->btm_states->size() + splitBpB->non_btm_states->size() + splitBpB->marked_btm_states->size() + splitBpB->marked_non_btm_states->size()) / 2;
                                   //mCRL2log(log::verbose) << "sizes: " << (splitBpB->btm_states->size() + splitBpB->non_btm_states->size() + splitBpB->marked_btm_states->size() + splitBpB->marked_non_btm_states->size()) << " " << maxsize_detect << "\n";
                                   block_detect = splitBpB;
                                   constellation_splitter_detect = setB;
                                   detect1_finished = CONTINUE;
                                   detect2_finished = CONTINUE;
                                   // lockstep search
#ifndef NDEBUG
                                   //print_partition();
#endif
                                   current_state_detect1 = NULL;
                                   current_state_detect2 = NULL;
                                   in_forward_check_detect2 = false;
                                   iter_trans_state_added_detect1 = splitBpB->coconstln_ref->trans_list->before_begin();
                                   iter_state_added_detect2 = splitBpB->marked_btm_states->before_begin();
                                   //check_consistency_trans_lists(B, setB);
                                   //mCRL2log(log::verbose) << "Launching lockstep search 2\n";
                                   //mCRL2log(log::verbose) << "block: " << Bp << " splitting const: " << setB << "\n";
                                   while (detect1_finished != TERMINATED && detect2_finished != TERMINATED) {
                                        if (detect1_finished != STOPPED) {
                                             detect1_finished = detect1_step(&bisim_partitioner_gw<LTS_TYPE>::has_next_state_detect1_2, &bisim_partitioner_gw<LTS_TYPE>::next_state_detect1_2);
                                        }
                                        if (detect1_finished != TERMINATED && detect2_finished != STOPPED) {
                                             detect2_finished = detect2_step(&bisim_partitioner_gw<LTS_TYPE>::has_next_state_detect2_2, &bisim_partitioner_gw<LTS_TYPE>::next_state_detect2_2, true, true);
                                        }
                                        // DEBUG EXIT
                                        //if (detect1_finished == STOPPED && detect2_finished == STOPPED) {
                                        //     exit(1);
                                        //}
                                   }
#ifndef NDEBUG
                                   check_consistency_blocks();
                                   check_consistency_transitions();
#endif
                                   // split
                                   Bp3 = create_new_block(splitBpB->type);
                                   Bp3->constellation = splitBpB->constellation;
                                   Bp3->constellation->blocks->insert_linked(Bp3);
                                   // Let N point to correct list
                                   sized_forward_list <state_T>* N;
                                   if (detect1_finished == TERMINATED) {
                                        N = &L;
                                   }
                                   else {
                                        N = &Lp;
                                   }
                                   for (auto sit = N->begin(); sit != N->end(); ++sit) {
                                        state_T* s = *sit;
                                        //mCRL2log(log::verbose) << "processing state " << s->id << "\n";
                                        // 5.3.4 (5.3.2.a)
                                        move_state_to_block(s, Bp3);
#ifndef NDEBUG
                                        //print_partition();
#endif
                                        // 5.3.4 (5.3.2.b)
                                        for (auto tit = s->Ttgt->begin(); tit != s->Ttgt->end(); ++tit) {
                                             transition_T* t = *tit;
                                             
                                             // Pseudo-code: How can s be in B'? It was removed at step 5.3.2.a
                                             // Different from pseudo-code: only do this if transition is non-inert
                                             // 5.3.4 (5.3.2.b.i)
                                             if (t->to_constln_ref != NULL) {
                                                  to_constlns_element_T* l = t->to_constln_ref;
                                                  to_constlns_element_T* lp;
                                                  if (l->new_element != NULL) {
                                                       lp = l->new_element;
                                                  }
                                                  else {
                                                       //assert(Bp3->inconstln_ref == NULL || l->C != Bp3->constellation);
                                                       lp = new to_constlns_element_T(l->C);
                                                       l->new_element = lp;
                                                       // add lp to Bp3.to_constlns (IMPLIED IN PSEUDO-CODE)
                                                       Bp3->to_constlns->insert_linked(lp);
                                                       // possibly set Bp3.inconstln_ref
                                                       if (l->C == Bp3->constellation) {
                                                            //mCRL2log(log::verbose) << "bla\n";
                                                            Bp3->inconstln_ref = lp;
                                                       }
     //                                                  // possibly set Bp3.constln_ref and Bp3.coconstln_ref
     //                                                  if (l == splitBpB->constln_ref) {
     //                                                       Bp3->constln_ref = lp;
     //                                                  }
     //                                                  else if (l == splitBpB->coconstln_ref) {
     //                                                       Bp3->coconstln_ref = lp;
     //                                                  }
                                                       // point lp to l, to be able to efficiently reset new_element pointers later on
                                                       lp->new_element = l;
                                                  }
                                                  l->trans_list->remove_linked(t);
                                                  lp->trans_list->insert_linked(t);
                                                  t->to_constln_ref = lp;
                                                  //if (Bp3->id == 355) {
                                                  //     mCRL2log(log::verbose) << "2080: setting t->to_constln_ref to " << lp << "\n";
                                                  //}
                                                  //t.block_constln_list = lp.trans_list;
                                                  // postpone deleting element l until cleaning up new_element pointers
                                             }
                                             // 5.3.4 (5.3.2.b.ii)
                                             else {
                                                  state_T* sp = t->target;
                                             
                                                  if (sp->block == splitBpB && (N == &L ? !sp->is_in_L_detect1 : !sp->is_in_Lp_detect2)) {
                                                       // an inert transition becomes non-inert
                                                       s->inert_cnt--;
                                                       if (s->inert_cnt == 0) {
                                                            XBp3.insert(s);
                                                            if (s->type == NON_BTM_STATE) {
                                                                 Bp3->non_btm_states->remove_linked(s);
                                                                 Bp3->btm_states->insert_state_linked(s, BTM_STATE);
                                                            }
                                                            else if (s->type == MARKED_NON_BTM_STATE) {
                                                                 Bp3->marked_non_btm_states->remove_linked(s);
                                                                 Bp3->marked_btm_states->insert_state_linked(s, MARKED_BTM_STATE);
                                                            }
                                                       }
                                                       // Different from pseudo-code: add the transition to the corresponding trans_list
                                                       if (Bp3->inconstln_ref == NULL) {
                                                            Bp3->inconstln_ref = new to_constlns_element_T(Bp3->constellation);
                                                            Bp3->to_constlns->insert_linked(Bp3->inconstln_ref);
                                                            if (splitBpB->inconstln_ref != NULL) {
                                                                 splitBpB->inconstln_ref->new_element = Bp3->inconstln_ref;
                                                                 Bp3->inconstln_ref->new_element = splitBpB->inconstln_ref;
                                                            }
                                                       }
                                                       Bp3->inconstln_ref->trans_list->insert_linked(t);
                                                       t->to_constln_ref = Bp3->inconstln_ref;
                                                  }
                                             }
                                        }
                                        // 5.3.4 (5.3.2.c)
                                        for (auto tit = s->Tsrc->begin(); tit != s->Tsrc->end(); ++tit) {
                                             transition_T* t = *tit;
                                             state_T* sp = t->source;
                                             
                                             if (sp->block == splitBpB && (N == &L ? !sp->is_in_L_detect1 : !sp->is_in_Lp_detect2)) {
                                                  // an inert transition becomes non-inert
                                                  sp->inert_cnt--;
                                                  if (sp->inert_cnt == 0) {
                                                       XBp4.insert(sp);
                                                       if (sp->type == NON_BTM_STATE) {
                                                            splitBpB->non_btm_states->remove_linked(sp);
                                                            splitBpB->btm_states->insert_state_linked(sp, BTM_STATE);
                                                       }
                                                       else if (sp->type == MARKED_NON_BTM_STATE) {
                                                            splitBpB->marked_non_btm_states->remove_linked(sp);
                                                            splitBpB->marked_btm_states->insert_state_linked(sp, MARKED_BTM_STATE);
                                                       }
                                                  }
                                                  // Different from pseudo-code: add the transition to the corresponding trans_list
                                                  if (splitBpB->inconstln_ref == NULL) {
                                                       splitBpB->inconstln_ref = new to_constlns_element_T(splitBpB->constellation);
                                                       splitBpB->to_constlns->insert_linked(splitBpB->inconstln_ref);
                                                       if (Bp3->inconstln_ref != NULL) {
                                                            Bp3->inconstln_ref->new_element = splitBpB->inconstln_ref;
                                                            splitBpB->inconstln_ref->new_element = Bp3->inconstln_ref;
                                                       }
                                                  }
                                                  splitBpB->inconstln_ref->trans_list->insert_linked(t);
                                                  t->to_constln_ref = splitBpB->inconstln_ref;
                                                  //if (splitBpB->id == 355) {
                                                  //     mCRL2log(log::verbose) << "2147: setting t->to_constln_ref to " << splitBpB->inconstln_ref << "\n";
                                                  //}
                                             }
                                        }
                                   }
                                   //mCRL2log(log::verbose) << splitBpB->id << ": \n";
                                   //for (auto it = splitBpB->to_constlns->begin(); it != splitBpB->to_constlns->end(); ++it) {
                                   //     to_constlns_element_T* l = *it;
                                   //     mCRL2log(log::verbose) << l << " " << l->new_element << " " << l->C->id << "\n";
                                   //}
                                   //mCRL2log(log::verbose) << "inconstln: " << splitBpB->inconstln_ref << "\n";
                                   //mCRL2log(log::verbose) << "22---\n";
                                   //mCRL2log(log::verbose) << Bp3->id << ": \n";
                                   //for (auto it = Bp3->to_constlns->begin(); it != Bp3->to_constlns->end(); ++it) {
                                   //     to_constlns_element_T* l = *it;
                                   //     mCRL2log(log::verbose) << l << " " << l->new_element << " " << l->C->id << "\n";
                                   //}
                                   //mCRL2log(log::verbose) << "inconstln: " << Bp3->inconstln_ref << "\n";
                                   //mCRL2log(log::verbose) << "22---\n";
                                   // reset temporary pointers of states
                                   for (auto it = L.begin(); it != L.end(); ++it) {
                                        state_T* s = *it;
                                        s->is_in_L_detect1 = false;
                                   }
                                   for (auto it = Lp.begin(); it != Lp.end(); ++it) {
                                        state_T* s = *it;
                                        s->is_in_Lp_detect2 = false;
                                   }
                                   while (!P->empty()) {
                                        state_T* s = *(P->begin());
                                        P->erase(P->begin());
                                        s->is_in_P_detect2 = false;
                                        //mCRL2log(log::verbose) << "remove from P: " << s->id << "\n";
                                   }
                                // Different from pseudo-code: reset temporary pointers (new elements) of blocks
                                   // Remove the associated element if the transition list is empty, UNLESS the element is pointed to
                                   // by either Bp->constln_ref or Bpp->coconstln_ref
                                   // 5.3.4 (5.3.3)
                                   for (auto it = Bp3->to_constlns->begin(); it != Bp3->to_constlns->end(); ++it) {
                                        to_constlns_element_T* l = *it;
                                        if (l->new_element != NULL) {
                                             if (size(l->new_element) == 0 && l->new_element != splitBpB->constln_ref && l->new_element != splitBpB->coconstln_ref) {
                                                  if (l->new_element->C == splitBpB->constellation) {
                                                       splitBpB->inconstln_ref = NULL;
                                                  }
                                                  splitBpB->to_constlns->remove_linked(l->new_element);
                                                  deleteobject (l->new_element);
                                             }
                                             else {
                                                  l->new_element->new_element = NULL;
                                             }
                                             l->new_element = NULL;
                                        }
                                   }
                                   //mCRL2log(log::verbose) << splitBpB->id << ": \n";
                                   //for (auto it = splitBpB->to_constlns->begin(); it != splitBpB->to_constlns->end(); ++it) {
                                   //     to_constlns_element_T* l = *it;
                                   //     mCRL2log(log::verbose) << l << " " << l->new_element << " " << l->C->id << "\n";
                                   //}
                                   //mCRL2log(log::verbose) << "inconstln: " << splitBpB->inconstln_ref << "\n";
                                   //mCRL2log(log::verbose) << "21---\n";
                                   // set splitsplitBpB
                                   if (detect1_finished == TERMINATED) {
                                        splitsplitBpB = Bp3;
                                   }
                                   else if (detect2_finished == TERMINATED) {
                                        splitsplitBpB = splitBpB;
                                        splitBpB = Bp3;
                                   }
                                   // determine the XB sets
                                   sized_forward_list<state_T> *X2 = NULL;
                                   if (detect1_finished == TERMINATED) {
                                        XsplitsplitBpB = &XBp3;
                                        X2 = &XBp4;
                                   }
                                   else {
                                        XsplitsplitBpB = &XBp4;
                                        X2 = &XBp3;
                                   }
                                   // redistribute content of XsplitBpB
                                   for (auto sit = XsplitBpB->begin(); sit != XsplitBpB->end(); ++sit) {
                                        state_T* s = *sit;
                                        if (s->block == splitsplitBpB) {
                                             XsplitsplitBpB->insert(s);
                                        }
                                        else {
                                             X2->insert(s);
                                        }
                                   }
                                   //deleteobject (XsplitBpB);
                                   // needed?
                                   //XsplitBpB = new sized_forward_list < state_T >;
                                   XsplitBpB = X2;
                              } // end if stable (5.3.4)
                              // remove markings and reset constln_ref and coconstln_ref
                              //if (splitBpB != NULL) {
                              //     mCRL2log(log::verbose) << "split clean " << splitBpB->id << "\n";
                              //}
                              // 5.3.5
                              clean_temp_refs_block(splitBpB);
                              //if (cosplitBpB != NULL) {
                              //     mCRL2log(log::verbose) << "cosplit clean " << cosplitBpB->id << "\n";
                              //}
                              clean_temp_refs_block(cosplitBpB);
                              //if (splitsplitBpB != NULL) {
                              //     mCRL2log(log::verbose) << "splitsplit clean " << splitsplitBpB->id << "\n";
                              //}
                              clean_temp_refs_block(splitsplitBpB);
                              // 5.3.6
                              block_T* Bhat = splitBpB;
                              block_T* Bhatp = NULL;
                              sized_forward_list<state_T> *XBhat = XsplitBpB;
                              while (Bhat != NULL) {
                                   for (auto sit = XBhat->begin(); sit != XBhat->end(); ++sit) {
                                        state_T* s = *sit;
                                        // 5.3.6.a
                                        for (auto tit = s->Ttgt->begin(); tit != s->Ttgt->end(); ++tit) {
                                             transition_T* t = *tit;
                                             to_constlns_element_T* setBp_entry = t->to_constln_ref;
                                             //mCRL2log(log::verbose) << t->to_constln_ref->C << " " << t->target->block->constellation << "\n";
                                             //mCRL2log(log::verbose) << Bhat->constellation << "\n";
                                             //assert(t->to_constln_ref->C == t->target->block->constellation);
                                             // 5.3.6.a.i
                                             if (setBp_entry->SClist == NULL) {
                                                  setBp_entry->SClist = new sized_forward_list < state_T >;
                                                  // move the to_constln entry to the front of the list
                                                  Bhat->to_constlns->remove_linked(setBp_entry);
                                                  Bhat->to_constlns->insert_linked(setBp_entry);
                                             }
                                             // 5.3.6.a.ii
                                             if (setBp_entry->SClist->size() == 0) {
                                                  //mCRL2log(log::verbose) << "adding state to constln " << setBp_entry->C << "\n";
                                                  setBp_entry->SClist->insert(s);
                                             }
                                             else if (setBp_entry->SClist->front() != s) {
                                                  //mCRL2log(log::verbose) << "adding state to constln " << setBp_entry->C << "\n";
                                                  setBp_entry->SClist->insert(s);
                                             }
                                        }
                                        // 5.3.6.b
                                        Bhat->new_btm_states->insert(s);
                                   }
                              
                                   // consider next block
                                   if (Bhat == splitsplitBpB) {
                                        break;
                                   }
                                   if (Bhat == cosplitBpB) {
                                        Bhat = splitsplitBpB;
                                        XBhat = XsplitsplitBpB;
                                   }
                                   else if (cosplitBpB != NULL) {
                                        Bhat = cosplitBpB;
                                        XBhat = XcosplitBpB;
                                   }
                                   else {
                                        Bhat = splitsplitBpB;
                                        XBhat = XsplitsplitBpB;
                                   }
                              }
#ifndef NDEBUG
                              //print_partition();
#endif
                              // 5.3.7
                              // We use a stack blocks_to_process to keep track of which blocks still need to be checked for stability
                              std::stack < block_T* > blocks_to_process;
                              // push blocks on the stack if they have new bottom states
                              if (splitBpB != NULL) {
                                   if (splitBpB->new_btm_states->size() > 0) {
                                        //mCRL2log(log::verbose) << "add split\n";
                                        blocks_to_process.push(splitBpB);
                                   }
                              }
                              if (cosplitBpB != NULL) {
                                   if (cosplitBpB->new_btm_states->size() > 0) {
                                        //mCRL2log(log::verbose) << "add cosplit\n";
                                        blocks_to_process.push(cosplitBpB);
                                   }
                              }
                              if (splitsplitBpB != NULL) {
                                   if (splitsplitBpB->new_btm_states->size() > 0) {
                                        //mCRL2log(log::verbose) << "add splitsplit with # new btm states = " << splitsplitBpB->new_btm_states->size() << "\n";
                                        blocks_to_process.push(splitsplitBpB);
                                   }
                              }
                              while (blocks_to_process.size() > 0) {
                                   // 5.3.7.a
                                   Bhat = blocks_to_process.top();
                                   blocks_to_process.pop();
                                   bool split = false;
                                   // 5.3.7.b
                                   // Find a constellation under which the block is not stable
                                   for (auto cit = Bhat->to_constlns->begin(); cit != Bhat->to_constlns->end(); ++cit) {
                                        to_constlns_element_T* e = *cit;
                                        bool splitcrit_met = false;
                                        if (e->SClist == NULL) {
                                             splitcrit_met = true;
                                             split = true;
                                        }
                                        else if (e->SClist->size() < Bhat->new_btm_states->size()) {
                                             //mCRL2log(log::verbose) << "size: " << e->SClist->size() << "\n";
                                             splitcrit_met = true;
                                             split = true;
                                        }
                                        if (splitcrit_met) {
                                             nr_of_splits++;
                                             // further splitting is required
                                             XBp.constant_clear();
                                             XBpp.constant_clear();
                                             // Prepare detect1 and detect2 for lockstep
                                             deleteobject (Q);
                                             Q = new std::stack< state_T*>;
                                             L.constant_clear();
                                             //std::priority_queue < state_T*, std::vector< state_T* >, LessThanByInert> P;
                                             Lp.constant_clear();
                                             // 5.3.7.b.i.A / B
                                             // !!! Different from the pseudo-code, we prepare detect1 by walking over the transitions from Bhat to constellation pointed
                                             // to by *cit. To optimise this, we should probably maintain a list of states that have a direct transition from Bhat to constellation
                                             // of *cit, and instead walk over that list
                                             maxsize_detect = (Bhat->btm_states->size() + Bhat->non_btm_states->size()) / 2;
                                             block_detect = Bhat;
                                             e_detect = e;
                                             constellation_splitter_detect = e->C;
                                             detect1_finished = CONTINUE;
                                             detect2_finished = CONTINUE;
                                             current_state_detect1 = NULL;
                                             current_state_detect2 = NULL;
                                             in_forward_check_detect2 = false;
                                             iter_trans_state_added_detect1 = e->trans_list->before_begin();
                                             iter_state_added_detect2 = Bhat->new_btm_states->before_begin();
                                             if (e->SClist == NULL) {
                                                  sclist_is_empty_detect2 = true;
                                             }
                                             else {
                                                  sclist_is_empty_detect2 = false;
                                                  iter_sclist_detect2 = e->SClist->begin();
                                             }
                                             while (detect1_finished != TERMINATED && detect2_finished != TERMINATED) {
                                                  if (detect1_finished != STOPPED) {
                                                       detect1_finished = detect1_step(&bisim_partitioner_gw<LTS_TYPE>::has_next_state_detect1_3, &bisim_partitioner_gw<LTS_TYPE>::next_state_detect1_3);
                                                  }
                                                  if (detect1_finished != TERMINATED && detect2_finished != STOPPED) {
                                                       detect2_finished = detect2_step(&bisim_partitioner_gw<LTS_TYPE>::has_next_state_detect2_3, &bisim_partitioner_gw<LTS_TYPE>::next_state_detect2_3, false, true);
                                                  }
                                             }
                                             // 5.3.7.b.ii (5.3.2)
                                             Bhatp = create_new_block(Bhat->type);
                                             Bhatp->constellation = Bhat->constellation;
                                             Bhatp->constellation->blocks->insert_linked(Bhatp);
                                             //mCRL2log(log::verbose) << "splitting " << Bhat->id << " producing " << Bhatp->id << "\n";
                                             // Let N point to correct list
                                             sized_forward_list <state_T> *N;
                                             if (detect1_finished == TERMINATED) {
                                                  N = &L;
                                             }
                                             else {
                                                  N = &Lp;
                                             }
                                             for (auto sit = N->begin(); sit != N->end(); ++sit) {
                                                  state_T* s = *sit;
                                                  move_state_to_block(s, Bhatp);
                                                  // Different from pseudo-code: we need to consider all constellations that can be reached from
                                                  // s, not just setB \ B and B. For this, we merge steps 5.3.2.c and 5.3.2.d
                                                  for (auto tit = s->Ttgt->begin(); tit != s->Ttgt->end(); ++tit) {
                                                       transition_T* t = *tit;

                                                       // Different from pseudo-code: only do this if transition is non-inert
                                                       if (t->to_constln_ref != NULL) {
                                                            to_constlns_element_T* l = t->to_constln_ref;
                                                            //mCRL2log(log::verbose) << "t->to_constln_ref: " << t->to_constln_ref << " " << t->to_constln_ref->C->id << "\n";
                                                            to_constlns_element_T* lp;
                                                            if (l->new_element != NULL) {
                                                                 //mCRL2log(log::verbose) << "point to new element " << l->new_element << " " << l->new_element->C->id << "\n";
                                                                 lp = l->new_element;
                                                            }
                                                            else {
                                                                 //mCRL2log(log::verbose) << Bhatp->constellation->id << "\n";
                                                                 //assert(Bhatp->inconstln_ref == NULL || l->C != Bhatp->constellation);
                                                                 lp = new to_constlns_element_T(l->C);
                                                                 l->new_element = lp;
                                                                 //mCRL2log(log::verbose) << "create new element " << l->new_element << " " << l->new_element->C->id << "\n";
                                                                 // Different from pseudo-code: point lp->new_element back to l, to efficiently reset
                                                                 // new_element pointers of 'old' elements later
                                                                 lp->new_element = l;
                                                                 //mCRL2log(log::verbose) << "pointing " << lp << "back to " << l << "\n";
                                                                 // add lp to Bhatp.to_constlns (IMPLIED IN PSEUDO-CODE)
                                                                 Bhatp->to_constlns->insert_linked(lp);
                                                                 // possibly set Bhatp.inconstln_ref
                                                                 if (l->C == Bhatp->constellation) {
                                                                      //mCRL2log(log::verbose) << "setting inconstln " << lp << " " << lp->C->id << "\n";
                                                                      Bhatp->inconstln_ref = lp;
                                                                 }
                                                            }
                                                            l->trans_list->remove_linked(t);
                                                            lp->trans_list->insert_linked(t);
                                                            t->to_constln_ref = lp;
                                                            //if (Bhatp->id == 355) {
                                                            //     mCRL2log(log::verbose) << "setting t->to_constln_ref to " << lp << "\n";
                                                            //}
                                                            //t.block_constln_list = lp.trans_list;
                                                            // postpone deleting element l until cleaning up new_element pointers
                                                       }
                                                       else {
                                                            state_T* sp = t->target;
                                        
                                                            if (sp->block == Bhat && (N == &L ? !sp->is_in_L_detect1 : !sp->is_in_Lp_detect2)) {
                                                                 // an inert transition becomes non-inert
                                                                 s->inert_cnt--;
                                                                 if (s->inert_cnt == 0) {
                                                                      XBpp.insert(s);
                                                                      Bhatp->non_btm_states->remove_linked(s);
                                                                      Bhatp->btm_states->insert_state_linked(s, BTM_STATE);
                                                                 }
                                                                 // Different from pseudo-code: add the transition to the corresponding trans_list
                                                                 if (Bhatp->inconstln_ref == NULL) {
                                                                      Bhatp->inconstln_ref = new to_constlns_element_T(Bhatp->constellation);
                                                                      Bhatp->to_constlns->insert_linked(Bhatp->inconstln_ref);
                                                                      if (Bhat->inconstln_ref != NULL) {
                                                                           Bhat->inconstln_ref->new_element = Bhatp->inconstln_ref;
                                                                           Bhatp->inconstln_ref->new_element = Bhat->inconstln_ref;
                                                                      }
                                                                 }
                                                                 Bhatp->inconstln_ref->trans_list->insert_linked(t);
                                                                 t->to_constln_ref = Bhatp->inconstln_ref;
                                                                 //if (Bhatp->id == 355) {
                                                                 //     mCRL2log(log::verbose) << "2479: setting t->to_toconstln_ref to " << Bhatp->inconstln_ref << "\n";
                                                                 //}
                                                            }
                                                       }
                                                  }
                                                  for (auto tit = s->Tsrc->begin(); tit != s->Tsrc->end(); ++tit) {
                                                       transition_T* t = *tit;
                                                       state_T* sp = t->source;
                                        
                                                       if (sp->block == Bhat && (N == &L ? !sp->is_in_L_detect1 : !sp->is_in_Lp_detect2)) {
                                                            // an inert transition becomes non-inert
                                                            sp->inert_cnt--;
                                                            if (sp->inert_cnt == 0) {
                                                                 XBp.insert(sp);
                                                                 Bhat->non_btm_states->remove_linked(sp);
                                                                 Bhat->btm_states->insert_state_linked(sp, BTM_STATE);
                                                            }
                                                            // Different from pseudo-code: add the transition to the corresponding trans_list
                                                            if (Bhat->inconstln_ref == NULL) {
                                                                 Bhat->inconstln_ref = new to_constlns_element_T(Bhat->constellation);
                                                                 Bhat->to_constlns->insert_linked(Bhat->inconstln_ref);
                                                                 if (Bhatp->inconstln_ref != NULL) {
                                                                      Bhatp->inconstln_ref->new_element = Bhat->inconstln_ref;
                                                                      Bhat->inconstln_ref->new_element = Bhatp->inconstln_ref;
                                                                 }
                                                            }
                                                            Bhat->inconstln_ref->trans_list->insert_linked(t);
                                                            t->to_constln_ref = Bhat->inconstln_ref;
                                                            //if (Bhat->id == 355) {
                                                            //     mCRL2log(log::verbose) << "2507: setting t->to_constln_ref to " << Bhat->inconstln_ref << "\n";
                                                            //}
                                                       }
                                                  }
                                             }
                                             sized_forward_list < state_T >* SinSC;
                                             sized_forward_list < state_T >* SnotinSC;
                                             if (detect1_finished == TERMINATED) {
                                                  // keep temp value in SinSC for swapping
                                                  SinSC = Bhatp->new_btm_states;
                                                  Bhatp->new_btm_states = Bhat->new_btm_states;
                                                  Bhat->new_btm_states = SinSC;
                                                  SinSC = Bhatp->new_btm_states;
                                                  SnotinSC = Bhat->new_btm_states;
                                             }
                                             else {
                                                  SinSC = Bhat->new_btm_states;
                                                  SnotinSC = Bhatp->new_btm_states;
                                             }
                                             // Walk through new states and add them to SnotinSC if they are not in SClist
                                             auto prev_sit = SinSC->before_begin();
                                             typename std::forward_list <state_T*>::iterator it_SClist;
                                             if (!sclist_is_empty_detect2) {
                                                  it_SClist = e->SClist->begin();
                                             }
                                             for (auto sit = SinSC->begin(); sit != SinSC->end(); ++sit) {
                                                  state_T* s = *sit;
                                                  if (sclist_is_empty_detect2) {
                                                       SinSC->remove(prev_sit, sit);
                                                       SnotinSC->insert_back(s);
                                                  }
                                                  else if (it_SClist == e->SClist->end()) {
                                                       SinSC->remove(prev_sit, sit);
                                                       SnotinSC->insert_back(s);
                                                  }
                                                  else if (s != *it_SClist) {
                                                       SinSC->remove(prev_sit, sit);
                                                       SnotinSC->insert_back(s);
                                                  }
                                                  else {
                                                       it_SClist++;
                                                  }
                                                  prev_sit = sit;
                                             }
                                             for (auto bit = Bhat->to_constlns->begin(); bit != Bhat->to_constlns->end(); ++bit) {
                                                  to_constlns_element_T* l = *bit;
                                                  // if SClist is empty, we can stop, since all subsequent elements will have empty SClists
                                                  if (l->SClist == NULL) {
                                                       break;
                                                  }
                                                  auto prev_sit = l->SClist->before_begin();
                                                  // 5.3.7.b.ii.A
                                                  for (auto sit = l->SClist->begin(); sit != l->SClist->end(); ++sit) {
                                                       state_T* s = *sit;
                                                       if (s->block == Bhatp) {
                                                            // new_element still points to corresponding element of Bhatp
                                                            // Note that we cannot have l->new_element == NULL, since then, l would be a new entry with an empty SClist
                                                            if (l->new_element->SClist == NULL) {
                                                                 l->new_element->SClist = new sized_forward_list < state_T >;
                                                                 // move to front in list
                                                                 Bhatp->to_constlns->remove_linked(l->new_element);
                                                                 Bhatp->to_constlns->insert_linked(l->new_element);
                                                            }
                                                            l->new_element->SClist->insert_back(s);
                                                            l->SClist->remove(prev_sit, sit);
                                                       }
                                                       prev_sit = sit;
                                                  }
                                                  // 5.3.7.b.ii.B
                                                  if (l->SClist->size() == 0) {
                                                       deleteobject(l->SClist);
                                                       //mCRL2log(log::verbose) << "SC: " << l->SClist << "\n";
                                                       // move the l entry to the back of the list. We do not need to worry about keeping iterator e valid (in while loop), since we will not use it on the current list anymore.
                                                       Bhat->to_constlns->remove_linked(l, bit);
                                                       Bhat->to_constlns->insert_linked_back(l);
                                                  }
                                             }
                                             // reset temporary pointers of states
                                             for (auto it = L.begin(); it != L.end(); ++it) {
                                                  state_T* s = *it;
                                                  s->is_in_L_detect1 = false;
                                             }
                                             for (auto it = Lp.begin(); it != Lp.end(); ++it) {
                                                  state_T* s = *it;
                                                  s->is_in_Lp_detect2 = false;
                                             }
                                             while (!P->empty()) {
                                                  state_T* s = *(P->begin());
                                                  P->erase(P->begin());
                                                  s->is_in_P_detect2 = false;
                                                  //mCRL2log(log::verbose) << "remove from P: " << s->id << "\n";
                                             }
                                             // Different from pseudo-code: reset temporary pointers (new elements) of blocks
                                             // 5.3.7.b.iii
                                             for (auto it = Bhatp->to_constlns->begin(); it != Bhatp->to_constlns->end(); ++it) {
                                                  to_constlns_element_T* l = *it;
                                                  if (l->new_element != NULL) {
                                                       if (size(l->new_element) == 0) {
                                                            if (l->new_element->C == Bhat->constellation) {
                                                                 Bhat->inconstln_ref = NULL;
                                                            }
                                                            Bhat->to_constlns->remove_linked(l->new_element);
                                                            deleteobject (l->new_element->trans_list);
                                                            deleteobject (l->new_element);
                                                       }
                                                       else {
                                                            l->new_element->new_element = NULL;
                                                       }
                                                       l->new_element = NULL;
                                                  }
                                             }
                                             // Different from pseudo-code: remove marking on states and reset constln_ref and coconstln_ref
                                             //clean_temp_refs_block(Bhat);
                                             //clean_temp_refs_block(Bhatp);
                                             // 5.3.5 (5.3.4)
                                             block_T* Btmp = Bhat;
                                             sized_forward_list<state_T> *XBtmp = &XBp;
                                             //check_consistency_transitions();
                                             //check_consistency_blocks();
                                             while (Btmp != NULL) {
                                                  //mCRL2log(log::verbose) << "processing new bottom states for " << Btmp->id << "\n";
                                                  for (auto sit = XBtmp->begin(); sit != XBtmp->end(); ++sit) {
                                                       state_T* s = *sit;
                                                       //mCRL2log(log::verbose) << "state " << s->id << "\n";
                                                       for (auto tit = s->Ttgt->begin(); tit != s->Ttgt->end(); ++tit) {
                                                            transition_T* t = *tit;
                                                            to_constlns_element_T* setBp_entry = t->to_constln_ref;
                                                            // 5.3.4.a.i
                                                            if (setBp_entry->SClist == NULL) {
                                                                 setBp_entry->SClist = new sized_forward_list < state_T >;
                                                                 // move the to_constln entry to the front of the list
                                                                 Btmp->to_constlns->remove_linked(setBp_entry);
                                                                 Btmp->to_constlns->insert_linked(setBp_entry);
                                                            }
                                                            // 5.3.4.a.ii
                                                            if (setBp_entry->SClist->size() == 0) {
                                                                 //mCRL2log(log::verbose) << "adding1 state to constln " << setBp_entry << " " << setBp_entry->C->id << "\n";
                                                                 setBp_entry->SClist->insert(s);
                                                            }
                                                            else if (setBp_entry->SClist->front() != s) {
                                                                 //mCRL2log(log::verbose) << "(" << t << "," << t->target->id << "): adding2 state " << s->id << " to constln " << setBp_entry << " " << setBp_entry->C->id << "\n";
                                                                 setBp_entry->SClist->insert(s);
                                                            }
                                                       }
                                                       // 5.3.4.a.iii
                                                       Btmp->new_btm_states->insert(s);
                                                  }
                                   
                                                  // consider next block
                                                  if (Btmp == Bhatp) {
                                                       break;
                                                  }
                                                  if (Btmp == Bhat) {
                                                       Btmp = Bhatp;
                                                       XBtmp = &XBpp;
                                                  }
                                             }
                                             // push resulting blocks on the work stack if they have new states
                                             // 5.3.7.b.iv
                                             if (Bhatp->new_btm_states->size() > 0) {
                                                  blocks_to_process.push(Bhatp);
                                             }
                                             if (Bhat->new_btm_states->size() > 0) {
                                                  blocks_to_process.push(Bhat);
                                             }
                                             break;
                                        } // end if splitcrit_met
                                        // Different from pseudo-code: we do not clean e->SClist and move e to the back.
                                        // We reset the list only later. This, since the original moving would lead to the list
                                        // of e representing a reason to split Bhat (an empty list is sufficient for that).
//                                        else {
//                                             // 5.3.5.b
//                                             deleteobject (e->SClist);
//                                             Bhat->to_constlns->remove_linked(e, cit);
//                                             Bhat->to_constlns->insert_linked_back(e);
//                                        }
                                   } // end of walk over to_constln of block
                                   // Different from pseudo-code: if block is stable, reset new_btm_states, remove remaining SClists
                                   // 5.3.7.c
                                   if (!split) {
                                        deleteobject(Bhat->new_btm_states);
                                        Bhat->new_btm_states = new sized_forward_list <state_T>;
                                        for (auto it = Bhat->to_constlns->begin(); it != Bhat->to_constlns->end(); ++it) {
                                             to_constlns_element_T* l = *it;
                                             if (l->SClist != NULL) {
                                                  deleteobject (l->SClist);
                                             }
                                             else {
                                                  break;
                                             }
                                        }
                                   }
                              } // end while loop for stabilising blocks
                              // 5.3.8
                              if (Bp->constellation->type == TRIVIAL) {
                                   Bp->constellation->type = NONTRIVIAL;
                                   trivial_constlns.remove_linked(Bp->constellation);
                                   non_trivial_constlns.insert_linked(Bp->constellation);
                              }
                         } // end walking over splittable blocks
                    } // end walk over non-trivial constellations
               } // end while there are non-trivial constellations
#ifndef NDEBUG
               //print_partition();
#endif
               mCRL2log(log::verbose) << "number of splits performed: " << nr_of_splits << "\n";
               mCRL2log(log::verbose) << num_eq_classes();
          }

#ifndef NDEBUG
    // The methods below are intended to check the consistency of the internal data
    // structure. Their sole purpose is to detect programming errors. They have no
    // side effects on the data structure. If a problem occurs, execution halts with
    // an assert.

          void check_non_empty_blocks() {
               sized_forward_list < constellation_T >* L = &trivial_constlns;
               while (L != NULL) {
                    for (auto cit = L->begin(); cit != L->end(); ++cit) {
                         constellation_T* C = *cit;
                         for (auto bit = C->blocks->begin(); bit != C->blocks->end(); ++bit) {
                              block_T* B = *bit;
                              assert(B->btm_states->size() + B->non_btm_states->size() + B->marked_btm_states->size() + B->marked_non_btm_states->size() > 0);
                         }
                    }
               
                    if (L == &trivial_constlns) {
                         L = &non_trivial_constlns;
                    }
                    else {
                         L = NULL;
                    }
               }
          }

          void check_consistency_transitions() {
               bool found = false;
               for (auto sit = states.begin(); sit != states.end(); ++sit) {
                    state_T* s = *sit;
                    for (auto tit = s->Ttgt->begin(); tit != s->Ttgt->end(); ++tit) {
                         transition_T* t = *tit;
                         block_T* B = t->source->block;
                         found = false;
                         if (t->to_constln_ref != NULL) {
                              assert(t->target->block->constellation == t->to_constln_ref->C);
                              for (auto eit = B->to_constlns->begin(); eit != B->to_constlns->end(); ++eit) {
                                   to_constlns_element_T* e = *eit;
                                   if (e == t->to_constln_ref) {
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
               //size_t count2;
               while (L != NULL) {
                    for (auto cit = L->begin(); cit != L->end(); ++cit) {
                         constellation_T* C = *cit;
                         for (auto bit = C->blocks->begin(); bit != C->blocks->end(); ++bit) {
                              block_T* B = *bit;
                              // walk over elements in to_constlns
                              //count2 = 0;
                              std::forward_list < to_constlns_element_T* >::iterator prev_eit = B->to_constlns->before_begin();
                              for (auto eit = B->to_constlns->begin(); eit != B->to_constlns->end(); ++eit) {
                                   to_constlns_element_T* e = *eit;
                                   assert(e->trans_list != NULL);
                                   //if (e->C->id == 12) {
                                   //     if (B->id == 10 || B->id == 39) {
                                   //          mCRL2log(log::verbose) << B->id << ": found const 12 at " << e << "\n";
                                   //     }
                                   //     count2++;
                                   //}
                                   std::forward_list < transition_T* >::iterator prev_tit = e->trans_list->before_begin();
                                   for (auto tit = e->trans_list->begin(); tit != e->trans_list->end(); ++tit) {
                                        transition_T* t = *tit;
                                        assert(t->ptr_in_list == prev_tit);
                                        assert(t->to_constln_ref == e);
                                        //mCRL2log(log::verbose) << t->source->id << "\n";
                                        assert(t->source->block == B);
                                        assert(t->target->block->constellation == e->C);
                                        // assert that t->to_constln_ref is valid
                                        count = 0;
                                        for (auto eit2 = B->to_constlns->begin(); eit2 != B->to_constlns->end(); ++eit2) {
                                             to_constlns_element_T* l = *eit2;
                                             if (l == t->to_constln_ref) {
                                                  count = 1;
                                                  break;
                                             }
                                        }
                                        assert(count == 1);
                                        prev_tit = tit;
                                   }
                                   //assert(e->new_element == NULL);
                                   //assert(e->SClist == NULL);
                                   assert(e->ptr_in_list == prev_eit);
                                   
                                   prev_eit = eit;
                              }
                              //assert (count2 < 2);
                         }
                    }
                    if (L == &trivial_constlns) {
                         L = &non_trivial_constlns;
                    }
                    else {
                         L = NULL;
                    }
               }
          }

          void check_consistency_state_list(block_T* B, state_type t)
          {
               sized_forward_list < state_T >* list;
               switch (t) {
                    case BTM_STATE:
                         list = B->btm_states;
                         break;
                    case NON_BTM_STATE:
                         list = B->non_btm_states;
                         break;
                    case MARKED_BTM_STATE:
                         list = B->marked_btm_states;
                         break;
                    case MARKED_NON_BTM_STATE:
                         list = B->marked_non_btm_states;
                         break;
               }
               std::forward_list < state_T* >::iterator prev_sit = list->before_begin();
               for (auto sit = list->begin(); sit != list->end(); ++sit) {
                    state_T* s = *sit;
                    assert(s->block == B);
                    mCRL2log(log::verbose) << B->id << ": " << s->id << ", " << &(*(s->ptr_in_list)) << " " << &(*prev_sit) << "\n";
                    assert(prev_sit == s->ptr_in_list);
                    assert(s->type == t);
                    assert(s->constln_cnt == NULL);
                    assert(s->coconstln_cnt == NULL);
                    assert(s->is_in_L_detect1 == false);
                    assert(s->is_in_Lp_detect2 == false);
                    assert(s->is_in_P_detect2 == false);
                    size_t local_inert_cnt = 0;
                    // check outgoing transitions
                    for (auto tit = s->Ttgt->begin(); tit != s->Ttgt->end(); ++tit) {
                         transition_T* t = *tit;
                         assert(t->source == s);
                         // TODO: CHECK TO_CONSTLN_CNT
                         mCRL2log(log::verbose) << "cnts: " << t->target->block << " " << B << "\n";
                         if (t->target->block == B) {
                              local_inert_cnt++;
                              mCRL2log(log::verbose) << "inc " << local_inert_cnt << "\n";
                         }
                    }
                    mCRL2log(log::verbose) << local_inert_cnt << " " << s->inert_cnt << "\n";
                    assert(s->inert_cnt == local_inert_cnt);
                    // check incoming transitions
                    for (auto tit = s->Tsrc->begin(); tit != s->Tsrc->end(); ++tit) {
                         transition_T* t = *tit;
                         assert(t->target == s);
                         // TODO: CHECK TO_CONSTLN_CNT
                         assert(t->to_constln_ref->C == B->constellation);
                    }
                    
                    prev_sit = sit;
               }
          }

    void check_internal_consistency_of_constellations(
               const size_t consttype,
      const bool branching)
    {
               // check the constellations of given type and blocks
               sized_forward_list < constellation_T >* Clist;
               if (consttype == TRIVIAL) {
                    Clist = &trivial_constlns;
               }
               else {
                    Clist = &non_trivial_constlns;
               }
               std::forward_list < constellation_T* >::iterator prev_cit = Clist->before_begin();
               for (auto cit = Clist->begin(); cit != Clist->end(); ++cit) {
                    constellation_T* C = *cit;
                    // check consistency of ptr_in_list
                    assert(prev_cit == C->ptr_in_list);
                    size_t const_size = 0;
                    std::forward_list < block_T* >::iterator prev_bit = C->blocks->before_begin();
                    for (auto bit = C->blocks->begin(); bit != C->blocks->end(); ++bit) {
                         block_T* B = *bit;
                         const_size += B->btm_states->size() + B->non_btm_states->size() + B-> marked_btm_states->size() + B->marked_non_btm_states->size();
                         // check consistency of ID (for now, should be <= 100)
                         //assert(B->id <= 100);
                         assert(B->type == STD_BLOCK || B->type == EXTRA_KRIPKE_BLOCK);
                         assert(B->constellation == C);
                         assert(B->constellation->type == consttype);
                         // check consistency of ptr_in_list
                         assert(prev_bit == B->ptr_in_list);
                         // empty state lists
                         assert(B->marked_btm_states->size() == 0);
                         assert(B->marked_non_btm_states->size() == 0);
                         assert(B->new_btm_states->size() == 0);
                         assert(B->constln_ref == NULL);
                         assert(B->coconstln_ref == NULL);
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

    void check_internal_consistency_of_the_partitioning_data_structure_gw(
      const bool branching)
    {
               // check the constellations and blocks
               check_internal_consistency_of_constellations(TRIVIAL, branching);
               check_internal_consistency_of_constellations(NONTRIVIAL, branching);
    }
     
          void check_consistency_trans_lists(block_T* split_block, constellation_T* split_const)
          {
               // check consistency of the block to constellation transition lists
               sized_forward_list < constellation_T >* L = &trivial_constlns;
               while (L != NULL) {
                    for (auto cit = L->begin(); cit != L->end(); ++cit) {
                         constellation_T* C = *cit;
                         for (auto bit = C->blocks->begin(); bit != C->blocks->end(); ++bit) {
                              block_T* B = *bit;
                              // traverse the lists
                              for (auto eit = B->to_constlns->begin(); eit != B->to_constlns->end(); ++eit) {
                                   to_constlns_element_T* e = *eit;
                                   if (e->C == split_const) {
                                        for (auto tit = e->trans_list->begin(); tit != e->trans_list->end(); ++tit) {
                                             transition_T* t = *tit;
                                             //assert(t->target->block->constellation == e->C);
                                             if (t->source->coconstln_cnt != NULL) {
                                                       assert(t->source->coconstln_cnt->cnt > 0);
                                             }
                                        }
                                   }
                              }
                         }
                    }
               
               
                    if (L == &trivial_constlns) {
                         L = &non_trivial_constlns;
                    }
                    else {
                         L = NULL;
                    }
               }
          }
     
          void check_consistency_trans_lists_2(constellation_T* split_const)
          {
               // check consistency of the block to constellation transition lists
               sized_forward_list < constellation_T >* L = &trivial_constlns;
               while (L != NULL) {
                    for (auto cit = L->begin(); cit != L->end(); ++cit) {
                         constellation_T* C = *cit;
                         for (auto bit = C->blocks->begin(); bit != C->blocks->end(); ++bit) {
                              block_T* B = *bit;
                              // traverse the lists
                              for (auto eit = B->to_constlns->begin(); eit != B->to_constlns->end(); ++eit) {
                                   to_constlns_element_T* e = *eit;
                                   if (e->C == split_const) {
                                        for (auto tit = e->trans_list->begin(); tit != e->trans_list->end(); ++tit) {
                                             transition_T* t = *tit;
                                             assert(t->source->coconstln_cnt == NULL);
                                        }
                                   }
                              }
                         }
                    }
               
               
                    if (L == &trivial_constlns) {
                         L = &non_trivial_constlns;
                    }
                    else {
                         L = NULL;
                    }
               }
          }
#endif // not NDEBUG

};


/** \brief Reduce transition system l with respect to strong or (divergence preserving) branching bisimulation.
 * \param[in/out] l The transition system that is reduced.
 * \param[in] branching If true branching bisimulation is applied, otherwise strong bisimulation.
 * \param[in] preserve_divergences Indicates whether loops of internal actions on states must be preserved. If false
 *            these are removed. If true these are preserved.  */
template < class LTS_TYPE>
void bisimulation_reduce_gw(
  LTS_TYPE& l,
  const bool branching = false);


/** \brief Checks whether the two initial states of two lts's are strong or branching bisimilar.
 * \details This lts and the lts l2 are not usable anymore after this call.
 *          The space consumption is O(n) and time is O(m log n). It uses the branching bisimulation
 *          algorithm by Groote and Wijs from 2015.
 * \param[in/out] l1 A first transition system.
 * \param[in/out] l2 A second transistion system.
 * \param[branching] If true branching bisimulation is used, otherwise strong bisimulation is applied.
 * \param[preserve_divergences] If true and branching is true, preserve tau loops on states.
 * \retval True iff the initial states of the current transition system and l2 are (divergence preserving) (branching) bisimilar */
template < class LTS_TYPE>
bool destructive_bisimulation_compare_gw(
  LTS_TYPE& l1,
  LTS_TYPE& l2,
  const bool branching=false,
  const bool preserve_divergences=false,
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
 * \param[preserve_divergences] If true and branching is true, preserve tau loops on states.
 * \retval True iff the initial states of the current transition system and l2 are (divergence preserving) (branching) bisimilar */
template < class LTS_TYPE>
bool bisimulation_compare_gw(
  const LTS_TYPE& l1,
  const LTS_TYPE& l2,
  const bool branching=false);


template < class LTS_TYPE>
void bisimulation_reduce_gw(LTS_TYPE& l,
                         const bool branching) /*=false */
{
  // First, remove tau loops in case of branching bisimulation.
  if (branching)
  {
    scc_reduce(l,false);
  }

  // Secondly, apply the branching bisimulation reduction algorithm. If there are no tau's,
  // this will automatically yield strong bisimulation.
  detail::bisim_partitioner_gw<LTS_TYPE> bisim_part(l, branching);

  // Clear the state labels of the LTS l
  l.clear_state_labels();

  // Assign the reduced LTS
  l.set_num_states(bisim_part.num_eq_classes());
  l.set_initial_state(bisim_part.get_eq_class(l.initial_state()));
  bisim_part.replace_transitions(branching);
}

template < class LTS_TYPE>
bool bisimulation_compare_gw(
  const LTS_TYPE& l1,
  const LTS_TYPE& l2,
  const bool branching) /* =false*/
{
  LTS_TYPE l1_copy(l1);
  LTS_TYPE l2_copy(l2);
  return destructive_bisimulation_compare_gw(l1_copy,l2_copy,branching);
}

template < class LTS_TYPE>
bool destructive_bisimulation_compare_gw(
  LTS_TYPE& l1,
  LTS_TYPE& l2,
  const bool branching) /* =false*/
{
  size_t init_l2 = l2.initial_state() + l1.num_states();
  mcrl2::lts::detail::merge(l1,l2);
  l2.clear(); // No use for l2 anymore.


  // First remove tau loops in case of branching bisimulation.
  if (branching)
  {
    detail::scc_partitioner<LTS_TYPE> scc_part(l1);
    scc_part.replace_transitions(false);
    l1.set_num_states(scc_part.num_eq_classes());
    l1.set_initial_state(scc_part.get_eq_class(l1.initial_state()));
    init_l2 = scc_part.get_eq_class(init_l2);
  }

  detail::bisim_partitioner_gw<LTS_TYPE> bisim_part(l1, branching);
  return bisim_part.in_same_class(l1.initial_state(),init_l2);
}

}
}
}
#endif
