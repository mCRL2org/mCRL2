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
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/detail/io.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/detail/liblts_swap_to_from_probabilistic_lts.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{

using namespace atermpp;

static atermpp::function_symbol transition_empty_header()
{
  static atermpp::function_symbol tr("transition_empty_list",0);
  return tr;
}
static atermpp::function_symbol transition_list_header()
{
  static atermpp::function_symbol tr("transition_list",4);
  return tr;
}

static atermpp::function_symbol num_of_states_labels_and_initial_state()
{
  static atermpp::function_symbol nslais("num_of_states_labels_and_initial_state",3);
  return nslais;
}

static atermpp::function_symbol lts_header()
{
  static atermpp::function_symbol lts("labelled_transition_system",4);
  return lts;
}

static atermpp::function_symbol meta_data_header()
{
  static atermpp::function_symbol mdh("meta_data_header",4);
  return mdh;
}

static atermpp::function_symbol temporary_multi_action_header()
{
  static atermpp::function_symbol mdh("multi_action",2);
  return mdh;
}

static aterm_list state_probability_list(const probabilistic_lts_lts_t::probabilistic_state_t& target)
{
  aterm_list result;
  for(const lps::state_probability_pair<std::size_t, mcrl2::lps::probabilistic_data_expression>& p: target)
  {
    result.push_front(p.probability());
    result.push_front(aterm_int(p.state()));
  }
  return result;
}

static probabilistic_lts_lts_t::probabilistic_state_t aterm_list_to_probabilistic_state(const atermpp::aterm_list& l)
{
  std::vector<lps::state_probability_pair<std::size_t, mcrl2::lps::probabilistic_data_expression>> result;
  for(aterm_list::const_iterator i=l.begin(); i!=l.end(); ++i)
  {
    const std::size_t state_number=down_cast<aterm_int>(*i).value();
    assert(i!=l.end());
    ++i;
    const lps::probabilistic_data_expression& t=down_cast<lps::probabilistic_data_expression>(*i);
    result.push_back(lps::state_probability_pair<std::size_t, mcrl2::lps::probabilistic_data_expression>(state_number,t));
  }
  return probabilistic_lts_lts_t::probabilistic_state_t(result.begin(),result.end());
}

/// Below we introduce aterm representations for a list with all transition,
/// which can subsequently be stored as an aterm.
class aterm_probabilistic_transition_list: public aterm_appl
{
  public:
    aterm_probabilistic_transition_list()
      : aterm_appl(transition_empty_header())
    {}

    aterm_probabilistic_transition_list(const std::size_t source, 
                                        const std::size_t label, 
                                        const probabilistic_lts_lts_t::probabilistic_state_t& target,
                                        const aterm_probabilistic_transition_list& next_transition)
      : aterm_appl(transition_list_header(), 
                   aterm_int(source),
                   aterm_int(label), 
                   state_probability_list(target),
                   next_transition)
    {} 

    std::size_t source() const
    {
      return (atermpp::down_cast<aterm_int>((*this)[0]).value());
    }

    std::size_t label() const
    {
      return (atermpp::down_cast<aterm_int>((*this)[1]).value());
    }

    probabilistic_lts_lts_t::probabilistic_state_t target() const
    {
      return aterm_list_to_probabilistic_state(atermpp::down_cast<aterm_list>((*this)[2]));
    }

    const aterm_probabilistic_transition_list& next() const 
    {
      return atermpp::down_cast<aterm_probabilistic_transition_list>((*this)[3]);
    }
};

// typedef term_list<aterm_probabilistic_transition> aterm_transition_list;
typedef term_list<state_label_lts> state_labels_t;               // The state labels listed consecutively.
typedef term_list<atermpp::aterm_appl> action_labels_t;          // A multiaction has the shape "multi_action(action_list,data_expression)
typedef term_list<data::data_expression> probabilistic_labels_t; // This contains a list of probabilities.
typedef term_list<data::function_symbol> boolean_list_t;         // A list with constants true or false, indicating
                                                                 // whether a state is probabilistic.

class aterm_labelled_transition_system: public atermpp::aterm_appl
{
  public:
    aterm_labelled_transition_system(const aterm& a)
      : aterm_appl(a)
    {}

    aterm_labelled_transition_system(
               const probabilistic_lts_lts_t& ts,
               const aterm_probabilistic_transition_list& transitions,
               const state_labels_t& state_label_list,
               const action_labels_t& action_label_list)
      : aterm_appl(lts_header(),
                   aterm_appl(meta_data_header(),
                              ts.has_data() ? data::detail::data_specification_to_aterm_data_spec(ts.data()) : core::nil(),
                              ts.has_process_parameters() ? static_cast<aterm>(ts.process_parameters()) : core::nil(),
                              ts.has_action_labels() ? static_cast<aterm>(ts.action_labels()) : core::nil(),
                              aterm_appl(num_of_states_labels_and_initial_state(),
                                         aterm_int(ts.num_states()),
                                         aterm_int(ts.num_action_labels()),
                                         state_probability_list(ts.initial_probabilistic_state()))),
                   transitions,
                   state_label_list,
                   action_label_list
                  )
    {}

    // \brief add_index() adds a unique index to some term types, such as variables, to access data about them 
    //        quickly. When loading a term, these indices must first be added before a term can be used in the toolset.
    void add_indices()
    {
      aterm_appl md=meta_data();
      aterm_probabilistic_transition_list trans=transitions(); 
      state_labels_t state_labels=get_state_labels();
      action_labels_t action_labels=get_action_labels();
      
      (*this)=aterm(); // Clear this aterm. It is often huge, and in this way the list of transitions can be garbage collected,
                       // while being transformed. 
      //
      // Add indices to the transitions. This list is often too long to fit on the stack.
      // Therefore, it is done using this ad_hoc routine. Note that the list is reverted. 
      std::unordered_map<atermpp::aterm_appl, atermpp::aterm> cache;
      aterm_appl new_trans=aterm_appl(transition_empty_header());
      while (trans.function()!= transition_empty_header())
      {
        assert(trans.size()>3);
        new_trans=aterm_appl(transition_list_header(),
                             trans[0],
                             trans[1],
                             data::detail::add_index(trans[2],cache),
                             new_trans);
        trans=trans.next();
      }

      *this = aterm_appl(lts_header(),
                             data::detail::add_index(md,cache),
                             new_trans,
                             data::detail::add_index(state_labels,cache),
                             data::detail::add_index(action_labels,cache));
    }

    /// \brief Remove indices from dedicated terms such as variables and process names. 
    void remove_indices()
    {
      aterm_appl md=meta_data();
      aterm_probabilistic_transition_list trans=transitions(); 
      state_labels_t state_labels=get_state_labels();
      action_labels_t action_labels=get_action_labels();
      
      (*this)=aterm(); // Clear this aterm. It is often huge, and in this way the list of transitions can be garbage collected,
                       // while being transformed. 
      
      // Remove indices from the transitions. This list is often too long to fit on the stack.
      // Therefore, it is done using this ad_hoc routine. Note that the list is reverted. 
      std::unordered_map<atermpp::aterm_appl, atermpp::aterm> cache;
      aterm_appl new_trans=aterm_appl(transition_empty_header());
      while (trans.function()!= transition_empty_header())
      {
        new_trans=aterm_appl(transition_list_header(),
                                 trans[0],
                                 trans[1],
                                 data::detail::remove_index(trans[2],cache),
                                 new_trans);
        trans=trans.next();
      }

      *this = aterm_appl(lts_header(),
                             data::detail::remove_index(md,cache),
                             new_trans,
                             data::detail::remove_index(state_labels,cache),
                             data::detail::remove_index(action_labels,cache));
    }

    bool has_data() const
    {
      return meta_data()[0]!=core::nil();
    } 

    const data::data_specification data() const
    {
      assert(meta_data()[0]!=core::nil());
      return data::data_specification(down_cast<aterm_appl>(meta_data()[0]));
    }
    
    bool has_process_parameters() const
    {
      return meta_data()[1]!=core::nil();
    } 

    const data::variable_list process_parameters() const
    {
      assert(meta_data()[1]!=core::nil());
      return down_cast<data::variable_list>(meta_data()[1]);
    }
    
    bool has_action_labels() const
    {
      return meta_data()[2]!=core::nil();
    } 

    const process::action_label_list action_labels() const
    {
      assert(meta_data()[2]!=core::nil());
      return down_cast<process::action_label_list>(meta_data()[2]);
    }

    std::size_t num_states() const
    {
      const aterm_appl& t=down_cast<aterm_appl>(meta_data()[3]);
      assert(t.function()==num_of_states_labels_and_initial_state());
      return down_cast<aterm_int>(t[0]).value();
    }

    std::size_t num_action_labels() const
    {
      const aterm_appl& t=down_cast<aterm_appl>(meta_data()[3]);
      assert(t.function()==num_of_states_labels_and_initial_state());
      return down_cast<aterm_int>(t[1]).value();
    }

    probabilistic_lts_lts_t::probabilistic_state_t initial_probabilistic_state() const
    {
      const aterm_appl& t=down_cast<aterm_appl>(meta_data()[3]);
      assert(t.function()==num_of_states_labels_and_initial_state());
      
      return aterm_list_to_probabilistic_state(down_cast<aterm_list>(t[2]));
    }
    
    aterm_probabilistic_transition_list transitions() const
    {
      return down_cast<aterm_probabilistic_transition_list>((*this)[1]);
    }
  
    state_labels_t get_state_labels() const
    {
      return down_cast<state_labels_t>((*this)[2]);
    }
  
    action_labels_t get_action_labels() const
    {
      return down_cast<action_labels_t>((*this)[3]);
    }
  
  protected:
    
    const aterm_appl& meta_data() const
    {
      assert(atermpp::down_cast<aterm_appl>((*this)[0]).function()==meta_data_header());
      return atermpp::down_cast<aterm_appl>((*this)[0]);
    }
};

static void read_from_lts(probabilistic_lts_lts_t& l, const std::string& filename)
{
  aterm input;
  if (filename=="")
  {
    input=read_term_from_binary_stream(std::cin);
  }
  else 
  {
    std::ifstream stream;
    stream.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
    try
    {  
      stream.open(filename, std::ifstream::in | std::ifstream::binary);
    }
    catch (std::ifstream::failure)
    {
      if (filename=="")
      {
        throw mcrl2::runtime_error("Fail to open standard input to read an lts.");
      }
      else 
      {
        throw mcrl2::runtime_error("Fail to open file " + filename + " to read an lts.");
      }
    }

    try
    {
      input=atermpp::read_term_from_binary_stream(stream);
      stream.close();
    }
    catch (std::ifstream::failure)
    {
      if (filename=="")
      {
        throw mcrl2::runtime_error("Fail to correctly read an lts from standard input.");
      }
      else
      {
        throw mcrl2::runtime_error("Fail to correctly read an lts from the file " + filename + ".");
      }
    }
    
  }

  if (!input.type_is_appl() || down_cast<aterm_appl>(input).function()!=lts_header())
  {
    throw runtime_error("The input file " + filename + " is not in proper .lts format.");
  }
  
  // First check whether the input is a valid lts.
  const aterm meta_data=atermpp::down_cast<aterm_appl>(input)[0];
  if (!meta_data.type_is_appl() || down_cast<aterm_appl>(meta_data).function()!=meta_data_header())
  {
    throw runtime_error("The input file " + filename + " is not in proper .lts format. There is a problem with the datatypes, process parameters and action declarations.");
  }
  
  aterm_labelled_transition_system input_lts(input);
  input=aterm(); // The input is a large term. We do not need it anymore. 
  input_lts.add_indices();  // Add indices to certain term types, such as variables, and process/pbes names. 
  
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
  
  aterm_probabilistic_transition_list input_transitions=input_lts.transitions();
  while (input_transitions.function()!= transition_empty_header()) 
  {
    assert(input_transitions.function()==transition_list_header());
    const std::size_t prob_state_index=l.add_probabilistic_state(input_transitions.target());
    l.add_transition(transition(input_transitions.source(), input_transitions.label(), prob_state_index));
    input_transitions=input_transitions.next();
  }
  
  if (input_lts.get_state_labels().size()==0)
  {
    l.set_num_states(input_lts.num_states());
  }
  else
  {
    assert(input_lts.num_states()==input_lts.get_state_labels().size());
    for (const state_label_lts& state_label: input_lts.get_state_labels())
    {
      l.add_state(state_label_lts(state_label));
    }
  }

  if (input_lts.get_action_labels().size()==0)
  {
    l.set_num_action_labels(input_lts.num_action_labels());
  }
  else
  {
    assert(input_lts.num_action_labels()==input_lts.get_action_labels().size());
    for (const atermpp::aterm_appl& t: input_lts.get_action_labels())
    {
      assert(t.function()==temporary_multi_action_header());
      const lps::multi_action action=lps::multi_action(process::action_list(t[0]), data::data_expression(t[1]));
      if (!action.actions().empty() || action.has_time()) // The empty label is tau, which is present by default.
      {
        l.add_action(action_label_lts(action)); 
      }
    }
  }
  l.set_initial_probabilistic_state(input_lts.initial_probabilistic_state());
}

static void write_to_lts(const probabilistic_lts_lts_t& l, const std::string& filename)
{
  aterm_probabilistic_transition_list transitions;
  
  for(std::vector<transition>::const_reverse_iterator i=l.get_transitions().rbegin();
                i!=l.get_transitions().rend(); ++i)
  {
    // The transitions are stored in reverse order.
    transitions=aterm_probabilistic_transition_list(
                                i->from(), 
                                l.apply_hidden_label_map(i->label()), 
                                l.probabilistic_state(i->to()),
                                transitions);
  }

  state_labels_t state_label_list;
  if (l.has_state_info())
  { for(std::size_t i=l.num_state_labels(); i>0;)
    {
      --i;
      state_label_list.push_front(l.state_label(i));
    }
  }

  action_labels_t action_label_list;
  if (l.has_action_info())
  { 
    for(std::size_t i=l.num_action_labels(); i>0;)
    {
      --i;
      action_label_list.push_front(atermpp::aterm_appl(temporary_multi_action_header(),l.action_label(i).actions(),l.action_label(i).time()));
    }
  }

  aterm_labelled_transition_system t0(l,
                                      transitions,
                                      state_label_list,
                                      action_label_list);
  t0.remove_indices();

  if (filename=="")
  {
    atermpp::write_term_to_binary_stream(t0, std::cout);
  }
  else 
  {
    std::ofstream stream;
    stream.exceptions ( std::ofstream::failbit | std::ofstream::badbit );
    try
    {
      stream.open(filename, std::ofstream::out | std::ofstream::binary);
    }
    catch (std::ofstream::failure )
    {
      throw mcrl2::runtime_error("Fail to open file " + filename + " for writing.");
    }
    try
    { 
      atermpp::write_term_to_binary_stream(t0, stream);
      stream.close();
    }
    catch (std::ofstream::failure)
    {
      throw mcrl2::runtime_error("Fail to write lts correctly to the file " + filename + ".");
    }
  }
}

} // namespace detail

void probabilistic_lts_lts_t::save(const std::string& filename) const
{
  mCRL2log(log::verbose) << "Starting to save file " << filename << "\n";
  detail::write_to_lts(*this,filename);
}

void probabilistic_lts_lts_t::load(const std::string& filename)
{
  mCRL2log(log::verbose) << "Starting to load file " << filename << "\n";
  detail::read_from_lts(*this,filename);
}

void lts_lts_t::load(const std::string& filename)
{
  probabilistic_lts_lts_t l;
  l.load(filename);
  detail::swap_to_non_probabilistic_lts
             <state_label_lts,
              action_label_lts,
              probabilistic_state<std::size_t, lps::probabilistic_data_expression>,
              detail::lts_lts_base>(l,*this);
}

void lts_lts_t::save(std::string const& filename) const
{
  probabilistic_lts_lts_t l;
  detail::translate_to_probabilistic_lts
            <state_label_lts,
             action_label_lts,
             probabilistic_state<std::size_t, lps::probabilistic_data_expression>,
             detail::lts_lts_base >(*this,l);
  l.save(filename);
}


}
}
