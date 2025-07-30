// Author(s): Muck van Weerdenburg. Jan Friso Groote.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lts/trace.h
/// \brief This class allows to flexibly manipulate traces.
/// \details This class allows to build, traverse and store traces.
/// Traces are sequences of (possibly timed) multi actions, that may be endowed with state
/// information for the intermediate states. In the trace the current position is being
/// maintained. Adding actions or setting states is always relative to the current position. 
/// \author Muck van Weerdenburg

#ifndef MCRL2_LTS_TRACE_H
#define MCRL2_LTS_TRACE_H

#include <fstream>
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/state.h"
#include "mcrl2/lts/lts_lts.h"

/** \brief The namespace for traces.
* \details The namespace trace contains all data structures and members of the
* trace library.
*/
namespace mcrl2::lts
{


/// \brief This class contains a trace consisting of a sequence of (timed) actions possibly with intermediate states
/// \details A trace is a sequence of actions. Actions can have a time
/// tag as a positive real number. Between two actions, before the first action and after the last
/// action there can be a state. In the current version of the trace library an action, a state and
/// a time tag are arbitrary expressions of sort AtermAppl. It is expected that this will change
/// in the near future to match the data types used in the LPS library.
///
/// An important property of a state is its current position. All operations on a state
/// operate with respect to its current position. A trace can be traversed by increasing
/// and decreasing the current position between 0 up to the length. If a new action is
/// added to a trace, the trace above the current position is lost. For each action
/// a state can only be added once.
///
/// States can be saved in two formats. A human readable ascii format containging only a
/// sequence of untimed actions and a more compact aterm format also containing time and
/// state information.
class trace
{
  public:
    ////////////////////////////////////////////////////////////
    //
    /// \brief Formats in which traces can be saved on disk
    /// \details There are several formats for traces.
    /// The tfMcrl2 format saves a trace as an mCRL2 term in aterm internal format.
    /// This is a compact but unreadable format.
    /// The tfPlain format is an ascii representation of the trace, which is
    /// human readable but only contains the actions and no time or state information.
    /// tfUnknown is only used to read traces, when it is
    /// not known what the format is. In this case it is determined based
    /// on the format of the input file.
    
    enum trace_format
    {
      tfMcrl2,  /**< Format is stored as an aterm */
      tfPlain,  /**< Format is stored in plain text. In this format there are only actions */
      tfLine,   /**< Format is stored in a line of text. In this format there are only actions */
      tfUnknown /**< This value indicates that the format is unknown */
    };

  protected:
    // The number of states is always less than one plus the number of actions.
    // In case all states are there, then it is one more than the number of actions.
    // Otherwise there are less.  So, an invariant is actions.size()+1 >= states.size();
    // The states and actions are supposed to be contiguous,
    // in the sense that if a state, or action, at position n exists, then also the
    // states and actions at positions n'<n exist.

    std::vector < lps::state > m_states;
    std::vector < mcrl2::lps::multi_action > m_actions;
    std::size_t m_pos = 0UL; // Invariant: m_pos <= actions.size().

    mcrl2::data::data_specification m_spec;
    process::action_label_list m_act_decls;
    bool m_data_specification_and_act_decls_are_defined;

  public:
    /// \brief Default constructor for an empty trace.
    /// \details The current position
    /// and length of trace are set to 0.
    trace()
      : m_data_specification_and_act_decls_are_defined(false)
    {
      init();
    }

    /// \brief Constructor for an empty trace.
    /// \details The current position
    /// and length of trace are set to 0.
    /// \param[in] spec The data specification that is used when parsing multi actions.
    /// \param[in] act_decls An action label list with action declarations that is used to parse multi actions.
    trace(const mcrl2::data::data_specification& spec, const mcrl2::process::action_label_list& act_decls)
      : m_spec(spec),
        m_act_decls(act_decls),
        m_data_specification_and_act_decls_are_defined(true)
    {
      init();
    }

