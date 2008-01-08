// Author(s): Muck van Weerdenburg. Documentation Jan Friso Groote.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/trace.h
/// \brief This library allows to flexibly manipulate traces.
/// This library allows to build, traverse and store traces.
/// Traces are sequences of state-action-time triples.
/// The state is a vector of data values, the action is the outgoing
/// action in this state, and the time is an absolute
/// real number indicating the current time or NULL if the trace 
/// is untimed.

#include <aterm2.h>
#include <iostream>
#include <string>

////////////////////////////////////////////////////////////
//
/// \brief Formats in which traces can be saved on disk
/// There are several formats for traces.
/// The tfMcrl2 format saves a trace as an mcrl2 term in ATerm internal.
/// This is a compact but unreadable format.
/// The tfPlain format is an ascii representation of the trace, which is
/// human readable but only contains the actions and not time or state information. 
/// tfUnknown is only used to read traces, when it is
/// not known what the format is. In this case it is determined based
/// on the format of the input file.
enum TraceFormat { tfMcrl2, tfPlain, tfUnknown };

/// \brief This class contains a trace consisting of a sequence of (timed) actions possibly with intermediate states
/// This class contains a trace. A trace is a sequence of actions. Actions can have a time
/// tag as a positive real number. Between two actions, before the first action and after the last
/// action there can be a state. In the current version of the trace library an action, a state and
/// a time tag are arbitrary expressions of sort AtermAppl. It is expected that this will change
/// in the near future to match the data types used in the lps library.
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
/// \details This is the default constructor for an empty trace. The current position
/// and length of trace are set to 0. The initial state, its outgoing action
/// and time are set to NULL. Initially space is reserved for 64 actions,
/// states and time tags which is increased by need.
		Trace();
/// \brief Construct the trace in the basis of an input stream.
/// \details A trace is read from the input stream. If the format is tfUnknown it
/// is automatically determined what the format of the trace is. At
/// termination the current position of the trace is at the end of the trace,
/// after the last action.
/// \param[in] is The input stream from which the trace is read.
/// \param[in] tf The format in which the trace was stored. Default: '''tfUnknown'''.
		Trace(std::istream &is, TraceFormat tf = tfUnknown);
/// \brief Construct the trace on the basis of an input file.
/// \details A trace is read from the input file. If the format is tfUnknown it
/// is automatically determined what the format of the trace is. At
/// termination the current position of the trace is at the end of the trace,
/// after the last action.
/// \param[in] filename The name of the file from which the trace is read.
/// \param[in] tf The format in which the trace was stored. Default: '''tfUnknown'''.
		Trace(std::string const& filename, TraceFormat tf = tfUnknown);
/// \brief Destructor for the trace.
/// \details Destructor for the trace. It frees all the associated memory of the trace.
		~Trace();

/// \brief Set the current position back to the beginning of the trace
/// \details Set the current position back to the beginning of the trace. The 
/// trace itself remains unaltered. 
		void resetPosition();

/// \brief Set the current position after the pos'th action of the trace
/// \details Set the current position after the pos'th action of the trace. The
/// initial position corresponds to pos=0. If pos is larger or equal to
/// the length of the trace, no new position is set.
/// \param[in] pos The new position in the trace.
		void setPosition(unsigned int pos);

/// \brief Get the current position in the trace
/// \details Get the current position of the trace as a non negative number
/// smaller than the length of the trace.
/// \return The current position of the trace.
		unsigned int getPosition();

/// \brief Get the current length of the trace
/// \details Get the length of the trace.
/// \return A positive number indicating the number of actions in the trace.
		unsigned int getLength();

/// \brief Get the state at the current position in the trace
/// \details Get the state at the current position. The state at
/// position 0 is the initial state. If no state is defined at
/// the current position NULL is returned.
/// \return The state at the current position of the trace.
		ATermAppl currentState();
/// \brief Get the outgoing action from the current position in the trace
/// \details This routine returns the action at the current position of the
/// trace. This actions is always defined.
/// \return An ATermAppl representing the action at the current position of the
/// trace.
		ATermAppl nextAction();
/// \brief Get the time of the current action from the current state in the trace
/// \details Get the time of the current action from the current state in the trace. If 
/// the current action is untimed, NULL is returned.
/// \return An ATermAppl representing the current time, or NULL if the time is not defined.
		ATermAppl currentTime();

/// \brief Truncates the trace from the current position.
/// \details This function removes the action at the current position and all
/// subsequent actions, times and states. The state and the time at the current
/// position remain untouched. The length is set to the current position.
		void truncate();

/// \brief
/// \details
/// \param [in]
/// \return
		void addAction(ATermAppl action, ATermAppl time = NULL);

/// \brief
/// \details
/// \param [in]
/// \return
		bool setState(ATermAppl state);

/// \brief
/// \details
/// \param [in]
/// \return
		bool canSetState();


/// \brief Not documented. Use nextAction.
/// \details
/// \param [in]
/// \return
		ATermAppl getAction();

/// \brief Not documented.  Use currentState.
/// \details
/// \param [in]
/// \return
		ATermAppl getState();

/// \brief Not documented. Use currentTime.
/// \details
/// \param [in]
/// \return
		ATermAppl getTime();


/// \brief Replace the trace with the content of the stream.
/// \details The trace is replaced with the trace in the stream.
/// If a problem occurs while reading the stream, a core dump occurs.
/// If the format is tfPlain the trace can only consist of actions.
/// \param [in] is The stream from which the trace is read.
/// \param [in] tf The expected format of the trace in the stream (default: tfUnknown).
		void load(std::istream &is, TraceFormat tf = tfUnknown);

/// \brief Replace the trace with the trace in the file.
/// \details The trace is replaced with the trace in the file.
/// If the format is tfPlain the trace can only consist of actions.
/// \param [in] filename The name of the file from which the trace is read.
/// \param [in] tf The expected format of the trace in the stream (default: tfUnknown).
/// \retval true The string was successfully read from the file.
/// \retval false A problem occurred when reading the string from the file.

		bool load(std::string const& filename, TraceFormat tf = tfUnknown);

/// \brief Output the trace into the indicated stream.
/// \details Output the trace into the indicated stream. 
/// If a problem occurs, this routine dumps core. 
/// \param [in] os The stream to which the trace is written.
/// \param [in] tf The format used to represent the trace in the stream. If 
/// the format is tfPlain only actions are written. Default: tfMcrl2.

		void save(std::ostream &os, TraceFormat tf = tfMcrl2);

/// \brief Output the trace into a file with the indicated name.
/// \details Write the trace to a file with the indicated name. 
/// \param [in] filename The name of the file that is written.
/// \param [in] tf The format used to represent the trace in the stream. If 
/// the format is tfPlain only actions are written. Default: tfMcrl2.
/// \return A boolean is returned to indicate whether writing of the trace
/// was successful.

		bool save(std::string const& filename, TraceFormat tf = tfMcrl2);

	private:
		ATermAppl *states;
		ATermAppl *actions;
		ATermAppl *times;
		unsigned int buf_size;
		unsigned int len;
		unsigned int pos;

		void init();

		TraceFormat detectFormat(std::istream &is);
		void loadMcrl2(std::istream &is);
		void loadPlain(std::istream &is);
		void saveMcrl2(std::ostream &os);
		void savePlain(std::ostream &os);
};
