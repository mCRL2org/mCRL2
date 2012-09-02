Code generation
===============
A substantial part of the mCRL2 code is generated using scripts. This
section gives an overview of the scripts and their purpose.

* ``libraries/core/build/generate_classes.py``               Generates classes that derive from atermpp::aterm_appl. The class specifications that are used
                                                             as input are located in ``libraries/core/build/mcrl2_classes.py``.

* ``libraries/core/build/generate_parser_code.py``           Generates code for converting a parse tree into mCRL2 data types. This script takes the DParser grammars
                                                             in ``doc/specs`` as input, and generates code for traversing the parse tree. The output of the script
                                                             is by no means complete, but it can be used as a starting point.

* ``libraries/core/build/generate_template_overloads.py``    Generates overloads for several template functions (pp, find, normalize_sorts etc). These overloads
                                                             are added to header files to avoid the inclusion of heavy templated header files that are expensive
                                                             to compile. The implementations of the functions are added to files in the source directory, while
                                                             the declarations need to be added manually (since it is not always known where to put them).

* ``libraries/core/build/generate_term_functions.py``        Generates low level code for creating terms (soundness checks, constructors). The script requires
                                                             command line arguments (see the help function for details). The description of the internal format
                                                             in the file ``doc/specs/mcrl2.internal.txt`` is used as input for the script.

* ``libraries/core/build/generate_traverser_functions.py``   Generates functions that are implemented using the traverser framework (find/replace/rewrite etc).

* ``libraries/core/build/generate_traversers.py``            Generates the contents of traverser.h and builder.h files. The class specifications that are used
                                                             as input are located in ``libraries/core/build/mcrl2_classes.py``.

.. note::

   Most of the scripts give an overview of the files that have been updated, and the files that have stayed the same.