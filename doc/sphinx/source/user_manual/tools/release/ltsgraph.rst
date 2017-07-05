.. index:: ltsgraph

.. _tool-ltsgraph:

ltsgraph
========

Draws a labelled transition system (LTS). A convential graph drawing is provided
either in 2D or 3D.

The main window of the tool shows the labelled transition system described by
the input file as a directed, labelled graph. The graph can be layed out
automatically by using the "Automatic layout" option from the "Tools" menu.
Alternatively, individual states can also be moved by dragging them with the
mouse. When clicking a state with the right mouse button, its position is fixed.

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

.. warning::

   This tool is only usable for small state spaces of up to about 200 states.
   For visualising larger state spaces it is recommended to use
   :ref:`tool-ltsview`.

