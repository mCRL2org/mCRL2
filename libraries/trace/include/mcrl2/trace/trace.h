// Author(s): Muck van Weerdenburg. Documentation Jan Friso Groote.
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/trace/trace.h
/// \brief This library allows to flexibly manipulate traces.
/// \details This library allows to build, traverse and store traces.
/// Traces are sequences of state-action-time triples.
/// The state is a vector of data values, the action is the outgoing
/// action in this state, and the time is an absolute
/// real number indicating the current time or NIL if the trace
/// is untimed.
/// \author Muck van Weerdenburg

#ifndef _TRACE_H__
#define _TRACE_H__

#include <iostream>
#include <fstream>
#include <string>
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/construction_utility.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lps/action_parse.h"
#include "mcrl2/lps/state.h"
#include "mcrl2/data/data_specification.h"

namespace mcrl2
{
/** \brief The namespace for traces.
* \details The namespace trace contains all data structures and members of the
* trace library.
*/
namespace trace
{


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

enum TraceFormat
{
  tfMcrl2,  /**< Format is stored as an aterm */
  tfPlain,  /**< Format is stored in plain text. In this format there are only actions */
  tfUnknown /**< This value indicates that the format is unknown */
};

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
class Trace
{
  private:
    // The number of states is always less then one plus the number of actions. 
    // In case all states are there, then it is one more than the number of actions.
    // Otherwise there are less.  So, an invariant is actions.size()+1 >= states.size();
    // The states and actions are supposed to be contiguous,
    // in the sense that if a state, or action, at position n exists, then also the
    // states and actions at positions n'<n exist.
    
    std::vector < mcrl2::lps::state > states;
    std::vector < mcrl2::lps::multi_action > actions;
    size_t pos; // Invariant: pos <= actions.size().

    mcrl2::data::data_specification m_spec;
    lps::action_label_list m_act_decls;
    bool m_data_specification_and_act_decls_are_defined;

    atermpp::function_symbol const& trace_pair() const
    {
      static atermpp::function_symbol trace_pair = atermpp::function_symbol("pair",2);
      return trace_pair;
    }

#define TRACE_MCRL2_MARKER "mCRL2Trace"
#define TRACE_MCRL2_MARKER_SIZE 10
#define TRACE_MCRL2_VERSION "\x01\x00"
#define TRACE_MCRL2_VERSION_SIZE 2

#define INIT_BUF_SIZE 64

  public:
    /// \brief Default constructor for an empty trace.
    /// \details The current position
    /// and length of trace are set to 0. 
    Trace()
      : m_data_specification_and_act_decls_are_defined(false)
    {
      init();
    }

    /// \brief Constructor for an empty trace.
    /// \details The current position
    /// and length of trace are set to 0. 
    /// \param[in] spec The data specification that is used when parsing multi actions.
    /// \param[in] act_decls An action label list with action declarations that is used to parse multi actions.
    Trace(const mcrl2::data::data_specification &spec, const mcrl2::lps::action_label_list &act_decls)
      : m_spec(spec),
        m_act_decls(act_decls),
        m_data_specification_and_act_decls_are_defined(true)
    {
      init();
    }


    /// \brief Construct the trace in the basis of an input stream.
    /// \details A trace is read from the input stream. If the format is tfUnknown it
    /// is automatically determined what the format of the trace is.
    /// \param[in] is The input stream from which the trace is read.
    /// \param[in] tf The format in which the trace was stored. Default: '''tfUnknown'''.
    /// \exception mcrl2::runtime_error message in case of failure
    Trace(std::istream& is, TraceFormat tf = tfUnknown)
      : m_data_specification_and_act_decls_are_defined(false)
    {
      init();
      try
      {
        load(is,tf);
      }
      catch (...)
      {
        throw;
      }
    }

