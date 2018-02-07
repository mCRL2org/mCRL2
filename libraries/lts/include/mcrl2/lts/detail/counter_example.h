// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/liblts_liblts_counter_example.h
/// \details This file contains a class in which counter examples 
///          for divergence preserving failures refinement can 
///          be constructed. A counterexample is a trace from 
///          the root. The data structure to construct such a 
///          trace is a tree pointing to the root where each branch
///          is labelled with an action. By walking from a leaf to
///          the root the desired counterexample can be constructed.

#ifndef _LIBLTS_COUNTER_EXAMPLE_H
#define _LIBLTS_COUNTER_EXAMPLE_H

#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/trace/trace.h"


namespace mcrl2
{
namespace lts
{

namespace detail
{


/// \brief A simple class storing the index of a label and an index
//         to the counterexample three in the direction of the root.
class action_index_pair
{
  protected:
    std::size_t m_label_index;
    std::size_t m_previous_entry_index;

  public:
    action_index_pair(std::size_t label_index, std::size_t previous_entry_index)
     : m_label_index(label_index), 
       m_previous_entry_index(previous_entry_index)
    {}

    std::size_t label_index() const
    {
      return m_label_index;
    }

    std::size_t previous_entry_index() const
    {
      return m_previous_entry_index;
    }
};

/// \brief A class that can be used to store counterexample trees and 
//         from which a counterexample trace can be extracted.
class counter_example_constructor
{
  public:
    typedef std::size_t index_type;

  protected:
    // The backward three is stored in a deque. 
    // The root has entry std::size_t(-1).

    static const index_type m_root_index=-1;
    std::deque< action_index_pair > m_backward_tree;
    const std::string m_filename;
  
  public:
    counter_example_constructor(const std::string& filename)
     : m_filename(filename)
    {}

    /// \brief Return the index of the root.
    static index_type root_index() 
    {
      return m_root_index;
    }

    /// \brief This function stores a label to the counterexample tree.
    //  \param label_index The label index is the label as used in transitions.
    //  \param previous_entry The index of the counterexample trace to which
    //                        this action label must be attached.
    //  \ret It returns a new index which can be used to extend the counterexample.
    index_type add_transition(std::size_t label_index, index_type previous_entry)
    {
      m_backward_tree.emplace_back(label_index, previous_entry);
      return m_backward_tree.size()-1;
    }

    /* A trace to index is saved, followed by a list of actions in extra_actions. */
    template < class LTS_TYPE >
    void save_counter_example(index_type index, const LTS_TYPE& l, const std::vector<size_t>& extra_actions=std::vector<size_t>()) const
    {
      // We first store the label indices in reverse order in a stack.
      std::stack< index_type > reversed_label_indices;
      for(index_type current_index=index; 
          current_index!=m_root_index; 
          current_index=m_backward_tree[current_index].previous_entry_index())
      {
        reversed_label_indices.push(m_backward_tree[current_index].label_index());
      }

      trace::Trace result;
      while (!reversed_label_indices.empty())
      {
        result.addAction(mcrl2::lps::multi_action(mcrl2::process::action(
                                mcrl2::process::action_label(
                                       core::identifier_string(mcrl2::lts::pp(l.action_label(reversed_label_indices.top()))),
                                       mcrl2::data::sort_expression_list()),
                                mcrl2::data::data_expression_list())));
        reversed_label_indices.pop();
      }

      /* Add the actions in extra actions. */
      for(const size_t& a: extra_actions)
      {
        result.addAction(mcrl2::lps::multi_action(mcrl2::process::action(
                                mcrl2::process::action_label(
                                       core::identifier_string(mcrl2::lts::pp(l.action_label(a))),
                                       mcrl2::data::sort_expression_list()),
                                mcrl2::data::data_expression_list())));
      }
      mCRL2log(log::verbose) << "Saved trace to file " + m_filename + "\n";
      result.save(m_filename);
    }

    /// \brief This function indicates that this is not a dummy counterexample class and that a serious counterexample is required.
    bool is_dummy() const
    {
      return false;
    }
};


/// \brief A class that can be used to construct counter examples if no
//         counter example is desired. Its index_type is empty.
class dummy_counter_example_constructor
{
  public:
    typedef dummy_counter_example_constructor index_type;  // This yields an empty type.

    static index_type root_index()
    {
      return dummy_counter_example_constructor();  // This returns nothing. 
    }

    index_type add_transition(std::size_t /* label_index*/, index_type /* previous_entry*/)
    {
      // This dummy counter example generator intentionally does nothing.
      return *this;
    }
 
    template < class LTS_TYPE >
    void save_counter_example(index_type /* index */, const LTS_TYPE& /*l*/, const std::vector<size_t>& /* extra_actions */ =std::vector<size_t>()) const
    {
      // This dummy counter example generator intentionally does not save anything.
    }

    /// \brief This function indicates that this is a dummy counterexample class and that no counterexample is required.
    bool is_dummy() const
    {
      return true;
    }
};

} // namespace detail
} // namespace lts
} // namespace mcrl2

#endif // _LIBLTS_COUNTER_EXAMPLE_H
