// Author(s): Muck van Weerdenburg; completely rewritten by Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_lts.cpp

#include <string>
#include <cstring>
#include <sstream>
#include "mcrl2/core/nil.h"
#include "mcrl2/atermpp/aterm_int.h"
//#include "mcrl2/atermpp/aterm_balanced_tree.h"
#include "mcrl2/data/detail/io.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/data/data_expression.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{

using namespace atermpp;

static atermpp::function_symbol transition_header()
{
  static atermpp::function_symbol tr("transition ",3);
  return tr;
}

static atermpp::function_symbol num_of_states_labels_and_initial_state()
{
  static atermpp::function_symbol nslais("num_of_states_labels_and_initial_state",3);
  return nslais;
}

static atermpp::function_symbol lts_header()
{
  static atermpp::function_symbol lts("labelled_transition_system ",7);
  return lts;
}

/// Below we introduce aterm representations for a transition,
/// which can subsequently be stored as an aterm.
class aterm_transition: public atermpp::aterm_appl
{
  public:
    aterm_transition(const size_t source, const size_t label, const size_t target)
      : aterm_appl(transition_header(),aterm_int(source),aterm_int(label),aterm_int(target))
    {}

    size_t source() const
    {
      return (atermpp::down_cast<aterm_int>((*this)[0]).value());
    }

    size_t label() const
    {
      return (atermpp::down_cast<aterm_int>((*this)[1]).value());
    }

    size_t target() const
    {
      return (atermpp::down_cast<aterm_int>((*this)[2]).value());
    }
};

typedef term_list<aterm_transition> aterm_transition_list;
typedef term_list<aterm_appl> state_labels_t; // The state labels have the shape STATE(a1,...,an).
typedef term_list<process::action_list> action_labels_t; // An action label is a lists of actions.

class aterm_labelled_transition_system: public atermpp::aterm_appl
{
  public:
    aterm_labelled_transition_system(const aterm& a)
      : aterm_appl(a)
    {}

    aterm_labelled_transition_system(
               const lts_lts_t& ts,
               const aterm_transition_list& transitions,
               const state_labels_t& state_label_list,
               const action_labels_t& action_label_list)
      : aterm_appl(lts_header(),
                   ts.has_data() ? data::detail::data_specification_to_aterm_data_spec(ts.data()) : core::nil(),
                   ts.has_process_parameters() ? static_cast<aterm>(ts.process_parameters()) : core::nil(),
                   ts.has_action_labels() ? static_cast<aterm>(ts.action_labels()) : core::nil(),
                   aterm_appl(num_of_states_labels_and_initial_state(),
                              aterm_int(ts.num_states()),
                              aterm_int(ts.num_action_labels()),
                              aterm_int(ts.initial_state())),
                   transitions,
                   state_label_list,
                   action_label_list)
    {}

    bool has_data() const
    {
      return (*this)[0]!=core::nil();
    } 

    const data::data_specification data() const
    {
      assert((*this)[0]!=core::nil());
      return data::data_specification(down_cast<aterm_appl>((*this)[0]));
    }
    
    bool has_process_parameters() const
    {
      return (*this)[1]!=core::nil();
    } 

    const data::variable_list process_parameters() const
    {
      assert((*this)[1]!=core::nil());
      return down_cast<data::variable_list>((*this)[1]);
    }
    
    bool has_action_labels() const
    {
      return (*this)[2]!=core::nil();
    } 

    const process::action_label_list action_labels() const
    {
      assert((*this)[2]!=core::nil());
      return down_cast<process::action_label_list>((*this)[2]);
    }

    size_t num_states() const
    {
      const aterm_appl t=down_cast<aterm_appl>((*this)[3]);
      assert(t.function()==num_of_states_labels_and_initial_state());
      return down_cast<aterm_int>(t[0]).value();
    }

    size_t num_action_labels() const
    {
      const aterm_appl t=down_cast<aterm_appl>((*this)[3]);
      assert(t.function()==num_of_states_labels_and_initial_state());
      return down_cast<aterm_int>(t[1]).value();
    }

