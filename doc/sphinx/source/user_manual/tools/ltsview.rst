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
   
References
----------

.. [HWW01] F. van Ham, H. van de Wetering and J.J. van Wijk. "Visualization of
           State Transition Graphs." In: Proc. IEEE Symp. Information
           Visualization 2001, IEEE CS Press, pp. 59-66, 2001.
           `(DOI) <http://dx.doi.org/10.1109/INFVIS.2001.963281>`_
           `(PDF) <http://www.research.ibm.com/visual/papers/FSM2002.pdf>`_
           
