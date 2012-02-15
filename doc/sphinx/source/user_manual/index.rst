.. include:: toplevel.inc

.. title:: Home

.. image:: _static/mcrl2.png
   :align: center
   :class: frontpage-logo

.. _release: http://www.mcrl2.org/release/user_manual/download.html
.. _version: download.html

.. ifconfig:: release == version

   .. admonition:: Release

      Latest mCRL2 as of |today| release: |release|_.
      
   .. admonition:: Release
   
      Latest `development version documentation <http://www.mcrl2.org/dev>`_ is also available.
  
.. ifconfig:: release != version

   .. warning::
   
      You are looking at the documentation for the development version of mCRL2.
      If you are looking for the release version, check out the `homepage <http://www.mcrl2.org>`_!
   
   .. admonition:: Release
      
      Latest SVN version as of |today|: |version|_.
      
   .. admonition:: Release
   
      For the documentation of the latest release: |release|_ check out `mcrl2.org <http://www.mcrl2.org>`_.

.. list-table::
   :class: frontpage-table
   :widths: 40,60

   * - .. image:: statespace.*
          :align: left
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
       <http://www.tue.nl>`_, in collaboration with `LaQuSo
       <http://www.laquso.com>`_, `CWI <http://www.cwi.nl>`_ and the `University
       of Twente <http://fmt.cs.utwente.nl>`_. 

.. toctree::
   :hidden:

   user
