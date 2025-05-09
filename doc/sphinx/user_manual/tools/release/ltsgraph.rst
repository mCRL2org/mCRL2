.. index:: ltsgraph

.. _tool-ltsgraph:

ltsgraph
========

This tool draws a labelled transition system (LTS). It can read graphs in .aut, .lts
and .fsm format. Graphs can be drawn in 2D and 3D format.

.. figure:: /_static/tools/ltsgraph.png
   :width: 400px
   :align: center
   :height: 200px
   :alt: The alternating bit protocol visualised in ltsgraph.
   :figclass: align-center

The main window of the tool shows the labelled transition system described by
the input file as a directed, labelled graph. The graph can be layed out
automatically by using the "Automatic layout" option from the "Tools" menu.
Alternatively, individual states can also be moved by dragging them with the
mouse. Clicking a state with the right mouse button will anchor it in place,
preventing it from being moved by the automatic layout. This can also be used
on handles and labels to perform a manual layout (partially).

The viewpoint can be moved by holding down the "Control" button and dragging with
the left mouse button. Zooming in or out can be with the scroll wheel or by
dragging up/down while pressing the middle mouse button. In 3D mode (activated
from the "View" menu), the graph can rotated by dragging with the right mouse
button or by holding "Shift" and dragging with the left mouse button.

Exploration mode (activated from the "Tools" menu) allows the user to investigate
a specific part of the graph. After exploration mode is turned on, only the
initial state and its successors are displayed. When clicking one of the
successors, it is expanded. It is also possible to collapse states for which
no successor has been expanded.

.. note::

   This normal mode ltsgraph is only usable for small state spaces of up to about 200 states.
   In exploration mode small parts of large graphs can be explored.
   For visualising larger state spaces it is also recommended to use
   :ref:`tool-ltsview` to visualise the structure of large graphs. The tool :ref:`tool-diagraphica`
   is especially designed to inspect the structure of the data occurring in large graphs.

.. mcrl2_manual:: ltsgraph
