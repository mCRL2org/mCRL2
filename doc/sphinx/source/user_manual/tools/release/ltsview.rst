.. index:: ltsview

.. _tool-ltsview:

ltsview
=======

Visualise a labelled transition system (LTS) using a 3D view.

The tool clusters states based on structural properties. These clusters are then
visualised to form a backbone of the LTS on which the states and transitions can
be drawn. The tool provides functionalities to explore the visualisation
(zooming, panning, rotating), mark deadlocks, mark states based on the values of
their state vector parameters and mark transitions based on their labels. An
image of the visualisation can also be saved in various bitmap formats. The
approach is described in detail in [HWW01]_.

.. note::

   In contrast to :ref:`tool-ltsgraph` this tool is able to handle large state
   spaces.

The graph can be rotated by dragging with the right mouse button or dragging
with the left mouse button while holding the shift key. Zooming in and out is
possible by using the mouse wheel or by dragging the mouse up/down while holding
the middle mouse button. The graph can be moved around by holding the "Control"
button and dragging with the mouse.

ltsview supports simulation of the system in a way similar to :ref:`tool-lpsxsim`.
Simulation can be started from the initial state by clicking "Start" in the 
simulation window (which can be turned on/off in the "View" menu). By double
clicking one of the next states in the simulation window, the simulation
progresses to that state.

A useful feature of ltsview is the possibility to find a trace to a state.
First, make sure that no simulation is currently running. Turn on the 
"Display states" option from the "View" menu and select a state by clicking
on it. Now start a simulation and click the button "Backtrace". A trace from
the initial state to the selected state will now be shown in the graph.
The "Undo" button allows you to step through that trace backwards.

References
----------

.. [HWW01] F. van Ham, H. van de Wetering and J.J. van Wijk. "Visualization of
           State Transition Graphs." In: Proc. IEEE Symp. Information
           Visualization 2001, IEEE CS Press, pp. 59-66, 2001.
           `(DOI) <http://dx.doi.org/10.1109/INFVIS.2001.963281>`_
           `(PDF) <http://www.research.ibm.com/visual/papers/FSM2002.pdf>`_
           
