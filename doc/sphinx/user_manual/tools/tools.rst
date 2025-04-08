.. _tool-documentation:

Tool documentation
==================

Below the main tools provided in the toolset are given. The common tools
are suitable for all main tasks to be carried out with the toolset. The experimental
tools are tools that are under development and provide additional but still experimental
functionality.

List of the common tools
------------------------

.. compound::
   :class: tool-docs

   .. toctree::
      :maxdepth: 1
      :glob:

      release/*


List of the experimental tools
------------------------------

.. compound::
   :class: tool-docs

   .. toctree::
      :maxdepth: 1
      :glob:

      experimental/*


In the source distribution there are more tools such as the deprecated and developer tools.

File formats
---------------------

.. highlight:: mcrl2 

This page lists all file formats supported by the mCRL2 toolset.

  ===========  =========   =======  ===========
  File format  Extension   Type     Description
  ===========  =========   =======  ===========
  mCRL2        .mcrl2      textual  :ref:`language-mcrl2`
  MCF          .mcf        textual  :ref:`language-mu-calculus`
  LPS          .lps        binary   :ref:`language-lps`
  PBES         .pbes       binary   :ref:`language-pbes`
  BES          .bes        binary   :ref:`language-bes`
  GM           .gm         textual  parity game in the PGSolver format
  LTS          .lts        binary   labelled transition system in the :ref:`language-mcrl2-lts`
  AUT          .aut        textual  labelled transition system in the :ref:`language-aut-lts`
  FSM          .fsm        textual  labelled transition system in the :ref:`language-fsm-lts`
  trace        .trc        binary   trace for simulation
  DOT          .dot        textual  `DOT file format <http://www.graphviz.org/doc/info/lang.html>`_ (subgraphs as nodes are not supported)
  ===========  =========   =======  ===========

.. toctree::
   :maxdepth: 1

   lts

.. _external_tools:

External tools
--------------------------

The tools given below are not part of the toolset, but are standalone tools that have mCRL2-related functionalities.

* A VSCode `extension <https://marketplace.visualstudio.com/items/?itemName=CptWesley.mcrl2>`_ similar to the `mcrl2ide`, allowing visualisation, simulation and verification.
* A tool for generating LaTeX from a mu-calculus formula in the mCRL2 syntax: `GitHub <https://github.com/TarVK/mCRL2-formatter>`_, `Web app <https://tarvk.github.io/mCRL2-formatter/demo/build/>`_.
