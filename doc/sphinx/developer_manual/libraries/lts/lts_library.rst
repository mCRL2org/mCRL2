Introduction
============
The LTS library provides data structures and methods to handle *labelled
transition systems* (LTS). There are derived classes for probabilistic LTSs. 
There is also a separate class of traces, which are essentially LTSs consisting
of a single trace. Traces are not probabilistic. 

LTSs can be created, transfered from and to 
file and transformed by for instance a bisimulation reduction, determinisation
or the removal of unreachable states.

Essentially, a labelled transition system consists
of a collection of transitions, where a transition is a triple of three 
numbers <start_state,action,end_state> each of type :mcrl2:`size_t` (i.e. unsigned machine word). 
Each labelled transition system contains an initial state. Furthermore, each
action has an associated label, for instance a string. Optionally, each state index
has also an associated label. Finally, each label is either internal
(eg., hidden or a tau action) or externally visible. 

For probabilistic transition systems the target of a transition refers to a probabilistic
state. A probabilistic state is a vector of state_probability pairs `s,p` indicating
with which probability `p` an ordinary state `s` can be reached. For a probabilistic
transition system the initial state is also a probabilistic transition system. 

A labelled transition system has template parameters for the state labels,
action labels and probabilistic states. There are several standard labelled transition systems defined.
For example, the :mcrl2:`lts_aut_t` is an LTS with strings as transition labels, and no state
labels. The :mcrl2:`lts_lts_t` has multi actions as state labels, and vectors
of data expressions as state labels. Furthermore, this latter contains
a data specification, action declarations and a declaration of process
parameters. 

Structure
=========
The LTS library resides in the namespace mcrl2::lts. The main class of this
library is the :mcrl2:`lts` class. This class represents an LTS and contains almost all
available functionality to work with LTSs. States and actions are identified by unsigned integers of
the type :mcrl2:`size_t` and transitions are triples (from_state, action, to_state).
Each LTS has an initial state and every transition 
label is either internal or external.

For each number representing an action, there is an associated value and
for each number representing a state, there can be an associated label. An 
LTS has two template parameters that determine the types of these labels. The LTS
class behaves as a standard container, in the sense that any class that can be
used in a standard C++ container is usable for the labels in a labelled transition 
system. 

There are four standard template instantiations of labelled transitions systems:
  * :mcrl2:`lts_lts_t`: Multi actions as label values. Vectors of data expressions as state values. Contains a data specification, action declarations and process parameters.
  * :mcrl2:`lts_aut_t`: Strings as label values. No state values. Is stored in the aut file format.
  * :mcrl2:`lts_fsm_t`: Strings as label values. Vectors of strings as state values. Contains the names and sorts of process parameters. Is stored in the fsm file format.
  * :mcrl2:`lts_dot_t`: Label values are strings. State values are a pair of a state name, and a state label. 

The first three classes contain load and save functionality. The `lts_dot_t` is only provided for saving. 

Creating and accessing an LTS
=============================

As an example we create a transition system with strings as values for action
labels and states. We add 2 states, and one transition. The initial state is
state 1. When the transition system is created, it is shown how
information is extracted.