    /// \brief Construct the trace in the basis of an input stream.
    /// \details A trace is read from the input stream. If the format is tfUnknown it
    /// is automatically determined what the format of the trace is.
    /// \param[in] is The input stream from which the trace is read.
    /// \param[in] tf The format in which the trace was stored. Default: '''tfUnknown'''.
    /// \param[in] spec The data specification that is used when parsing multi actions.
    /// \param[in] act_decls An action label list with action declarations that is used to parse multi actions.
    /// \exception mcrl2::runtime_error message in case of failure
    Trace(std::istream& is, 
          const mcrl2::data::data_specification &spec, 
          const mcrl2::lps::action_label_list &act_decls, 
          TraceFormat tf = tfUnknown)
      : m_spec(spec),
        m_act_decls(act_decls),
        m_data_specification_and_act_decls_are_defined(true)
    {
      init();
      try
      {
        load(is,tf);
      }
      catch (...)
      {
        throw;
      }
    }

    /// \brief Construct the trace on the basis of an input file.
    /// \details A trace is read from the input file. If the format is tfUnknown it
    /// is automatically determined what the format of the trace is.
    /// \param[in] filename The name of the file from which the trace is read.
    /// \param[in] tf The format in which the trace was stored. Default: '''tfUnknown'''.
    /// \exception mcrl2::runtime_error message in case of failure
    Trace(std::string const& filename, TraceFormat tf = tfUnknown)
      : m_data_specification_and_act_decls_are_defined(false)
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

    /// \brief Construct the trace on the basis of an input file.
    /// \details A trace is read from the input file. If the format is tfUnknown it
    /// is automatically determined what the format of the trace is.
    /// \param[in] filename The name of the file from which the trace is read.
    /// \param[in] tf The format in which the trace was stored. Default: '''tfUnknown'''.
    /// \exception mcrl2::runtime_error message in case of failure
    Trace(std::string const& filename, 
          const mcrl2::data::data_specification &spec, 
          const mcrl2::lps::action_label_list &act_decls,
          TraceFormat tf = tfUnknown)
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

    bool operator <(const Trace& t) const
    {
      return ((states<t.states) ||
              (states==t.states && actions<t.actions));  
    }

    /// \brief Set the current position back to the beginning of the trace
    /// \details The trace itself remains unaltered.
    void resetPosition()
    {
      assert(actions.size()+1 >= states.size() && pos <=actions.size());
      pos = 0;
    }

    /// \brief Increase the current position by one, except if this brings one beyond the end of the trace.
    /// \details The initial position corresponds to pos=0. 
    void increasePosition()
    {
      if (pos < actions.size())
      {
        ++pos;
      }
    }
    
    /// \brief Decrease the current position in the trace by one provided the largest position is larger than 0.
    /// \details The initial position corresponds to pos=0. 
    void decreasePosition()
    {
      if (pos >0)
      {
        --pos;
      }
    }

    /// \brief Set the current position after the pos'th action of the trace.
    /// \details The initial position corresponds to pos=0. If pos is larger than
    /// the length of the trace, no new position is set.
    /// \param[in] pos The new position in the trace.
    void setPosition(size_t pos)
    {
      if (pos <= actions.size())
      {
        this->pos = pos;
      }
    }

    /// \brief Get the current position in the trace.
    /// \details The current position of the trace is a non negative number
    /// smaller than the length of the trace.
    /// \return The current position of the trace.
    size_t getPosition() const
    {
      assert(actions.size()+1 >= states.size() && pos <=actions.size());
      return pos;
    }

    /// \brief Get the number of actions in the current trace.
    /// \return A positive number indicating the number of actions in the trace.
    size_t number_of_actions() const
    {
      assert(actions.size()+1 >= states.size() && pos <=actions.size());
      return actions.size();
    }

    /// \brief Get the number of states in the current trace.
    /// \return A positive number indicating the number of states in the trace.
    size_t number_of_states() const
    {
      assert(actions.size()+1 >= states.size() && pos <=actions.size());
      return states.size();
    }

    /// \brief Remove the current state and all states following it.
    void clear_current_state()
    {
      assert(actions.size()+1 >= states.size() && pos <=actions.size());
      states.resize(pos);
    }

    /// \brief Indicate whether a current state exists.
    /// \return A boolean indicating whether the current state exists.
    bool current_state_exists() const
    {
      assert(actions.size()+1 >= states.size() && pos <=actions.size());
      return states.size()>pos;
    }

