.. include:: ../_includes/toplevel.inc

.. title:: Home

.. image:: /_templates/mcrl2/static/mcrl2.png
   :align: center
   :class: frontpage-logo

.. _release: https://www.mcrl2.org/release/user_manual/download.html
.. _version: download.html

.. admonition:: Release

   This website is last updated on: |today|.

.. list-table::
   :class: frontpage-table
   :widths: 40,60

   * - .. image:: /_static/book.*
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
       the `University of Twente <http://fmt.cs.utwente.nl>`_.

.. note::

   The latest release, mCRL2 202106.0, is now available from the download page.
   This version sees a number of UI improvements and the addition of 
   coupled simulation that can now be checked using the ltscompare tool. 
   Furthermore, this release contains a number of performance improvements 
   and bug fixes. 
   See the `changelog <https://github.com/mCRL2org/mCRL2/blob/master/CHANGES>`_
   for a detailed list of changes.

.. note::
  .. image:: /_static/tacas-ae.*
          :align: right
          :width: 90px
          :alt: TACAS artefact evaluation badge

  A tool paper explaining the latest developments in mCRL2 was published in TACAS2019.
  The new features include counterexamples, probabilistic processes and efficient algorithms for several types of behavioural relations.
  As part of the review process, mCRL2 was tested and accepted by the TACAS artefact evaluation committee.
  The paper is `available <https://doi.org/10.1007/978-3-030-17465-1_2>`_ under open access.

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
   .. image:: /_static/Verum-2-Logo-Vert-Colour.*
          :align: right
          :width: 75px
          :alt: Verum is the industry leader in reliable software design.
          :target: https://www.verum.com

   mCRL2 is used by the company `Verum <https://www.verum.com>`_ as their verification engine.
   Verum provides model based software development environments for the languages ASD and Dezyne that
   allow to program proven correct embedded software with much less effort than "classical" programming.
   As it stands Verum is the industry leader in reliable software design.

.. list-table::
   :class: teamcity-blurb
   :widths: 13,87

   * - .. image:: /_static/teamcity.*
          :width: 75
          :alt: TeamCity
          :target: https://www.jetbrains.com/teamcity/
     - The mCRL2 project uses `TeamCity <https://www.jetbrains.com/teamcity/>`_ for regression testing during
       development and building the daily snapshots.

.. toctree::
   :hidden:

   user
   build_instructions/boost
   ../developer_manual/index
