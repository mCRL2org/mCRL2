mCRL2 library documentation
###########################

Atermpp
=========
     
.. toctree::
   :maxdepth: 1
   
   libraries/atermpp/aterm_library


*PDF documentation*

 * :mcrl2_pdflatex:`mCRL2 Term Library <libraries/atermpp/latex/term_library.tex>`

Core
======
     
.. toctree::
   :maxdepth: 1
   
   libraries/core/core_library
   libraries/core/common_functionality
   

*PDF documentation*

 * :mcrl2_pdflatex:`mCRL2 syntax definition <libraries/core/latex/mcrl2_syntax_definition.tex>`

Data
======
     
.. toctree::
   :maxdepth: 1
   
   libraries/data/data_library


*PDF documentation*

* :mcrl2_pdflatex:`Some basic notions concerning the mCRL2 data library <libraries/data/latex/data.tex>`
* :mcrl2_pdflatex:`Data types for mCRL2 <libraries/data/latex/mcrl2data.tex>`
* :mcrl2_pdflatex:`Enumerator <libraries/data/latex/enumerator.tex>`
* :mcrl2_pdflatex:`An algorithm to find a representant for sorts in the context of sort aliases and recursive sorts <libraries/data/latex/algorithm-for-sort-equivalence.tex>`
* :mcrl2_pdflatex:`substitutions.tex <libraries/data/latex/substitutions.tex>`
* :mcrl2_pdflatex:`Rewriter Implementation Notes <libraries/data/latex/rewriter.tex>`

GUI
=====

.. toctree::
   :maxdepth: 1
   
   libraries/gui/gui_library


*PDF documentation*

* :mcrl2_pdflatex:`Architecture of mcrl2ide <libraries/gui/latex/architecture_mcrl2ide.tex>`

LPS
======
     
.. toctree::
   :maxdepth: 1
   
   libraries/lps/lps_library


*PDF documentation*

* :mcrl2_pdflatex:`A rewriting-strategies-based tool for transforming process-algebraic equations <libraries/lps/latex/lin_impl.tex>`
* :mcrl2_pdflatex:`Next-state computation templates in state space exploration <libraries/lps/latex/next-state-templates-in-state-space-exploration.tex>`
* :mcrl2_pdflatex:`lpsconstelm <libraries/lps/latex/lpsconstelm.tex>`
* :mcrl2_pdflatex:`lps-implementation-notes.tex <libraries/lps/latex/lps-implementation-notes.tex>`
* :mcrl2_pdflatex:`mcrl2lps.tex <libraries/lps/latex/mcrl2lps.tex>`
* :mcrl2_pdflatex:`lpsparelm <libraries/lps/latex/lpsparelm.tex>`
* :mcrl2_pdflatex:`State Space Exploration <libraries/lps/latex/state-space-exploration.tex>`
* :mcrl2_pdflatex:`lps-implementation-communication-operator.tex <libraries/lps/latex/lps-implementation-communication-operator.tex>`
* :mcrl2_pdflatex:`Confluence Detection <libraries/lps/latex/confluence-detection.tex>`

LTS
=====
     
.. toctree::
   :maxdepth: 1
   
   libraries/lts/lts_library


*PDF documentation*

* :mcrl2_pdflatex:`Implementation of LTSGraph3D <libraries/lts/latex/ltsgraph3d_report.tex>`
* :mcrl2_pdflatex:`Notes on the bisimulation partitioner <libraries/lts/latex/bisimulation-partitioner-notes.tex>`


Modal formula
================
     
.. toctree::
   :maxdepth: 1
   
   libraries/modal_formula/modal_formulas_library

PBES
======
     
.. toctree::
   :maxdepth: 1
   
   libraries/pbes/pbes_library

*PDF documentation*

* :mcrl2_pdflatex:`PBES Abstraction <libraries/pbes/latex/pbes-abstraction.tex>`
* :mcrl2_pdflatex:`PBES Generation <libraries/pbes/latex/pbes-generation.tex>`
* :mcrl2_pdflatex:`PBES Implementation Notes <libraries/pbes/latex/pbes-implementation-notes.tex>`
* :mcrl2_pdflatex:`PBES rewriters <libraries/pbes/latex/pbes-rewriters.tex>`
* :mcrl2_pdflatex:`PBES Instantiation and Solving <libraries/pbes/latex/pbes-instantiation-solving.tex>`
* :mcrl2_pdflatex:`kant-pbes-greybox-notes.tex <libraries/pbes/latex/kant-pbes-greybox-notes.tex>`
* :mcrl2_pdflatex:`Some notes on a BES library <libraries/pbes/latex/bes-library.tex>`

Process
=========
     
.. toctree::
   :maxdepth: 1
   
   libraries/process/process_library


*PDF documentation*

* :mcrl2_pdflatex:`On free variables in process specifications, LPSs and PBESs <libraries/process/latex/global-variables.tex>`
* :mcrl2_pdflatex:`Process Library Implementation Notes <libraries/process/latex/process-implementation-notes.tex>`

SMT
======
     
.. toctree::
   :maxdepth: 1

   libraries/smt/smt_library

Utilities
===========
          
.. toctree::
   :maxdepth: 1

   libraries/utilities/utilities_library
   
Symbolic
===========
          
.. toctree::
   :maxdepth: 1

   libraries/symbolic/symbolic_library


*PDF documentation*

* :mcrl2_pdflatex:`Symbolic Reachability using LDDs <libraries/symbolic/latex/symbolic-reachability.tex>`

Source code reference
=====================

We generate the complete source code reference using Doxygen, which can be viewed `here <../doxygen/index.html>`_.
   
Code generation
===============
A substantial part of the mCRL2 code is generated using scripts. This section gives an overview of the scripts and their purpose.

* ``build/code_generation/generate_classes.py``               Generates classes that derive from atermpp::aterm_appl. The class specifications that are used as input are located in ``build/code_generation/mcrl2_classes.py``.

* ``build/code_generation/generate_data_types.py``            Generates code for standard data types like bag, bool, int, etc.

* ``build/code_generation/generate_template_overloads.py``    Generates overloads for several template functions (pp, find, normalize_sorts, etc.). These overloads are added to header files to avoid the inclusion of heavy templated header files that are expensive to compile. The implementations of the functions are added to files in the source directory, while the declarations need to be added manually (since it is not always known where to put them).

* ``build/code_generation/generate_term_functions.py``        Generates low level code for creating terms (soundness checks, constructors). The script requires command line arguments (see the help function for details). The description of the internal format in the file ``doc/specs/mcrl2.internal.txt`` is used as input for the script.

* ``build/code_generation/generate_traverser_functions.py``   Generates functions that are implemented using the traverser framework (find/replace/rewrite etc).

* ``build/code_generation/generate_traversers.py``            Generates the contents of traverser.h and builder.h files. The class specifications that are used as input are located in ``build/code_generation/mcrl2_classes.py``.

.. note::

   Most of the scripts print a report of the files that have been updated, and the files that have stayed the same.
