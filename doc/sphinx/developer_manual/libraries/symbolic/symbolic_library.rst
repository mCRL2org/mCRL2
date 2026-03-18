Symbolic library
================

The symbolic library contains the implementation of LDD helper functions and
classes used by the symbolic instantiation and solving algorithms. The symbolic
instantiation and solving algorithms themselves are located in the
:doc:`PBES library <../pbes/pbes_library>` and
:doc:`LPS library <../lps/lps_library>` respectively since these depend on the
PBES and LPS data
structures respectively. However, since these are all closely related we put the
documentation together here.


List Decision Diagrams
----------------------

See the document below for a pseudocode of algorithms for LDDs.

*PDF documentation*

* :mcrl2_pdflatex:`Notes on list decision diagrams <latex/list_decision_diagrams.tex>`

.. include:: zielonka.rst
