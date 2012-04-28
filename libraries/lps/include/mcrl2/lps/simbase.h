// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simbase.h

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
 *   This is an variable_list containing a sequence of DataVarIds
 *   that describe the name and sort of the process parameters.
 * - States
 *   An state type containing data_expressions that are the values of the
 *   above parameters in a state.
 * - Transitions
 *   A MultiAct that describes some transitiion.
 * - NextStates
 *   An vector of states that contains the next states from the current state.
 * - NextActions
 *   actually a vector of multi actions, giving the actions that can be done
 *   in the current state. The actions in NextActions and the states in NextStates
 *   correspond.
 *
 */

#ifndef __simbase_H__
#define __simbase_H__

#include <list>
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/lps/nextstate/standard.h"
#include "mcrl2/trace/trace.h"

class SimulatorInterface;
class SimulatorViewInterface;
class SimulatorViewDLLInterface;
class SimViewsDLL;

typedef std::list<SimulatorViewInterface*> viewlist;
typedef viewlist::iterator viewlistiterator;

class SimulatorInterface
{
  public:
    virtual inline ~SimulatorInterface() {};

    virtual void Register(SimulatorViewInterface* View) = 0;
    /* Register *View to this Simulator */
    virtual void Unregister(SimulatorViewInterface* View) = 0;
    /* Unregister previously registered *View */

    virtual void Reset() = 0;
    /* Reset trace to initial state */
    virtual void Reset(mcrl2::lps::state State) = 0;
    /* Reset trace to new initial state State */
    virtual bool Undo() = 0;
    /* Go to previous state in trace, if possible.
     * Returns true iff successful. */
    virtual bool Redo() = 0;
    /* Dual of Undo(). */

    virtual mcrl2::data::variable_list GetParameters() = 0;
    /* Returns the parameter names that correspond to the
     * elements in states. */
    virtual mcrl2::lps::state GetState() = 0;
    /* Returns the current state. */
    virtual std::vector < mcrl2::lps::state > GetNextStates()=0;
    /* Returns the currently enabled next states; The vectors with next states and next actions
       correspond at their respective positions */
    virtual std::vector < mcrl2::lps::multi_action > GetNextActions()=0;
    /* Returns the currently enabled next multi_actions;  */

    virtual mcrl2::lps::NextState* GetNextState() = 0;
    /* Returns the NextState currently in use by the
     * simulator. */
    virtual bool ChooseTransition(size_t index) = 0;
    /* Goto a state x with a transition y, where [y,x] is
     * the index'th element in GetNextStates(). */

    virtual size_t GetTraceLength() = 0;
    /* Get the length of the current trace, i.e. the number
     * of states that have been encountered. */
    virtual size_t GetTracePos() = 0;
    /* Get the position of the current state in the current
     * trace. */
    virtual bool SetTracePos(size_t pos) = 0;
    /* Set the current state to the pos'th element in the
     * trace. */
    virtual mcrl2::trace::Trace GetTrace() = 0;
    /* Get the whole trace. */
    virtual mcrl2::lps::state GetNextStateFromTrace() = 0;
    /* Get the the state following the current state in the trace.
     * Returns NULL is there is no such state. */
    virtual mcrl2::lps::multi_action GetNextTransitionFromTrace() = 0;
    /* Get the the transition following the current state in the trace.
     * Returns NULL is there is no such transitions. */
};

class SimulatorViewInterface
{
  public:
    virtual ~SimulatorViewInterface() {};

    virtual void Registered(SimulatorInterface* Simulator) = 0;
    /* Is called when this View is added to *Simulator by a call
     * to Simulator->Register(View). */
    virtual void Unregistered() = 0;
    /* Is called when this View is removed from the Simulator it
     * was previously added to. */

    virtual void Initialise(const mcrl2::data::variable_list Pars) = 0;
    /* Is called whenever a (new) simulation is started.
     * Pars contains the process parameters that correspond to
     * the elements in states. */