.. code-block:: c++

  #include <string>
  #include "mcrl2/lts/lts.h"

  void make_color_lts()
  {
    using namespace mcrl2::lts;

    // Create the labelled transition system l.
    lts < std::string, std::string > l;

    // Add states 0 and 1. The state value is optional, but states must either
    // all have state values, or not have state values at all.
    l.add_state("Green");
    l.add_state("Red");

    // Add an action label with index 0. The second (optional) argument
    // indicates that this is not an internal label. All action labels
    // must be unique. Reduction algorithms use the index of action
    // labels, and confusion can arise when multiple identical action labels
    // exist. The lts library does not enforce that action labels are unique.
    l.add_label("Become green",true);
    
    // Add a transition from state 1 to 0.
    l.add_transition(transition(1,0,0));

    // Set the initial state (i.e., the red state)
    l.set_initial_state(1);

    // Get the number of states, state values, action labels and transitions.
    std::cout << "#states: "        << l.num_states() << "\n" <<
                 "#state values: "  << l.num_state_values() << "\n" <<
                 "#action labels: " << l.num_action_labels() << "\n"<<
                 "#transitions: "   << l.num_transitions() << "\n" <<
                 "#has state labels" << (l.has_state_info?" yes\n":" no\n");

    // Get the index of the initial state
    std::cout << "Initial state is " << l.initial_state() << "\n";

    // Traverse and print the state labels.
    for(std::size_t i=0; i<l.num_state_values(); ++i)
    {
      std::cerr << "State " << i << " has value " << l.state_value(i) << "\n";
    }

    // Traverse and print the values for action labels. Also print whether they are internal.
    for(unsigned int i=0; i<l.num_action_labels(); ++i)
    {
      std::cerr << "Action label " << i << " has value " << l.label_value(i) << 
                      (l.is_tau(i)?" (is internal)":"(is external)") << "\n";
    }

    // Traverse and print the transitions
    for(transition_const_range r=get_transitions; !r.empty(); r.advance_begin(1))
    { 
      const transition t=r.front();
      std::cerr << "Transition [" << r.from() << "," << r.label() << "," << r.to() << "]\n";
    }

    // Finally, clear the transition system. Not really necessary, because this is also done
    // by the destructor.
    l.clear();
  } 

Note that there are no load and save methods in this base class as these depend on the nature of the state and
action values. They are provided in the derived classes belonging to each specific format. 
There are however standard functions to make actions internal, based on a set
of action strings, as well as utility functions to sort the transitions based on various criteria.
See the __lts_reference__ for this.

The standard labelled transition systems
========================================

There are four standard labelled transition systems. In addition to determining the 
value types of states and action labels, they can contain additional information. 
Each of these labelled transition systems are related to some file format and therefore,
they all provide load and save functionality. 

The enumerated type :mcrl2:`lts_type` contains for all the formats an element. The default element
is :mcrl2:`lts_none`, not referring to any type.
Furthermore, each standard
labelled transition system has its own file extension. The Extra information refers to data and action declarations for the 
`lts_lts_t` format. For the `lts_fsm_t` it is recalled which variables occur in the state vector, which labels a state, and
for each of these variables the values that it can attain are also recalled. 
The table below shows them.

.. table:: Standard LTS formats

   +-----------------------+-----------------------+----------------+-----------------------------+-------------------------------+-------------------+
   |Class                  | Element from lts_type | File extension | State value type            | Label value type              | Extra information |
   +=======================+=======================+================+=============================+===============================+===================+
   |:mcrl2:`lts_lts_t`     | :mcrl2:`lts_lts`      | .lts           |:mcrl2:`state_label_lts`     |:mcrl2:`action_label_lts`      |Yes                |
   +-----------------------+-----------------------+----------------+-----------------------------+-------------------------------+-------------------+
   |:mcrl2:`lts_aut_t`     | :mcrl2:`lts_aut`      | .aut           |:mcrl2:`state_label_empty`   |:mcrl2:`action_label_string`   |No                 |
   +-----------------------+-----------------------+----------------+-----------------------------+-------------------------------+-------------------+   
   |:mcrl2:`lts_fsm_t`     | :mcrl2:`lts_fsm`      | .fsm           |:mcrl2:`state_label_fsm`     |:mcrl2:`action_label_string`   |Yes                |
   +-----------------------+-----------------------+----------------+-----------------------------+-------------------------------+-------------------+   
   |:mcrl2:`lts_dot_t`     | :mcrl2:`lts_dot`      | .dot           |:mcrl2:`state_label_dot`     |:mcrl2:`action_label_string`   |No                 |
   +-----------------------+-----------------------+----------------+-----------------------------+-------------------------------+-------------------+   

For the reduction we simply call the reduce() method with the option
lts_eq_trace.

.. code-block:: c++

  l.reduce(lts_eq_trace);

The LTS l has now been reduced, so we can print the result. We iterate over all
transitions in a loop as follows.

