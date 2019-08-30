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
       the `University of Twente <http://fmt.cs.utwente.nl>`_.

.. note::

  The 201908.0 release of mCRL2 is now available. Packages for various platforms can be
  found on the :ref:`download<download>` page. In this release, the ATerm library, which is
  the main internal data storage engine of mCRL2, has been re-implemented completely.
  Furthermore, the mCRL2ide has seen many improvements, including the ability to check
  whether two processes are equal under a given behavioural equivalence.

.. note::
  .. image:: tacas-ae.png
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

   .. image:: Verum-2-Logo-Vert-Colour.jpg
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