    /// \brief Get the state at the current position in the trace.
    /// \details The state at
    /// position 0 is the initial state. If no state is defined at
    /// the next position an exception is thrown.
    /// \return The state at the current position of the trace.
    const mcrl2::lps::state &nextState() const
    {
      assert(actions.size()+1 >= states.size() && pos+1 <=actions.size());
      if (pos>=states.size())
      {
        std::stringstream ss;
        ss << "Requesting a non existing state in a trace at position " << pos+1;
        throw mcrl2::runtime_error(ss.str());
      }
      return states[pos+1];
    }

    /// \brief Get the state at the current position in the trace.
    /// \details The state at
    /// position 0 is the initial state. If no state is defined at
    /// the current position an exception is thrown.
    /// \return The state at the current position of the trace.
    const mcrl2::lps::state &currentState() const
    {
      assert(actions.size()+1 >= states.size() && pos <=actions.size());
      if (pos>=states.size())
      {
        std::stringstream ss;
        ss << "Requesting a non existing state in a trace at position " << pos;
        throw mcrl2::runtime_error(ss.str());
      }
      return states[pos];
    }

    /// \brief Get the outgoing action from the current position in the trace.
    /// \details This routine returns the action at the current position of the
    /// trace. It is not allowed to request an action if no action is available.
    /// \return An action_list representing the action at the current position of the
    /// trace. 
    mcrl2::lps::multi_action currentAction()
    {
      assert(actions.size()+1 >= states.size() && pos <=actions.size());
      assert(pos < actions.size());
      return actions[pos];
    }
    
    /// \brief Get the time of the current state in the trace.
    /// \details This is the time at which
    /// the last action occurred (if any).
    /// \return A data_expression representing the current time, or a default data_expression if the time is not defined.
    mcrl2::data::data_expression currentTime()
    { 
      assert(actions.size()+1 >= states.size() && pos <=actions.size());
      return actions[pos].time(); 
    }

    /// \brief Truncates the trace at the current position.
    /// \details This function removes the next action at the current position and all
    /// subsequent actions and states. The state and the time at the current
    /// position remain untouched.
    void truncate()
    {
      actions.resize(pos);
      if (pos+1<states.size())  // Only throw states away that exist.
      { 
        states.resize(pos+1);
      }
    }

    /// \brief Add an action to the current trace.
    /// \details Add an action to the current trace at the current position. The current
    /// position is increased and the length of the trace is set to this new current position.
    /// The old actions in the trace at the current at higher positions are removed.
    /// \param [in] action The multi_action to be stored in the trace.

    void addAction(const mcrl2::lps::multi_action action)
    {
      assert(actions.size()+1 >= states.size() && pos <=actions.size());
      truncate(); // Take care that actions and states have the appropriate size.
      pos++;
      actions.push_back(action);
    }

    /// \brief Set the state at the current position.
    /// \details It is necessary that all states at earlier positions are also set.
    /// If not an mcrl2::runtime_error exception is thrown.
    /// \param [in] state The new state.

    void setState(const mcrl2::lps::state &s)
    {
      assert(actions.size()+1 >= states.size() && pos <=actions.size());
      if (pos>states.size())
      {
        std::stringstream ss;
        ss << "Setting a state in a trace at a position " << pos << " where there are no states at earlier positions";
        throw mcrl2::runtime_error(ss.str());
      }
      
      if (states.size()==pos)
      {
        states.push_back(s);
      }
      else 
      {
        states[pos] = s;
      }
    }

    /// \brief Replace the trace with the content of the stream.
    /// \details The trace is replaced with the trace in the stream.
    /// If a problem occurs while reading the stream, a core dump occurs.
    /// If the format is tfPlain the trace can only consist of actions.
    /// \param [in] is The stream from which the trace is read.
    /// \param [in] tf The expected format of the trace in the stream (default: tfUnknown).
    /// \exception mcrl2::runtime_error message in case of failure

    void load(std::istream& is, TraceFormat tf = tfUnknown)
    {
      try
      {

        if (tf == tfUnknown)
        {
          tf = detectFormat(is);
        }

        switch (tf)
        {
          case tfMcrl2:
            loadMcrl2(is);
            break;
          case tfPlain:
            loadPlain(is);
            break;
          default:
            break;
        }

      }
      catch (mcrl2::runtime_error err)
      {
        std::string s;
        s = "error loading trace: ";
        s += err.what();
        s += ".";
        throw mcrl2::runtime_error(s);
      }
    }

