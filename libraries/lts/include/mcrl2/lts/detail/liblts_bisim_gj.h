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
#include <deque>
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_merge.h"
// #include "mcrl2/lts/detail/check_complexity.h"
// #include "mcrl2/lts/detail/fixed_vector.h"

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

typedef std::size_t state_index;
typedef std::size_t action_index;
typedef std::size_t transition_index;
typedef std::size_t block_index;
typedef std::size_t constellation_index;

constexpr transition_index null_transition=-1;

// Below the four main data structures are listed.
struct state_type_gj
{
  block_index block=0;
  std::vector<transition_index>::iterator start_incoming_transitions;
  std::vector<transition_index>::iterator start_outgoing_inert_transitions;
  std::vector<transition_index>::iterator start_outgoing_non_inert_transitions;
  std::vector<state_index>::iterator ref_states_in_blocks;
};

struct transition_type
{
  // The position of the transition type corresponds to m_aut.get_transitions(). 
  // std::size_t from, label, to are found in m_aut.get_transitions().
  std::forward_list<transition_index>::iterator transitions_per_block_to_constellation;
  transition_index previous_L_B_C_element=null_transition;
  transition_index next_L_B_C_element=null_transition;
  std::deque<std::size_t>::iterator trans_count;
};

struct block_type
{
  constellation_index constellation;
  std::vector<state_index>::iterator start_bottom_states;
  std::vector<state_index>::iterator start_non_bottom_states;
  std::vector<state_index>::iterator end_states;
  std::forward_list< transition_index > block_to_constellation;

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

// The struct below facilitates to walk through a L_B_C_list starting from an arbitrary transition.
struct L_B_C_list_iterator
{
  const std::vector<transition_type>& m_transitions;
  transition_index m_current_transition;

  L_B_C_list_iterator(const transition_index ti, const std::vector<transition_type>& transitions)
    : m_transitions(transitions),
      m_current_transition((ti==null_transition?null_transition:*m_transitions[ti].transitions_per_block_to_constellation))
  {}

  void operator++()
  {
    assert(m_current_transition!=null_transition);
    m_current_transition=m_transitions[m_current_transition].next_L_B_C_element;
  }

  transition_index operator *() const
  {
    assert(m_current_transition!=null_transition);
    return m_current_transition;
  }

  // Equality is implemented minimally for the purpose of this algorithm,
  // essentially only intended to compare the iterator to its end, i.e., null_transition. 
  bool operator ==(const L_B_C_list_iterator& other) const
  {
    return m_current_transition==other.m_current_transition;
  }

  bool operator !=(const L_B_C_list_iterator& other) const
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
/// \brief implements the main algorithm for the branching bisimulation
/// quotient
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
    typedef std::unordered_map<std::pair<block_index, label_index>, std::forward_list<transition_index>::iterator >
                      block_label_to_forward_list_iterator_map;
    typedef std::unordered_map<std::pair<label_index, constellation_index>, set_of_states_type> 
                      label_constellation_to_set_of_states_map;
    typedef std::unordered_map<label_index, std::size_t> label_to_size_t_map;

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
    std::vector<transition_index> m_outgoing_transitions;
    std::vector<transition_type> m_transitions;
    std::deque<std::size_t> m_state_to_constellation_count;
    std::vector<state_index> m_states_in_blocks;
    std::vector<block_type> m_blocks;
    std::vector<constellation_type> m_constellations;
    set_of_states_type m_P;

    /// \brief true iff branching (not strong) bisimulation has been requested
    const bool m_branching;
  
    /// \brief true iff divergence-preserving branching bisimulation has been
    /// requested
    /// \details Note that this field must be false if strong bisimulation has
    /// been requested.  There is no such thing as divergence-preserving strong
    /// bisimulation.
    const bool m_preserve_divergence;

    /// The following variable contains all non trivial constellations.
    set_of_constellations_type m_non_trivial_constellations;

    void check_data_structures() const
    {
      assert(m_states.size()==m_aut.num_states());
      assert(m_incoming_transitions.size()==m_aut.num_transitions());
      assert(m_outgoing_transitions.size()==m_aut.num_transitions());

      for(state_index si=0; si< m_states.size(); si++)
      {
        assert(*m_states[si].ref_states_in_blocks==si);
      }
    }