    /// \brief Construct the trace on the basis of an input file.
    /// \details A trace is read from the input file. If the format is tfUnknown it
    /// is automatically determined what the format of the trace is.
    /// \param[in] filename The name of the file from which the trace is read.
    /// \param[in] tf The format in which the trace was stored. Default: '''tfUnknown'''.
    /// \exception mcrl2::runtime_error message in case of failure
    trace(const std::string& filename, trace_format tf = tfUnknown)
      : m_data_specification_and_act_decls_are_defined(false)
    {
      init();
      load(filename,tf);
    }

    /// \brief Construct the trace on the basis of an input file.
    /// \details A trace is read from the input file. If the format is tfUnknown it
    /// is automatically determined what the format of the trace is.
    /// \param[in] filename The name of the file from which the trace is read.
    /// \param[in] spec A data specification.
    /// \param[in] act_decls A list of action declarations.
    /// \param[in] tf The format in which the trace was stored. Default: '''tfUnknown'''.
    /// \exception mcrl2::runtime_error message in case of failure
    trace(const std::string& filename,
          const mcrl2::data::data_specification& spec,
          const mcrl2::process::action_label_list& act_decls,
          trace_format tf = tfUnknown)
      : m_spec(spec),
        m_act_decls(act_decls),
        m_data_specification_and_act_decls_are_defined(true)
    {
      init();
      try
      {
        load(filename,tf);
      }
      catch (...)
      {
        throw;
      }
    }

    bool operator <(const trace& t) const
    {
      return ((m_states<t.m_states) ||
              (m_states==t.m_states && m_actions<t.m_actions));
    }

    /// \brief Set the current position back to the beginning of the trace
    /// \details The trace itself remains unaltered.
    void reset_position()
    {
      assert(m_actions.size()+1 >= m_states.size() && m_pos <=m_actions.size());
      m_pos = 0;
    }

    /// \brief Increase the current position by one, except if this brings one beyond the end of the trace.
    /// \details The initial position corresponds to m_pos=0.
    void increase_position()
    {
      if (m_pos < m_actions.size())
      {
        ++m_pos;
      }
    }

    /// \brief Decrease the current position in the trace by one provided the largest position is larger than 0.
    /// \details The initial position corresponds to m_pos=0.
    void decrease_position()
    {
      if (m_pos >0)
      {
        --m_pos;
      }
    }

    /// \brief Set the current position after the m_pos'th action of the trace.
    /// \details The initial position corresponds to m_pos=0. If m_pos is larger than
    /// the length of the trace, no new position is set.
    /// \param[in] m_pos The new position in the trace.
    void set_position(std::size_t pos)
    {
      if (pos <= m_actions.size())
      {
        this->m_pos = pos;
      }
    }

    /// \brief Get the current position in the trace.
    /// \details The current position of the trace is a non negative number
    /// smaller than the length of the trace.
    /// \return The current position of the trace.
    std::size_t get_position() const
    {
      assert(m_actions.size()+1 >= m_states.size() && m_pos <=m_actions.size());
      return m_pos;
    }

    /// \brief Get the number of actions in the current trace.
    /// \return A positive number indicating the number of actions in the trace.
    std::size_t number_of_actions() const
    {
      assert(m_actions.size()+1 >= m_states.size() && m_pos <=m_actions.size());
      return m_actions.size();
    }

    /// \brief Get the number of states in the current trace.
    /// \return A positive number indicating the number of states in the trace.
    std::size_t number_of_states() const
    {
      assert(m_actions.size()+1 >= m_states.size() && m_pos <=m_actions.size());
      return m_states.size();
    }

    /// \brief Remove the current state and all states following it.
    void clear_current_state()
    {
      assert(m_actions.size()+1 >= m_states.size() && m_pos <=m_actions.size());
      m_states.resize(m_pos);
    }

    /// \brief Indicate whether a current state exists.
    /// \return A boolean indicating whether the current state exists.
    bool current_state_exists() const
    {
      assert(m_actions.size()+1 >= m_states.size() && m_pos <=m_actions.size());
      return m_states.size()>m_pos;
    }

    /// \brief Get the state at the current position in the trace.
    /// \details The state at
    /// position 0 is the initial state. If no state is defined at
    /// the next position an exception is thrown.
    /// \return The state at the current position of the trace.
    const lps::state& next_state() const
    {
      assert(m_actions.size()+1 >= m_states.size() && m_pos+1 <=m_actions.size());
      if (m_pos>=m_states.size())
      {
        throw mcrl2::runtime_error("Requesting a state in a trace at a non existing position " + std::to_string(m_pos+1) + ".");
      }
      return m_states[m_pos+1];
    }