    /// \brief Replace the trace with the trace in the file.
    /// \details The trace is replaced with the trace in the file.
    /// If the format is tfPlain the trace can only consist of actions.
    /// \param [in] filename The name of the file from which the trace is read.
    /// \param [in] tf The expected format of the trace in the stream (default: tfUnknown).
    /// \exception mcrl2::runtime_error message in case of failure

    void load(std::string const& filename, TraceFormat tf = tfUnknown)
    {
      using std::ifstream;
      ifstream is(filename.c_str(),ifstream::binary|ifstream::in);

      if (!is.is_open())
      {
        throw runtime_error("error loading trace (could not open file)");
      }

      try
      {
        load(is, tf);
      }
      catch (...)
      {
        is.close();
        throw;
      }

      is.close();
    }

    /// \brief Output the trace into the indicated stream.
    /// \details Output the trace into the indicated stream.
    /// If a problem occurs, this routine dumps core.
    /// \param [in] os The stream to which the trace is written.
    /// \param [in] tf The format used to represent the trace in the stream. If
    /// the format is tfPlain only actions are written. Default: tfMcrl2.
    /// \exception mcrl2::runtime_error message in case of failure

    void save(std::ostream& os, TraceFormat tf = tfMcrl2)
    {
      try
      {
        switch (tf)
        {
          case tfMcrl2:
            saveMcrl2(os);
            break;
          case tfPlain:
            savePlain(os);
            break;
          default:
            break;
        }
      }
      catch (runtime_error err)
      {
        std::string s;
        s = "error saving trace (";
        s += err.what();
        s += ")";
        throw runtime_error(s);
      }
    }

    /// \brief Output the trace into a file with the indicated name.
    /// \details Write the trace to a file with the indicated name.
    /// \param [in] filename The name of the file that is written.
    /// \param [in] tf The format used to represent the trace in the stream. If
    /// the format is tfPlain only actions are written. Default: tfMcrl2.
    /// \exception mcrl2::runtime_error message in case of failure

    void save(std::string const& filename, TraceFormat tf = tfMcrl2)
    {
      using std::ofstream;
      ofstream os(filename.c_str(),ofstream::binary|ofstream::out|ofstream::trunc);
      if (!os.is_open())
      {
        throw runtime_error("error saving trace (could not open file)");
      }

      try
      {
        save(os, tf);
      }
      catch (...)
      {
        os.close();
        throw;
      }

      os.close();
    }

  private:

    bool isTimedMAct(const atermpp::aterm_appl &t)
    {
      return t.type_is_appl() && t.function()==trace_pair();
    }

    atermpp::aterm_appl makeTimedMAct(const mcrl2::lps::multi_action &ma)
    {
      return atermpp::aterm_appl(trace_pair(),ma.actions(), ma.time());
    }

    void init()
    {
      pos = 0;
      truncate(); // Take care that pos 0 exists.
    }

    TraceFormat detectFormat(std::istream& is)
    {
      char buf[TRACE_MCRL2_MARKER_SIZE];
      TraceFormat fmt = tfPlain;

      is.read(buf,TRACE_MCRL2_MARKER_SIZE);
      if (is.bad())
      {
        throw runtime_error("could not read from stream");
      }
      is.clear();

      if ((is.gcount() == TRACE_MCRL2_MARKER_SIZE) && !strncmp(buf,TRACE_MCRL2_MARKER,TRACE_MCRL2_MARKER_SIZE))
      {
        fmt = tfMcrl2;
      }

      is.seekg(-is.gcount(),std::ios::cur);
      if (is.fail())
      {
        throw runtime_error("could set position in stream");
      }

      return fmt;
    }

    atermpp::aterm readATerm(std::istream& is)
    {
      atermpp::aterm t = atermpp::read_term_from_binary_stream(is);
      if (!t.defined())
      {
        throw runtime_error("failed to read aterm from stream");
      }

      return t;
    }