    void print_data_structures(const std::string& header, const bool initialisation=false) const
    {
      mCRL2log(log::debug) << "========= PRINT DATASTRUCTURE: " << header << " =======================================\n";
      mCRL2log(log::debug) << "++++++++++++++++++++  States     ++++++++++++++++++++++++++++\n";
      for(state_index si=0; si<m_states.size(); ++si)
      {
        mCRL2log(log::debug) << "State " << si <<":\n";
        mCRL2log(log::debug) << "  Block: " << m_states[si].block <<"\n";

        mCRL2log(log::debug) << "  Incoming transitions:\n";
        for(std::vector<transition_index>::iterator it=m_states[si].start_incoming_transitions;
                        it!=m_incoming_transitions.end() &&
                        (si+1>=m_states.size() || it!=m_states[si+1].start_incoming_transitions);
                     ++it)
        {
           const transition& t=m_aut.get_transitions()[*it];
           mCRL2log(log::debug) << "  " << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to() << "\n";
        }

        mCRL2log(log::debug) << "  Outgoing inert transitions:\n";
        for(std::vector<transition_index>::iterator it=m_states[si].start_outgoing_inert_transitions;
                        it!=m_states[si].start_outgoing_non_inert_transitions;
                     ++it)
        {
           const transition& t=m_aut.get_transitions()[*it];
           mCRL2log(log::debug) << "  " << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to() << "\n";
        }
        mCRL2log(log::debug) << "  Outgoing non-inert transitions:\n";
        for(std::vector<transition_index>::iterator it=m_states[si].start_outgoing_non_inert_transitions;
                        it!=m_outgoing_transitions.end() &&
                        (si+1>=m_states.size() || it!=m_states[si+1].start_outgoing_inert_transitions);
                     ++it)
        {
           const transition& t=m_aut.get_transitions()[*it];
           mCRL2log(log::debug) << "  " << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to() << "\n";
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
        mCRL2log(log::debug) << "Transition count " << *m_transitions[ti].trans_count << "\n";
        if (!initialisation)
        {
          mCRL2log(log::debug) << "  L_B_C_List\n";
          for(L_B_C_list_iterator i(ti,m_transitions); i!=L_B_C_list_iterator(null_transition,m_transitions); ++i)
          {
            const transition& t=m_aut.get_transitions()[*i];
            mCRL2log(log::debug) << "    " << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to() << "\n";
          }
          mCRL2log(log::debug) << "  L_B_C_List end\n";
        }
      }

      mCRL2log(log::debug) << "++++++++++++++++++++ Blocks ++++++++++++++++++++++++++++\n";
      for(state_index bi=0; bi<m_blocks.size(); ++bi)
      {
        mCRL2log(log::debug) << "  Block " << bi << ":\n";
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
          mCRL2log(log::debug) << "\n  L_B_C_List\n";
          for(const transition_index tti: m_blocks[bi].block_to_constellation)
          {
            mCRL2log(log::debug) << "\n    L_B_C_sublist\n";
            mCRL2log(log::debug) << "      L_B_C_List element\n";
            for(L_B_C_list_iterator i(tti,m_transitions); i!=L_B_C_list_iterator(null_transition,m_transitions); ++i)
            {
               const transition& t=m_aut.get_transitions()[*i];
               mCRL2log(log::debug) << "        " << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to() << "\n";
            }
          }
          mCRL2log(log::debug) << "  L_B_C_List end\n";
        }
        mCRL2log(log::debug) << "\n";
      }

      mCRL2log(log::debug) << "++++++++++++++++++++ Constellations ++++++++++++++++++++++++++++\n";
      for(state_index ci=0; ci<m_constellations.size(); ++ci)
      {
        mCRL2log(log::debug) << "  Constellation " << ci << ":\n";
        mCRL2log(log::debug) << "  Blocks in constellation: ";
        for(const block_index bi: m_constellations[ci].blocks)
        {
           mCRL2log(log::debug) << bi << " ";
        }
        mCRL2log(log::debug) << "\n  End blocks in constellation\n";
      }
      mCRL2log(log::debug) << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
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
      assert(si<m_blocks.size());
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
        T.insert(transition(get_eq_class(t.from()), t.label(), get_eq_class(t.to())));
      }
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

    /*----------------- SplitB -- Algorithm 3 of [GJ 2024] -----------------*/

    std::size_t number_of_states_in_block(const block_index B) const
    {
      if (m_blocks.size()==B+1) // This is the last block.
      {
        return m_states_in_blocks.end()-m_blocks[B].start_bottom_states; 
      }
      return m_blocks[B+1].start_bottom_states-m_blocks[B].start_bottom_states; 
    }

    void swap_states_in_states_in_block(
              typename std::vector<state_index>::iterator pos1, 
              typename std::vector<state_index>::iterator pos2) 
    {
std::cerr << "Xpos1 " << std::distance(m_states_in_blocks.begin(),pos1) << 
             "  Xpos2 " << std::distance(m_states_in_blocks.begin(),pos2) << "\n";
std::cerr << "pos1 " << *pos1 << "  pos2 " << *pos2 << "\n";
std::cerr << "STATINBLOCK1 "; for(auto s: m_states_in_blocks){ std::cerr << "- " << s ; }; std::cerr << "\n";
      if (pos1!=pos2)
      {
        std::swap(*pos1,*pos2);
        m_states[*pos1].ref_states_in_blocks=pos1;
        m_states[*pos2].ref_states_in_blocks=pos2;
std::cerr << "STATINBLOCK4 "; for(auto s: m_states_in_blocks){ std::cerr << "- " << s ; }; std::cerr << "\n";
      }
    }

    block_index split_block_B_into_R_and_BminR(const block_index B, const std::unordered_set<state_index>& R)
    {
std::cerr << "Splt block " << B << " by removing \n";
for(auto si: R){ std::cerr << "S: " << si << "\n"; }
      m_blocks.emplace_back(m_blocks[B].start_bottom_states,m_blocks[B].constellation);
      m_non_trivial_constellations.insert(m_blocks[B].constellation);
      const block_index new_block_index=m_blocks.size()-1;
      for(state_index s: R)
      {
std::cerr << "MOVE STATE " << s << " to a new block\n";
        m_states[s].block=new_block_index;
        typename std::vector<state_index>::iterator pos=m_states[s].ref_states_in_blocks;
        if (pos>=m_blocks[B].start_non_bottom_states) // the state is a non bottom state.
        {
std::cerr << "NON BOTTOM STATE\n";
          // swap_states_in_states_in_block(pos,m_blocks[B].start_bottom_states,m_blocks[B].start_non_bottom_states);
          swap_states_in_states_in_block(pos,m_blocks[B].start_bottom_states);
          swap_states_in_states_in_block(pos,m_blocks[B].start_non_bottom_states);
          m_blocks[B].start_non_bottom_states++;
        }
        else // the state is a bottom state
        {
std::cerr << "BOTTOM STATE\n";
        //  swap_states_in_states_in_block(pos,m_blocks[new_block_index].start_bottom_states,m_blocks[B].start_non_bottom_states);
          swap_states_in_states_in_block(pos,m_blocks[new_block_index].start_non_bottom_states);
          swap_states_in_states_in_block(pos,m_blocks[B].start_non_bottom_states);
        }
        m_blocks[B].start_bottom_states++;
        m_blocks[new_block_index].end_states++;
      }
      return new_block_index;
    }
    