    /// \brief Get the state at the current position in the trace.
    /// \details The state at
    /// position 0 is the initial state. If no state is defined at
    /// the current position an exception is thrown.
    /// \return The state at the current position of the trace.
    const lps::state& current_state() const
    {
      assert(m_actions.size()+1 >= m_states.size() && m_pos <=m_actions.size());
      if (m_pos>=m_states.size())
      {
        throw mcrl2::runtime_error("Requesting a state in a trace at a non existing position " + std::to_string(m_pos) + ".");
      }
      return m_states[m_pos];
    }

    /// \brief Indicate whether the current action exists. 
    /// \return A boolean indicating that the current action exists;
    bool current_action_exists() const
    {
      assert(m_actions.size()+1 >= m_states.size() && m_pos <=m_actions.size());
      return m_pos < m_actions.size(); 
    }

    /// \brief Get the outgoing action from the current position in the trace.
    /// \details This routine returns the action at the current position of the
    /// trace. It is not allowed to request an action if no action is available.
    /// \return An action_list representing the action at the current position of the
    /// trace.
    mcrl2::lps::multi_action current_action()
    {
      assert(m_actions.size()+1 >= m_states.size() && m_pos <=m_actions.size());
      assert(m_pos < m_actions.size());
      return m_actions[m_pos];
    }

    /// \brief Get the time of the current state in the trace.
    /// \details This is the time at which
    /// the last action occurred (if any).
    /// \return A data_expression representing the current time, or a default data_expression if the time is not defined.
    mcrl2::data::data_expression current_time()
    {
      assert(m_actions.size()+1 >= m_states.size() && m_pos <=m_actions.size());
      return m_actions[m_pos].time();
    }

    /// \brief Truncates the trace at the current position.
    /// \details This function removes the next action at the current position and all
    /// subsequent actions and states. The state and the time at the current
    /// position remain untouched.
    void truncate()
    {
      m_actions.resize(m_pos);
      if (m_pos+1<m_states.size())  // Only throw states away that exist.
      {
        m_states.resize(m_pos+1);
      }
    }

    /// \brief Add an action to the current trace.
    /// \details Add an action to the current trace at the current position. The current
    /// position is increased and the length of the trace is set to this new current position.
    /// The old actions in the trace at the current at higher positions are removed.
    /// \param [in] action The multi_action to be stored in the trace.

    void add_action(const mcrl2::lps::multi_action& action)
    {
      assert(m_actions.size()+1 >= m_states.size() && m_pos <=m_actions.size());
      truncate(); // Take care that actions and states have the appropriate size.
      m_pos++;
      m_actions.push_back(action);
    }

    /// \brief Set the state at the current position.
    /// \details It is necessary that all states at earlier positions are also set.
    /// If not an mcrl2::runtime_error exception is thrown.
    /// \param [in] s The new state.
    // void set_state(const mcrl2::lps::state& s)
    void set_state(const lps::state& s)
    {
      assert(m_actions.size()+1 >= m_states.size() && m_pos <=m_actions.size());
      if (m_pos>m_states.size())
      {
        throw mcrl2::runtime_error("Setting a state in a trace at a position " + std::to_string(m_pos) +
                                   " where there are no states at earlier positions.");
      }

      if (m_states.size()==m_pos)
      {
        m_states.push_back(s);
      }
      else
      {
        m_states[m_pos] = s;
      }
    }

    /// \brief Replace the trace with the trace in the file.
    /// \details The trace is replaced with the trace in the file.
    /// If the format is tfPlain the trace can only consist of actions.
    /// \param [in] filename The name of the file from which the trace is read.
    /// \param [in] tf The expected format of the trace in the stream (default: tfUnknown).
    /// \exception mcrl2::runtime_error message in case of failure

