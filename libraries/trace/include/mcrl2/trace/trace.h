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
/// real number indicating the current time or NULL if the trace
/// is untimed.
/// \author Muck van Weerdenburg

#ifndef _TRACE_H__
#define _TRACE_H__

#include <aterm2.h>
#include <iostream>
#include <string>
#include "mcrl2/atermpp/vector.h"

namespace mcrl2 {
  /** \brief The namespace for traces.
 * \details The namespace trace contains all data structures and members of the
 * trace library.
 */
  namespace trace {


    ////////////////////////////////////////////////////////////
    //
    /// \brief Formats in which traces can be saved on disk
    /// \details There are several formats for traces.
    /// The tfMcrl2 format saves a trace as an mCRL2 term in ATerm internal format.
    /// This is a compact but unreadable format.
    /// The tfPlain format is an ascii representation of the trace, which is
    /// human readable but only contains the actions and no time or state information.
    /// tfUnknown is only used to read traces, when it is
    /// not known what the format is. In this case it is determined based
    /// on the format of the input file.

    enum TraceFormat
    { tfMcrl2,  /**< Format is stored as an ATerm */
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
    /// sequence of untimed actions and a more compact ATerm format also containing time and
    /// state information.

    class Trace
    {
    public:
      /// \brief Default constructor for an empty trace.
      /// \details The current position
      /// and length of trace are set to 0. The initial state and time are set to NULL.

      Trace();

      /// \brief Construct the trace in the basis of an input stream.
      /// \details A trace is read from the input stream. If the format is tfUnknown it
      /// is automatically determined what the format of the trace is.
      /// \param[in] is The input stream from which the trace is read.
      /// \param[in] tf The format in which the trace was stored. Default: '''tfUnknown'''.
      /// \exception mcrl2::runtime_error message in case of failure

      Trace(std::istream &is, TraceFormat tf = tfUnknown);

      /// \brief Construct the trace on the basis of an input file.
      /// \details A trace is read from the input file. If the format is tfUnknown it
      /// is automatically determined what the format of the trace is.
      /// \param[in] filename The name of the file from which the trace is read.
      /// \param[in] tf The format in which the trace was stored. Default: '''tfUnknown'''.
      /// \exception mcrl2::runtime_error message in case of failure

      Trace(std::string const& filename, TraceFormat tf = tfUnknown);

      /// \brief Destructor for the trace.
      /// \details This destructor frees all the associated memory of the trace.

      ~Trace();

      /// \brief Set the current position back to the beginning of the trace
      /// \details The trace itself remains unaltered.

      void resetPosition();

      /// \brief Set the current position after the pos'th action of the trace.
      /// \details The initial position corresponds to pos=0. If pos is larger than
      /// the length of the trace, no new position is set.
      /// \param[in] pos The new position in the trace.

      void setPosition(unsigned int pos);

      /// \brief Get the current position in the trace.
      /// \details The current position of the trace is a non negative number
      /// smaller than the length of the trace.
      /// \return The current position of the trace.

      unsigned int getPosition();

      /// \brief Get the current length of the trace.
      /// \return A positive number indicating the number of actions in the trace.

      unsigned int getLength();

      /// \brief Get the state at the current position in the trace.
      /// \details The state at
      /// position 0 is the initial state. If no state is defined at
      /// the current position NULL is returned.
      /// \return The state at the current position of the trace.

      ATermAppl currentState();

      /// \brief Get the outgoing action from the current position in the trace and
      ///  move to the next position.
      /// \details This routine returns the action at the current position of the
      /// trace and moves to the next position in the trace. When the current position
      /// is at the end of the trace, nothing happens and NULL is returned.
      /// \return An ATermAppl representing the action at the current position of the
      /// trace. This is NULL when at the end of the trace.

      ATermAppl nextAction();