    virtual void StateChanged(
                  mcrl2::lps::multi_action Transition,
                  const mcrl2::lps::state State,
                  std::vector < mcrl2::lps::multi_action > next_actions,
                  std::vector < mcrl2::lps::state > next_states) = 0;
    virtual void StateChanged(
                  const mcrl2::lps::state State,
                  std::vector < mcrl2::lps::multi_action > next_actions,
                  std::vector < mcrl2::lps::state > next_states) = 0;
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

    virtual void Reset(mcrl2::lps::state State) = 0;
    /* Is called whenever the current trace is reset to the
     * singleton trace containing State. */
    virtual void Undo(size_t Count) = 0;
    /* Is called whenever one or more Undos are done, i.e.
     * the trace is still the same, but one of the previous
     * states is selected. */
    virtual void Redo(size_t Count) = 0;
    /* Dual of Undo(). */

    virtual void TraceChanged(mcrl2::trace::Trace T, size_t From) = 0;
    /* Is called whenever the current trace is (partially)
     * changed. (Not when the trace is extended (or partially
     * replaced) with one state because of a normal transition.
     * Note that this is also called when a view is loaded
     * to initialise it with the current trace.
     */
    virtual void TracePosChanged(size_t Index) = 0;
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
 * SimViewsDLL is a class that stores Views and the Simulator where they
 * are registered. On destruction of this class, all Views are unregistered
 * (if needed) and destroyed.
 *
 * SimulatorViewDLLInterface is a SimulatorViewInterface with some added
 * implementation to interact with a SimViewsDLL object. The implementor
 * of a subclass does not have to worry about this, except when he gives
 * an implementation of Registered or Unregistered (see below).
 *
 * A typical DLL source file would look as follows:
 *
 *   static SimViewsDLL *svdll;
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
 *     // Let view know which SimViewsDLL he is in and add him
 *     v->SetSimViewsDLL(svdll);
 *     svdll->Add(v,Simulator);
 *   }
 *
 *   extern "C" __attribute__((constructor)) void SimulatorViewDLLInit()
 *   {
 *     svdll = new SimViewsDLL;
 *   }
 *
 *   extern "C" __attribute__((destructor)) void SimulatorViewDLLCleanUp()
 *   {
 *     delete svdll; // This unregisters and destroys all Views in svdll
 *   }
 *
 * If MyView would implement the Registered and/or Unregistered functions,
 * they need to call the corresponding functions from
 * SimulatorViewDLLInterface to assure the correctness of the information
 * in the SimViewsDLL. This means it should look like:
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
    SimViewsDLL* simdll;
  public:
    virtual ~SimulatorViewDLLInterface();

    void Registered(SimulatorInterface* Simulator);
    void Unregistered();

    virtual void SetSimViewsDLL(SimViewsDLL* dll);
    /* Sets simdll to dll */
};

class SimViewsDLL
{
  public:
    ~SimViewsDLL();
    /* Destructor
     * Unregisters every View in views if it is linked to a
     * Simulator and the destroys the view
     */

    void Add(SimulatorViewDLLInterface* View, SimulatorInterface* Simulator, bool Register = true);
    /* Append View to views and Simulator to sims
     * If Register is true, then Simulator->Register(View) is called
     */
    void Remove(SimulatorViewDLLInterface* View, bool Unregister = true);
    /* Remove View from views and the corresponding Simulator from sims
     * If Unregister is true, then Simulator->Unregister(View) is called first
     */
    void SetSimulator(SimulatorViewDLLInterface* View, SimulatorInterface* Simulator);
    /* Set the simulator at which View is registered to Simulator */
    void ClearSimulator(SimulatorViewDLLInterface* View);
    /* Reset the simulator corresponding to View (i.e. make it NULL) */

  private:
    std::list<SimulatorInterface*> sims;
    std::list<SimulatorViewDLLInterface*> views;
};

#endif