    void load(const std::string& filename, trace_format tf = tfUnknown)
    {
      using std::ifstream;
      ifstream is(filename.c_str(),ifstream::binary|ifstream::in);

      if (!is.is_open())
      {
        throw runtime_error("Error loading trace (could not open file " + filename +").");
      } 

      try
      {
        if (tf == tfUnknown)
        {
          tf = detectFormat(is);
        }

        switch (tf)
        {
          case tfMcrl2:
            load_mcrl2(filename);
            break;
          case tfPlain:
            load_plain(is);
            break;
          default:
            break;
        }

      }
      catch (mcrl2::runtime_error& err)
      {
        throw mcrl2::runtime_error("Error loading trace: " + std::string(err.what()));
      }
    }

    /// \brief Output the trace into a file with the indicated name.
    /// \details Write the trace to a file with the indicated name.
    /// \param [in] filename The name of the file that is written.
    /// \param [in] tf The format used to represent the trace in the stream. If
    /// the format is tfPlain only actions are written. Default: tfMcrl2.
    /// \exception mcrl2::runtime_error message in case of failure

    void save(const std::string& filename, trace_format tf = tfMcrl2) const
    {
      try
      {
        switch (tf)
        {
          case tfMcrl2:
            save_mcrl2(filename);
            break;
          case tfPlain:
            save_plain(filename);
            break;
          case tfLine:
            save_line(filename);
            break;
          default:
            throw runtime_error("Error saving trace to " + (filename.empty()?std::string(" stdout"):filename) + ".");
        }
      }
      catch (runtime_error& err)
      {
        throw runtime_error("Error saving trace to " + (filename.empty()?std::string(" stdout"):filename) + ".\n" + err.what());
      }
    }

    const std::vector<lps::state>& states() const
    {
      return m_states;
    }

    const std::vector<lps::multi_action>& actions() const
    {
      return m_actions;
    }

  protected:

    void init()
    {
      m_pos = 0;
      truncate(); // Take care that m_pos 0 exists.
    }

    trace_format detectFormat(std::istream& is)
    {
      trace_format fmt = tfPlain;

      char c=is.peek();
      if (is.bad())
      {
        throw runtime_error("Could not read from stream.");
      }

      if (c==0)  // Weak check. A lts in aterm format starts with a 0. This is not possible 
                 // for a trace in textual format. 
      {
        fmt = tfMcrl2;
      }

      return fmt;
    }

    void load_mcrl2(const std::string& filename)
    {
      mcrl2::lts::lts_lts_t lts;
      try
      {
        lts.load(filename);
      } 
      catch (mcrl2::runtime_error& e)
      {
        throw runtime_error(std::string("stream does not contain an mCRL2 trace.\n") + e.what());
      }

      m_spec = lts.data();
      m_act_decls = lts.action_label_declarations();
      m_data_specification_and_act_decls_are_defined = m_spec==data::data_specification() && m_act_decls == process::action_label_list();
      
      reset_position();
      truncate();

      std::vector<bool> has_state_outgoing_transition(lts.num_states(),false);
      std::vector<transition> outgoing_transition(lts.num_states(),transition(0,0,0));

      // The transition system that is read may not be a trace, but a more complex transition system.
      for(const transition& t: lts.get_transitions())
      {
        if (has_state_outgoing_transition.at(t.from()))
        {
          throw runtime_error("The tracefile contains an labelled transition system that is not a trace. State " + 
                                   std::to_string(t.from()) + " has multiple outgoing transitions.");
        }
        has_state_outgoing_transition[t.from()]=true;
        outgoing_transition[t.from()]=t;
      }

      // The transition file that is read is a trace. We read the trace sequentially,
      // but it might be that the transitions are non consecutive in the trace, if the
      // trace file results from an arbitrary .lts. So, we first put all transitions in
      // a vector with transition (i,label,j) put at position i. 
      
      lts_lts_t::states_size_type current_state=lts.initial_state(); 
      if (lts.has_state_info())
      {
        assert(current_state<lts.num_state_labels());
        if (lts.state_label(current_state).size()!=1)
        {
          throw mcrl2::runtime_error("The trace has multiple state labels for some states, and hence is not a trace.");
        }
        set_state(lts.state_label(current_state).front());
      }

      while (has_state_outgoing_transition.at(current_state))
      {
        add_action(lts.action_label(outgoing_transition[current_state].label()));
        current_state=outgoing_transition[current_state].to();
        if (lts.has_state_info())
        {
          assert(current_state<lts.num_state_labels());
          if (lts.state_label(current_state).size()!=1)
          {
            throw mcrl2::runtime_error("The trace has multiple state labels for some states, and hence is not a trace.");
          }
          set_state(lts.state_label(current_state).front());
        }
      }

      reset_position();
    }

