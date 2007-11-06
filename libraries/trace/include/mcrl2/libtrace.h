// Author(s): Muck van Weerdenburg. Documentation Jan Friso Groote.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file libtrace.h
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
/// human readable. tfUnknown is only used to read traces, when it is
/// not known what the format is. In this case it is determined based
/// on the format of the input file.
enum TraceFormat { tfMcrl2, tfPlain, tfUnknown };

class Trace
{
	public:
/// \brief Default constructor for an empty trace
		Trace();
/// \brief Construct the trace in the basis of an input stream
		Trace(std::istream &is, TraceFormat tf = tfUnknown);
/// \brief Construct the trace on the basis of an input file
		Trace(std::string const& filename, TraceFormat tf = tfUnknown);
/// \brief Destructor for traces
		~Trace();

/// \brief Set the current position back to the beginning of the trace
		void resetPosition();

/// \brief Set the current position after the Pos'th action of the trace
/// Set the current position after the Pos'th action of the trace. The
/// initial position corresponds to Pos=0. If Pos is beyond the last
/// action in the trace, no new position is set.
		void setPosition(unsigned int pos);

/// \brief Get the current position in the trace
		unsigned int getPosition();

/// \brief Get the current position in the trace
/// Get the length of the trace. If getLength()=n, valid positions
/// in the trace are smaller than n.
		unsigned int getLength();

/// \brief Get the state at the current position in the trace
		ATermAppl currentState();
/// \brief Get the outgoing action from the current state in the trace
		ATermAppl nextAction();
/// \brief Get the time of the current action from the current state in the trace
/// Get the time of the current action from the current state in the trace. If 
/// the current action is untimed, NULL is returned.
		ATermAppl currentTime();

		void truncate();

		void addAction(ATermAppl action, ATermAppl time = NULL);
		bool setState(ATermAppl state);
		bool canSetState();

		ATermAppl getAction();
		ATermAppl getState();
		ATermAppl getTime();

		void load(std::istream &is, TraceFormat tf = tfUnknown);
		bool load(std::string const& filename, TraceFormat tf = tfUnknown);
		void save(std::ostream &os, TraceFormat tf = tfMcrl2);
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
