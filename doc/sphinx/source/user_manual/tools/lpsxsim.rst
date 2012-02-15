.. index:: lpsxsim

.. _tool-lpsxsim:

lpsxsim
=======

Simulate a linear process specification (LPS) using a graphical user interface.
The interface consists of two parts. In the top part every transition that is
possible from the current state, is listed including the changes to the state
vector that would result from executing that transition. A transition can be
executed by double clicking it. In the bottom part, the state vector of the
current state is shown.

Apart from basic simulation, :ref:`tool-lpsxsim` provides the following
additional functionalities:

* A trace can be loaded from a .trc file. Also, the action trace executed so far
  can be saved to a .trc file.
* Loaded traces and random traces can be played automatically.
* The action trace executed so far can be shown in a separate window.
* A plugin can be loaded dynamically. This plugin connects to the running
  :ref:`tool-lpsxsim` application and can respond to actions taken in the basic
  :ref:`tool-lpsxsim` interface. This functionality can be used by plugins that
  provide a visual representation of the system being simulated to keep that
  visual representation up-to-date with the current state in the basic
  :ref:`tool-lpsxsim` interface. Such plugins can give the user a better
  overview / understanding of what is going on in the system being simulated.
* Various options can be set.