.. code-block:: c++

  for (const transition& t: l.get_transitions())
  {

We show the states by printing their identifiers (i.e. the unsigned integers),
but for the labels we wish to use the actual value as a string, which we can
obtain as follows.

.. code-block:: c++

    string label = pp(l.action_label(t.label());

To print each transition we do the following.

.. code-block:: c++

    cout << t.from() << "  -- " << label << " -->  " t.to() << endl;
  }

The output is as follows::

  1  -- open_door -->  0
  0  -- win_flowers -->  2
  0  -- win_car -->  2

Note that the initial state is 1. To verify this one could also print
``l.initial_state()``. 

Reducing and comparing labelled transition systems
==================================================

It is possible to reduce an lts modulo different equivalences. 
The transition system will be replaced by another transition system
that is generally smaller in such a way that the initial state is
still equivalent to the old initial state. The equivalence that 
are available change all the time. It is best to see the help
text of tools such as `ltscompare` and `ltsconvert` for the latest
available reductions. Some that have been implemented are:

  * :mcrl2:`lts_eq_none`:             No reduction
  * :mcrl2:`lts_eq_bisim`:            Strong bisimulation equivalence, using an O(m log m) algorithm [Groote/Jansen/Keiren/Wijs 2017]
  * :mcrl2:`lts_eq_bisim_gv`:         Strong bisimulation equivalence, using the traditional O(mn) algorithm [Groote/Vaandrager 1990]
  * :mcrl2:`lts_eq_bisim_dnj`:        Strong bisimulation equivalence, using an experimental O(m log n) algorithm (Jansen, not yet published)
  * :mcrl2:`lts_eq_bisim_sigref`:     Strong bisimulation equivalence, using the signature refinement algorithm [Blom/Orzan 2003]
  * :mcrl2:`lts_eq_branching_bisim`:  Branching bisimulation equivalence, using an O(m log m) algorithm [Groote/Jansen/Keiren/Wijs 2017]
  * :mcrl2:`lts_eq_branching_bisim_gv`: Branching bisimulation equivalence, using the traditional O(mn) algorithm [Groote/Vaandrager 1990]
  * :mcrl2:`lts_eq_branching_bisim_dnj`: Branching bisimulation equivalence, using an experimental O(m log n) algorithm (Jansen, not yet published)
  * :mcrl2:`lts_eq_branching_bisim_sigref`: Branching bisimulation equivalence, using the signature refinement algorithm [Blom/Orzan 2003]
  * :mcrl2:`lts_eq_divergence_preserving_branching_bisim`: Divergence-preserving branching bisimulation equivalence, using an O(m log m) algorithm [Groote/Jansen/Keiren/Wijs 2017]
  * :mcrl2:`lts_eq_divergence_preserving_branching_bisim_gv`: Divergence-preserving branching bisimulation equivalence, using the traditional O(mn) algorithm [Groote/Vaandrager 1990]
  * :mcrl2:`lts_eq_divergence_preserving_branching_bisim_dnj`: Divergence-preserving branching bisimulation equivalence, using an experimental O(m log n) algorithm (Jansen, not yet published)
  * :mcrl2:`lts_eq_divergence_preserving_branching_bisim_sigref`: Divergence-preserving branching bisimulation equivalence, using the signature refinement algorithm [Blom/Orzan 2003]
  * :mcrl2:`lts_eq_weak_bisim`:       Weak bisimulation equivalence
  * :mcrl2:`lts_eq_divergence_preserving_weak_bisim`: Divergence-preserving weak bisimulation equivalence
  * :mcrl2:`lts_eq_sim`:              Strong simulation equivalence
  * :mcrl2:`lts_eq_ready_sim`:        Strong ready simulation equivalence     
  * :mcrl2:`lts_eq_trace`:            Strong trace equivalence
  * :mcrl2:`lts_eq_weak_trace`:       Weak trace equivalence
  * :mcrl2:`lts_eq_isomorph`:         Isomorphism.

Application of the reduction of an lts is pretty simple. Note that the
lts is replaced by the reduced lts. The original lts will be destroyed.

.. code-block:: c++

    lts_aut_t l;
    l.load("an_lts.aut");
    reduce(l,lts_eq_branching_bisim))
    cout << "Transition system is succesfully reduced modulo branching bisimulation";