    void load_plain(std::istream& is)
    {
      reset_position();
      truncate();
      m_states.clear(); // Throw all states away, also the initial state. 

      std::string action;
      while (!is.eof())
      {
        std::getline(is,action);
        if (is.bad())
        {
          throw mcrl2::runtime_error("Error while reading from stream.");
        }
 
        action = action.substr(0,action.find_last_not_of(" \r")+1);
        if (!action.empty())
        {
          if (m_data_specification_and_act_decls_are_defined)
          {
            add_action(mcrl2::lps::parse_multi_action(action,m_act_decls,m_spec));
          }
          else
          {
            add_action(mcrl2::lps::multi_action(mcrl2::process::action(
                      mcrl2::process::action_label(mcrl2::core::identifier_string(action),mcrl2::data::sort_expression_list()),
                                           mcrl2::data::data_expression_list())));
          }
        }
      }
      is.clear();

      reset_position();
    }

    void save_mcrl2(const std::string& filename) const
    {
      // The trace is saved as an .lts in lts format. 
      lts_lts_t lts;
      if (m_data_specification_and_act_decls_are_defined) 
      {
        lts.set_data(m_spec);
        lts.set_action_label_declarations(m_act_decls);
      }
      assert(m_actions.size()+1 >= m_states.size());

      lts.add_state(m_states.size()>0?
                        lts_lts_t::state_label_t(m_states[0]):
                        lts_lts_t::state_label_t());
      
      std::map<mcrl2::lps::multi_action,std::size_t> 
                obtain_unique_numbers_for_action_labels({std::pair(action_label_lts::tau_action(),0)});
      for(std::size_t i=0; i<m_actions.size(); ++i)
      {
        auto [element, inserted] =
                        obtain_unique_numbers_for_action_labels.insert(
                               std::pair(m_actions[i],
                                         obtain_unique_numbers_for_action_labels.size()));
        if (inserted)
        {
          lts.add_action(action_label_lts(m_actions[i]));
        }
        lts.add_transition(transition(i,element->second,i+1));
        
        lts.add_state(m_states.size()>i+1?
                          lts_lts_t::state_label_t(m_states[i+1]):
                          lts_lts_t::state_label_t());
      }
      lts.set_initial_state(0);

      lts.save(filename);
    }

    void save_text_to_stream(std::ostream& os, std::string separator) const
    {
      std::string sep;
      for (const lps::multi_action& action : m_actions)
      {
        os << sep << pp(action);
        sep = separator;
        if (os.bad())
        {
          throw runtime_error("Could not write to stream.");  
        }
      }
      os << std::endl;
    }

    void save_text(const std::string& filename, std::string separator) const
    {
      if (filename=="")
      {
        save_text_to_stream(std::cout, separator);
      }
      else 
      {
        using std::ofstream;
        ofstream os;
        os.open(filename.c_str(),ofstream::binary|ofstream::out|ofstream::trunc);
        if (!os.is_open())
        {
          os.close();
          throw runtime_error("Could not open file.");
        }
        save_text_to_stream(os, separator);
        os.close();
      }
    }

    void save_line(const std::string& filename) const
    {
      save_text(filename, ";");
    }

    void save_plain(const std::string& filename) const
    {
      save_text(filename, "\n");
    }

};

/// \brief Output operator for a trace.
/// \details Outputs the trace in plain text format with actions separated by dots.
/// \param[in] os The output stream to write to.
/// \param[in] t The trace to output.
/// \return The output stream.
inline std::ostream& operator<<(std::ostream& os, const trace& t)
{
  std::string sep;
  for (std::size_t i = 0; i < t.number_of_actions(); ++i)
  {
    os << sep << pp(t.actions()[i]);
    sep = ".";
  }
  return os;
}

}

#endif  // MCRL2_LTS_TRACE_H
