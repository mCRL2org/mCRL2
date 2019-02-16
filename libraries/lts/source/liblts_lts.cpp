// Author(s): Muck van Weerdenburg; completely rewritten by Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_lts.cpp

#include <string>
#include <cstring>
#include <sstream>
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/detail/io.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/lts/lts_lts.h"
// #include "mcrl2/lts/detail/liblts_swap_to_from_probabilistic_lts.h"

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

static atermpp::function_symbol probabilistic_transition_list_header()  // This contains a transition with a probabilistic target.
{
  static atermpp::function_symbol tr("transition_list",4);
  return tr;
}

static atermpp::function_symbol plain_transition_list_header()  // This contains a transition with a single state as target. 
{
  static atermpp::function_symbol tr("plain_transition_list",4);
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

    // Construct a probabilistic transition list element. If the target probabilistic state only contains
    // a single state, store it as a plain_transition_element. 
    aterm_probabilistic_transition_list(const std::size_t source, 
                                        const std::size_t label, 
                                        const probabilistic_lts_lts_t::probabilistic_state_t& target,
                                        const aterm_probabilistic_transition_list& next_transition)
      : aterm_appl((target.size()==1)?plain_transition_list_header():probabilistic_transition_list_header(), 
                   aterm_int(source),
                   aterm_int(label), 
                   ((target.size()==1)?
                           static_cast<aterm>(aterm_int(target.begin()->state())):
                           static_cast<aterm>(state_probability_list(target))),
                   next_transition)
    {} 

    aterm_probabilistic_transition_list(const std::size_t source, 
                                        const std::size_t label, 
                                        const std::size_t target,
                                        const aterm_probabilistic_transition_list& next_transition)
      : aterm_appl(plain_transition_list_header(), 
                   aterm_int(source),
                   aterm_int(label), 
                   aterm_int(target),
                   next_transition)
    {} 

    bool is_probabilistic_transition()
    {
      assert(function()==probabilistic_transition_list_header() || function()==plain_transition_list_header());
      return function()==probabilistic_transition_list_header();
    }

    std::size_t source() const
    {
      return (atermpp::down_cast<aterm_int>((*this)[0]).value());
    }

    std::size_t label() const
    {
      return (atermpp::down_cast<aterm_int>((*this)[1]).value());
    }

    probabilistic_lts_lts_t::probabilistic_state_t probabilistic_target() const
    {
      if (function()==plain_transition_list_header())
      {
        return probabilistic_lts_lts_t::probabilistic_state_t(atermpp::down_cast<aterm_int>((*this)[2]).value());
      } 
      assert(function()==probabilistic_transition_list_header());
      return aterm_list_to_probabilistic_state(atermpp::down_cast<aterm_list>((*this)[2]));
    }

    std::size_t plain_target() const
    {
      return atermpp::down_cast<aterm_int>((*this)[2]).value();
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
      : aterm_appl(atermpp::down_cast<atermpp::aterm_appl>(a))
    {}

    aterm_labelled_transition_system(
               const probabilistic_lts_lts_t& ts,
               const aterm_probabilistic_transition_list& transitions,
               const state_labels_t& state_label_list,
               const action_labels_t& action_label_list)
      : aterm_appl(lts_header(),
                   aterm_appl(meta_data_header(),
                              data::detail::data_specification_to_aterm(ts.data()),
                              ts.process_parameters(),
                              ts.action_label_declarations(),
                              aterm_appl(num_of_states_labels_and_initial_state(),
                                         aterm_int(ts.num_states()),
                                         aterm_int(ts.num_action_labels()),
                                         state_probability_list(ts.initial_probabilistic_state()))),
                   transitions,
                   state_label_list,
                   action_label_list
                  )
    {}

    aterm_labelled_transition_system(
               const lts_lts_t& ts,
               const aterm_probabilistic_transition_list& transitions,
               const state_labels_t& state_label_list,
               const action_labels_t& action_label_list)
      : aterm_appl(lts_header(),
                   aterm_appl(meta_data_header(),
                              data::detail::data_specification_to_aterm(ts.data()),
                              ts.process_parameters(),
                              ts.action_label_declarations(),
                              aterm_appl(num_of_states_labels_and_initial_state(),
                                         aterm_int(ts.num_states()),
                                         aterm_int(ts.num_action_labels()),
                                         state_probability_list(probabilistic_lts_lts_t::probabilistic_state_t(ts.initial_state())))),
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
      action_labels_t action_label_declarations=get_action_label_declarations();
      
      (*this)=aterm(); // Clear this aterm. It is often huge, and in this way the list of transitions can be garbage collected,
                       // while being transformed. 
      
      // Add indices to the transitions. This list is often too long to fit on the stack.
      // Therefore, it is done using this ad_hoc routine. Note that the list is reverted. 

      std::unordered_map<atermpp::aterm_appl, atermpp::aterm> cache;
      aterm_appl new_trans=aterm_appl(transition_empty_header());
      while (trans.function()!= transition_empty_header())
      {
        assert(trans.size()>3);
        if (trans.function()==probabilistic_transition_list_header())
        {
          new_trans=aterm_appl(probabilistic_transition_list_header(),
                             trans[0],
                             trans[1],
                             data::detail::add_index(trans[2],cache),
                             new_trans);
        }
        else
        {
          new_trans=aterm_appl(plain_transition_list_header(),
                             trans[0],
                             trans[1],
                             trans[2],
                             new_trans);
        }
        trans=trans.next();
      }
      
      *this = aterm_appl(lts_header(),
                             data::detail::add_index(md,cache),
                             new_trans,
                             data::detail::add_index(state_labels,cache),
                             data::detail::add_index(action_label_declarations,cache));
    }

    /// \brief Remove indices from dedicated terms such as variables and process names. 
    //  Note that this code can be optimised, as in non probabilistic transition systems
    //  the code for add indices can be skipped on transition lists. 
    void remove_indices()
    {
      aterm_appl md=meta_data();
      aterm_probabilistic_transition_list trans=transitions(); 
      state_labels_t state_labels=get_state_labels();
      action_labels_t action_label_declarations=get_action_label_declarations();
      
      (*this)=aterm(); // Clear this aterm. It is often huge, and in this way the list of transitions can be garbage collected,
                       // while being transformed. 
      
      std::unordered_map<atermpp::aterm_appl, atermpp::aterm> cache;
      // Remove indices from the transitions. This list is often too long to fit on the stack.
      // Therefore, it is done using this ad_hoc routine. Note that the list is reverted. 
      
      aterm_appl new_trans=aterm_appl(transition_empty_header());
      while (trans.function()!= transition_empty_header())
      {
        if (trans.function()==probabilistic_transition_list_header())
        {
          new_trans=aterm_appl(probabilistic_transition_list_header(),
                                 trans[0],
                                 trans[1],
                                 data::detail::remove_index(trans[2],cache),
                                 new_trans);
        }
        else 
        {
          new_trans=aterm_appl(plain_transition_list_header(),
                                 trans[0],
                                 trans[1],
                                 trans[2],
                                 new_trans);
        }
        trans=trans.next();
      } 

      *this = aterm_appl(lts_header(),
                             data::detail::remove_index(md,cache),
                             new_trans,
                             data::detail::remove_index(state_labels,cache),
                             data::detail::remove_index(action_label_declarations,cache));
    }

    const data::data_specification data() const
    {
      return data::data_specification(down_cast<aterm_appl>(meta_data()[0]));
    }
    
    const data::variable_list process_parameters() const
    {
      return down_cast<data::variable_list>(meta_data()[1]);
    }
    
    const process::action_label_list action_label_declarations() const
    {
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
  
    action_labels_t get_action_label_declarations() const
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

static void read_transitions(lts_lts_t& l, const aterm_labelled_transition_system& input_lts)
{
  aterm_probabilistic_transition_list input_transitions=input_lts.transitions();

  while (input_transitions.function()!= transition_empty_header()) 
  {
    assert(input_transitions.function()==probabilistic_transition_list_header() ||
           input_transitions.function()==plain_transition_list_header()); // In this last case, there is one probabilistic target state. 
    if (input_transitions.is_probabilistic_transition())
    {
      throw mcrl2::runtime_error("Trying to read a non probabilistic LTS that appears to contain a probabilistic transition.");
    }
    l.add_transition(transition(input_transitions.source(), input_transitions.label(), input_transitions.plain_target()));
    input_transitions=input_transitions.next();
  }
}

static void read_transitions(probabilistic_lts_lts_t& l, const aterm_labelled_transition_system& input_lts)
{
  aterm_probabilistic_transition_list input_transitions=input_lts.transitions();

  while (input_transitions.function()!= transition_empty_header()) 
  {
    assert(input_transitions.function()==probabilistic_transition_list_header() ||
           input_transitions.function()==plain_transition_list_header()); // In this last case, there is one probabilistic target state. 
    const std::size_t prob_state_index=l.add_probabilistic_state(input_transitions.probabilistic_target());
    l.add_transition(transition(input_transitions.source(), input_transitions.label(), prob_state_index));
    input_transitions=input_transitions.next();
  }
}

static void set_initial_state(lts_lts_t& l, const aterm_labelled_transition_system& input_lts)
{
  const probabilistic_lts_lts_t::probabilistic_state_t& initial_state=input_lts.initial_probabilistic_state();
  if (initial_state.size()>1)
  {
    throw mcrl2::runtime_error("The initial state of the non probabilistic input lts is probabilistic.");
  }
  l.set_initial_state(input_lts.initial_probabilistic_state().begin()->state());
}

static void set_initial_state(probabilistic_lts_lts_t& l, const aterm_labelled_transition_system& input_lts)
{
  l.set_initial_probabilistic_state(input_lts.initial_probabilistic_state());
}

template <class LTS_TRANSITION_SYSTEM>     
static void read_from_lts(LTS_TRANSITION_SYSTEM& l, const std::string& filename)
{
  static_assert(std::is_same<LTS_TRANSITION_SYSTEM,probabilistic_lts_lts_t>::value || 
                std::is_same<LTS_TRANSITION_SYSTEM,lts_lts_t>::value,
                "Function read_from_lts can only be applied to a (probabilistic) lts. ");
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
    catch (std::ifstream::failure&)
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
    catch (std::ifstream::failure&)
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
  
  aterm_labelled_transition_system input_lts(input); // We are copying an aterm. This is cheap. 
  input_lts.add_indices();  // Add indices to certain term types, such as variables, and process/pbes names. 
  
  l.set_data(input_lts.data());

  l.set_process_parameters(input_lts.process_parameters());

  l.set_action_label_declarations(input_lts.action_label_declarations());
  
  // reading transition differs for an lts_lts and a probabilistic_lts.
  read_transitions(l,input_lts);

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

  if (input_lts.get_action_label_declarations().size()==0)
  {
    l.set_num_action_labels(input_lts.num_action_labels());
  }
  else
  {
    assert(input_lts.num_action_labels()==input_lts.get_action_label_declarations().size());
    for (const atermpp::aterm_appl& t: input_lts.get_action_label_declarations())
    {
      assert(t.function()==temporary_multi_action_header());
      const lps::multi_action action=lps::multi_action(down_cast<process::action_list>(t[0]), down_cast<data::data_expression>(t[1]));
      if (!action.actions().empty() || action.has_time()) // The empty label is tau, which is present by default.
      {
        l.add_action(action_label_lts(action)); 
      }
    }
  }
  set_initial_state(l, input_lts);
}

aterm_probabilistic_transition_list make_transition_list(const lts_lts_t& l)
{
  aterm_probabilistic_transition_list resulting_transitions;
  
  for(std::vector<transition>::const_reverse_iterator i=l.get_transitions().rbegin();
                i!=l.get_transitions().rend(); ++i)
  {
    // The transitions are stored in reverse order.
    resulting_transitions=aterm_probabilistic_transition_list(
                                i->from(), 
                                l.apply_hidden_label_map(i->label()), 
                                i->to(),
                                resulting_transitions);
  }
  return resulting_transitions;
}

aterm_probabilistic_transition_list make_transition_list(const probabilistic_lts_lts_t& l)
{
  aterm_probabilistic_transition_list resulting_transitions;
  
  for(std::vector<transition>::const_reverse_iterator i=l.get_transitions().rbegin();
                i!=l.get_transitions().rend(); ++i)
  {
    const probabilistic_lts_lts_t::probabilistic_state_t& probabilistic_state = l.probabilistic_state(i->to());
    // The transitions are stored in reverse order.
    if (probabilistic_state.size()==1)
    {
      // The target is a single state with probability 1. 
      resulting_transitions=aterm_probabilistic_transition_list(
                                i->from(), 
                                l.apply_hidden_label_map(i->label()), 
                                probabilistic_state.begin()->state(),
                                resulting_transitions);
    }
    else
    {
      // The target is a probabilistic state with at least two plain states. 
      resulting_transitions=aterm_probabilistic_transition_list(
                                i->from(), 
                                l.apply_hidden_label_map(i->label()), 
                                probabilistic_state,
                                resulting_transitions);
    }
  }
  return resulting_transitions;
}

template <class LTS_TRANSITION_SYSTEM>     
static void write_to_lts(const LTS_TRANSITION_SYSTEM& l, const std::string& filename)
{
  static_assert(std::is_same<LTS_TRANSITION_SYSTEM,probabilistic_lts_lts_t>::value || 
                std::is_same<LTS_TRANSITION_SYSTEM,lts_lts_t>::value,
                "Function read_from_lts can only be applied to a (probabilistic) lts. ");
  
  // This is different for an probabilistic lts and a non probabilistic lts. 
  const aterm_probabilistic_transition_list transitions=make_transition_list(l);  
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
    catch (std::ofstream::failure&)
    {
      throw mcrl2::runtime_error("Fail to open file " + filename + " for writing.");
    }
    try
    { 
      atermpp::write_term_to_binary_stream(t0, stream);
      stream.close();
    }
    catch (std::ofstream::failure&)
    {
      throw mcrl2::runtime_error("Fail to write lts correctly to the file " + filename + ".");
    }
  }
}

} // namespace detail

void probabilistic_lts_lts_t::save(const std::string& filename) const
{
  mCRL2log(log::verbose) << "Starting to save a probabilistic lts to the file " << filename << ".\n";
  detail::write_to_lts(*this,filename);
}

void lts_lts_t::save(std::string const& filename) const
{
  mCRL2log(log::verbose) << "Starting to save an lts to the file " << filename << ".\n";
  detail::write_to_lts(*this,filename);
}

void probabilistic_lts_lts_t::load(const std::string& filename)
{
  mCRL2log(log::verbose) << "Starting to load a probabilistic lts from the file " << filename << ".\n";
  detail::read_from_lts(*this,filename);
}

void lts_lts_t::load(const std::string& filename)
{
  mCRL2log(log::verbose) << "Starting to load an lts from the file " << filename << ".\n";
  detail::read_from_lts(*this,filename);
}


}
}