    void insert_in_the_doubly_linked_list_L_B_C_in_blocks(
               const transition& t,
               const transition_index ti,
               std::forward_list<transition_index>::iterator position)
    {
      std::forward_list<transition_index > :: iterator this_block_to_constellation=
                                      m_transitions[ti].transitions_per_block_to_constellation;
      transition_index& current_transition_index= *position;
      // Check whether this is an inert transition.
      if (m_aut.is_tau(t.label()) &&
          m_states[t.from()].block==m_states[t.to()].block)
      {
        // insert before current transition.
        m_transitions[ti].next_L_B_C_element=current_transition_index;
        m_transitions[ti].previous_L_B_C_element=m_transitions[ti].previous_L_B_C_element;;
        if (m_transitions[current_transition_index].previous_L_B_C_element!=null_transition)
        {
          m_transitions[m_transitions[current_transition_index].previous_L_B_C_element].next_L_B_C_element=ti;
        }
        m_transitions[current_transition_index].previous_L_B_C_element=ti;
      }
      else
      {
        // insert after current transition.
        m_transitions[ti].previous_L_B_C_element=current_transition_index;
        m_transitions[ti].next_L_B_C_element=m_transitions[ti].previous_L_B_C_element;;
        if (m_transitions[current_transition_index].next_L_B_C_element!=null_transition)
        {
          m_transitions[m_transitions[current_transition_index].next_L_B_C_element].previous_L_B_C_element=ti;
        }
        m_transitions[current_transition_index].next_L_B_C_element=ti;
      }
    }

    // Move the transition t with transition index ti to an new 
    // L_B_C list if its source state is in block B and the target state switches to a new constellation.
    
    void update_the_doubly_linked_list_L_B_C_new_constellation(
               const block_index index_block_B, 
               const transition& t,
               const transition_index ti)
    {
      if (m_states[t.from()].block==index_block_B)
      {
        std::forward_list<transition_index > :: iterator this_block_to_constellation=
                             m_transitions[ti].transitions_per_block_to_constellation;
        std::forward_list<transition_index > :: iterator next_block_to_constellation=
                             ++std::forward_list<transition_index > :: iterator(this_block_to_constellation);
        if (next_block_to_constellation==m_blocks[m_states[t.from()].block].block_to_constellation.end() ||
            *next_block_to_constellation==null_transition ||
            m_states[m_aut.get_transitions()[*next_block_to_constellation].to()].block!=index_block_B ||
            m_aut.get_transitions()[*next_block_to_constellation].label()!=t.label())
        { 
          // Make a new entry in the list next_block_to_constellation;
          next_block_to_constellation=
                  m_blocks[m_states[m_aut.get_transitions()[ti].from()].block].block_to_constellation.
                           insert_after(this_block_to_constellation, ti);
        }
        // Move the current transition to the next list.
        // First check whether this_block_to_constellation contains exactly transition ti.
        // It must be replaced by a later or earlier element from the L_B_C_list.
        bool last_element_removed=remove_from_the_doubly_linked_list_L_B_C_in_blocks(ti);
        insert_in_the_doubly_linked_list_L_B_C_in_blocks(t, ti, next_block_to_constellation);
        
        if (last_element_removed)
        {
          // move the L_B_C_list in next_block_to_constellation to block_to_constellation
          // and remove the next element.
          *this_block_to_constellation = *next_block_to_constellation;
          m_blocks[m_states[m_aut.get_transitions()[ti].from()].block].block_to_constellation.
                        erase_after(this_block_to_constellation);
        }
      }
    }

    // Update the L_B_C list of a transition, when the from state of the transition moves
    // from block old_bi to new_bi. 
    void update_the_doubly_linked_list_L_B_C_new_block(
               const block_index old_bi,
               const block_index new_bi,
               const transition_index ti,
               std::unordered_map< std::pair <action_index, constellation_index>, 
                            std::forward_list< transition_index >::iterator>& new_LBC_list_entries)
    {
      const transition& t=m_aut.get_transitions()[ti];
      assert(m_states[t.from()].block==old_bi);
      
      std::forward_list<transition_index >::iterator this_block_to_constellation=
                           m_transitions[ti].transitions_per_block_to_constellation;
      std::forward_list<transition_index >::iterator next_block_to_constellation=
                           ++std::forward_list<transition_index > :: iterator(this_block_to_constellation);
      std::unordered_map< std::pair <action_index, constellation_index>,
                          std::forward_list< transition_index >::iterator>::iterator it=
                     new_LBC_list_entries.find(std::pair(t.label(), m_blocks[m_states[t.from()].block].constellation));
      bool last_element_removed=remove_from_the_doubly_linked_list_L_B_C_in_blocks(ti);
      if (it==new_LBC_list_entries.end())
      { 
        // Make a new entry in the list next_block_to_constellation;
        m_blocks[new_bi].block_to_constellation.push_front(ti);
        new_LBC_list_entries[std::pair(t.label(), m_blocks[m_states[t.from()].block].constellation)]=
                 m_blocks[new_bi].block_to_constellation.begin();
        transition_type tt=m_transitions[ti];
        tt.next_L_B_C_element=null_transition;
        tt.previous_L_B_C_element=null_transition;
      }
      else
      {
        // Move the current transition to the next list indicated by the iterator it.
        insert_in_the_doubly_linked_list_L_B_C_in_blocks(t, ti, it->second);
      }
      
      if (last_element_removed)
      {
        // move the L_B_C_list in next_block_to_constellation to block_to_constellation
        // and remove the next element.
        *this_block_to_constellation = *next_block_to_constellation;
        m_blocks[m_states[m_aut.get_transitions()[ti].from()].block].block_to_constellation.
                      erase_after(this_block_to_constellation);
      }
    }