It is also possible to compare an lts to another lts. This can be done
using the equivalence options mentioned above. But it is also possible to
use the other preorders such as: 

  * :mcrl2:`lts_pre_none`:             No preorder 
  * :mcrl2:`lts_pre_sim`:              Strong simulation preorder
  * :mcrl2:`lts_pre_ready_sim`:        Strong ready simulation preorder     
  * :mcrl2:`lts_pre_trace`:            Strong trace preorder 
  * :mcrl2:`lts_pre_weak_trace`:       Weak trace preorder 

Comparing labelled transition systems is done using the reduction algorithms.
This means that the transition systems are destroyed when the comparison is
calculated. To avoid destruction a copy is made of the transition system. But as
transition systems can be extremely large, this is not always desired. Therefore,
we provide a :mcrl2:`compare` function that makes copies of the transition system to 
avoid that they get damaged and a :mcrl2:`destructive_compare` which may change both
transition systems.

.. code-block:: c++

    lts_lts_t l1,l2;
    l1.load("lts1.lts");
    l2.load("lts2.lts");

    if (compare(l1,l2,lts_eq_bisim))    // Non destructive compare.
    { cout << "Transition systems are bisimilar\n";
    }
    else
    { cout << "Transitions systems are not bisimilar";
    }
    
    if (destructive_compare(l1,l2,lts_pre_sim))  // Destructive compare.
    { cout << "Transitions system l1 is strongly simulated by l2";
    }
    else
    { cout << "Lts l1 is not strongly simulated by l2";
    }

The non-destructive compares may make a copy of the transition system, which can
be expensive as transition systems can be large. 

Some utility functions
======================
There are a number of standard functions implemented on labelled transition systems,
such as making a transition system deterministic (can lead to a huge transition system),
calculating the strongly connected components, etc. 
   
  * determinise(l);
  * reachability_check(l,remove_unreachable);
  * is_deterministic(l);
  * scc_reduce(l, preserve_divergence_loops);
 
Traces
======

There is a special class `trace` to store traces. 
A trace is a sequence of multi actions [^a1 a2 a3 ... an]. Between the multi actions there can
be states and the multi actions can have time tags. In the most extensive form
a trace is a sequence [^s1 a1@t1 s2 a2@t2 ... an@tn sn+1] where [^si]
is a state [^i], [^ti] is a time tag [^i] and [^ai] is a multiaction [^i].

Traces can be generated using a
simulation tool but they can also be the result of an analysis tool. E.g., an
analysis tool can generate one or more traces to a deadlock. Such a generated
trace can subsequently be inspected by a tool capable of reading a trace.
A trace can be stored in readable format, as a sequence of multi-actions, or
in internal format, in which case it is stored as a lts_lts_t transition system.
This has the advantage that tools such as `ltsgraph` can be used to view them.

Internally traces are stored as a vector of multi actions, an optional vector
of state labels and a position indicating what the current position in the
trace is. Traces are typically used by simulators, such as the tool `lpssim` of
`lpsxsim`. They are generated by for instance the tool `lps2lts`, to indicate
a path to for instance a deadlock or a particular action. 

The following fragment of code shows how to read a trace from standard in and
print its contents to standard out. Moving to the next transition in the
state is done by incrementing the current position explicitly. 

.. code-block:: c++

  #include <iostream>
  #include "mcrl2/trace/trace.h"

  using namespace std;
  using namespace mcrl2::lts;

  int main(int argc, char **argv)
  {
    trace t;
    t.load(""); // read trace from stdin

    for(std::size_t i=0 ; i<tr.number_of_actions() ; i++)
    {
      if (t.current_state_exists())
      {
        std::cout << "State: " << t.current_state() << "\n";
      }
      std::cout << "Action: " << t.current_action() << "\n";
      t.increase_position();
    }
    if (t.current_state_exists())
    {
      std::cout << "Final state: " << t.next_state() << "\n";
    }
  }

There are many other methods available, such as methods to truncate the current
trace, adding new states and transitions. This is for instance useful when 
doing a simulation, where it is decided that halfway a simulation another branch
of the behaviour needs to be explored. 
