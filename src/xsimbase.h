/* To make a new view one has to make a new class derived from
 * the SimulatorViewInterface below. Views are registered at
 * the actual Simulator with the Register(View) which will
 * result in the calling of Registered of the view and possibly
 * some of the other functions to initialise the view to the
 * current state.
 *
 * Objects that are communicated between the simulator and
 * views are the following:
 *
 * - Parameters
 *   This is an ATermList containing a sequence of DataVarIds
 *   that describe the name and sort of the process parameters.
 * - States
 *   An ATerm containing DataExprs that are the values of the
 *   above parameters in a state.
 * - Transitions
 *   A MultiAct that describes some transitiion.
 * - NextStates
 *   An ATermList that consist of a sequence of pairs (or
 *   actually a ATermList of length 2) of a transition and a
 *   state that describe enabled mulitaction and the resulting
 *   state.
 *
 * Note that states may contain DataVarIds. These are free
 * variables.
 */

#ifndef __xsimbase_H__
#define __xsimbase_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "xsimbase.h"
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <list>
#include <aterm2.h>

using namespace std;

class SimulatorInterface;
class SimulatorViewInterface;
class SimulatorViewDLLInterface;
class XSimViewsDLL;

typedef list<SimulatorViewInterface *> viewlist;
typedef viewlist::iterator viewlistiterator;

class SimulatorInterface
{
public:
	virtual void Register(SimulatorViewInterface *View) = 0;
	/* Register *View to this Simulator */
	virtual void Unregister(SimulatorViewInterface *View) = 0;
	/* Unregister previously registered *View */

	virtual wxWindow *MainWindow() = 0;
	/* Returns the main window of the simulator. */

	virtual void Reset() = 0;
	/* Reset trace to initial state */
	virtual void Reset(ATerm State) = 0;
	/* Reset trace to new initial state State */
	virtual bool Undo() = 0;
	/* Go to previous state in trace, if possible.
	 * Returns true iff successful. */
	virtual bool Redo() = 0;
	/* Dual of Undo(). */

	virtual ATermList GetParameters() = 0;
	/* Returns the parameter names that correspond to the
	 * elements in states. */
	virtual ATerm GetState() = 0;
	/* Returns the current state. */
	virtual ATermList GetNextStates() = 0;
	/* Returns the currently enabled transitions and the
	 * resulting states. */
	virtual bool ChooseTransition(int index) = 0;
	/* Goto a state x with a transition y, where [y,x] is
	 * the index'th element in GetNextStates(). */

	virtual int GetTraceLength() = 0;
	/* Get the length of the current trace, i.e. the number
	 * of states that have been encountered. */
	virtual int GetTracePos() = 0;
	/* Get the position of the current state in the current
	 * trace. */
	virtual bool SetTracePos(int pos) = 0;
	/* Set the current state to the pos'th element in the
	 * trace. */
	virtual ATermList GetTrace() = 0;
	/* Get the whole trace. */
	virtual bool SetTrace(ATermList Trace, int From = 0) = 0;
	/* Set the trace to Trace starting at position From. */
};

class SimulatorViewInterface
{
public:
	virtual void Registered(SimulatorInterface *Simulator) = 0;
	/* Is called when this View is added to *Simulator by a call
	 * to Simulator->Register(View). */
	virtual void Unregistered() = 0;
	/* Is called when this View is removed from the Simulator it
	 * was previously added to. */

	virtual void Initialise(ATermList Pars) = 0;
	/* Is called whenever a (new) simulation is started.
	 * Pars contains the process parameters that correspond to
	 * the elements in states. */

	virtual void StateChanged(ATermAppl Transition, ATerm State, ATermList NextStates) = 0;
	/* Is called whenever the current state in the simulator is
	 * changed.
	 * Transition is the action that was 'executed', which can
	 * possibly be NULL. If it is not NULL, then it means that
	 * the current trace is extended (or partially replaces) with
	 * this Transition and State.
	 * NextStates is the list of currently enabled transitions
	 * and the resulting states.
	 * Note that this function is always called when the state
	 * changes, even when, for example, Reset() has already
	 * been called. */

	virtual void Reset(ATerm State) = 0;
	/* Is called whenever the current trace is reset to the
	 * singleton trace containing State. */
	virtual void Undo(int Count) = 0;
	/* Is called whenever one or more Undos are done, i.e.
	 * the trace is still the same, but one of the previous
	 * states is selected. */
	virtual void Redo(int Count) = 0;
	/* Dual of Undo(). */

	virtual void TraceChanged(ATermList Trace, int From) = 0;
	/* Is called whenever the current trace is (partially)
	 * changed. (Not when the trace is extended (or partially
	 * replaced) with one state because of a normal transition.
	 * Note that this is also called when a view is loaded
	 * to initialise it with the current trace.
	 */
	virtual void TracePosChanged(ATermAppl Transition, ATerm State, int Index) = 0;
	/* Is called whenever another element of the current trace
	 * is selected. Note that Transition might be Nil in the
	 * case that State is the initial state (i.e. Index is 0).
	 * Note that this is aloso called when a view is loaded
	 * to initialis it with the current position (if the trace
	 * is not empty). */
};

class SimulatorViewDLLInterface: public SimulatorViewInterface
{
	protected:
		XSimViewsDLL *xsimdll;
	public:
		void SetXSimViewsDLL(XSimViewsDLL *dll);
};

class XSimViewsDLL
{
public:
	XSimViewsDLL();
	~XSimViewsDLL();

	void Add(SimulatorViewDLLInterface *View, SimulatorInterface *Simulator, bool Register = true);
	void Remove(SimulatorViewDLLInterface *View, bool Unregister = true);
	void SetSimulator(SimulatorViewDLLInterface *View, SimulatorInterface *Simulator);
	void ClearSimulator(SimulatorViewDLLInterface *View);

private:
	list<SimulatorInterface *> sims;
	list<SimulatorViewDLLInterface *> views;
};

#endif
