.. index:: lpssim

.. _tool-lpssim:

lpssim
======

The lpssim tool simulates a linear process specification (LPS) using a
command-line interface.

In the first screen, lpssim shows the state vector of the initial state and a
list of all possible transitions that can be taken from this state. These
transitions are numbered from 0 onwards. For each transition, the state vector
of the state that is reached after taking that transition, is shown. When a
transition has been taken, lpssim shows the action label of that transition, the
state vector of the current state and (again) a list of numbered transitions
that can be taken from the current state along with the state vector of the
resulting state, respectively.

On the lpssim command-line - which starts with a question mark ``?`` - the
following commands can be entered:

``n``

  Execute the action with number n from the list of actions that are possible in
  the current state.
  
``u, undo``

  Go to previous state in the trace.
  
``r, redo``

  Go to next state in the trace.
  
``i, initial``

  Go to initial state. Note that this preserves the trace; it is equivalent to
  ``goto 0``.
  
``g n, goto n``

  Go to position n of the trace.
  
``t, trace``

  Print the current trace. The current position in the trace is indicated with
  ``>``.
  
``l filename, load filename``

  Load a trace from filename.
  
``s filename, save filename``

  Save the current trace to filename.
  
``h, help``

  Display a help message explaining the available commands.
  
``q, quit, Ctrl-D``

  Quit :ref:`tool-lpssim`.


