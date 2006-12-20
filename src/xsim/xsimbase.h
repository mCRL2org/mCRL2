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
 *   state that describe enabled multiaction and the resulting
 *   state.
 *
 * Note that states may contain DataVarIds. These are free
 * variables.
 */

#ifndef __xsimbase_H__
#define __xsimbase_H__

#include <wx/wx.h>
#include <list>
#include <aterm2.h>
#include "libnextstate.h"

class SimulatorInterface;
class SimulatorViewInterface;
class SimulatorViewDLLInterface;
class XSimViewsDLL;

typedef std::list<SimulatorViewInterface *> viewlist;
typedef viewlist::iterator viewlistiterator;

class SimulatorInterface
{
public:
	virtual inline ~SimulatorInterface() {};

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
	/* Returns the NextState currently in use by the
	 * simulator. */
	virtual NextState *GetNextState() = 0;
	/* Returns the currently enabled transitions and the
	 * resulting states. */
	virtual bool ChooseTransition(unsigned int index) = 0;
	/* Goto a state x with a transition y, where [y,x] is
	 * the index'th element in GetNextStates(). */

	virtual int GetTraceLength() = 0;
	/* Get the length of the current trace, i.e. the number
	 * of states that have been encountered. */
	virtual int GetTracePos() = 0;
	/* Get the position of the current state in the current
	 * trace. */
	virtual bool SetTracePos(unsigned int pos) = 0;
	/* Set the current state to the pos'th element in the
	 * trace. */
	virtual ATermList GetTrace() = 0;
	/* Get the whole trace. */
	virtual bool SetTrace(ATermList Trace, unsigned int From = 0) = 0;
	/* Set the trace to Trace starting at position From. */
};

class SimulatorViewInterface
{
public:
	virtual ~SimulatorViewInterface() {};

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
	 * been called.
	 * Furthermore the function is called when a view is loaded
	 * to initialise it with the current state. */

	virtual void Reset(ATerm State) = 0;
	/* Is called whenever the current trace is reset to the
	 * singleton trace containing State. */
	virtual void Undo(unsigned int Count) = 0;
	/* Is called whenever one or more Undos are done, i.e.
	 * the trace is still the same, but one of the previous
	 * states is selected. */
	virtual void Redo(unsigned int Count) = 0;
	/* Dual of Undo(). */

	virtual void TraceChanged(ATermList Trace, unsigned int From) = 0;
	/* Is called whenever the current trace is (partially)
	 * changed. (Not when the trace is extended (or partially
	 * replaced) with one state because of a normal transition.
	 * Note that this is also called when a view is loaded
	 * to initialise it with the current trace.
	 */
	virtual void TracePosChanged(ATermAppl Transition, ATerm State, unsigned int Index) = 0;
	/* Is called whenever another element of the current trace
	 * is selected. Note that Transition might be Nil in the
	 * case that State is the initial state (i.e. Index is 0).
	 * Note that this is also called when a view is loaded
	 * to initialise it with the current position (if the trace
	 * is not empty). */
};


/* Creating a dynamic library containing Views.
 *
 * It is assumed that every dynamically loaded library has the following
 * function:
 *
 *   extern "C" void SimulatorViewDLLAddView(SimulatorInterface *);
 *
 * AFter loading the library, this function is called such that the DLL
 * knowns with Simulator it is dealing with and to give it an opportunity
 * to register one or more Views at this Simulator.
 *
 * Because of the dynamic nature of these libraries, one usually wishes
 * to keep some administration of the registered Views. This is, for
 * example, necessary to be able to cleanly unload a DLL or avoid problems
 * if a library is loaded twice. For this prupose, the following classes
 * are available.
 *
 * XSimViewsDLL is a class that stores Views and the Simulator where they
 * are registered. On destruction of this class, all Views are unregistered
 * (if needed) and destroyed.
 *
 * SimulatorViewDLLInterface is a SimulatorViewInterface with some added
 * implementation to interact with a XSimViewsDLL object. The implementor
 * of a subclass does not have to worry about this, except when he gives
 * an implementation of Registered or Unregistered (see below).
 *
 * A typical DLL source file would look as follows:
 *
 *   static XSimViewsDLL *xsvdll;
 *
 *   extern "C" void SimulatorViewDLLAddView(SimulatorInterface *Simulator)
 *   {
 *     MyView *v;
 *
 *     // Create View with the main window of Simulator as parent
 *     // and show it.
 *     v = new MyView(Simulator->MainWindow());
 *     v->Show();
 *
 *     // Let view know which XSimViewsDLL he is in and add him
 *     v->SetXSimViewsDLL(xsvdll);
 *     xsvdll->Add(v,Simulator);
 *   }
 *
 *   extern "C" __attribute__((constructor)) void SimulatorViewDLLInit()
 *   {
 *     xsvdll = new XSimViewsDLL;
 *   }
 *
 *   extern "C" __attribute__((destructor)) void SimulatorViewDLLCleanUp()
 *   {
 *     delete xsvdll; // This unregisters and destroys all Views in xsvdll
 *   }
 *
 * If MyView would implement the Registered and/or Unregistered functions,
 * they need to call the corresponding functions from
 * SimulatorViewDLLInterface to assure the correctness of the information
 * in the XSimViewsDLL. This means it should look like:
 *
 *   void MyView::Registered(SimulatorInterface *Simulator)
 *   {
 *     // Call function of superclass
 *     SimulatorViewDLLInterface::Registered(Simulator)
 *
 *     // Own code
 *     ...
 *   }
 *
 *   void MyView::Unregistered()
 *   {
 *     // Call function of superclass
 *     SimulatorViewDLLInterface::Unregistered()
 *
 *     // Own code
 *     ...
 *   }
 */

class SimulatorViewDLLInterface: public SimulatorViewInterface
{
	protected:
		XSimViewsDLL *xsimdll;
	public:
		virtual ~SimulatorViewDLLInterface();
		
		void Registered(SimulatorInterface *Simulator);
		void Unregistered();

		virtual void SetXSimViewsDLL(XSimViewsDLL *dll);
		/* Sets xsimdll to dll */
};

class XSimViewsDLL
{
public:
	XSimViewsDLL();
	/* Constructor */

	~XSimViewsDLL();
	/* Destructor
	 * Unregisters every View in views if it is linked to a
	 * Simulator and the destroys the view
	 */

	void Add(SimulatorViewDLLInterface *View, SimulatorInterface *Simulator, bool Register = true);
	/* Append View to views and Simulator to sims
	 * If Register is true, then Simulator->Register(View) is called
	 */
	void Remove(SimulatorViewDLLInterface *View, bool Unregister = true);
	/* Remove View from views and the corresponding Simulator from sims
	 * If Unregister is true, then Simulator->Unregister(View) is called first
	 */
	void SetSimulator(SimulatorViewDLLInterface *View, SimulatorInterface *Simulator);
	/* Set the simulator at which View is registered to Simulator */
	void ClearSimulator(SimulatorViewDLLInterface *View);
	/* Reset the simulator corresponding to View (i.e. make it NULL) */

private:
	std::list<SimulatorInterface *> sims;
	std::list<SimulatorViewDLLInterface *> views;
};

#endif
