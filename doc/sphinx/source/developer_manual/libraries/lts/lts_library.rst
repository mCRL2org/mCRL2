Introduction
============
The LTS library provides data structures and methods to handle *labelled
transition systems* (LTS). LTS's can be created, transfered from and to 
file and transformed by for instance a bisimulation reduction, determinisation
or the removal of unreachable states.

Essentially, a labelled transition system consists
of a collection of transitions, where a transition is a triple of three 
numbers <start_state,action,end_state> each of type :cpp:type:`size_t` (i.e. unsigned machine word). 
Each labelled transition system contains an initial state. Furthermore, each
action has an associated label, for instance a string. Optionally, each state index
has also an associated label. Finally, each label is either internal
(eg., hidden or a tau action) or externally visible. 

A labelled transition system has template parameters for the state and 
action labels. There are several standard labelled transition systems defined.
For example, the :cpp:type:`lts_aut_t` is an LTS with strings as transition labels, and no state
labels. The :cpp:type:`lts_lts_t` has multi actions as state labels, and vectors
of data expressions as state labels. Furthermore, this latter contains
a data specification, action declarations and a declaration of process
parameters. 

Structure
=========
The LTS library resides in the namespace mcrl2::lts. The main class of this
library is the :cpp:class:`lts` class. This class represents an LTS and contains almost all
available functionality to work with LTSs. States and actions are identified by unsigned integers of
the type :cpp:type:`size_t` and transitions are triples (from_state, action, to_state).
Each LTS has an initial state and every transition 
label is either internal or external.

For each number representing an action, there is an associated value and
for each number representing a state, there can be an associated label. An 
LTS has two template parameters that determine the types of these labels. The LTS
class behaves as a standard container, in the sense that any class that can be
used in a standard C++ container is usable for the labels in a labelled transition 
system. 

There are six standard template instantiations of labelled transitions systems:
  * :cpp:type:`lts_lts_t`: Multi actions as label values. Vectors of data expressions as state values. Contains a data specification, action declarations and process parameters.
  * :cpp:type:`lts_aut_t`: Strings as label values. No state values. Is stored in the aut file format.
  * :cpp:type:`lts_fsm_t`: Strings as label values. Vectors of strings as state values. Contains the names and sorts of process parameters. Is stored in the fsm file format.
  * :cpp:type:`lts_bcg_t`: Strings as label values. No state values. Is stored in the bcg file format.
  * :cpp:type:`lts_dot_t`: Label values are strings. State values are a pair of a state name, and a state label.
  * :cpp:type:`lts_svc_t`: The values of action labels and states are ATerms.

Each of these six classes contains at least load and save functionality, as well as as function pp to
transform state and label values to strings. 

Creating and accessing an LTS
=============================

As an example we create a transition system with strings as values for action
labels and states. We add 2 states, and one transition. The initial state is
state 1. When the transition system is created, it is shown how
information is extracted.

.. code-block:: c++

  #include <string>
  #include <mcrl2/lts/lts.h>

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
    // indicates that this is not an internal label.
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
    for(unsigned int i=0; i<l.num_state_values(); ++i)
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

Note that there are no standard load and save methods as these depend on the nature of the state and
action values. There are however standard functions to make actions internal, based on a set
of action strings, as well as utility functions to sort the transitions based on various criteria.
See the __lts_reference__ for this.

The standard labelled transition systems
========================================

There are six standard labelled transition systems. In addition to determining the 
value types of states and action labels, they can contain additional information. 
Each of theselabelled transition systems are related to some file format and therefore,
they all provide load and save functionality. 

The enumerated type :cpp:type:`lts_type` contains for all the formats an element. The default element
is :cpp:member:`lts_none`, not referring to any type.
Furthermore, each standard
labelled transition system has its own file extension. The table below shows them.

.. table:: Standard LTS formats

   +-----------------------+-----------------------+----------------+-----------------------------+-------------------------------+-------------------+
   |Class                  | Element from lts_type | File extension | State value type            | Label value type              | Extra information |
   +=======================+=======================+================+=============================+===============================+===================+
   |:cpp:class:`lts_lts_t` | :cpp:member:`lts_lts` | .lts           |:cpp:type:`state_label_lts`  |:cpp:type:`action_label_lts`   |Yes                |
   +-----------------------+-----------------------+----------------+-----------------------------+-------------------------------+-------------------+
   |:cpp:class:`lts_aut_t` | :cpp:member:`lts_aut` | .aut           |:cpp:type:`state_label_empty`|:cpp:type:`action_label_string`|No                 |
   +-----------------------+-----------------------+----------------+-----------------------------+-------------------------------+-------------------+   
   |:cpp:class:`lts_fsm_t` | :cpp:member:`lts_fsm` | .fsm           |:cpp:type:`state_label_fsm`  |:cpp:type:`action_label_string`|Yes                |
   +-----------------------+-----------------------+----------------+-----------------------------+-------------------------------+-------------------+   
   |:cpp:class:`lts_dot_t` | :cpp:member:`lts_dot` | .dot           |:cpp:type:`state_label_dot`  |:cpp:type:`action_label_string`|No                 |
   +-----------------------+-----------------------+----------------+-----------------------------+-------------------------------+-------------------+   
   |:cpp:class:`lts_bcg_t` | :cpp:member:`lts_bcg` | .bcg           |:cpp:type:`state_label_empty`|:cpp:type:`action_label_string`|No                 |
   +-----------------------+-----------------------+----------------+-----------------------------+-------------------------------+-------------------+   
   |:cpp:class:`lts_svc_t` | :cpp:member:`lts_svc` | .svc           |:cpp:type:`state_label_svc`  |:cpp:type:`action_label_lts`   |No                 |
   +-----------------------+-----------------------+----------------+-----------------------------+-------------------------------+-------------------+