    void loadMcrl2(std::istream& is)
    {
      using namespace atermpp;
      char buf[TRACE_MCRL2_MARKER_SIZE+TRACE_MCRL2_VERSION_SIZE];
      is.read(buf,TRACE_MCRL2_MARKER_SIZE+TRACE_MCRL2_VERSION_SIZE);
      if (is.bad() || strncmp(buf,TRACE_MCRL2_MARKER,TRACE_MCRL2_MARKER_SIZE))
      {
        throw runtime_error("stream does not contain an mCRL2 trace");
      }
      is.clear();

      resetPosition();
      truncate();
      aterm_list trace(readATerm(is));
      assert(trace.type_is_list());
      for (; !trace.empty(); trace=trace.tail())
      {
        using namespace mcrl2::lps;
        const aterm& e = trace.front();

        if (e.type_is_appl() && lps::is_multi_action(aterm_cast<aterm_appl>(e)))   // To be compatible with old untimed version
        {
          addAction(multi_action(action_list(aterm_cast<aterm_appl>(e))));
        }
        else if (e.type_is_appl() && isTimedMAct(aterm_cast<aterm_appl>(e)))
        {
          if (aterm_cast<aterm_appl>(e)[1]==data::data_expression())  // There is no time tag.
          {
            addAction(multi_action(action_list(aterm_cast<aterm_appl>(e)[0])));
          }
          else
          {
            addAction(multi_action(action_list(aterm_cast<aterm_appl>(e)[0]),
                               mcrl2::data::data_expression(aterm_cast<aterm_appl>(e)[1])));
          }
        }
        else
        {
          // So, e is a list of data expressions.
          aterm_list l(e);
          mcrl2::lps::state s;
          for( ; !l.empty() ; l=l.tail())
          {
            s.push_back(mcrl2::data::data_expression(l.front()));
          }
          setState(s);
        }
      }

      resetPosition();
    }

    void loadPlain(std::istream& is)
    {
#define MAX_LINE_SIZE 1024
      char buf[MAX_LINE_SIZE];
      resetPosition();
      truncate();

      while (!is.eof())
      {
        is.getline(buf,MAX_LINE_SIZE);
        if (is.bad())
        {
          throw mcrl2::runtime_error("error while reading from stream");
        }
        if ((strlen(buf) > 0) && (buf[strlen(buf)-1] == '\r'))
        {
          // remove CR
          buf[strlen(buf)-1] = '\0';
        }

        if (is.gcount() > 0)
        {
          if (m_data_specification_and_act_decls_are_defined)
          {
            addAction(mcrl2::lps::parse_multi_action(buf,m_act_decls,m_spec));
          }
          else  
          {
            addAction(mcrl2::lps::multi_action(mcrl2::lps::action(
                      mcrl2::lps::action_label(mcrl2::core::identifier_string(buf),mcrl2::data::sort_expression_list()),
                                           mcrl2::data::data_expression_list())));
          }
        }
      }
      is.clear();

      resetPosition();
    }

    void saveMcrl2(std::ostream& os)
    {
      assert(actions.size()+1 >= states.size());
      atermpp::aterm_list trace;

      size_t i=actions.size()+1;
      while (i > 0)
      {
        i--;
        if (i<actions.size())
        {
          assert(actions.size()>i);
          trace.push_front(atermpp::aterm(makeTimedMAct(actions[i])));
        }
        if (states.size()>i)
        {
          using namespace mcrl2::lps;
          // Translate the vector into a list of aterms representing data expressions.
          atermpp::aterm_list l;
          const state & s=states[i];
          for(mcrl2::lps::state::const_reverse_iterator j=s.rbegin(); j!=s.rend(); ++j)
          {
            l.push_front(atermpp::aterm(*j));
          }
          trace.push_front(atermpp::aterm(l));
        }
      }

      // write marker
      os << TRACE_MCRL2_MARKER;
      os.write(TRACE_MCRL2_VERSION,TRACE_MCRL2_VERSION_SIZE);
      if (os.bad())
      {
        throw runtime_error("could not write to stream");
      }

      // write trace

      atermpp::write_term_to_binary_stream(trace,os);  
      if (os.bad())
      {
        throw runtime_error("could not write to stream");
      }
    }

    void savePlain(std::ostream& os)
    {
      for (size_t i=0; i<actions.size(); i++)
      {
        os << pp(actions[i]);
        os << std::endl;
        if (os.bad())
        {
          throw runtime_error("could not write to stream");
        }
      }
    }
};

}
}
#endif
