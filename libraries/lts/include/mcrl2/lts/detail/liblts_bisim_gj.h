// Author(s): Jan Friso Groote
//
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file lts/detail/liblts_bisim_gj.h
///
/// \brief O(m log n)-time branching bisimulation algorithm similar to liblts_bisim_dnj.h
///        which does not use bunches, i.e., partitions of transitions. This algorithm
///        should be slightly faster, but in particular use less memory than liblts_bisim_dnj.h.
///        Otherwise the functionality is exactly the same. 
///

#ifndef LIBLTS_BISIM_GJ_H
#define LIBLTS_BISIM_GJ_H

#include <forward_list>
#include <list>
#include "mcrl2/utilities/hash_utility.h"
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_merge.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{
namespace bisimulation_gj
{

// Forward declaration.
struct block_type;
struct transition_type;
struct transition_pointer_pair;

typedef std::size_t state_index;
typedef std::size_t action_index;
typedef std::size_t transition_index;
typedef std::size_t block_index;
typedef std::size_t constellation_index;
typedef std::vector<transition_pointer_pair>::iterator outgoing_transitions_it;

constexpr transition_index null_transition=-1;
constexpr action_index null_action=-1;
constexpr state_index null_state=-1;


struct transition_pointer_pair
{
  transition_index transition;
  outgoing_transitions_it start_same_saC; // Refers to the last state with the same state, action and constellation,
                                          // unless it is the last, which refers to the first state.
  transition_pointer_pair(const transition_index &t, const outgoing_transitions_it& sssaC)
   : transition(t),
     start_same_saC(sssaC)
  {}
};

// Below the four main data structures are listed.
struct state_type_gj
{
  block_index block=0;
  std::vector<transition_index>::iterator start_incoming_inert_transitions;
  std::vector<transition_index>::iterator start_incoming_non_inert_transitions;
  outgoing_transitions_it start_outgoing_transitions;
  std::vector<state_index>::iterator ref_states_in_blocks;
  std::size_t no_of_outgoing_inert_transitions=0;
};

struct transition_type
{
  // The position of the transition type corresponds to m_aut.get_transitions(). 
  // std::size_t from, label, to are found in m_aut.get_transitions().
  std::list<transition_index>::iterator transitions_per_block_to_constellation;
  transition_index previous_LBC=null_transition;
  transition_index next_LBC=null_transition;
  std::vector<transition_index>::iterator ref_incoming_transitions;
  outgoing_transitions_it ref_outgoing_transitions;
};

struct block_type
{
  constellation_index constellation;
  std::vector<state_index>::iterator start_bottom_states;
  std::vector<state_index>::iterator start_non_bottom_states;
  std::vector<state_index>::iterator end_states;
  // The list below seems too expensive. Maybe a cheaper construction is possible. Certainly the size of the list is not important. 
  std::list< transition_index > block_to_constellation;

  block_type(const std::vector<state_index>::iterator beginning_of_states, constellation_index c)
    : constellation(c),
      start_bottom_states(beginning_of_states),
      start_non_bottom_states(beginning_of_states),
      end_states(beginning_of_states)
  {}
};

struct constellation_type
{
  std::forward_list<block_index> blocks;

  constellation_type(const block_index bi)
  {
    blocks.push_front(bi);
  }
};

// The struct below facilitates to walk through a LBC_list starting from an arbitrary transition.
struct LBC_list_iterator
{
  typedef std::forward_iterator_tag iterator_category;
  typedef transition_index value_type;

  const std::vector<transition_type>& m_transitions;
  transition_index m_current_transition;

  LBC_list_iterator(const transition_index ti, const std::vector<transition_type>& transitions)
    : m_transitions(transitions),
      m_current_transition((ti==null_transition?null_transition:*m_transitions[ti].transitions_per_block_to_constellation))
  {}

  LBC_list_iterator operator++()
  {
    assert(m_current_transition!=null_transition);
    m_current_transition=m_transitions[m_current_transition].next_LBC;
    return *this;
  }

  LBC_list_iterator operator++(int)
  {
    LBC_list_iterator temp=*this;
    assert(m_current_transition!=null_transition);
    m_current_transition=m_transitions[m_current_transition].next_LBC;
    return temp;
  }

  transition_index operator *() const
  {
    assert(m_current_transition!=null_transition);
    return m_current_transition;
  }

  // Equality is implemented minimally for the purpose of this algorithm,
  // essentially only intended to compare the iterator to its end, i.e., null_transition. 
  bool operator ==(const LBC_list_iterator& other) const
  {
    return m_current_transition==other.m_current_transition;
  }

  bool operator !=(const LBC_list_iterator& other) const
  {
    return !operator==(other);
  }
};

// The struct below facilitates to walk through a LBC_list yielding the initial states. 
struct LBC_list_state_iterator
{
  const std::vector<transition_type>& m_transitions;
  const std::vector<transition>& m_aut_transitions;
  transition_index m_current_transition;

  LBC_list_state_iterator(const transition_index ti, 
                            const std::vector<transition_type>& transitions, 
                            const std::vector<transition>& aut_transitions)
    : m_transitions(transitions),
      m_aut_transitions(aut_transitions),
      m_current_transition((ti==null_transition?null_transition:*m_transitions[ti].transitions_per_block_to_constellation))
  {}

  void operator++()
  {
    assert(m_current_transition!=null_transition);
    m_current_transition=m_transitions[m_current_transition].next_LBC;
  }

  state_index operator *() const
  {
    assert(m_current_transition!=null_transition);
    return m_aut_transitions[m_current_transition].from();
  }

  // Equality is implemented minimally for the purpose of this algorithm,
  // essentially only intended to compare the iterator to its end, i.e., null_transition. 
  bool operator ==(const LBC_list_state_iterator& other) const
  {
    return m_current_transition==other.m_current_transition;
  }

  bool operator !=(const LBC_list_state_iterator& other) const
  {
    return !operator==(other);
  }
};

} // end namespace bisimulation_gj


/*=============================================================================
=                                 main class                                  =
=============================================================================*/


using namespace mcrl2::lts::detail::bisimulation_gj;

/// \class bisim_partitioner_gj
/// \brief implements the main algorithm for the branching bisimulation quotient
template <class LTS_TYPE>
class bisim_partitioner_gj
{
  protected:

    typedef typename LTS_TYPE::labels_size_type label_index;
    typedef typename LTS_TYPE::states_size_type state_index;
    typedef std::unordered_set<state_index> set_of_states_type;
    typedef std::unordered_set<state_index> set_of_transitions_type;
    typedef std::unordered_set<constellation_index> set_of_constellations_type;
    typedef std::unordered_map<label_index, set_of_states_type > states_per_action_label_type;
    typedef std::unordered_map<label_index, set_of_transitions_type > transitions_per_action_label_type;
    typedef std::unordered_map<block_index, set_of_states_type > states_per_block_type;
    typedef std::unordered_map<std::pair<state_index, label_index>, std::size_t> state_label_to_size_t_map;
    typedef std::unordered_map<std::pair<block_index, label_index>, std::list<transition_index>::iterator >
                      block_label_to_list_iterator_map;
    typedef std::unordered_map<std::pair<label_index, constellation_index>, set_of_states_type> 
                      label_constellation_to_set_of_states_map;
    typedef std::unordered_map<std::pair<block_index, label_index>, std::size_t> block_label_to_size_t_map;
    typedef std::unordered_map<std::pair<state_index, label_index>, transition_index> state_label_to_transition_map;

    set_of_states_type& empty_state_set()
    {
      static const set_of_states_type s=set_of_states_type();
      return s;
    }

    /// \brief automaton that is being reduced
    LTS_TYPE& m_aut;
    
    // Generic data structures.
    std::vector<state_type_gj> m_states;
    std::vector<transition_index> m_incoming_transitions;
    std::vector<transition_pointer_pair> m_outgoing_transitions;
    std::vector<transition_type> m_transitions;
    std::vector<state_index> m_states_in_blocks;
    std::vector<block_type> m_blocks;
    std::vector<constellation_type> m_constellations;
    std::vector<state_index> m_P;
    /// The following variable contains all non trivial constellations.
    set_of_constellations_type m_non_trivial_constellations;


    /// \brief true iff branching (not strong) bisimulation has been requested
    const bool m_branching;
  
    /// \brief true iff divergence-preserving branching bisimulation has been
    /// requested
    /// \details Note that this field must be false if strong bisimulation has
    /// been requested.  There is no such thing as divergence-preserving strong
    /// bisimulation.
    const bool m_preserve_divergence;

#ifndef NDEBUG // This suppresses many unused variable warnings. 
    void check_transitions() const
    {
      for(std::size_t ti=0; ti<m_transitions.size(); ++ti)
      {
        if (m_transitions[ti].previous_LBC!=null_transition)
        {
          assert(ti==m_transitions[m_transitions[ti].previous_LBC].next_LBC);
        }
        if (m_transitions[ti].next_LBC!=null_transition)
        {           
          assert(ti==m_transitions[m_transitions[ti].next_LBC].previous_LBC);
        }       
        bool found=false;
        for(LBC_list_iterator tti=LBC_list_iterator(ti,m_transitions); tti!=LBC_list_iterator(null_transition,m_transitions); ++tti)
        {
          if (*tti==ti)
          {
            found=true;
          }
        }
        assert(found);
      }
    }

    bool check_data_structures(const std::string& tag, const bool initialisation=false) const
    {
      mCRL2log(log::debug) << "Check data structures: " << tag << ".\n";
      assert(m_states.size()==m_aut.num_states());
      assert(m_incoming_transitions.size()==m_aut.num_transitions());
      assert(m_outgoing_transitions.size()==m_aut.num_transitions());

      // Check that the elements in m_states are well formed. 
      for(state_index si=0; si< m_states.size(); si++)
      {
        const state_type_gj& s=m_states[si];

        assert(m_blocks[s.block].start_bottom_states<m_blocks[s.block].start_non_bottom_states);
        assert(m_blocks[s.block].start_non_bottom_states<=m_blocks[s.block].end_states);

        assert(std::find(m_blocks[s.block].start_bottom_states, m_blocks[s.block].end_states,si)!=m_blocks[s.block].end_states);

        for(std::vector<transition_index>::iterator it=s.start_incoming_inert_transitions;
                        it!=s.start_incoming_non_inert_transitions; ++it)
        {
          const transition& t=m_aut.get_transitions()[*it];
          assert(t.to()==si);
          assert(m_transitions[*it].ref_incoming_transitions==it);
          // Check that inert transitions come first. 
          assert(m_aut.is_tau(t.label()) && m_states[t.from()].block==m_states[t.to()].block);
        }

        for(std::vector<transition_index>::iterator it=s.start_incoming_non_inert_transitions;
                        it!=m_incoming_transitions.end() &&
                        (si+1>=m_states.size() || it!=m_states[si+1].start_incoming_inert_transitions);
                     ++it)
        {
          const transition& t=m_aut.get_transitions()[*it];
          assert(t.to()==si);
          assert(m_transitions[*it].ref_incoming_transitions==it);
          // Check that inert transitions come first. 
          assert(!m_aut.is_tau(t.label()) || m_states[t.from()].block!=m_states[t.to()].block);
        }

        for(outgoing_transitions_it it=m_states[si].start_outgoing_transitions;
                        it!=m_outgoing_transitions.end() &&
                        (si+1>=m_states.size() || it!=m_states[si+1].start_outgoing_transitions);
                     ++it)
        {
          const transition& t=m_aut.get_transitions()[it->transition];
          assert(t.from()==si);
          assert(m_transitions[it->transition].ref_outgoing_transitions==it);
          assert((it->start_same_saC>it && it+1!=m_outgoing_transitions.end() && 
                        ((it+1)->start_same_saC==it->start_same_saC || (it+1)->start_same_saC<=it)) || 
                 (it->start_same_saC<=it && (it+1==m_outgoing_transitions.end() || (it+1)->start_same_saC>it)));
          const transition& t1=m_aut.get_transitions()[it->start_same_saC->transition];
          assert(t1.from()==si);
          assert(t.label()==t1.label());
          assert(m_blocks[m_states[t.to()].block].constellation==m_blocks[m_states[t1.to()].block].constellation);
        }
        assert(*(s.ref_states_in_blocks)==si);

        // Check that for each state the outgoing transitions satisfy the following invariant.
        // First there are inert transitions. Then there are the transitions belonging to blocks 
        // in the same constellation, and subsequently there are other transitions sorted per block
        // and constellation. 
        std::unordered_set<std::pair<action_index, constellation_index>> constellations_seen;
        for(outgoing_transitions_it it=m_states[si].start_outgoing_transitions;
                        it!=m_outgoing_transitions.end() &&
                        (si+1>=m_states.size() || it!=m_states[si+1].start_outgoing_transitions);
                     ++it)
        {
          const transition& t=m_aut.get_transitions()[it->transition];
          // Check that if the target constellation, if not new, is equal to the target constellation of the previous outgoing transition.
          if (constellations_seen.count(std::pair(t.label(),m_blocks[m_states[t.to()].block].constellation))>0)
          {
            assert(it!=m_states[si].start_outgoing_transitions);
            assert(m_blocks[m_states[m_aut.get_transitions()[(it-1)->transition].to()].block].constellation==
                   m_blocks[m_states[t.to()].block].constellation);
          }
          constellations_seen.insert(std::pair(t.label(),m_blocks[m_states[t.to()].block].constellation));
        }
      }
      // Check that the elements in m_transitions are well formed. 
      if (!initialisation)
      {
        check_transitions();
      }
      // Check that the elements in m_blocks are well formed. 
      set_of_transitions_type all_transitions;
      for(block_index bi=0; bi<m_blocks.size(); ++bi)
      {
        const block_type& b=m_blocks[bi];
        const constellation_type& c=m_constellations[b.constellation];
        assert(std::find(c.blocks.begin(),c.blocks.end(),bi)!=c.blocks.end());
        assert(b.start_bottom_states<m_states_in_blocks.end());
        assert(b.start_bottom_states>=m_states_in_blocks.begin());
        assert(b.start_non_bottom_states<=m_states_in_blocks.end()); 
        assert(b.start_non_bottom_states>=m_states_in_blocks.begin());
        
        for(typename std::vector<state_index>::iterator is=b.start_bottom_states;
                   is!=b.start_non_bottom_states; ++is)
        {
          const state_type_gj& s=m_states[*is];
          assert(s.block==bi);
          assert(s.no_of_outgoing_inert_transitions==0);
        }
        for(typename std::vector<state_index>::iterator is=b.start_non_bottom_states;
                   is!=b.end_states; ++is)
        {
          const state_type_gj& s=m_states[*is];
          assert(s.block==bi);
          assert(s.no_of_outgoing_inert_transitions>0);
        }

        std::size_t count=0;
        for(std::list< transition_index >::const_iterator ti=b.block_to_constellation.begin();
                     ti!=b.block_to_constellation.end(); ti++)
        {
          transition first_transition=m_aut.get_transitions()[*ti];
          assert(count==0 || 
                 !(m_states[first_transition.from()].block==m_states[first_transition.to()].block &&
                   m_aut.is_tau(first_transition.label())));
          count++;
          for(LBC_list_iterator i(*ti,m_transitions); i!=LBC_list_iterator(null_transition,m_transitions); ++i)
          {
            const transition& t=m_aut.get_transitions()[*i];
            all_transitions.insert(*i);
            assert(m_states[t.from()].block==bi);
            assert(m_blocks[m_states[t.to()].block].constellation==
                               m_blocks[m_states[first_transition.to()].block].constellation);
            assert(t.label()==first_transition.label());
          }
        }
      }
      assert(initialisation || all_transitions.size()==m_transitions.size());

      // TODO Check that the elements in m_constellations are well formed. 
      std::unordered_set<block_index> all_blocks;
      for(constellation_index ci=0; ci<m_constellations.size(); ci++)
      {
        for(const block_index bi: m_constellations[ci].blocks)
        {
          assert(bi<m_blocks.size());
          assert(all_blocks.insert(bi).second);  // Block is not already present. Otherwise a block occurs in two constellations. 
        }
      }
      assert(all_blocks.size()==m_blocks.size());

      // Check that the states in m_states_in_blocks refer to with ref_states_in_block to the right position.
      // and that a state is correctly designated as a (non-)bottom state. 
      for(typename std::vector<state_index>::const_iterator si=m_states_in_blocks.begin(); si!=m_states_in_blocks.end(); ++si)
      {
        assert(si==m_states[*si].ref_states_in_blocks);
      }
      
      // Check that the states in m_P are non bottom states.
      for(const state_index si: m_P)
      {
        bool found_inert_outgoing_transition=false;
        for(outgoing_transitions_it it=m_states[si].start_outgoing_transitions;
                        it!=m_outgoing_transitions.end() &&
                        (si+1>=m_states.size() || it!=m_states[si+1].start_outgoing_transitions);
                     ++it)
        {
          const transition& t=m_aut.get_transitions()[it->transition];
          if (m_aut.is_tau(t.label()) && m_states[t.from()].block==m_states[t.to()].block)
          {
            found_inert_outgoing_transition=true;
          }
        }
        assert(!found_inert_outgoing_transition);
      }

      // Check that the non-trivial constellations are non trivial.
      for(const constellation_index ci: m_non_trivial_constellations)
      {
        // There are at least two blocks in a non-trivial constellation.
        std::forward_list<block_index>::const_iterator blocks_it=m_constellations[ci].blocks.begin();
        assert(blocks_it!=m_constellations[ci].blocks.end());
        blocks_it++;
        assert(blocks_it!=m_constellations[ci].blocks.end());
      }
      return true;
    }
#endif //#ifndef NDEBUG

    void print_data_structures(const std::string& header, const bool initialisation=false) const
    {
      mCRL2log(log::debug) << "========= PRINT DATASTRUCTURE: " << header << " =======================================\n";
      mCRL2log(log::debug) << "++++++++++++++++++++  States     ++++++++++++++++++++++++++++\n";
      for(state_index si=0; si<m_states.size(); ++si)
      {
        mCRL2log(log::debug) << "State " << si <<" (Block: " << m_states[si].block <<"):\n";
        mCRL2log(log::debug) << "  #Inert outgoing transitions " << m_states[si].no_of_outgoing_inert_transitions <<"):\n";

        mCRL2log(log::debug) << "  Incoming inert transitions:\n";
        for(std::vector<transition_index>::iterator it=m_states[si].start_incoming_inert_transitions;
                                                    it!=m_states[si].start_incoming_non_inert_transitions;
                                                  ++it)
        {
           const transition& t=m_aut.get_transitions()[*it];
           mCRL2log(log::debug) << "  " << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to() << "\n";
        }

        mCRL2log(log::debug) << "  Incoming non-inert transitions:\n";
        for(std::vector<transition_index>::iterator it=m_states[si].start_incoming_non_inert_transitions;
                        it!=m_incoming_transitions.end() &&
                        (si+1>=m_states.size() || it!=m_states[si+1].start_incoming_inert_transitions);
                     ++it)
        {
           const transition& t=m_aut.get_transitions()[*it];
           mCRL2log(log::debug) << "  " << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to() << "\n";
        }

        mCRL2log(log::debug) << "  Outgoing transitions:\n";
        for(outgoing_transitions_it it=m_states[si].start_outgoing_transitions;
                        it!=m_outgoing_transitions.end() &&
                        (si+1>=m_states.size() || it!=m_states[si+1].start_outgoing_transitions);
                     ++it)
        {
           const transition& t=m_aut.get_transitions()[it->transition];
           mCRL2log(log::debug) << "  " << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to() << "\n";;
        }
        mCRL2log(log::debug) << "  Ref states in blocks: " << *(m_states[si].ref_states_in_blocks) << ". Must be " << si <<".\n";
        mCRL2log(log::debug) << "---------------------------------------------------\n";
      }
      mCRL2log(log::debug) << "++++++++++++++++++++ Transitions ++++++++++++++++++++++++++++\n";
      for(state_index ti=0; ti<m_transitions.size(); ++ti)
      {
        const transition& t=m_aut.get_transitions()[ti];
        mCRL2log(log::debug) << "Transition " << ti <<": " << t.from() 
                                              << " -" << m_aut.action_label(t.label()) << "-> " 
                                              << t.to() << "\n";
      }

      mCRL2log(log::debug) << "++++++++++++++++++++ Blocks ++++++++++++++++++++++++++++\n";
      for(state_index bi=0; bi<m_blocks.size(); ++bi)
      {
        mCRL2log(log::debug) << "  Block " << bi << " (const: " << m_blocks[bi].constellation <<"):\n";
        mCRL2log(log::debug) << "  Bottom states: ";
        for(typename std::vector<state_index>::iterator sit=m_blocks[bi].start_bottom_states; 
                        sit!=m_blocks[bi].start_non_bottom_states; ++sit)
        {
          mCRL2log(log::debug) << *sit << "  ";
        }
        mCRL2log(log::debug) << "\n  Non bottom states: ";
        for(typename std::vector<state_index>::iterator sit=m_blocks[bi].start_non_bottom_states; 
                                 sit!=m_blocks[bi].end_states; ++sit)
        {
          mCRL2log(log::debug) << *sit << "  ";
        }
        if (!initialisation)
        {
          mCRL2log(log::debug) << "\n  LBC_List\n";
          for(const transition_index tti: m_blocks[bi].block_to_constellation)
          {
            mCRL2log(log::debug) << "\n    LBC_sublist\n";
            for(LBC_list_iterator i(tti,m_transitions); i!=LBC_list_iterator(null_transition,m_transitions); ++i)
            {
               const transition& t=m_aut.get_transitions()[*i];
               mCRL2log(log::debug) << "        " << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to() << "\n";
            }
          }
          mCRL2log(log::debug) << "  LBC_List end\n";
        }
        mCRL2log(log::debug) << "\n";
      }

      mCRL2log(log::debug) << "++++++++++++++++++++ Constellations ++++++++++++++++++++++++++++\n";
      for(state_index ci=0; ci<m_constellations.size(); ++ci)
      {
        mCRL2log(log::debug) << "  Constellation " << ci << ":\n";
        mCRL2log(log::debug) << "    Blocks in constellation: ";
        for(const block_index bi: m_constellations[ci].blocks)
        {
           mCRL2log(log::debug) << bi << " ";
        }
        mCRL2log(log::debug) << "\n";
      }
      mCRL2log(log::debug) << "Non trivial constellations: ";
      for(const constellation_index ci: m_non_trivial_constellations)
      {
        mCRL2log(log::debug) << ci << " ";
      }

      mCRL2log(log::debug) << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
      mCRL2log(log::debug) << "Incoming transitions:\n";
      for(const transition_index ti: m_incoming_transitions)
      {
        const transition& t=m_aut.get_transitions()[ti];
        mCRL2log(log::debug) << "  " << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to() << "\n";
      }
        
      mCRL2log(log::debug) << "Outgoing transitions:\n";

      for(const transition_pointer_pair pi: m_outgoing_transitions)
      {
        const transition& t=m_aut.get_transitions()[pi.transition];
        mCRL2log(log::debug) << "  " << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to();
        const transition& t1=m_aut.get_transitions()[pi.start_same_saC->transition]; 
        mCRL2log(log::debug) << "  \t(same saC: " << t1.from() << " -" << m_aut.action_label(t1.label()) << "-> " << t1.to() << ");\n";
      }

      mCRL2log(log::debug) << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
      mCRL2log(log::debug) << "New bottom states to be investigated: ";
      
      for(state_index si: m_P)
      {
        mCRL2log(log::debug) << si << " ";
      }
        
      mCRL2log(log::debug) << "\n========= END PRINT DATASTRUCTURE: " << header << " =======================================\n";
    }

  public:
    /// \brief constructor
    /// \details The constructor constructs the data structures and immediately
    /// calculates the partition corresponding with the bisimulation quotient.
    /// It destroys the transitions on the LTS (to save memory) but does not
    /// adapt the LTS to represent the quotient's transitions.
    /// It is assumed that there are no tau-loops in aut.
    /// \param aut                 LTS that needs to be reduced
    /// \param branching           If true branching bisimulation is used,
    ///                                otherwise strong bisimulation is
    ///                                applied.
    /// \param preserve_divergence If true and branching is true, preserve
    ///                                tau loops on states.
    bisim_partitioner_gj(LTS_TYPE& aut, 
                         const bool branching = false,
                         const bool preserve_divergence = false)
      : m_aut(aut),
        m_states(aut.num_states()),
        m_transitions(aut.num_transitions()),
        m_blocks(1,{m_states_in_blocks.begin(),0}),
        m_constellations(1,constellation_type(0)),   // Algorithm 1, line 1.2.
        m_branching(branching),
        m_preserve_divergence(preserve_divergence)
    {                                                                           
      assert(m_branching || !m_preserve_divergence);
      create_initial_partition();        
      refine_partition_until_it_becomes_stable();
      assert(check_data_structures("READY"));
    }


    /// \brief Calculate the number of equivalence classes
    /// \details The number of equivalence classes (which is valid after the
    /// partition has been constructed) is equal to the number of states in the
    /// bisimulation quotient.
    std::size_t num_eq_classes() const
    {
      return m_blocks.size();
    }


    /// \brief Get the equivalence class of a state
    /// \details After running the minimisation algorithm, this function
    /// produces the number of the equivalence class of a state.  This number
    /// is the same as the number of the state in the minimised LTS to which
    /// the original state is mapped.
    /// \param s state whose equivalence class needs to be found
    /// \returns sequence number of the equivalence class of state s
    state_index get_eq_class(const state_index si) const
    {
      assert(si<m_states.size());
      return m_states[si].block;
    }


    /// \brief Adapt the LTS after minimisation
    /// \details After the efficient branching bisimulation minimisation, the
    /// information about the quotient LTS is only stored in the partition data
    /// structure of the partitioner object.  This function exports the
    /// information back to the LTS by adapting its states and transitions:  it
    /// updates the number of states and adds those transitions that are
    /// mandated by the partition data structure.  If desired, it also creates
    /// a vector containing an arbritrary (example) original state per
    /// equivalence class.
    ///
    /// The main parameter and return value are implicit with this function: a
    /// reference to the LTS was stored in the object by the constructor.
    void finalize_minimized_LTS()
    {
      std::unordered_set<transition> T;
      for(const transition& t: m_aut.get_transitions())
      {
        const transition_index new_from=get_eq_class(t.from());
        const transition_index new_to=get_eq_class(t.to());
        if (!m_aut.is_tau(t.label()) || new_from!=new_to) 
        {
          T.insert(transition(new_from, t.label(), new_to));
        }
      }
      m_aut.clear_transitions();
      for (const transition t: T)
      {
        m_aut.add_transition(t);
      }

      // Merge the states, by setting the state labels of each state to the
      // concatenation of the state labels of its equivalence class.

      if (m_aut.has_state_info())   /* If there are no state labels this step is not needed */
      {
        /* Create a vector for the new labels */
        std::vector<typename LTS_TYPE::state_label_t> new_labels(num_eq_classes());

    
        for(std::size_t i=0; i<m_aut.num_states(); ++i)
        {
          const state_index new_index(get_eq_class(i));
          new_labels[new_index]=new_labels[new_index]+m_aut.state_label(i);
        }

        m_aut.set_num_states(num_eq_classes());
        for (std::size_t i=0; i<num_eq_classes(); ++i)
        {
          m_aut.set_state_label(i, new_labels[i]);
        }
      }
      else
      {
        m_aut.set_num_states(num_eq_classes());
      }

      m_aut.set_initial_state(get_eq_class(m_aut.initial_state()));
    }


    /// \brief Check whether two states are in the same equivalence class.
    /// \param s first state that needs to be compared.
    /// \param t second state that needs to be compared.
    /// \returns true iff the two states are in the same equivalence class.
    bool in_same_class(state_index const s, state_index const t) const
    {
      return get_eq_class(s) == get_eq_class(t);
    }
  protected:

    /*--------------------------- main algorithm ----------------------------*/

    /*----------------- splitB -- Algorithm 3 of [GJ 2024] -----------------*/

    std::size_t number_of_states_in_block(const block_index B) const
    {
      return std::distance(m_blocks[B].start_bottom_states, m_blocks[B].end_states);
    }

    void swap_states_in_states_in_block(
              typename std::vector<state_index>::iterator pos1, 
              typename std::vector<state_index>::iterator pos2) 
    {
      assert(pos1!=pos2);
      std::swap(*pos1,*pos2);
      m_states[*pos1].ref_states_in_blocks=pos1;
      m_states[*pos2].ref_states_in_blocks=pos2;
    }

    // Move pos1 to pos2, pos2 to pos3 and pos3 to pos1;
    void swap_states_in_states_in_block(
              typename std::vector<state_index>::iterator pos1, 
              typename std::vector<state_index>::iterator pos2, 
              typename std::vector<state_index>::iterator pos3) 
    {
      assert(pos1!=pos2 && pos2!=pos3 && pos3!=pos1);
      const state_index temp=*pos1;
      *pos1=*pos3;
      *pos3=*pos2;
      *pos2=temp;

      m_states[*pos1].ref_states_in_blocks=pos1;
      m_states[*pos2].ref_states_in_blocks=pos2;
      m_states[*pos3].ref_states_in_blocks=pos3;
    }

    // Split the block B by moving the elements in R to the front in m_states, and add a 
    // new element B_new at the end of m_blocks referring to R. Adapt B.start_bottom_states,
    // B.start_non_bottom_states and B.end_states, and do the same for B_new. 
    block_index split_block_B_into_R_and_BminR(
                     const block_index B, 
                     const std::unordered_set<state_index>& R, 
                     std::function<void(const state_index)> update_Ptilde)
    {
      // Basic administration. Make a new block and add it to the current constellation.
      const block_index B_new=m_blocks.size();
      m_blocks.emplace_back(m_blocks[B].start_bottom_states,m_blocks[B].constellation);
      m_non_trivial_constellations.insert(m_blocks[B].constellation);
      m_constellations[m_blocks[B].constellation].blocks.push_front(B_new);

      // Carry out the split. 
      for(state_index s: R)
      {
        m_states[s].block=B_new;
        update_Ptilde(s);
        typename std::vector<state_index>::iterator pos=m_states[s].ref_states_in_blocks;
        if (pos>=m_blocks[B].start_non_bottom_states) // the state is a non bottom state.
        {
          // We know that B must have a bottom state.
          assert(m_blocks[B].start_bottom_states!=m_blocks[B].start_non_bottom_states);
          // There are two situations.
          if (pos==m_blocks[B].start_non_bottom_states)
          {
            // pos is located at the first non-bottom-state of B. 
            swap_states_in_states_in_block(pos,m_blocks[B].start_bottom_states);
          }
          else
          {
            // pos is a later non-bottom-state of B. We need to swap:
            // pos --> B.start_bottom_states --> B.start_non_bottom_states --> pos.
            swap_states_in_states_in_block(pos,m_blocks[B].start_bottom_states, m_blocks[B].start_non_bottom_states);
          }
          m_blocks[B].start_non_bottom_states++;
          m_blocks[B].start_bottom_states++;
          m_blocks[B_new].end_states++;
        }
        else // the state is a bottom state
        {
          // Three cases. pos==B_new.start_non_bottom_states
          if (pos==m_blocks[B_new].start_non_bottom_states)
          {
            // It holds that B.start_bottom_states==B_new_bottom_states.
            // Nothing needs to be swapped. 
          }
          else if (m_blocks[B_new].start_non_bottom_states==m_blocks[B].start_bottom_states)
          {
            // There are no non-bottom states in B_new. 
            swap_states_in_states_in_block(pos,m_blocks[B].start_bottom_states);
          }
          else if (m_blocks[B_new].start_non_bottom_states==m_blocks[B].start_bottom_states)
          {
            // pos --> B.start_bottom_states --> pos.
            swap_states_in_states_in_block(pos,m_blocks[B].start_bottom_states);
          }
          else if (pos==m_blocks[B].start_bottom_states)
          {
            // pos --> new_B.start_non_bottom_states --> pos.
            swap_states_in_states_in_block(pos,m_blocks[B_new].start_non_bottom_states);
          }
          else
          {
            // pos --> B_new.start_non_bottom_states --> B.start_bottom_states --> pos.
            swap_states_in_states_in_block(pos,m_blocks[B_new].start_non_bottom_states,m_blocks[B].start_bottom_states);
          }
          m_blocks[B_new].start_non_bottom_states++;
          m_blocks[B_new].end_states++;
          m_blocks[B].start_bottom_states++;
          assert(m_blocks[B].start_bottom_states<=m_blocks[B].start_non_bottom_states);
          assert(m_blocks[B_new].start_bottom_states<m_blocks[B_new].start_non_bottom_states);
        }
      }
      return B_new;
    }
    
    void insert_in_the_doubly_linked_list_LBC_in_blocks(
               const transition_index ti,
               std::list<transition_index>::iterator position)
    {
      transition_index& current_transition_index= *position;
      // insert after current transition.
      m_transitions[ti].previous_LBC=current_transition_index;
      m_transitions[ti].next_LBC=m_transitions[current_transition_index].next_LBC;
      m_transitions[ti].transitions_per_block_to_constellation=position;
      if (m_transitions[current_transition_index].next_LBC!=null_transition)
      {
        m_transitions[m_transitions[current_transition_index].next_LBC].previous_LBC=ti;
      }
      m_transitions[current_transition_index].next_LBC=ti;
    }

    // Move the transition t with transition index ti to a new 
    // LBC list as the target state switches to a new constellation.
    void update_the_doubly_linked_list_LBC_new_constellation(
               const block_index index_block_B, 
               const transition& t,
               const transition_index ti)
    {
      assert(m_states[t.to()].block==index_block_B);

      std::list<transition_index > :: iterator this_block_to_constellation=
                           m_transitions[ti].transitions_per_block_to_constellation;
      std::list<transition_index > :: iterator next_block_to_constellation=
                           ++std::list<transition_index > :: iterator(this_block_to_constellation);
      bool last_element_removed=remove_from_the_doubly_linked_list_LBC_in_blocks(ti);

      // if this transition is inert, it is inserted in a block in front. Otherwise, it is inserted after
      // the current element in the list. 
      if (m_aut.is_tau(t.label()) && m_states[t.from()].block==m_states[t.to()].block)
      {
        std::list<transition_index >::iterator first_block_to_constellation=m_blocks[m_states[t.from()].block].block_to_constellation.begin();
        if ( first_block_to_constellation==this_block_to_constellation ||
             m_states[m_aut.get_transitions()[*first_block_to_constellation].to()].block!=index_block_B ||
             m_aut.get_transitions()[*first_block_to_constellation].label()!=t.label())
        { 
          // Make a new entry in the list block_to_constellation, at the beginning;
          
          first_block_to_constellation=
                  m_blocks[m_states[t.from()].block].block_to_constellation.
                           insert(first_block_to_constellation, ti);
          m_transitions[ti].transitions_per_block_to_constellation=first_block_to_constellation;
          m_transitions[ti].previous_LBC=null_transition;
          m_transitions[ti].next_LBC=null_transition;
        }
        else
        {
          insert_in_the_doubly_linked_list_LBC_in_blocks(ti, first_block_to_constellation);
        }
      }
      else 
      {
        if (next_block_to_constellation==m_blocks[m_states[t.from()].block].block_to_constellation.end() ||
            m_states[m_aut.get_transitions()[*next_block_to_constellation].to()].block!=index_block_B ||
            m_aut.get_transitions()[*next_block_to_constellation].label()!=t.label())
        { 
          // Make a new entry in the list next_block_to_constellation, after the current list element.
          next_block_to_constellation=
                  m_blocks[m_states[t.from()].block].block_to_constellation.
                           insert(next_block_to_constellation, ti);
          m_transitions[ti].transitions_per_block_to_constellation=next_block_to_constellation;
          m_transitions[ti].previous_LBC=null_transition;
          m_transitions[ti].next_LBC=null_transition;
        }
        else
        {
          insert_in_the_doubly_linked_list_LBC_in_blocks(ti, next_block_to_constellation);
        }
      }
      
      if (last_element_removed)
      {
        m_blocks[m_states[t.from()].block].block_to_constellation.erase(this_block_to_constellation);
      }
    }

    // Update the LBC list of a transition, when the from state of the transition moves
    // from block old_bi to new_bi. 
    transition_index update_the_doubly_linked_list_LBC_new_block(
               const block_index old_bi,
               const block_index new_bi,
               const transition_index ti,
               std::unordered_map< std::pair <action_index, constellation_index>, 
                            std::list< transition_index >::iterator>& new_LBC_list_entries)
    {
      const transition& t=m_aut.get_transitions()[ti];
      transition_index remaining_transition=null_transition;

      assert(m_states[t.from()].block==new_bi);
      
      std::list<transition_index >::iterator this_block_to_constellation=
                           m_transitions[ti].transitions_per_block_to_constellation;
      std::unordered_map< std::pair <action_index, constellation_index>,
                          std::list< transition_index >::iterator>::iterator it=
                     new_LBC_list_entries.find(std::pair(t.label(), m_blocks[m_states[t.to()].block].constellation));
      bool last_element_removed=remove_from_the_doubly_linked_list_LBC_in_blocks(ti);

      if (it==new_LBC_list_entries.end())
      { 
        // Make a new entry in the list next_block_to_constellation;
        assert(m_states[t.from()].block==new_bi);
        
        // Put inert tau's to the front. Others, 
        std::list<transition_index >::iterator new_position;
        if (m_blocks[new_bi].block_to_constellation.empty() || 
            (m_aut.is_tau(t.label()) && 
             m_blocks[m_states[t.from()].block].constellation==m_blocks[m_states[t.to()].block].constellation))
        {
          m_blocks[new_bi].block_to_constellation.push_front(ti);
          new_position=m_blocks[new_bi].block_to_constellation.begin();
        }
        else 
        {
          new_position=m_blocks[new_bi].block_to_constellation.begin();
          new_position++;
          new_position= m_blocks[new_bi].block_to_constellation.insert(new_position,ti);
        }
        new_LBC_list_entries[std::pair(t.label(), m_blocks[m_states[t.to()].block].constellation)]=new_position;
        transition_type& tt=m_transitions[ti];
        tt.next_LBC=null_transition;
        tt.previous_LBC=null_transition;
        tt.transitions_per_block_to_constellation=new_position;
      }
      else
      {
        // Move the current transition to the next list indicated by the iterator it.
        insert_in_the_doubly_linked_list_LBC_in_blocks(ti, it->second);
      }
      
      if (last_element_removed)
      {
        // Remove this element. 
        m_blocks[old_bi].block_to_constellation.erase(this_block_to_constellation);
      }
      else
      {
        remaining_transition= *this_block_to_constellation;
      }

      return remaining_transition;
    }

    // Calculate the states R in block B that can inertly reach M and split
    // B in R and B\R. The complexity is conform the smallest block of R and B\R.
    // The LBC_list and bottom states are not updated. 
    // Provide the index of the newly created block as a result. This block is the smallest of R and B\R.
    // Return in M_in_bi whether the new block bi is the one containing M.
    template <class MARKED_STATE_ITERATOR, 
              class UNMARKED_STATE_ITERATOR>
    block_index simple_splitB(const block_index B, 
                              const MARKED_STATE_ITERATOR M_begin, 
                              const MARKED_STATE_ITERATOR M_end, 
                              const UNMARKED_STATE_ITERATOR M_co_begin,
                              const UNMARKED_STATE_ITERATOR M_co_end,
                              const std::function<bool(state_index)>& unmarked_blocker,
                              bool& M_in_bi,
                              std::function<void(const state_index)> update_Ptilde)
    {
      const std::size_t B_size=number_of_states_in_block(B);
      std::unordered_set<state_index> U, U_todo;
      std::unordered_set<state_index> R, R_todo;
      typedef enum { initializing, state_checking, aborted } status_type;
      status_type U_status=initializing;
      status_type R_status=initializing;
      MARKED_STATE_ITERATOR M_it=M_begin; 
      UNMARKED_STATE_ITERATOR M_co_it=M_co_begin; 

      // Algorithm 3, line 3.2 left.
      std::unordered_map<state_index, size_t> count;


      // start coroutines. Each co-routine handles one state, and then gives control
      // to the other co-routine. The coroutines can be found sequentially below surrounded
      // by a while loop.

      while (true)
      {
        assert(U_status!=aborted || R_status!=aborted);
        // The code for the left co-routine. 
        switch (U_status) 
        {
          case initializing:
          {
            // Algorithm 3, line 3.3 left.
            if (M_co_it==M_co_end)
            {
              U_status=state_checking;
            }
            else
            {
              const state_index si=*M_co_it;
              M_co_it++;
              if (!unmarked_blocker(si))
              {
                U_todo.insert(si);
              }
            }
            break;
          }
          case state_checking:
          {
            // Algorithm 3, line 3.23 and line 3.24, left. 
            if (U_todo.empty())
            {
              assert(!U.empty());
              // split_block B into U and B\U.
              assert(U.size()>0);
              block_index block_index_of_U=split_block_B_into_R_and_BminR(B, U, update_Ptilde);
              M_in_bi = false;
              return block_index_of_U;
            }
            else
            {
              const state_index s=U_todo.extract(U_todo.begin()).value();
              U.insert(s); 
              count[s]=0;
              // Algorithm 3, line 3.8, left.
              for(std::vector<transition_index>::iterator it=m_states[s].start_incoming_inert_transitions;
                                                          it!=m_states[s].start_incoming_non_inert_transitions;
                                                        it++)
              {
                // Algorithm 3, line 3.12, left.
                state_index from=m_aut.get_transitions()[*it].from();
                if (count.find(from)==count.end()) // count(from) is undefined;
                {
                   // Algorithm 3, line 3.13, left.
                  if (unmarked_blocker(from))
                  {
                    // Algorithm 3, line 3.14, left.
                    count[from]=std::numeric_limits<std::size_t>::max();
                  }
                  else
                  {
                    // Algorithm 3, line 3.15 and 3.18, left.
                    count[from]=m_states[from].no_of_outgoing_inert_transitions-1;
                  }
                }
                else
                {
                  // Algorithm 3, line 3.18, left.
                  count[from]--;
                }
                // Algorithm 3, line 3.19, left.
                if (count[from]==0)
                {
                  if (U.find(from)==U.end())
                  {
                    U_todo.insert(from);
                  }
                }
              }
            }
            // Algorithm 3, line 3.10 and line 3.11 left. 
            if (2*(U.size()+U_todo.size())>B_size+1)  // Compensate with +1 for division by 2. I.e. if B_size=3, U.size()+U.todo_size() of 2
                                                      // should not lead to an abort. 
            {
              U_status=aborted;
            }
          }
          default: break;
        }
        // The code for the right co-routine. 
        switch (R_status)
        {
          case initializing:
          {
            // Algorithm 3, line 3.3, right.
            if (M_it==M_end)
            {
              R_status=state_checking;
            }
            else
            {
              const state_index si= *M_it;
              ++M_it;
              R_todo.insert(si);
            }
            break;
          }
          case state_checking: 
          {
            if (R_todo.empty())
            {
              // split_block B into R and B\R.
              assert(R.size()>0);
              block_index block_index_of_R=split_block_B_into_R_and_BminR(B, R, update_Ptilde);
              M_in_bi=true;
              return block_index_of_R;
            }
            else
            {
              const state_index s=R_todo.extract(R_todo.begin()).value();
              R.insert(s);
              for(std::vector<transition_index>::iterator it=m_states[s].start_incoming_inert_transitions; 
                                                          it!=m_states[s].start_incoming_non_inert_transitions;
                                                         it++) 
              { 
                const transition& tr=m_aut.get_transitions()[*it];
                if (R.count(tr.from())==0)
                {
                  R_todo.insert(tr.from());
                }
              }
              // Algorithm 3, line 3.10 and line 3.11, right. 
              if (2*(R.size()+R_todo.size())>B_size+1) // See for "+1" remark above. 
              {
                R_status=aborted;
              }
            }
          }
          default: break;
        }
      }
    }

    void make_transition_non_inert(const transition_index ti)
    {
      const transition& t=m_aut.get_transitions()[ti];
      assert(m_aut.is_tau(t.label()));
      assert(m_states[t.to()].block!=m_states[t.from()].block);
      // Move the transition indicated by tti to the non inert transitions in m_incoming_transitions.
      state_type_gj& to=m_states[m_aut.get_transitions()[ti].to()];
      to.start_incoming_non_inert_transitions--;
      std::vector<transition_index>::iterator move_to1=to.start_incoming_non_inert_transitions;
      std::vector<transition_index>::iterator current_position1=m_transitions[ti].ref_incoming_transitions;
      if (move_to1!=current_position1)
      {
        std::swap(*move_to1,*current_position1);
        m_transitions[*move_to1].ref_incoming_transitions=move_to1;
        m_transitions[*current_position1].ref_incoming_transitions=current_position1;
      }

      m_states[t.from()].no_of_outgoing_inert_transitions--;
    }

    // Split block B in R, being the inert-tau transitive closure of M contains 
    // states that must be in block, and M\R. M_nonmarked, minus those in unmarked_blocker, are those in the other block. 
    // The splitting is done in time O(min(|R|,|B\R|). Return the block index of the smallest
    // block, which is newly created. Indicate in M_in_new_block whether this new block contains M.
    template <class MARKED_STATE_ITERATOR,
              class UNMARKED_STATE_ITERATOR>
    block_index splitB(const block_index B, 
                       const MARKED_STATE_ITERATOR M_begin, 
                       const MARKED_STATE_ITERATOR M_end, 
                       const UNMARKED_STATE_ITERATOR M_co_begin,
                       const UNMARKED_STATE_ITERATOR M_co_end,
                       const std::function<bool(state_index)>& unmarked_blocker,
                       bool& M_in_new_block,
                       std::function<void(const block_index, const block_index, const transition_index, const transition_index)> 
                                                 update_block_label_to_cotransition,
                       bool update_LBC_list=true,
                       std::function<void(const transition_index, const transition_index, const block_index)> process_transition=
                                                        [](const transition_index, const transition_index, const block_index){},
                       std::function<void(const state_index)> update_Ptilde=
                                                        [](const state_index){})
    {
      assert(M_begin!=M_end && M_co_begin!=M_co_end);
      block_index bi=simple_splitB(B, M_begin, M_end, M_co_begin, M_co_end, unmarked_blocker, M_in_new_block,update_Ptilde);

      // Update the LBC_list, and bottom states, and invariant on inert transitions.
      // Recall new LBC positions.
      std::unordered_map< std::pair <action_index, constellation_index>, 
                            std::list< transition_index >::iterator> new_LBC_list_entries;
      for(typename std::vector<state_index>::iterator ssi=m_blocks[bi].start_bottom_states;
                                                      ssi!=m_blocks[bi].end_states;
                                                      ++ssi)
      {
        const state_index si=*ssi;
        state_type_gj& s= m_states[si];
        s.block=bi;

        // Situation below is only relevant if M_in_new_block;
        if (M_in_new_block)
        {
          // si is a non_bottom_state in the smallest block containing M..
          bool non_bottom_state_becomes_bottom_state= (ssi>=m_blocks[bi].start_non_bottom_states); 
        
          for(outgoing_transitions_it ti=s.start_outgoing_transitions;
                        ti!=m_outgoing_transitions.end() &&
                        ((*ssi)+1>=m_states.size() || ti!=m_states[(*ssi)+1].start_outgoing_transitions);
              ti++)
          {       
            const transition& t=m_aut.get_transitions()[ti->transition];
            assert(m_states[t.from()].block==bi);
            if (m_aut.is_tau(t.label()))
            { 
              if  (m_states[t.to()].block==B)
              {
                // This is a transition that has become non-inert.
                make_transition_non_inert(ti->transition);
                // The LBC-list of this transition will be updated below, as it is now non-inert. 
              }
              else if (m_states[t.to()].block==bi)
              {
                non_bottom_state_becomes_bottom_state=false; // There is an outgoing inert tau. State remains non-bottom.
              }
            }
          }
          if (non_bottom_state_becomes_bottom_state)
          {
            // The state at si has become a bottom_state.
            assert(find(m_P.begin(), m_P.end(), si)==m_P.end());
            m_P.push_back(si);
            // Move this former non bottom state to the bottom states.
            if (ssi!=m_blocks[bi].start_non_bottom_states)
            {
              // Note that the call below damages the value of *ssi. Here it is not anymore equal to si.
              swap_states_in_states_in_block(ssi, m_blocks[bi].start_non_bottom_states);
            }
            m_blocks[bi].start_non_bottom_states++;
          }
        }

        if (update_LBC_list)
        {
          for(outgoing_transitions_it ti=s.start_outgoing_transitions; 
                      ti!=m_outgoing_transitions.end() &&
                      ((si+1)==m_states.size() || ti!=m_states[si+1].start_outgoing_transitions);
                  ti++)
          {       
            transition_index old_remaining_transition=update_the_doubly_linked_list_LBC_new_block(B, bi, ti->transition, new_LBC_list_entries);
            process_transition(ti->transition, old_remaining_transition, B);
            update_block_label_to_cotransition(B, bi, ti->transition, old_remaining_transition);
          }
        }
        
        // Investigate the incoming tau transitions. 
        if (!M_in_new_block)
        {
          for(std::vector<transition_index>::iterator ti=s.start_incoming_inert_transitions; 
                      ti!=s.start_incoming_non_inert_transitions; )
          {       
            const transition& t=m_aut.get_transitions()[*ti];
            assert(m_aut.is_tau(t.label()));
            assert(m_states[t.to()].block==bi);
            if (m_states[t.from()].block==B)
            { 
              // This transition did become non-inert.
              make_transition_non_inert(*ti);
              
              // Check whether from is a new bottom state.
              const state_index from=t.from();
              if (m_states[from].no_of_outgoing_inert_transitions==0)
              {
                // This state has not outgoing inert transitions. It becomes a bottom state. 
                assert(find(m_P.begin(), m_P.end(), from)==m_P.end());
                m_P.push_back(from);
                // Move this former non bottom state to the bottom states.
                block_index temp_bi=m_states[from].block;
                if (m_states[from].ref_states_in_blocks!=m_blocks[temp_bi].start_non_bottom_states)
                {
                  swap_states_in_states_in_block(m_states[from].ref_states_in_blocks, m_blocks[temp_bi].start_non_bottom_states);
                }
                m_blocks[temp_bi].start_non_bottom_states++;
              }
            }
            else
            {
              ti++;
            }
          }
        }
      }

      return bi;
    }


    void create_initial_partition()
    {
      mCRL2log(log::verbose) << "An O(m log n) "
           << (m_branching ? (m_preserve_divergence
                                         ? "divergence-preserving branching "
                                         : "branching ")
                         : "")
           << "bisimulation partitioner created for " << m_aut.num_states()
           << " states and " << m_aut.num_transitions() << " transitions.\n";
      // Initialisation.
    
      // Initialise m_incoming_(non-)inert-transitions and m_states[si].no_of_outgoing_transitions 
      typedef std::unordered_map<typename LTS_TYPE::states_size_type, set_of_transitions_type> temporary_store_type;
      temporary_store_type temporary_store;
      transition_index transition_count=0;
      for(const transition& t: m_aut.get_transitions())
      {
        temporary_store[t.to()].insert(transition_count);
        transition_count++;
        if (m_aut.is_tau(t.label()))
        {
          m_states[t.from()].no_of_outgoing_inert_transitions++;
        }
      }
        
      transitions_per_action_label_type label_to_transition_set_map;
      m_incoming_transitions.reserve(m_aut.num_transitions());
      for(state_index si=0; si<m_states.size(); ++si)
      {
        const set_of_transitions_type& transitions_for_si=temporary_store[si];
        label_to_transition_set_map.clear();
        for(const transition_index ti: transitions_for_si)
        {
          const transition& t=m_aut.get_transitions()[ti];
          label_to_transition_set_map[t.label()].insert(ti);
        }
        // First get the tau transitions.
        set_of_transitions_type& tset=label_to_transition_set_map[m_aut.tau_label_index()];
        
        m_states[si].start_incoming_inert_transitions=m_incoming_transitions.end();
        for(const transition_index ti: tset)
        {
          m_transitions[ti].ref_incoming_transitions=m_incoming_transitions.end();
          m_incoming_transitions.emplace_back(ti);
        }

        // Subsequently, put all the non-tau transitions in incoming transitions. 
        m_states[si].start_incoming_non_inert_transitions=m_incoming_transitions.end();
        for(const auto& [ai, transition_set]: label_to_transition_set_map)
        {
          if (!m_aut.is_tau(ai))
          {
            for(const transition_index ti: transition_set)
            {
              m_transitions[ti].ref_incoming_transitions=m_incoming_transitions.end();
              m_incoming_transitions.emplace_back(ti);
            }
          }
        }
      }
      
      // Initialise m_outgoing_transitions.
      // initialise m_states_in_blocks, together with start_bottom_states start_non_bottom_states in m_blocks.
      temporary_store.clear();
      std::vector<bool> state_has_outgoing_tau(m_aut.num_states(),false);
      transition_count=0;
      for(const transition& t: m_aut.get_transitions())
      {
        temporary_store[t.from()].insert(transition_count);
        if (m_aut.is_tau(t.label()))
        {
          state_has_outgoing_tau[t.from()]=true;
        }
        transition_count++;
      }

      label_to_transition_set_map.clear();
      m_outgoing_transitions.reserve(m_aut.num_transitions());
      for(state_index si=0; si<m_states.size(); ++si)
      {
        const set_of_transitions_type& transitions_for_si=temporary_store[si];

        label_to_transition_set_map.clear();
        for(const transition_index ti: transitions_for_si)
        {
          const transition& t=m_aut.get_transitions()[ti];
          label_to_transition_set_map[t.label()].insert(ti);
        }
        
        // Subsequently, put all the non-tau transitions in outgoing_transitions. 
        m_states[si].start_outgoing_transitions=m_outgoing_transitions.end();

        for(const auto& [ai, transition_set]: label_to_transition_set_map)
        {
          std::size_t count_down=transition_set.size()-1;
          outgoing_transitions_it start=m_outgoing_transitions.end();
          assert(!transition_set.empty());
          for(const transition_index ti: transition_set)
          {
            m_outgoing_transitions.emplace_back(ti,count_down==0?start:m_outgoing_transitions.end()+count_down);
            m_transitions[ti].ref_outgoing_transitions=m_outgoing_transitions.end()-1;
            count_down--;
          }
        }
      }
      temporary_store=temporary_store_type(); // release memory. 

      m_states_in_blocks.reserve(m_aut.num_states());
      std::size_t i=0;
      for(bool b: state_has_outgoing_tau)
      {
        if (!b)
        {
          m_states_in_blocks.emplace_back(i);
          m_states[i].ref_states_in_blocks=m_states_in_blocks.end()-1;
        }
        i++;
      }
      m_blocks[0].start_bottom_states=m_states_in_blocks.begin();
      m_blocks[0].start_non_bottom_states=m_states_in_blocks.end();
      i=0;
      for(bool b: state_has_outgoing_tau)
      {
        if (b)
        {
          m_states_in_blocks.emplace_back(i);
          m_states[i].ref_states_in_blocks=m_states_in_blocks.end()-1;
        }
        i++;
      }
      m_blocks[0].end_states=m_states_in_blocks.end();

      assert(check_data_structures("After initial reading before splitting in the initialisation",true));

      // The following implements line 1.3 of Algorithm 1. 
      states_per_action_label_type states_per_action_label;
      for(const transition& t: m_aut.get_transitions())
      {
        if (!m_aut.is_tau(t.label()))
        {
          states_per_action_label[t.label()].insert(t.from());
        }
      }

      for(const auto& [a, M]: states_per_action_label)
      {
        states_per_block_type Bprime;
        for(const state_index s: M)
        {
          Bprime[m_states[s].block].insert(s);
        }
        
        for(const auto& [block_ind, split_states]: Bprime)
        {
          // Check whether the block B, indexed by block_ind, can be split.
          // This means that the bottom states of B are not all in the split_states.
          const set_of_states_type& split_states_=split_states;
          const block_type& B=m_blocks[block_ind];
          bool split_is_possible=false;
          for(typename std::vector<state_index>::iterator i= B.start_bottom_states; 
                       !split_is_possible && i< B.start_non_bottom_states; ++i)
          {
            if (!(split_states.count(*i)>0))
            {
              split_is_possible=true;
            }
          }  
          if (split_is_possible)
          { 
            bool dummy=false;
            const bool do_not_split_the_LBC_list=false;
            splitB(block_ind, 
                   split_states.begin(),
                   split_states.end(),
                   B.start_bottom_states,
                   B.start_non_bottom_states,
                   [&split_states_](const state_index si){ return split_states_.count(si)>0; },
                   dummy,
                   [](const block_index, const block_index, const transition_index, const transition_index){},
                   do_not_split_the_LBC_list);
          }
        }
      }
     
      // The initial partition has been constructed. Continue with the initiatialisation.
      // Initialise m_transitions[...].transitions_per_block_to_constellation and the doubly linked list
      // represented by next_LBC and previous_LBC in transition_type.

      block_label_to_list_iterator_map block_label_to_iterator;
      std::vector<bool> stable_states(m_aut.num_states(), true);
      transition_count=0;
      for(const transition& t: m_aut.get_transitions())
      {
        typename block_label_to_list_iterator_map::iterator it=
                 block_label_to_iterator.find(std::pair(m_states[t.from()].block, t.label()));
        if (it==block_label_to_iterator.end())
        {
          block_type& b=m_blocks[m_states[t.from()].block];
          typename  std::list< transition_index>::iterator new_position=b.block_to_constellation.begin();
          if (!(m_aut.is_tau(t.label()) || b.block_to_constellation.empty()))
          {
            // The tau transitions must remain/be inserted in front.
            new_position++;
          }
            // The tau transitions must remain in front.
          typename  std::list< transition_index>::iterator newly_inserted_it=
                           b.block_to_constellation.insert(new_position, transition_count);
          block_label_to_iterator[std::pair(m_states[t.from()].block, t.label())]=newly_inserted_it;
          m_transitions[transition_count].transitions_per_block_to_constellation=newly_inserted_it;
          m_transitions[transition_count].previous_LBC=null_transition;
          m_transitions[transition_count].next_LBC=null_transition;
        }
        else
        {
          m_transitions[transition_count].next_LBC= m_transitions[*(it->second)].next_LBC; 
          if (m_transitions[*(it->second)].next_LBC!=null_transition)
          {
            m_transitions[m_transitions[*(it->second)].next_LBC].previous_LBC=transition_count;
          }
          m_transitions[transition_count].previous_LBC=*(it->second);
          m_transitions[*(it->second)].next_LBC=transition_count;
          m_transitions[transition_count].transitions_per_block_to_constellation=it->second;
        }
        transition_count++;
      }

      // The data structures have now been completely initialized.

      // Algorithm 1, line 1.4 is implicitly done in the call to splitB above.
      
      // Algorithm 1, line 1.5.
      assert(check_data_structures("End initialisation"));
      stabilizeB();
    }
 
    // Update the doubly linked list L_B->C in blocks.
    // First removing and adding a single element are implemented.
    //
    // If there is more than one element it is removed.
    // In that case false is returned. Otherwise, the result is true, 
    // and the element is actually not removed.
    bool remove_from_the_doubly_linked_list_LBC_in_blocks(const transition_index ti)
    {
      if (m_transitions[ti].next_LBC==null_transition &&
          m_transitions[ti].previous_LBC==null_transition)
      {
        // This is the only element in the list. Leave it alone.
        return true;
      }
      else
      {
        // There is more than one element.
        // If this element is the first element, replace it by the second element in the LBC-List.
        if (*m_transitions[ti].transitions_per_block_to_constellation==ti)
        {
          assert(m_transitions[ti].next_LBC!=null_transition);
          *m_transitions[ti].transitions_per_block_to_constellation=m_transitions[ti].next_LBC;
        }
 
        if (m_transitions[ti].previous_LBC!=null_transition)
        {
          m_transitions[m_transitions[ti].previous_LBC].next_LBC=
              m_transitions[ti].next_LBC;
        }
        if (m_transitions[ti].next_LBC!=null_transition)
        {
          m_transitions[m_transitions[ti].next_LBC].previous_LBC=
              m_transitions[ti].previous_LBC;
        }
        return false;
      }
    }


    // Algorithm 4. Stabilize the current partition with respect to the current constellation
    // given that the states in m_P did become new bottom states. 

    void stabilizeB()
    {
      // Algorithm 4, line 4.3.
      std::unordered_map<block_index, set_of_states_type> Phat;
      for(const state_index si: m_P)
      {
        Phat[m_states[si].block].insert(si);
      }
      if (m_P.size()<1000) {m_P.clear(); } else {m_P=std::vector<state_index>(); }  // Avoid unnecessary memory usage.

      // Algorithm 4, line 4.4.
      while (!Phat.empty()) 
      {
        // Algorithm 4, line 4.5. 
        const block_index bi=Phat.begin()->first;
        const set_of_states_type& V=Phat.begin()->second;
        
        // Algorithm 4, line 4.6.
        // Collect all new bottom states and group them per action-constellation pair.
        label_constellation_to_set_of_states_map grouped_transitions;
        for(const state_index si: V)
        {
          for(outgoing_transitions_it ti=m_states[si].start_outgoing_transitions;
                        ti!=m_outgoing_transitions.end() &&
                        (si+1>=m_states.size() || ti!=m_states[si+1].start_outgoing_transitions);
                    ++ti)
          {
            transition& t=m_aut.get_transitions()[ti->transition];
            if (!(m_states[t.from()].block==m_states[t.to()].block && m_aut.is_tau(t.label())))
            {
              grouped_transitions[std::pair(t.label(), m_blocks[m_states[t.to()].block].constellation)].insert(t.from());
            }
          }
        }

        std::unordered_map<block_index, set_of_states_type> Ptilde;
        assert(!V.empty());
        Ptilde[bi]=V;
        
        // Algorithm 4, line 4.7.
        typedef std::unordered_map< std::pair < block_index, std::pair<label_index, constellation_index > >, transition_index > Qhat_map;
        Qhat_map Qhat;
        for(std::list< transition_index >::iterator ti=m_blocks[bi].block_to_constellation.begin();
                                   ti!=m_blocks[bi].block_to_constellation.end(); ti++)
        {
          const transition& t=m_aut.get_transitions()[*ti];
          if (!m_aut.is_tau(t.label()) || m_blocks[m_states[t.to()].block].constellation!=m_blocks[bi].constellation)
          {
            Qhat[std::pair(bi, std::pair(t.label(), m_blocks[m_states[t.to()].block].constellation))]=*ti;
          }
        }
  
        
        // Algorithm 4, line 4.8.
        while (!Qhat.empty())
        {
          // Algorithm 4, line 4.9.
          const typename Qhat_map::iterator Qit=Qhat.begin();
          const transition_index t_ind=Qit->second;
          const transition& t=m_aut.get_transitions()[t_ind];
          Qhat.erase(Qit);
          // Algorithm 4, line 4.10.
          const block_index bi=m_states[t.from()].block;
          set_of_states_type W=Ptilde[bi];
          const set_of_states_type& aux=grouped_transitions[std::pair(t.label(), m_blocks[m_states[t.to()].block].constellation)];
          bool W_empty=true;
          for(const state_index si: W) 
          {
            if (aux.count(si)==0) 
            {
              W_empty=false;
              break;
            }
          }
          // Algorithm 4, line 4.10.
          if (!W_empty)
          {
            // Algorithm 4, line 4.11, and implicitly 4.12, 4.13 and 4.18. 
            bool V_in_bi=false;
            splitB(bi, 
                   LBC_list_state_iterator(t_ind, m_transitions, m_aut.get_transitions()),
                   LBC_list_state_iterator(null_transition, m_transitions, m_aut.get_transitions()),
                   W.begin(), 
                   W.end(),
                   [&t, this]
                      (const state_index si)
                      { // Check whether there is an outgoing transition in si with the same label and target
                        // constellation as the transition it. If so, return true to block this state; 
                        const state_type_gj& s=m_states[si];
                        for(outgoing_transitions_it tti=s.start_outgoing_transitions;
                                       tti!=m_outgoing_transitions.end() &&
                                       (si+1>=m_states.size() || tti!=m_states[si+1].start_outgoing_transitions);
                                       ++tti)
                        {
                          const transition& t_local=m_aut.get_transitions()[tti->transition];
                          if (t_local.label()==t.label() && 
                              m_blocks[m_states[t_local.to()].block].constellation==
                                        m_blocks[m_states[t.to()].block].constellation)
                          {
                            return true;
                          }
                        }
                        return false;
                      },
                   V_in_bi,
                   [](const block_index, const block_index, const transition_index, const transition_index){},
                   true,  // Update LBC-list.
            // Algorithm 4, line 4.14 and 4.15.
                   // [&grouped_transitions_per_block, &Q, bi, this]
                   [&Qhat, this]
                   (const transition_index ti, const transition_index remaining_transition, const block_index old_block)
                     { 
                       // Move the new bottom state to its appropriate block.
                       const transition& t=m_aut.get_transitions()[ti];
 
                       // Check whether this transition represented a label/constellation in the old block. 
                       typename Qhat_map::iterator Qti_it=Qhat.find(std::pair(old_block, std::pair(t.label(),m_blocks[m_states[t.to()].block].constellation)));
                       if (Qti_it!=Qhat.end())  
                       {
                         // This transition is not investigated, or was not inert for the target constellation, and did not need to be investigated.
                         if (Qti_it->second==ti)
                         {
                           // This transition was used a representation. Remove it, and Insert the alternative transition. 
                           if (remaining_transition==null_transition)
                           {
                             Qhat.erase(Qti_it);
                           }
                           else
                           {
                             Qti_it->second=remaining_transition;
                           }
                         }
                       
                         // Check whether for the new block this transition is to be used as a representation for an outgoing label/constellation. 
                         Qti_it=Qhat.find(std::pair(m_states[t.from()].block, std::pair(t.label(),m_blocks[m_states[t.to()].block].constellation)));
                         if (Qti_it==Qhat.end())
                         {
                           // Insert this transition as a representation for an outgoing label/constellation pair.
                           Qhat[std::pair(m_states[t.from()].block, std::pair(t.label(),m_blocks[m_states[t.to()].block].constellation))]=ti;
                         }
                       }
                     },
                     // Algorithm 4, line 4.16 and 4.17.
                    [&Ptilde, bi, this](const state_index si)
                       {
                         if (Ptilde[bi].count(si)>0) // if si is a new bottom state, move it. 
                         {  
                           assert(bi!=m_states[si].block);
                           Ptilde[bi].erase(si);
                           if (Ptilde[bi].empty())
                           { 
                             Ptilde.erase(bi);
                           }
                           Ptilde[m_states[si].block].insert(si);
                         }
                       });

          }
        }
        Phat.erase(bi);
        
        // Algorithm 4, line 4.17.
        for(const state_index si: m_P)
        {
          Phat[m_states[si].block].insert(si);
        }
        if (m_P.size()<1000) {m_P.clear(); } else {m_P=std::vector<state_index>(); }  // Avoid unnecessary memory usage.

      }
    }

    void maintain_block_label_to_cotransition(
                   const block_index old_block,
                   const block_index new_block,
                   const transition_index moved_transition,
                   const transition_index alternative_transition,
                   block_label_to_size_t_map& block_label_to_cotransition,
                   const constellation_index ci) const
    {
      const transition& t_move=m_aut.get_transitions()[moved_transition];
      if (m_blocks[m_states[t_move.to()].block].constellation==ci &&
          (!m_aut.is_tau(t_move.label()) || m_blocks[m_states[t_move.from()].block].constellation!=ci))
      { 
        // This is a transition to the current co-constellation.
        
        typename block_label_to_size_t_map::const_iterator bltc_it=
                       block_label_to_cotransition.find(std::pair(old_block,t_move.label()));
        if (bltc_it!=block_label_to_cotransition.end())
        {
          if (bltc_it->second==moved_transition)
          {
            // This transition is being moved. Find a replacement in block_label_to_cotransition.
            block_label_to_cotransition[std::pair(old_block,t_move.label())]=alternative_transition;
          }
        }

        // Check whether there is a representation for the new_block in block_label_to_cotransition.
        bltc_it=block_label_to_cotransition.find(std::pair(new_block,t_move.label()));
        if (bltc_it==block_label_to_cotransition.end())
        {
          // No such transition exists as yet. Give moved transition this purpose.
          block_label_to_cotransition[std::pair(new_block,t_move.label())]=moved_transition;
        }
      }
    }

    transition_index find_inert_co_transition_for_block(const block_index index_block_B, const constellation_index old_constellation)
    {
      transition_index co_t=null_transition;
      std::list< transition_index >::iterator btc_it= m_blocks[index_block_B].block_to_constellation.begin();
      if (btc_it!=m_blocks[index_block_B].block_to_constellation.end())
      {  
        const transition& btc_t=m_aut.get_transitions()[*btc_it];
        if (m_aut.is_tau(btc_t.label()) && m_blocks[m_states[btc_t.to()].block].constellation==old_constellation)
        {
          co_t=*btc_it;
        }
        else 
        {
          btc_it++;
          if (btc_it!=m_blocks[index_block_B].block_to_constellation.end())
          { 
            const transition& btc_t=m_aut.get_transitions()[*btc_it];
            if (m_aut.is_tau(btc_t.label()) && m_blocks[m_states[btc_t.to()].block].constellation==old_constellation)
            {
              co_t=*btc_it;
            }
          }
        } 
      } 
      return co_t;
    }


    bool state_has_outgoing_co_transition(const transition_index transition_to_bi, const constellation_index old_constellation)
    {
      // i1 refers to the position of the transition_to_bi;
      outgoing_transitions_it i1=m_transitions[transition_to_bi].ref_outgoing_transitions;
      // i2 refers to the last position, unless i1 is the last, then i2 is the first of all transitions with the same s, a and C.
      outgoing_transitions_it i2=i1->start_same_saC;
      if (i2<=i1) 
      {
        // So, i1 is the last element with the same s, a, C.
        i2=i1+1;
      }
      else
      {
        // In this case i2 refers to the last element with the same s, a, C.
        i2++;
      }
      if (i2==m_outgoing_transitions.end())
      {
        return false;
      }
      const transition& t1=m_aut.get_transitions()[transition_to_bi];
      const transition& t2=m_aut.get_transitions()[i2->transition];
      return t1.from()==t2.from() && t1.label()==t2.label() && m_blocks[m_states[t2.to()].block].constellation==old_constellation;
    }


    void refine_partition_until_it_becomes_stable()
    {
      // This represents the while loop in Algorithm 1 from line 1.6 to 1.25.

      // Algorithm 1, line 1.6.
      while (!m_non_trivial_constellations.empty())
      {
        assert(check_data_structures("MAIN LOOP"));
        const set_of_constellations_type::const_iterator i=m_non_trivial_constellations.begin();
        constellation_index ci= *i;

        // Algorithm 1, line 1.7.
        std::forward_list<block_index>::iterator fl=m_constellations[ci].blocks.begin();
        std::size_t index_block_B=*fl;       // The first block.
        std::size_t second_block_B=*(++fl);  // The second block.

        if (number_of_states_in_block(index_block_B)<number_of_states_in_block(second_block_B))
        {
          m_constellations[ci].blocks.pop_front();
        }
        else
        {
          m_constellations[ci].blocks.erase_after(m_constellations[ci].blocks.begin());
          index_block_B=second_block_B;
        }
        
        // Algorithm 1, line 1.8.
        fl=m_constellations[ci].blocks.begin();
        if (++fl == m_constellations[ci].blocks.end()) // Constellation is trivial.
        {
          m_non_trivial_constellations.erase(ci);
        }
        m_constellations.emplace_back(index_block_B);
        const constellation_index old_constellation=m_blocks[index_block_B].constellation;
        const constellation_index new_constellation=m_constellations.size()-1;
        m_blocks[index_block_B].constellation=new_constellation;
        // Here the variables block_to_constellation and the doubly linked list L_B->C in blocks must be still be updated.
        // This happens further below.

        // Algorithm 1, line 1.9.
        states_per_action_label_type calM;
        state_label_to_size_t_map newly_created_state_to_constellation_count_entry;
        block_label_to_size_t_map block_label_to_cotransition;
        state_label_to_transition_map transition_that_marked_a_state_per_label; 
        // state_label_to_size_t_map outgoing_transitions_count_per_label_state;
        
 
        // The following data structure maintains per state and action label from where to where the start_same_saC pointers
        // in m_outgoing_transitions still have to be set. 
        typedef std::unordered_map < std::pair<state_index, label_index>, std::pair < outgoing_transitions_it, outgoing_transitions_it> >
                repair_pointers_type;
        repair_pointers_type repair_pointers;
 
        // Walk through all states in block B
        for(typename std::vector<state_index>::iterator i=m_blocks[index_block_B].start_bottom_states;
                         i!=m_blocks[index_block_B].end_states; 
                         ++i)
        {
          // and visit the incoming transitions. 

          for(std::vector<transition_index>::iterator j=m_states[*i].start_incoming_inert_transitions;
                 j!=m_incoming_transitions.end() &&
                 ((*i)+1==m_states_in_blocks.size() || j!=m_states[(*i)+1].start_incoming_inert_transitions); 
                ++j)
          {
            const transition& t=m_aut.get_transitions()[*j];
            transition_that_marked_a_state_per_label[std::pair(t.from(), t.label())]=*j;
            
            // Add the source state grouped per label in calM, provided the transition is non inert.
            if (!m_aut.is_tau(t.label()) || m_states[t.from()].block!=m_states[t.to()].block)
            {
              calM[t.label()].insert(t.from());
            }
 
            // Update the state-action-constellation (saC) references in m_outgoing_transitions. 
            const outgoing_transitions_it pos1=m_transitions[*j].ref_outgoing_transitions;
            outgoing_transitions_it end_same_saC;
            if (pos1->start_same_saC<pos1)
            {
              // This is the last transition with the same saC. 
              end_same_saC=pos1;
            }
            else
            {  
              end_same_saC=pos1->start_same_saC;
            }
            const outgoing_transitions_it pos2=end_same_saC->start_same_saC;
            if (pos1!=pos2)
            {
              std::swap(pos1->transition,pos2->transition);
              m_transitions[pos1->transition].ref_outgoing_transitions=pos1;
              m_transitions[pos2->transition].ref_outgoing_transitions=pos2;
            }
            if (end_same_saC->start_same_saC<end_same_saC)
            {
              end_same_saC->start_same_saC++;
            }
       
            typename repair_pointers_type::iterator rep_it=repair_pointers.find(std::pair(t.from(), t.label()));
            if (rep_it==repair_pointers.end())
            {
              repair_pointers[std::pair(t.from(), t.label())]=std::pair(pos2,pos2);
            }
            else
            {
              rep_it->second.second=pos2;  // set the end of the items to repair to pos2.
            }
            
            // Update the block_label_to_cotransition map.
            if (block_label_to_cotransition.find(std::pair(m_states[t.from()].block,t.label()))==block_label_to_cotransition.end())
            {
              // Not found. Add a transition from the LBC_list to block_label_to_cotransition
              // that goes to C\B, or the null_transition if no such transition exists, which prevents searching
              // the list again. Except if t.from is in C\B and a=tau, because in that case it is an inert transition.
              bool found=false;

              if (!m_aut.is_tau(t.label()) || m_blocks[m_states[t.from()].block].constellation!=ci)
              {
                LBC_list_iterator transition_walker(*j, m_transitions);
                
                while (!found && transition_walker!=LBC_list_iterator(null_transition,m_transitions))
                {
                  const transition& tw=m_aut.get_transitions()[*transition_walker];
                  if (m_blocks[m_states[tw.to()].block].constellation==ci)
                  {
                    found=true;
                  }
                  else
                  {
                    ++transition_walker;
                  }
                }
                block_label_to_cotransition[std::pair(m_states[t.from()].block, t.label())]= (found?*transition_walker:null_transition);
              }
              else 
              {
                block_label_to_cotransition[std::pair(m_states[t.from()].block,t.label())]= null_transition;
              }
            }
            // Update the doubly linked list L_B->C in blocks as the constellation is split in B and C\B. 
            update_the_doubly_linked_list_LBC_new_constellation(index_block_B, t, *j);
          }
        }

        // Repair the start_same_saC links.
        for(typename repair_pointers_type::iterator rep_it=repair_pointers.begin(); rep_it!=repair_pointers.end(); rep_it++)
        {
          const outgoing_transitions_it start_repair_pointer=rep_it->second.first;
          const outgoing_transitions_it end_repair_pointer=rep_it->second.second;
          for(outgoing_transitions_it i=start_repair_pointer; i!=end_repair_pointer; ++i)
          {
            i->start_same_saC=end_repair_pointer;
          }
          assert(end_repair_pointer!=m_outgoing_transitions.end());
          end_repair_pointer->start_same_saC=start_repair_pointer;
        }

        // ---------------------------------------------------------------------------------------------
        // First carry out a co-split of B with respect to C\B and an action tau.
        bool hatU_does_not_cover_B_bottom=false;
        for(typename std::vector<state_index>::iterator si=m_blocks[index_block_B].start_bottom_states;
                          !hatU_does_not_cover_B_bottom &&
                          si!=m_blocks[index_block_B].start_non_bottom_states; 
                        ++si)
        {
          bool found=false;
          for(outgoing_transitions_it tti=m_states[*si].start_outgoing_transitions;
                                       !found &&
                                       tti!=m_outgoing_transitions.end() &&
                                       ((*si)+1>=m_states.size() || tti!=m_states[(*si)+1].start_outgoing_transitions);
                                       ++tti)
          { 
            const transition& t=m_aut.get_transitions()[tti->transition];
            if (m_aut.is_tau(t.label()) && m_blocks[m_states[t.to()].block].constellation==old_constellation)
            { 
              found =true;
            }
          }
          if (!found)
          {
            // This state has notransition to the old constellation. 
            hatU_does_not_cover_B_bottom=true;
          }
        }
        if (hatU_does_not_cover_B_bottom)
        {
          // Algorithm 1, line 1.10.
          transition_index co_t=find_inert_co_transition_for_block(index_block_B, old_constellation);
          if (co_t!=null_transition) 
          {
            // Algorithm 1, line 1.19.
            
            bool dummy=false;
            splitB(index_block_B, 
                        LBC_list_state_iterator(co_t,m_transitions, m_aut.get_transitions()), 
                        LBC_list_state_iterator(null_transition, m_transitions, m_aut.get_transitions()), 
                        m_blocks[index_block_B].start_bottom_states, 
                        m_blocks[index_block_B].start_non_bottom_states,
                        [&](const state_index si)
                          { 
                            const state_type_gj s=m_states[si];
                            for(outgoing_transitions_it tti=s.start_outgoing_transitions;
                                 tti!=m_outgoing_transitions.end() &&
                                 (si+1>=m_states.size() || tti!=m_states[si+1].start_outgoing_transitions);
                                 ++tti)
                            {
                              const transition& t=m_aut.get_transitions()[tti->transition];
                              if (m_aut.is_tau(t.label()) && m_blocks[m_states[t.to()].block].constellation==old_constellation)
                              {
                                return true; // this transition does not pass the filter.
                              }
                              tti=tti->start_same_saC; // This is an optimisation.
                            }
                            return false;
                          },
                        dummy,
                        [&block_label_to_cotransition, ci, this]
                          (const block_index old_block, 
                           const block_index new_block,
                           const transition_index moved_transition,
                           const transition_index alternative_transition)
                          {
                            maintain_block_label_to_cotransition(
                                    old_block,
                                    new_block,
                                    moved_transition,
                                    alternative_transition, block_label_to_cotransition,
                                    ci);
                          });
            }
        }
        // Algorithm 1, line 1.10.
        for(const auto& [a, M]: calM)
        {
          assert(check_data_structures("Main loop"));
          // Algorithm 1, line 1.11.
          states_per_block_type Mleft_map;
          for(const state_index si: M)
          {
            Mleft_map[m_states[si].block].insert(si);
          }
          for(const auto& [bi, Mleft]: Mleft_map)
          {
            assert(!Mleft.empty());
            // Check whether the bottom states of bi are not all included in Mleft. 
            bool bottom_states_all_included=true;
            for(typename std::vector<state_index>::iterator state_it=m_blocks[bi].start_bottom_states;
                      state_it!=m_blocks[bi].start_non_bottom_states;
                    state_it++)
            {
              if (Mleft.count(*state_it)==0)
              {
                bottom_states_all_included=false;
                break; // leave the for loop.
              }
            }
            block_index Bpp=bi;
            if (!bottom_states_all_included)
            {
              // Algorithm 1, line 1.12.
              bool M_in_bi1=true;
              const set_of_states_type& Mleft_=Mleft;
              block_index bi1=splitB(bi, Mleft.begin(), 
                                         Mleft.end(), 
                                         m_blocks[bi].start_bottom_states, 
                                         m_blocks[bi].start_non_bottom_states, 
                                         [&Mleft_](const state_index si){ return Mleft_.count(si)>0; },
                                         M_in_bi1,
                                         [&block_label_to_cotransition, ci, this]
                                           (const block_index old_block, 
                                            const block_index new_block,
                                            const transition_index moved_transition,
                                            const transition_index alternative_transition)
                                           {
                                             maintain_block_label_to_cotransition(
                                                     old_block,
                                                     new_block,
                                                     moved_transition,
                                                     alternative_transition, block_label_to_cotransition,
                                                     ci);
                                           });
              // Algorithm 1, line 1.13.
              if (M_in_bi1)
              {
                Bpp=bi1;
              }
              // Algorithm 1, line 1.14 is implicitly done in the call of splitB above.
            }
            // Algorithm 1, line 1.17.
            bool size_U_smaller_than_Bpp_bottom=false;
            for(typename std::vector<state_index>::iterator si=m_blocks[Bpp].start_bottom_states;
                              !size_U_smaller_than_Bpp_bottom &&
                              si!=m_blocks[Bpp].start_non_bottom_states; 
                            ++si)
            {
              assert(transition_that_marked_a_state_per_label.count(std::pair(*si, a))>0);
              if (!state_has_outgoing_co_transition(transition_that_marked_a_state_per_label[std::pair(*si, a)], old_constellation))
              {
                // This state has notransition to the old constellation. 
                size_U_smaller_than_Bpp_bottom=true;
              }
            }
            // Algorithm 1, line 1.18.
            typename block_label_to_size_t_map::const_iterator bltc_it=block_label_to_cotransition.find(std::pair(Bpp,a));
            transition_index co_t=(bltc_it!=block_label_to_cotransition.end()?bltc_it->second:null_transition);
            if (co_t!=null_transition && size_U_smaller_than_Bpp_bottom) 
            {
              // Algorithm 1, line 1.19.
              
              bool dummy=false;
              const label_index& a_=a;
              splitB(Bpp, LBC_list_state_iterator(co_t,m_transitions, m_aut.get_transitions()), 
                          LBC_list_state_iterator(null_transition, m_transitions, m_aut.get_transitions()), 
                          m_blocks[Bpp].start_bottom_states, 
                          m_blocks[Bpp].start_non_bottom_states,
                          [&](const state_index si)
                            { 
                              const state_type_gj s=m_states[si];
                              for(outgoing_transitions_it tti=s.start_outgoing_transitions;
                                   tti!=m_outgoing_transitions.end() &&
                                   (si+1>=m_states.size() || tti!=m_states[si+1].start_outgoing_transitions);
                                   ++tti)
                              {
                                const transition& t=m_aut.get_transitions()[tti->transition];
                                if (t.label()==a_ && m_blocks[m_states[t.to()].block].constellation==old_constellation)
                                {
                                  return true; // this transition does not pass the filter.
                                }
                                tti=tti->start_same_saC; // This is an optimisation.
                              }
                              return false;
                            },
                          dummy,
                          [&block_label_to_cotransition, ci, this]
                            (const block_index old_block, 
                             const block_index new_block,
                             const transition_index moved_transition,
                             const transition_index alternative_transition)
                            {
                              maintain_block_label_to_cotransition(
                                      old_block,
                                      new_block,
                                      moved_transition,
                                      alternative_transition, block_label_to_cotransition,
                                      ci);
                            });
              // Algorithm 1, line 1.20 and 1.21. P is updated implicitly when splitting Bpp.

            }
          }
        }
        
        assert(check_data_structures("Before stabilize"));
        stabilizeB();
      }
    }
};

/* ************************************************************************* */
/*                                                                           */
/*                             I N T E R F A C E                             */
/*                                                                           */
/* ************************************************************************* */


/// \brief nonmember functions serving as interface with the rest of mCRL2
/// \details These functions are copied, almost without changes, from
/// liblts_bisim_gw.h, which was written by Anton Wijs.

/// \brief Reduce transition system l with respect to strong or
/// (divergence-preserving) branching bisimulation.
/// \param[in,out] l                   The transition system that is reduced.
/// \param         branching           If true branching bisimulation is
///                                    applied, otherwise strong bisimulation.
/// \param         preserve_divergence Indicates whether loops of internal
///                                    actions on states must be preserved.  If
///                                    false these are removed.  If true these
///                                    are preserved.
template <class LTS_TYPE>
void bisimulation_reduce_gj(LTS_TYPE& l, const bool branching = false,
                                        const bool preserve_divergence = false)
{
    if (1 >= l.num_states())
    {
        mCRL2log(log::warning) << "There is only 1 state in the LTS. It is not "
                "guaranteed that branching bisimulation minimisation runs in "
                "time O(m log n).\n";
    }
    // Line 1.2: Find tau-SCCs and contract each of them to a single state
    if (branching)
    {
        scc_reduce(l, preserve_divergence);
    }

    // Now apply the branching bisimulation reduction algorithm.  If there
    // are no taus, this will automatically yield strong bisimulation.
    bisim_partitioner_gj<LTS_TYPE> bisim_part(l, branching,
                                                          preserve_divergence);

    // Assign the reduced LTS
    bisim_part.finalize_minimized_LTS();
}


/// \brief Checks whether the two initial states of two LTSs are strong or
/// (divergence-preserving) branching bisimilar.
/// \details This routine uses the O(m log n) branching bisimulation algorithm
/// developed in 2018 by David N. Jansen.  It runs in O(m log n) time and uses
/// O(m) memory, where n is the number of states and m is the number of
/// transitions.
///
/// The LTSs l1 and l2 are not usable anymore after this call.
/// \param[in,out] l1                  A first transition system.
/// \param[in,out] l2                  A second transistion system.
/// \param         branching           If true branching bisimulation is used,
///                                    otherwise strong bisimulation is
///                                    applied.
/// \param         preserve_divergence If true and branching is true, preserve
///                                    tau loops on states.
/// \param         generate_counter_examples  (non-functional, only in the
///                                    interface for historical reasons)
/// \returns True iff the initial states of the transition systems l1 and l2
/// are ((divergence-preserving) branching) bisimilar.
template <class LTS_TYPE>
bool destructive_bisimulation_compare_gj(LTS_TYPE& l1, LTS_TYPE& l2,
        const bool branching = false, const bool preserve_divergence = false,
        const bool generate_counter_examples = false,
        const std::string& /*counter_example_file*/ = "", bool /*structured_output*/ = false)
{
    if (generate_counter_examples)
    {
        mCRL2log(log::warning) << "The GJ24 branching bisimulation "
                              "algorithm does not generate counterexamples.\n";
    }
    std::size_t init_l2(l2.initial_state() + l1.num_states());
    detail::merge(l1, std::move(l2));
    l2.clear(); // No use for l2 anymore.

    // Line 2.1: Find tau-SCCs and contract each of them to a single state
    if (branching)
    {
        detail::scc_partitioner<LTS_TYPE> scc_part(l1);
        scc_part.replace_transition_system(preserve_divergence);
        init_l2 = scc_part.get_eq_class(init_l2);
    }                                                                           else  assert(!preserve_divergence);
                                                                                assert(1 < l1.num_states());
    bisim_partitioner_gj<LTS_TYPE> bisim_part(l1, branching,
                                                          preserve_divergence);

    return bisim_part.in_same_class(l1.initial_state(), init_l2);
}


/// \brief Checks whether the two initial states of two LTSs are strong or
/// (divergence-preserving) branching bisimilar.
/// \details The LTSs l1 and l2 are first duplicated and subsequently reduced
/// modulo bisimulation.  If memory is a concern, one could consider to use
/// destructive_bisimulation_compare().  This routine uses the O(m log n)
/// branching bisimulation algorithm developed in 2018 by David N. Jansen.  It
/// runs in O(m log n) time and uses O(m) memory, where n is the number of
/// states and m is the number of transitions.
/// \param l1                  A first transition system.
/// \param l2                  A second transistion system.
/// \param branching           If true branching bisimulation is used,
///                            otherwise strong bisimulation is applied.
/// \param preserve_divergence If true and branching is true, preserve tau
///                            loops on states.
/// \retval True iff the initial states of the transition systems l1 and l2
/// are ((divergence-preserving) branching) bisimilar.
template <class LTS_TYPE>
inline bool bisimulation_compare_gj(const LTS_TYPE& l1, const LTS_TYPE& l2,
          const bool branching = false, const bool preserve_divergence = false)
{
    LTS_TYPE l1_copy(l1);
    LTS_TYPE l2_copy(l2);
    return destructive_bisimulation_compare_gj(l1_copy, l2_copy, branching,
                                                          preserve_divergence);
}


} // end namespace detail
} // end namespace lts
} // end namespace mcrl2

#endif // ifndef LIBLTS_BISIM_GJ_H