      /// \brief Get the time of the current state in the trace.
      /// \details This is the time at which
      /// the last action occurred (if any).
      /// \return An ATermAppl representing the current time, or NULL if the time is not defined.

      ATermAppl currentTime();

      /// \brief Truncates the trace at the current position.
      /// \details This function removes the next action at the current position and all
      /// subsequent actions, times and states. The state and the time at the current
      /// position remain untouched.

      void truncate();

      /// \brief Add an action to the current trace.
      /// \details Add an action to the current trace at the current position. The current
      /// position is increased and the length of the trace is set to this new current position.
      /// The old actions in the trace at the current at higher positions are removed.
      /// \param [in] action The action to be stored in the trace.
      /// \param [in] time The time to be associated with the current action and state.
      /// \return

      void addAction(ATermAppl action, ATermAppl time = NULL);

      /// \brief Set the state at the current position.
      /// \details If an action is set, the state
      /// that is reached is set to NULL. If this state is NULL it can be set once.
      /// \param [in] state The new state.
      /// \return The return value indicates whether the state is set.

      bool setState(ATermAppl state);

      /// \brief Determine whether a state can still be set.
      /// \details Initially a state is set to NULL. It can be set only once. This function
      /// returns whether a state can still be set to a new value.
      /// \retval true The state can be set to a new value.
      /// \retval false The state can not be set to a new value.

      bool canSetState();

      /// \brief Replace the trace with the content of the stream.
      /// \details The trace is replaced with the trace in the stream.
      /// If a problem occurs while reading the stream, a core dump occurs.
      /// If the format is tfPlain the trace can only consist of actions.
      /// \param [in] is The stream from which the trace is read.
      /// \param [in] tf The expected format of the trace in the stream (default: tfUnknown).
      /// \exception mcrl2::runtime_error message in case of failure

      void load(std::istream &is, TraceFormat tf = tfUnknown);

      /// \brief Replace the trace with the trace in the file.
      /// \details The trace is replaced with the trace in the file.
      /// If the format is tfPlain the trace can only consist of actions.
      /// \param [in] filename The name of the file from which the trace is read.
      /// \param [in] tf The expected format of the trace in the stream (default: tfUnknown).
      /// \exception mcrl2::runtime_error message in case of failure

      void load(std::string const& filename, TraceFormat tf = tfUnknown);

      /// \brief Output the trace into the indicated stream.
      /// \details Output the trace into the indicated stream.
      /// If a problem occurs, this routine dumps core.
      /// \param [in] os The stream to which the trace is written.
      /// \param [in] tf The format used to represent the trace in the stream. If
      /// the format is tfPlain only actions are written. Default: tfMcrl2.
      /// \exception mcrl2::runtime_error message in case of failure

      void save(std::ostream &os, TraceFormat tf = tfMcrl2);

      /// \brief Output the trace into a file with the indicated name.
      /// \details Write the trace to a file with the indicated name.
      /// \param [in] filename The name of the file that is written.
      /// \param [in] tf The format used to represent the trace in the stream. If
      /// the format is tfPlain only actions are written. Default: tfMcrl2.
      /// \exception mcrl2::runtime_error message in case of failure

      void save(std::string const& filename, TraceFormat tf = tfMcrl2);

    private:
      // The number of states is always one less than the number of actions, and times.
      // So, an invariant is actions.size()+1 == states.size() == times.size();
      // The number of actions present is actions.size().
      atermpp::vector < ATermAppl> states;
      atermpp::vector < ATermAppl> actions;
      atermpp::vector < ATermAppl> times;
      // unsigned int buf_size;
      // unsigned int len;
      unsigned int pos; // pos <= actions.size(). 

      void init();
      void cleanup();

      TraceFormat detectFormat(std::istream &is);
      void loadMcrl2(std::istream &is);
      void loadPlain(std::istream &is);
      void saveMcrl2(std::ostream &os);
      void savePlain(std::ostream &os);
    };

  }
}
#endif