For the reduction we simply call the reduce() method with the option
lts_eq_trace.

.. code-block:: c++

  l.reduce(lts_eq_trace);

The LTS l has now been reduced, so we can print the result. We iterate over all\
transitions in a loop as follows.

.. code-block:: c++

  for (transition_iterator i = l.get_transitions(); i.more(); ++i)
  {

We show the states by printing their identifiers (i.e. the unsigned integers),
but for the labels we wish to use the actual value as a string, which we can
obtain as follows.

.. code-block:: c++

    string label = l.label_value_str(i.label());

To print each transition we do the following.

.. code-block:: c++

    cout << i.from() << "  -- " << label << " -->  " i.to() << endl;
  }

The output is as follows::

  1  -- open_door -->  0
  0  -- win_flowers -->  2
  0  -- win_car -->  2

Note that the initial state is 1. To verify this one could also print
``l.initial_state()``. 

Reducing and comparing labelled transition systems
==================================================

It is possible to reduce an lts modulo different equivalencies. 
The transition system will be replaced by another transition system
that is generally smaller in such a way that the initial state is
still equivalent to the old initial state. The equivalencies that
have been implemented are:

  * :cpp:member:`lts_eq_none`:            No reduction
  * :cpp:member:`lts_eq_bisim`:            Strong bisimulation equivalence 
  * :cpp:member:`lts_eq_branching_bisim`:  Branching bisimulation equivalence
  * :cpp:member:`lts_eq_divergence_preserving_branching_bisim`: Divergence preserving branching bisimulation equivalence 
  * :cpp:member:`lts_eq_sim`:              Strong simulation equivalence 
  * :cpp:member:`lts_eq_trace`:            Strong trace equivalence
  * :cpp:member:`lts_eq_weak_trace`:       Weak trace equivalence
  * :cpp:member:`lts_eq_isomorph`:         Isomorphism.

Application of the reduction of an lts is pretty simple. Note that the
lts is replaced by the reduced lts. The original lts will be destroyed.

.. code-block:: c++

    lts l("an_lts.aut");
    if (l.reduce(lts_eq_branching_bisim))
    { cout << "Transition system is succesfully reduced modulo branching bisimulation";
    }
    else 
    { cout << "Reduction failed";
    }

It is also possible to compare an lts to another lts. This can be done
using the equivalence options mentioned above. But it is also possible to
use the following preorders. 

  * :cpp:member:`lts_pre_none`:             No preorder 
  * :cpp:member:`lts_pre_sim`:              Strong simulation preorder 
  * :cpp:member:`lts_pre_trace`:            Strong trace preorder 
  * :cpp:member:`lts_pre_weak_trace`:       Weak trace preorder 

Comparing labelled transition systems is done using the reduction algorithms.
This means that the transition systems are destroyed when the comparison is
calculated. To avoid destruction a copy is made of the transition system. But as
transition systems can be extremely large, this is not always desired. Therefore,
we provide a :cpp:func:`compare` function that makes copies of the transition system to 
avoid that they get damaged and a :cpp:func:`destructive_compare` which may change both
transition systems.

.. code-block:: c++

    lts l1("lts1.aut");
    lts l2("lts2.aut");

    if (l1.compare(l2,lts_eq_bisim))    // Non destructive compare.
    { cout << "Transition systems are bisimilar\n";
    }
    else
    { cout << "Transitions systems are not bisimilar";
    }
    
    if (l1.destructive_compare(l2,lts_pre_sim))  // Destructive compare.
    { cout << "Transitions system l1 is strongly simulated by l2";
    }
    else
    { cout << "Lts l1 is not strongly simulated by l2";
    }

Some utility functions
======================
Explain:
   
  * determinise(l);
  * reachability_check(l,remove_unreachable);
  * is_deterministic(l);
  * scc_reduce(l, preserve_divergence_loops);
 
