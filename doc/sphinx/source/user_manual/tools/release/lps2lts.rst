.. index:: lps2lts

.. _tool-lps2lts:

lps2lts
=======

The tool lps2lts generates a labelled transition system from a linear process (with extension .lps).
While doing so, various checks can be performed, such as checks for deadlock, livelock, and the presence of certain actions. 

The generated transition system can be saved in various formats (.aut, .lts, .fsm and .dot). 
Generally not saving, or saving in .aut format is the fastest, but it only contains information 
about transitions. The .lts format has the advantage
that all information about the transition system is included, including data types, action declarations and
state information. It is however not human readable. The .fsm format contains transition and state information,
and is human readable. 

A typical use of lps2lts is as follows::

  lps2lts --verbose infile.lps outfile.aut

This automatically generates the file in .aut format. If no outfile is given, the transition system will
be generated but not saved. The flag --verbose is the verbose flag, such that lps2lts gives some information about
progress.

The labelled transition systems can be inspected using graphical tools like :ref:`tool-ltsgraph`,
:ref: `tool-ltsview` and :ref:`tool-diagraphica`. Using the tool :ref:`tool-ltsconvert` the 
transition systems can be reduced modulo behavioural equivalences. Other tools allow to compare
labelled transition systems :ref:`tool-ltsconvert`, verify modal formulas :ref:`tool-lts2pbes`, etc.

There are options to search for particular action labels, multi-actions with data, deadlocks and livelocks.
The --trace option instructs the tool to save (a limited number) of traces to such states. Only one trace
per state is saved. If the trace count is reached lps2lts stops. As a typical example, searching for an
error action, saving one state and stop, can be done as follows::

  lps2lts --verbose --action=error --trace=1 infile.lps

Generating a labelled transition system can be quite time consuming. Choosing the compiling rewriter
using the --rewriter=jittyc can speed up the generation with a factor 10. The compiling rewriter is
not available on all platforms. The use of the flag --cached may also have a dramatic influence on 
the generation speed, at the expense of using more memory. It caches the results of evaluating conditions
in each summand in the linear process. 

There are several options to traverse the state space. Default is breadth-first. But depth-first, random, 
and prioritised are also possible. Of special note is highway search [EGWW09]_. When exploring the state
space there is a stack of encountered states not yet explored. Using::

  lps2lts --verbose --todo-max=100000 --action=error infile.lps 

the size of this stack is limited to in this case 100000 states. If the stack exceeds this size additional 
states are randomly ignored. High way search does not guarantee that all states are seen, but it an effective way
to explore a state space, far more effective than random simulation. 

When the linear process has confluent tau actions (which can be proven using the tool :ref:`tool-lpsconfcheck`)
then the flag --confluent generates a state space giving priority to confluent tau's [GM14]_. In certain cases
this can give an exponential reduction. The confluent tau is by default called ctau.

When generating the transition system is taking too much time, the generation can be aborted. lps2lts will attempt
to save the transition system before terminating. Using the flag --max the size of the state space can also be 
limited a priori. 

.. warning::

   Generation of an labelled transition system typically fails when 

      * The rewrite system is not terminating. A typical non terminating rewrite rule is eqn  f(x) = if(x==0, 0, f(x-1)); as 
        f can infinitely be rewritten. Use conditions in rewrite rules to avoid this. Non terminating rewrite systems lead
        to an overflow of the stack and a subsequent crash of lps2lts. 

      * There are sums over infinite domains in the linear process. E.g. having a summand sum x:Nat. a(x).X will cause the
        tool to enumerate all natural numbers first, filling up all the memory, not generating any transition.

      * The size of the state space is large as some process process parameters can attain an infinite or large amount of
        different values. An effective way to figure this out is to generate part of the transition system in a .fsm file.
        At the beginning of this file all process parameters and all the values that they got in some state are listed. 
         

[EGWW09] T.A.N. Engels, J.F. Groote, M.J. van Weerdenburg and T.A.C. Willemse. Search algorithms for automated validation. 
Journal of Logic and Algebraic Programming 78(4), 274-287, 2009. 

[GM14] J.F. Groote and M.R. Mousavi. Modeling and analysis of communicating systems. The MIT Press 2014.

