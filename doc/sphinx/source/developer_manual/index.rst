.. include:: toplevel.inc

.. title:: Home

.. image:: _static/mcrl2.png
   :align: center
   :class: frontpage-logo

.. _release: http://www.mcrl2.org/release/user_manual/download.html
.. _version: download.html

.. admonition:: Release

   This website is last updated on: |today|.

.. list-table::
   :class: frontpage-table
   :widths: 40,60

   * - .. image:: book.*
          :align: left
          :alt: Modeling and analysis of communicating systems (book)
          :target: http://mitpress.mit.edu/books/modeling-and-analysis-communicating-systems
     - mCRL2 is a formal specification language with an associated toolset. The
       toolset can be used for modelling, validation and verification of
       concurrent systems and protocols. It can be run on Windows, Linux, Apple
       Mac OS X and FreeBSD.

       The toolset supports a collection of tools for linearisation, simulation,
       state-space exploration and generation and tools to optimise and analyse
       specifications. Moreover, state spaces can be manipulated, visualised and
       analysed.

       The mCRL2 toolset is developed at the department of Mathematics and
       Computer Science of the `Technische Universiteit Eindhoven
       <http://www.tue.nl>`_, in collaboration with
       `CWI <http://www.cwi.nl>`_ and the `University
       of Twente <http://fmt.cs.utwente.nl>`_.

.. note::

   On Coursera four consecutive courses consisting of approximately 60 lectures provide an introduction into the theory and practical
   use of the mCRL2 toolset. It is based on the book 'Modeling and Analysis of Communicating
   Systems' shown above. The courses are:

   - `Automata and behavioural equivalences <https://www.coursera.org/learn/automata-system-validation>`_.
   - `Model process behaviour <https://www.coursera.org/learn/system-validation-behavior>`_.
   - `Requirements by modal formulas <https://www.coursera.org/learn/system-validation-modal-formulas>`_.
   - `Modelling software, protocols, and other behaviour <https://www.coursera.org/learn/system-validation-software-protocols>`_.

   The slides used to make these courses are available by contacting J.F.Groote@tue.nl. They can also
   be downloaded from `MIT Press <http://mitpress.mit.edu/books/modeling-and-analysis-communicating-systems>`_. 

.. note::

   mCRL2 received a prize for the distinguished user-assistance tool feature in the
   `VerifyThis verification challenge <http://etaps2015.verifythis.org/>`_
   held at `ETAPS2015 <http://www.etaps.org/>`_, in London. mCRL2 combined a rich specification
   language with excellent automatic reasoning
   features that made it an excellent tool to obtain
   insight in complex data structures and behaviours.

.. note::

   mCRL2 now can be compiled using `Zapcc 1.0.1 <https://www.zapcc.com/>`_, a fast C++ compiler
   based on clang-5.0. This speeds up the compilation with about 40%. To make it work add
   ``*/libraries/utilities/source/command_line_interface.cpp`` to the ``[DoNotZap]`` section in
   ``bin/zapccs.config``.

.. list-table::
   :class: teamcity-blurb
   :widths: 13,87

   * - .. image:: teamcity.png
          :width: 75
          :alt: TeamCity
          :target: https://www.jetbrains.com/teamcity/
     - The mCRL2 project uses `TeamCity <https://www.jetbrains.com/teamcity/>`_ for regression testing during
       development and building the daily snapshots.

.. toctree::
   :hidden:

   user
   boost