    // Calculate the states R in block B that can inertly reach M and split
    // B in R and B\R. The complexity is conform the smallest block of R and B\R.
    // The L_B_C_list, trans_count and bottom states are not updated. 
    // Provide the index of the newly created block as a result. This block is the smallest of R and B\R.
    // Return in M_in_bi whether the new block bi is the one containing M.
    template <class MARKED_STATE_ITERATOR, 
              class UNMARKED_STATE_ITERATOR>
    block_index simple_splitB(const block_index B, 
                              const MARKED_STATE_ITERATOR M_begin, 
                              const MARKED_STATE_ITERATOR M_end, 
                              const std::function<bool(state_index)>& marked_blocker,
                              const UNMARKED_STATE_ITERATOR M_co_begin,
                              const UNMARKED_STATE_ITERATOR M_co_end,
                              const std::function<bool(state_index)>& unmarked_blocker,
                              bool& M_in_bi)
    {
      const std::size_t B_size=number_of_states_in_block(B);
      std::unordered_set<state_index> U, U_todo;
      std::unordered_set<state_index> R, R_todo;
      typedef enum { initializing, state_checking, aborted } status_type;
      status_type U_status=initializing;
      status_type R_status=initializing;
      MARKED_STATE_ITERATOR M_it=M_begin; 
      UNMARKED_STATE_ITERATOR M_co_it=M_co_begin; 
      // const std::vector<state_index>::iterator bottom_state_walker_end=m_blocks[B].start_bottom_states;

      // Algorithm 3, line 3.2 left.
      std::unordered_map<state_index, size_t> count;


      // start coroutines. Each co-routine handles one state, and then gives control
      // to the other co-routine. The coroutines can be found sequentially below surrounded
      // by a while loop.

      while (true)
      {
for(auto si: U){ std::cerr << "SU: " << si << "\n"; }
for(auto si: U_todo){ std::cerr << "SU_todo: " << si << "\n"; }
for(auto si: R){ std::cerr << "RU: " << si << "\n"; }
for(auto si: R_todo){ std::cerr << "RU_todo: " << si << "\n"; }
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
std::cerr << "U_todo insert " << si << "\n";
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
std::cerr << "SPlit B and U\n";
              block_index block_index_of_U=split_block_B_into_R_and_BminR(B, U);
              M_in_bi = false;
              return block_index_of_U;
            }
            else
            {
              const state_index s=U_todo.extract(U_todo.begin()).value();
std::cerr << "U_todo remove " << s << "\n";
              U.insert(s); 
              count[s]=0;
              // Algorithm 3, line 3.8, left.
              for(std::vector<transition_index>::iterator it=m_states[s].start_incoming_transitions;
                       m_aut.is_tau(m_aut.get_transitions()[*it].label()) &&
                       m_states[m_aut.get_transitions()[*it].from()].block==
                            m_states[m_aut.get_transitions()[*it].to()].block &&
                       it!=m_incoming_transitions.end() &&
                       (s+1>=m_states.size() || it!=m_states[s+1].start_incoming_transitions);
                  it++)
              {
                // Algorithm 3, line 3.12, left.
                state_index from=m_aut.get_transitions()[*it].from();
                if (count.find(from)==count.end()) // count(from) is undefined;
                {
                   // Algorithm 3, line 3.13, left.
                  if (unmarked_blocker(from))
                  {
std::cerr << "UNMARKED BLOCKtrue " << from << "\n";
                    // Algorithm 3, line 3.14, left.
                    count[from]=std::numeric_limits<std::size_t>::max();
                  }
                  else
                  {
std::cerr << "UNMARKED BLOCKfalse " << from << "\n";
                    // Algorithm 3, line 3.15 and 3.18, left.
                    count[from]=std::distance(m_states[from].start_outgoing_inert_transitions,
                                              m_states[from].start_outgoing_non_inert_transitions)-1;
                  }
                }
                else
                {
                  // Algorithm 3, line 3.18, left.
                  count[from]--;
                }
                // Algorithm 3, line 3.19, left.
std::cerr << "COUNT " << from << " is " << count[from] << "\n";
                if (count[from]==0)
                {
                  if (U.find(from)==U.end())
                  {
                    U_todo.insert(from);
std::cerr << "U_todo insert II " << from << "\n";
                  }
                }
              }
            }
            // Algorithm 3, line 3.10 and line 3.11 left. 
            if (2*(U.size()+U_todo.size())>B_size)
            {
std::cerr << "Abort U\n";
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
              if (!marked_blocker(si)) 
              {
                R_todo.insert(si);
              }
            }
            break;
          }
          case state_checking: 
          {
            if (R_todo.empty())
            {
              // split_block B into R and B\R.
std::cerr << "SPlit B and R\n";
              block_index block_index_of_R=split_block_B_into_R_and_BminR(B, R);
              M_in_bi=true;
              return block_index_of_R;
            }
            else
            {
              const state_index s=R_todo.extract(R_todo.begin()).value();
std::cerr << "EXTRACT R: " << s << "\n";
              R.insert(s);
              for(std::vector<transition_index>::iterator it=m_states[s].start_incoming_transitions; 
                          m_aut.is_tau(m_aut.get_transitions()[*it].label()) && 
                          m_states[m_aut.get_transitions()[*it].from()].block==m_states[m_aut.get_transitions()[*it].to()].block &&
                          it!=m_incoming_transitions.end() &&
                                (s+1>=m_states.size() || it!=m_states[s+1].start_incoming_transitions);
                  it++) 
              { 
                const transition& tr=m_aut.get_transitions()[*it];
                if (R.count(tr.from())==0)
                {
                  R_todo.insert(tr.from());
                }
              }
              // Algorithm 3, line 3.10 and line 3.11, right. 
              if (2*(R.size()+R_todo.size())>B_size)
              {
                R_status=aborted;
              }
            }
          }
          default: break;
        }
      }
    }

    // Split block B in R, being the inert-tau transitive closure of M minus those in marked_blocker contains 
    // states that must be in block, and M\R. M_nonmarked, minus those in unmarked_blocker, are those in the other block. 
    // The splitting is done in time O(min(|R|,|B\R|). Return the block index of the smallest
    // block, which is newly created. Indicate in M_in_new_block whether this new block contains M.
    template <class MARKED_STATE_ITERATOR,
              class UNMARKED_STATE_ITERATOR>
    block_index splitB(const block_index B, 
                       const MARKED_STATE_ITERATOR M_begin, 
                       const MARKED_STATE_ITERATOR M_end, 
                       const std::function<bool(state_index)>& marked_blocker,
                       const UNMARKED_STATE_ITERATOR M_co_begin,
                       const UNMARKED_STATE_ITERATOR M_co_end,
                       const std::function<bool(state_index)>& unmarked_blocker,
                       bool& M_in_new_block,
                       bool update_L_B_C_list=true)
    {
      assert(M_begin!=M_end && M_co_begin!=M_co_end);
for(auto i=M_begin; i!=M_end; ++i)
{ std::cerr << "MARKED   " << *i << (marked_blocker(*i)?" out\n":" in\n"); }
for(auto i=M_co_begin; i!=M_co_end; ++i)
{ std::cerr << "UNMARKED " << *i << (unmarked_blocker(*i)?" out\n":" in\n"); }
      block_index bi=simple_splitB(B, M_begin, M_end, marked_blocker, M_co_begin, M_co_end, unmarked_blocker, M_in_new_block);

check_data_structures();
print_data_structures("After_simple_split", true);
      // Update the L_B_C_list, and bottom states, and invariant on inert transitions.
      for(typename std::vector<state_index>::iterator si=m_blocks[bi].start_bottom_states;
                                                      si!=m_blocks[bi].end_states;
                                                      ++si)
      {     
        state_type_gj& s= m_states[*si];
        s.block=bi;

        bool no_inert_transition_seen=true;
        
        // Adapt the L_B_C_list.
        std::unordered_map< std::pair <action_index, constellation_index>, 
                            std::forward_list< transition_index >::iterator> new_LBC_list_entries;
        for(std::vector<transition_index>::iterator ti=s.start_outgoing_inert_transitions; 
                    ti!=s.start_outgoing_non_inert_transitions;
                )
        {       
const transition t=m_aut.get_transitions()[*ti];
std::cerr << t.from() << " -" << m_aut.action_label(t.label()) << "-> " << t.to() << "\n";
          // Situation below is only relevant if not M_in_new_block;
          if (!M_in_new_block)
          {
            const transition& t=m_aut.get_transitions()[*ti];
            assert(m_aut.is_tau(t.label()));
            if (m_states[t.from()].block!=m_states[t.to()].block)
            {
              // This is a transition that has become non-inert.
              // Swap this transition to the non-inert transitions.
              transition_index temp= *s.start_outgoing_non_inert_transitions;
              *s.start_outgoing_non_inert_transitions= *ti;
              *ti=temp;
              s.start_outgoing_non_inert_transitions--;
              // Do not increment ti as it refers to a new transition. 
            }
            else
            {
              if (update_L_B_C_list)
              { 
                update_the_doubly_linked_list_L_B_C_new_block(B, bi, *ti, new_LBC_list_entries);
              }
              no_inert_transition_seen=false;
              ti++;
            }
          }
          else 
          {
            ti++;
          }
        }

        if (update_L_B_C_list)
        {
          for(std::vector<transition_index>::iterator ti=s.start_outgoing_non_inert_transitions; 
                      m_aut.is_tau(m_aut.get_transitions()[*ti].label()) &&
                      ti!=m_outgoing_transitions.end() &&
                      ti!=m_states[*(si+1)].start_outgoing_inert_transitions;
                  ti++)
          {       
            update_the_doubly_linked_list_L_B_C_new_block(B, bi, *ti, new_LBC_list_entries);
          }
        }
        
        if (no_inert_transition_seen)
        {
          // The state at si has become a bottom_state.
          m_P.insert(*si);
          // Move this former non bottom state to the bottom states.
          swap_states_in_states_in_block(si, m_blocks[bi].start_non_bottom_states);
          m_blocks[bi].start_non_bottom_states++;
        }

        // Investigate the incoming tau transitions. 
        if (M_in_new_block)
        {
          std::vector<std::vector<transition_index>::iterator> transitions_that_became_non_inert;
          std::vector<transition_index>::iterator last_former_inert_transition;
          for(std::vector<transition_index>::iterator ti=s.start_incoming_transitions; 
                      m_aut.is_tau(m_aut.get_transitions()[*ti].label()) &&
                      ti!=m_incoming_transitions.end() &&
                      ti!=m_states[*(si+1)].start_incoming_transitions;
                  ti++)
          {       
            if (m_states[m_aut.get_transitions()[*ti].from()].block==
                m_states[m_aut.get_transitions()[*ti].to()].block)
            {
              last_former_inert_transition=ti;
            }
            if (m_states[m_aut.get_transitions()[*ti].from()].block==B &&
                m_states[m_aut.get_transitions()[*ti].to()].block==bi)
            { 
              last_former_inert_transition=ti;
              // This transition did become non-inert.
              transitions_that_became_non_inert.push_back(ti);
              // Check whether from is a new bottom state.
              state_index from=m_aut.get_transitions()[*ti].from();
              transition& from_trans= m_aut.get_transitions()[*(m_states[from].start_outgoing_inert_transitions)];
              assert(from_trans.from()==from);
              if (m_states[from].block!=m_states[from_trans.to()].block ||
                  !m_aut.is_tau(from_trans.label()))
              {
                // This transition is not inert. The state from is a new bottom state.
                m_P.insert(from);
                // Move this former non bottom state to the bottom states.
                typename std::vector<state_index>::iterator position_in_states_in_blocks=m_states[from].ref_states_in_blocks;
                state_index temp=*position_in_states_in_blocks;
                block_index temp_bi=m_states[from].block;
                *position_in_states_in_blocks=*(m_blocks[temp_bi].start_non_bottom_states);
                *(m_blocks[temp_bi].start_non_bottom_states)=temp;
                m_blocks[temp_bi].start_non_bottom_states++;
              }
            }
          }
          // Move the non_inert_transitions to the end.
          while(!transitions_that_became_non_inert.empty())
          {
            std::vector<transition_index>::iterator tti=transitions_that_became_non_inert.back();
            transitions_that_became_non_inert.pop_back();
            transition_index temp= *tti;
            *tti=*last_former_inert_transition;
            *last_former_inert_transition=temp;
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
    
      // Initialise m_incoming_transitions and m_state_to_constellation, m_transitions[t].trans_count.
      typedef std::unordered_map<typename LTS_TYPE::states_size_type, set_of_transitions_type> temporary_store_type;
      temporary_store_type temporary_store;
      transition_index transition_count=0;
      for(const transition& t: m_aut.get_transitions())
      {
        temporary_store[t.to()].insert(transition_count);
        transition_count++;
      }
        
      transitions_per_action_label_type label_to_transition_set_map;
      m_incoming_transitions.reserve(m_aut.num_transitions());
      for(state_index si=0; si<m_states.size(); ++si)
      {
        m_states[si].start_incoming_transitions=m_incoming_transitions.end();
        const set_of_transitions_type& transitions_for_si=temporary_store[si];
        label_to_transition_set_map.clear();
        for(const transition_index ti: transitions_for_si)
        {
          const transition& t=m_aut.get_transitions()[ti];
          label_to_transition_set_map[t.label()].insert(ti);
        }
        // First get the tau transitions.
        set_of_transitions_type& tset=label_to_transition_set_map[m_aut.tau_label_index()];
        
        m_state_to_constellation_count.emplace_back(tset.size());
        for(const transition_index ti: tset)
        {
          m_incoming_transitions.emplace_back(ti);
          m_transitions[ti].trans_count=m_state_to_constellation_count.end()-1;
        }

        // Subsequently, put all the non-tau transitions in incoming transitions. 
        for(const auto& [ai, transition_set]: label_to_transition_set_map)
        {
          if (!m_aut.is_tau(ai))
          {
            m_state_to_constellation_count.emplace_back(tset.size());
            for(const transition_index ti: transition_set)
            {
              m_incoming_transitions.emplace_back(ti);
              m_transitions[ti].trans_count=m_state_to_constellation_count.end()-1;
            }
          }
        }
      }
      
      // Initialise m_outgoing_transitions and
      // initialise m_states_in_blocks, together with start_bottom_states start_non_bottom_states in m_blocks.
      //typedef std::unordered_multimap<typename std::pair<typename LTS_TYPE::states_size_type, typename LTS_TYPE::labels_size_type>, 
      //                                transition_index> temporary_store_type;
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
        
        // First put the tau transitions in_outgoing_transitions.
        set_of_transitions_type& tset=label_to_transition_set_map[m_aut.tau_label_index()];
        
        m_states[si].start_outgoing_inert_transitions=m_outgoing_transitions.end();
        for(const transition_index ti: tset)
        { 
          
          m_outgoing_transitions.emplace_back(ti);
        }

        // Subsequently, put all the non-tau transitions in outgoing_transitions. 
        m_states[si].start_outgoing_non_inert_transitions=m_outgoing_transitions.end();
        for(const auto& [ai, transition_set]: label_to_transition_set_map)
        {
          if (!m_aut.is_tau(ai))
          {
            for(const transition_index ti: transition_set)
            {
              m_outgoing_transitions.emplace_back(ti);
            }
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

      print_data_structures("After initial reading before splitting in the initialisation", true);
      check_data_structures();

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
std::cerr << "Split on " << m_aut.action_label(a)  << " =======================\n";
        states_per_block_type Bprime;
        for(const state_index s: M)
        {
          Bprime[m_states[s].block].insert(s);
        }
        
        for(const auto& [block_index, split_states]: Bprime)
        {
std::cerr << "HIER0\n";
          // Check whether the block B, indexed by block_index, can be split.
          // This means that the bottom states of B are not all in the split_states.
          const set_of_states_type& split_states_=split_states;
          const block_type& B=m_blocks[block_index];
          bool split_is_possible=false;
          for(typename std::vector<state_index>::iterator i= B.start_bottom_states; 
                       !split_is_possible && i< B.start_non_bottom_states; ++i)
          {
std::cerr << "HIER " << *i << "\n";
            if (!(split_states.count(*i)>0))
            {
              split_is_possible=true;
            }
          }  
          if (split_is_possible)
          { 
std::cerr << "HIER1 " << block_index << "\n";
for(typename std::vector<state_index>::iterator i= B.start_bottom_states; i< B.start_non_bottom_states; ++i){ std::cerr << "XX " << *i << "\n";}
            bool dummy=false;
            const bool do_not_split_the_LBC_list=false;
            splitB(block_index, 
                   split_states.begin(),
                   split_states.end(),
                   [](const state_index ){ return false; },
                   B.start_bottom_states,
                   B.start_non_bottom_states,
                   [&split_states_](const state_index si){ return split_states_.count(si)>0; },
                   dummy,
                   do_not_split_the_LBC_list);
          }
        }
      }
      print_data_structures("End initialisation222", true);
     
      // The initial partition has been constructed. Continue with the initiatialisation.
      // Initialise m_transitions[...].transitions_per_block_to_constellation and the doubly linked list
      // represented by next_L_B_C_element and previous_L_B_C_element in transition_type.

      block_label_to_forward_list_iterator_map block_label_to_iterator;
      std::vector<bool> stable_states(m_aut.num_states(), true);
      transition_count=0;
      for(const transition& t: m_aut.get_transitions())
      {
        typename block_label_to_forward_list_iterator_map::iterator it=
                 block_label_to_iterator.find(std::pair(m_states[t.from()].block, t.label()));
        if (it==block_label_to_iterator.end())
        {
          m_blocks[m_states[t.from()].block].block_to_constellation.push_front(transition_count);
          m_transitions[transition_count].previous_L_B_C_element=null_transition;
          m_transitions[transition_count].next_L_B_C_element=null_transition;
          m_transitions[transition_count].transitions_per_block_to_constellation=
                      m_blocks[m_states[t.from()].block].block_to_constellation.begin();
        }
        else
        {
          m_transitions[transition_count].next_L_B_C_element= m_transitions[*(it->second)].next_L_B_C_element; 
          if (m_transitions[*(it->second)].next_L_B_C_element!=null_transition)
          {
            m_transitions[m_transitions[*(it->second)].next_L_B_C_element].previous_L_B_C_element=transition_count;
          }
          m_transitions[transition_count].previous_L_B_C_element=*(it->second);
          m_transitions[*(it->second)].next_L_B_C_element=transition_count;
          m_transitions[transition_count].transitions_per_block_to_constellation=it->second;
        }
        transition_count++;
      }

      // The data structures have now been completely initialized.

      // Algorithm 1, line 1.4 is implicitly done in the call to splitB above.
      
      // Algorithm 1, line 1.5.
      print_data_structures("End initialisation");
      stabilizeB();
      print_data_structures("Stabilized initialisation");
      check_data_structures();
    }
 
    // Update the doubly linked list L_B->C in blocks.
    // First removing and adding a single element are implemented.
    //
    // If there is more than one element it is removed.
    // In that case false is returned. Otherwise, the result is true, 
    // and the element is actually not removed.
    bool remove_from_the_doubly_linked_list_L_B_C_in_blocks(const transition_index ti)
    {
      if (m_transitions[ti].previous_L_B_C_element==null_transition &&
          m_transitions[ti].previous_L_B_C_element==null_transition)
      {
        // This is the only element in the list. Leave it alone.
        return true;
      }
      else
      {
        // There is more than one element.
        if (m_transitions[ti].previous_L_B_C_element!=null_transition)
        {
          m_transitions[m_transitions[ti].previous_L_B_C_element].next_L_B_C_element=
              m_transitions[ti].next_L_B_C_element;
        }
        if (m_transitions[ti].next_L_B_C_element!=null_transition)
        {
          m_transitions[m_transitions[ti].next_L_B_C_element].previous_L_B_C_element=
              m_transitions[ti].previous_L_B_C_element;
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
      m_P.clear();

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
std::cerr << "BOTTOM STATE " << si << "\n";
          for(std::vector<transition_index>::iterator ti=m_states[si].start_outgoing_non_inert_transitions;
                        ti!=m_outgoing_transitions.end() &&
                        (si+1>=m_states.size() || ti!=m_states[si+1].start_outgoing_inert_transitions);
                    ++ti)
          {
            transition& t=m_aut.get_transitions()[*ti];
            grouped_transitions[std::pair(t.label(), m_blocks[m_states[t.to()].block].constellation)].insert(t.from());
          }
        }
        std::unordered_map<block_index, label_constellation_to_set_of_states_map> grouped_transitions_per_block;
        grouped_transitions_per_block[bi]=grouped_transitions;

        std::unordered_map<block_index, set_of_states_type> Ptilde;
        Ptilde[bi]=V;
        
        // Algorithm 4, line 4.7.
        std::unordered_set<transition_index> Q;
        for(const transition_index ti: m_blocks[bi].block_to_constellation)
        {
          Q.insert(ti);
        }
        
        // Algorithm 4, line 4.8.
        while (!Q.empty())
        {
std::cerr << "REPEAT Q ------------------------\n";
          // Algorithm 4, line 4.9.
          const transition_index it=*Q.begin();
          Q.erase(Q.begin());
          // Algorithm 4, line 4.10.
          transition& t=m_aut.get_transitions()[it];
          const block_index bi=m_states[t.from()].block;
          set_of_states_type W=Ptilde[bi];
std::cerr << "W "; for(auto s:W) { std::cerr << s << "   ";} std::cerr << "\n";
          set_of_states_type& aux=grouped_transitions_per_block[bi]
                             [std::pair(t.label(), m_blocks[m_states[t.to()].block].constellation)];
std::cerr << "aux "; for(auto s:aux) { std::cerr << s << "   ";} std::cerr << "\n";
          for(const state_index si: aux) 
          {
std::cerr << "REMOVE " << si << " from W\n";
            W.erase(si);
          }
std::cerr << "Wleeg "; for(auto s:W) { std::cerr << s << "   ";} std::cerr << "\n";
          // Algorithm 4, line 4.10.
          if (!W.empty())
          {
std::cerr << "ER WORDT GESPLITST\n";
            // Algorithm 4, line 4.11, and implicitly 4.12. 
            bool V_in_bi=false;
            block_index bi_new=splitB(bi, 
                                      m_blocks[bi].start_bottom_states, 
                                      m_blocks[bi].start_non_bottom_states,
                                      [&W](const state_index si){ return W.count(si)!=0; },
                                      W.begin(), 
                                      W.end(),
                                      [](const state_index ){ return false; },
                                      V_in_bi);
            // Algorithm 4, line 4.13.
            // XXXXX Split Q, grouped_transitions_per_block, Ptilde  TODO
            bi_new=bi_new+bi_new; //suppress warning

          }
        }
        Phat.erase(bi);
        
        // Algorithm 4, line 4.17.
        for(const state_index si: m_P)
        {
          Phat[m_states[si].block].insert(si);
        }
        m_P.clear();

      }
    }

    void refine_partition_until_it_becomes_stable()
    {
      // This represents the while loop in Algorithm 1 from line 1.6 to 1.25.

      // Algorithm 1, line 1.6.
      while (!m_non_trivial_constellations.empty())
      {
        const set_of_constellations_type::const_iterator i=m_non_trivial_constellations.begin();
        constellation_index ci= *i;
        m_non_trivial_constellations.extract(i);

        // Algorithm 1, line 1.7.
        std::size_t index_block_B=m_constellations[ci].blocks.front();
        std::forward_list<block_index>::iterator fl=m_constellations[ci].blocks.begin();
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
        if (++fl == m_constellations[ci].blocks.end()) // Constellation is not trivial.
        {
          m_non_trivial_constellations.insert(ci);
        }
        m_constellations.emplace_back(index_block_B);
        // Here the variables block_to_constellation and the doubly linked list L_B->C in blocks must be still be updated.
        // Moreover, the variable m_state_to_constellation_count in transitions requires updating.
        // This happens below.

        // Algorithm 1, line 1.9.
        states_per_action_label_type calM;
        state_label_to_size_t_map newly_created_state_to_constellation_count_entry;
        label_to_size_t_map label_to_transition;
        state_label_to_size_t_map outgoing_transitions_count_per_label_state;
        
 
        // Walk through all states in block B
        for(typename std::vector<state_index>::iterator i=m_blocks[index_block_B].start_bottom_states;
                i!=m_states_in_blocks.end() &&
                (index_block_B+1>m_blocks.size() || i!=m_blocks[index_block_B+1].start_bottom_states); 
               ++i)
        {
          // and visit the incoming transitions. 
          for(std::vector<transition_index>::iterator j=m_states[*i].start_incoming_transitions;
                 j!=m_incoming_transitions.end() &&
                 (i+1!=m_states_in_blocks.end() || j!=m_states[*(i+1)].start_incoming_transitions); 
                ++j)
          {
            const transition& t=m_aut.get_transitions()[*j];
            // Add the source state grouped per label in calM, provided the transition is non inert.
            if (!m_aut.is_tau(t.label()) || m_states[t.from()].block!=m_states[t.to()].block)
            {
              calM[t.label()].insert(t.from());
            }
            // Update outgoing_transitions_count_per_label_state;
            typename state_label_to_size_t_map::iterator it=outgoing_transitions_count_per_label_state.find(std::pair(t.label(),t.from()));
            if (it==outgoing_transitions_count_per_label_state.end())
            {
              outgoing_transitions_count_per_label_state[std::pair(t.label(),t.from())]=(*m_transitions[*j].trans_count)-1;
            }
            else 
            {
              (it->second)--; 
            }
            // Update m_state_to_constellation_count.
            if (m_states[t.to()].block==index_block_B)
            {
              const std::size_t new_position=m_state_to_constellation_count.size();
              const std::size_t found_position=
                                   newly_created_state_to_constellation_count_entry.try_emplace(
                                                                      std::pair(t.from(),t.label()),
                                                                      new_position).first->second;
              if (new_position==found_position)
              {
                m_state_to_constellation_count.push_back(1);
                (m_transitions[*j].trans_count)--;
                m_transitions[*j].trans_count=m_state_to_constellation_count.end()-1;
              }
            }
            // Update the label_to_transition map.
            if (label_to_transition.find(t.label())==label_to_transition.end())
            {
              // Not found. Add a transition from the L_B_C_list to label_to_transition
              // that goes to C\B, or the null_transition if no such transition exists, which prevents searching
              // the list again. 
              // First look backwards.
              L_B_C_list_iterator transition_walker(*j, m_transitions);

              bool found=false;
              
              while (!found && *transition_walker!=null_transition)
              {
                transition& tw=m_aut.get_transitions()[*transition_walker];
                if (m_states[tw.to()].block==ci)
                {
                  found=true;
                }
                else
                {
                  ++transition_walker;
                }
              }
              label_to_transition[t.label()]= *transition_walker;
            }
            // Update the doubly linked list L_B->C in blocks as the constellation is split in B and C\B. 
            update_the_doubly_linked_list_L_B_C_new_constellation(index_block_B, t, *j);
          }
        }
        
        // Algorithm 1, line 1.10.
        for(const auto& [a, M]: calM)
        {
          print_data_structures("Algorithm 1. Main loop");
          check_data_structures();
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
            if (!bottom_states_all_included)
            {
              // Algorithm 1, line 1.12.
              bool M_in_bi1=true;
              const set_of_states_type& Mleft_=Mleft;
              block_index bi1=splitB(bi, Mleft.begin(), 
                                         Mleft.end(), 
                                         [](const state_index ){ return false; },
                                         m_blocks[bi].start_bottom_states, 
                                         m_blocks[bi].start_non_bottom_states, 
                                         [&Mleft_](const state_index si){ return Mleft_.count(si)>0; },
                                         M_in_bi1);
              // Algorithm 1, line 1.13.
              block_index Bpp=(M_in_bi1? bi1: bi);
              // Algorithm 1, line 1.14 is implicitly done in the call of splitB above.
              // Algorithm 1, line 1.15.
              bool size_U_larger_than_zero=false;
              bool size_U_smaller_than_Bpp_bottom=false;
              for(typename std::vector<state_index>::iterator si=m_blocks[Bpp].start_bottom_states;
                                !(size_U_larger_than_zero && size_U_smaller_than_Bpp_bottom) &&
                                si!=m_blocks[Bpp].start_non_bottom_states; 
                              ++si)
              {
                if (outgoing_transitions_count_per_label_state[std::pair(a,*si)]>0)
                {
                  size_U_larger_than_zero=true;
                }
                else
                {
                  size_U_larger_than_zero=true;
                }
              }
              // Algorithm 1, line 1.16.
              if (size_U_larger_than_zero && size_U_smaller_than_Bpp_bottom) 
              {
                // Algorithm 1, line 1.17.
                transition_index co_t=label_to_transition[a];
                
                bool dummy=false;
                const label_index& a_=a;
                splitB(Bpp, L_B_C_list_iterator(co_t,m_transitions), 
                            L_B_C_list_iterator(null_transition, m_transitions), 
                            [](const state_index ){ return false; },
                            m_blocks[Bpp].start_bottom_states, 
                            m_blocks[Bpp].start_non_bottom_states,
                            [&](const state_index si){ return outgoing_transitions_count_per_label_state.count(std::pair(a_,si))>0; },
                            dummy);
                // Algorithm 1, line 1.18 and 1.19. P is updated implicitly when splitting Bpp.

              }
              
            }
          }
        }
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
    // Line 2.1: Find tau-SCCs and contract each of them to a single state
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
        mCRL2log(log::warning) << "The JGKW20 branching bisimulation "
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