    size_t initial_state() const
    {
      const aterm_appl t=down_cast<aterm_appl>((*this)[3]);
      assert(t.function()==num_of_states_labels_and_initial_state());
      return down_cast<aterm_int>(t[2]).value();
    }
    
    aterm_transition_list transitions() const
    {
      return down_cast<aterm_transition_list>((*this)[4]);
    }
  
    state_labels_t get_state_labels() const
    {
      return down_cast<state_labels_t>((*this)[5]);
    }
  
    action_labels_t get_action_labels() const
    {
      return down_cast<action_labels_t>((*this)[6]);
    }
};

static void read_from_lts(lts_lts_t& l, const std::string& filename)
{
  aterm input;
  if (filename=="")
  {
    input=read_term_from_binary_stream(std::cin);
  }
  else 
  {
    std::ifstream stream;
    stream.open(filename, std::ofstream::in | std::ofstream::binary);
    input=atermpp::read_term_from_binary_stream(stream);
    stream.close();
  }
  input=data::detail::add_index(input);
  
  if (!input.type_is_appl() || down_cast<aterm_appl>(input).function()!=lts_header())
  {
    throw runtime_error("The input file " + filename + " is not in proper .lts format.\n");
  }
  
  const aterm_labelled_transition_system input_lts(input);
  if (input_lts.has_data())
  {
    l.set_data(input_lts.data());
  }

  if (input_lts.has_process_parameters())
  {
    l.set_process_parameters(input_lts.process_parameters());
  }

  if (input_lts.has_action_labels())
  {
    l.set_action_labels(input_lts.action_labels());
  }
  
  const aterm_transition_list& input_transitions=input_lts.transitions();
  for(const aterm_transition& t: input_transitions)
  {
    l.add_transition(transition(t.source(), t.label(), t.target()));
  }
  
  if (input_lts.get_state_labels().size()==0)
  {
    l.set_num_states(input_lts.num_states());
  }
  else
  {
    assert(input_lts.num_states()==input_lts.get_state_labels().size());
    for (const aterm_appl& state_label:input_lts.get_state_labels())
    {
      l.add_state(state_label);
    }
  }

  if (input_lts.get_action_labels().size()==0)
  {
    l.set_num_action_labels(input_lts.num_action_labels());
  }
  else
  {
    assert(input_lts.num_action_labels()==input_lts.get_action_labels().size());
    for (const process::action_list& action_label:input_lts.get_action_labels())
    {
      l.add_action(lps::multi_action(action_label));
    }
  }

  l.set_initial_state(input_lts.initial_state());
}

static void write_to_lts(const lts_lts_t& l, const std::string& filename)
{
  aterm_transition_list transitions;
  
  for(std::vector<transition>::const_reverse_iterator i=l.get_transitions().rbegin();
                i!=l.get_transitions().rend(); ++i)
  {
    // The transitions are stored in reverse order.
    transitions.push_front(aterm_transition(i->from(),i->label(),i->to()));
  }

  state_labels_t state_label_list;
  if (l.has_state_info())
  { for(size_t i=l.num_state_labels(); i>0;)
    {
      --i;
      state_label_list.push_front(l.state_label(i));
    }
  }

  action_labels_t action_label_list;
  if (l.has_action_info())
  { 
    for(size_t i=l.num_action_labels(); i>0;)
    {
      --i;
      action_label_list.push_front(l.action_label(i).actions());
    }
  }

  const aterm_labelled_transition_system t0(l,transitions,state_label_list,action_label_list);
  const aterm t1 = data::detail::remove_index(t0);
  
  if (filename=="")
  {
    atermpp::write_term_to_binary_stream(t1, std::cout);
  }
  else 
  {
    std::ofstream stream;
    stream.open(filename, std::ofstream::out | std::ofstream::binary);
    atermpp::write_term_to_binary_stream(t1, stream);
    stream.close();
  }
}

} // namespace detail

void lts_lts_t::save(const std::string& filename) const
{
  mCRL2log(log::verbose) << "Starting to save file " << filename << "\n";
  detail::write_to_lts(*this,filename);
}

void lts_lts_t::load(const std::string& filename)
{
  mCRL2log(log::verbose) << "Starting to load file " << filename << "\n";
  detail::read_from_lts(*this,filename);
}

}
}
