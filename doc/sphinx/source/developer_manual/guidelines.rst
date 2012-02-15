Development guidelines
======================

File hierarchy
--------------
The following describes how libraries and tools should be organised in a file hierarchy.

Libraries
^^^^^^^^^

Organisation of libraries:

* A library named *foo* is located in the directory *libraries/foo*.
* Each library directory has the following directory structure:

  ============= =========================================================
  Sub-directory	Contents
  ============= =========================================================
  build	        Library build files such as a Jamfile
  doc	          Documentation (HTML) files
  example	      Sample program files
  include	      Header files
  src	          Source files which must be compiled to build the library
  test	        Regression or other test programs or scripts
  ============= =========================================================

* Include files belonging to library *foo* must be placed in the directory
  *include/mcrl2/foo*. Optionally a header file *include/mcrl2/foo/foo.h*
  may be added, that contains include directives for the most commonly used
  header files in *include/mcrl2/foo*.
* Library code is put in the namespace *mcrl2*. If needed, a library *foo*
  can use its own namespace mcrl2::foo.

Tools
^^^^^
Organisation of tools:

* A tool named *foo* is located in the directory *tools/foo*.
* Each tool directory has the following directory structure:

  ============= =========================================================
  Sub-directory	Contents
  ============= =========================================================
  doc           Documentation (HTML) files
  test          Regression or other test programs or scripts
  ============= =========================================================  

* Build files, source files and include files are located in the tool
  directory itself.
* Reusable code must be put in a library, not in a tool!

.. note::

   Some of these guidelines were taken from `<http://www.boost.org/more/lib_guide.htm>`_.

Programming
-----------
Except for third party software, every header and source file should adhere to
the following guidelines.

Preamble
^^^^^^^^
Every file should start with a preamble containing author names and links to
copying and license information. The preamble should be formatted as
follows, where `AUTHORS` stands for the names of the authors:

.. code-block:: c++

  // Author(s): AUTHORS
  //
  // Copyright: see the accompanying file COPYING or copy at
  // https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
  //
  // Distributed under the Boost Software License, Version 1.0.
  // (See accompanying file LICENSE_1_0.txt or copy at
  // http://www.boost.org/LICENSE_1_0.txt)

Naming conventions
^^^^^^^^^^^^^^^^^^
The following naming conventions regarding filenames should be used:

* Filenames should be all lowercase.
* The following file extensions should be used:
  * .cpp for source files;
  * .h for header files.

The following naming conventions regarding file contents should be used:

* Use the naming conventions of the C++ Standard Library.
* Names (except as noted below) should be all lowercase, with words separated by
  underscores.
* Template parameter names begin with an uppercase letter.
* Macro names all uppercase and begin with ``MCRL2_``.
* Choose meaningful names - explicit is better than implicit, and readability
  counts. There is a strong preference for clear and descriptive names, even if
  lengthy.

Header policy
^^^^^^^^^^^^^
The following guidelines are specific to header files
(based on `<http://www.boost.org/more/header.htm>`_):

* Unless multiple inclusion is intended, wrap the header in #ifndef guards.
* Wrap the header contents in a namespace to prevent global namespace pollution.
  mCRL2 libraries should be placed in the namespace mcrl2.
* Make sure that a translation unit consisting of just the contents of the
  header file will compile successfully. In other words, a source file with
  content `#include "foo.h"` should compile for every header file
  `foo.h`.
* Place the header file in a sub-directory to prevent conflict with identically
  named header files in other libraries.
* To avoid namespace pollution, the use of the `using` and `using
  namespace` directives is not allowed at the global parts of the header.

The following is a sample header:

.. code-block:: c++

  // Author(s): AUTHORS
  //
  // Copyright: see the accompanying file COPYING or copy at
  // https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
  //
  // Distributed under the Boost Software License, Version 1.0.
  // (See accompanying file LICENSE_1_0.txt or copy at
  // http://www.boost.org/LICENSE_1_0.txt)
  //
  /// \file FILE_NAME
  /// \brief BRIEF_DESCRIPTION
   
  #ifndef MCRL2_MY_CLASS_H
  #define MCRL2_MY_CLASS_H
   
  namespace mcrl2 {
    
    class my_class
    {
      public: 
        void f();
      private:
        int x;
    };
   
  } // namespace
   
  #endif  // MCRL2_MY_CLASS_H

Exception handling
^^^^^^^^^^^^^^^^^^
Use exceptions to report *errors* where appropriate, and write code that is
*safe* in the face of exceptions.

Standards compliance
^^^^^^^^^^^^^^^^^^^^
Aim for *ISO Standard C++*. That means making effective use of the standard
features of the language, and avoiding non-standard compiler extensions. It also
means using the *C++ Standard Library* where applicable.

Regression tests
^^^^^^^^^^^^^^^^
Provide a *regression test* program or programs.

Usability
^^^^^^^^^
Provide sample programs or confidence tests so potential users can see
how to use your library.

Platform independence
^^^^^^^^^^^^^^^^^^^^^
The source code must compile on the actively supported platforms
and supported build tools on those platforms.

Committing changes
------------------
When committing changes, the following guidelines should be adhered to:

* Make sure the updated code successfully compiles, installs, and passes all
  tests.
* Enter a clear commit message.
* Whenever a commit solves a Trac ticket, the commit message must refer to the
  ticket by its number, formatted as `fixes #n`, where `n` represents the ticket
  number. This automatically closes the ticket with a reference to the commit
  number and message.
    

Documentation guidelines
========================
This page describes the documentation guidelines for tools and libraries.

Tools
-----
Tool documentation should consists of help information and a user manual.
The help information is described in the tool interface guidelines, the
user manual is described here.

User manual
^^^^^^^^^^^
The tool use manual is automatically generated from the help description of the
tool, provided that the tool uses the :ref:`tool_classes`. We therefore
require that all tools use the tool classes.

Libraries
---------
Library documentation should consist of a user manual and reference manual.
Both should be available on the library documentation page.

User manual
^^^^^^^^^^^
The library user manual provides an informal explanation of the library and its
use. It should consist of the following sections (in that order):

* Introduction
* Concepts
* Structure
* Tutorial
* References (if any)
* Acknowledgements (if needed)
* Authors: the line ``Written by AUTHORS``.
  where ``AUTHORS`` stands for the names of the authors.
* Reporting bugs:
  ``Report bugs at our issue tracking system <http://www.mcrl2.org/issuetracker>``.

Reference manual
^^^^^^^^^^^^^^^^
The library reference manual provides a technical specification of the
library. It should consist of an explanation of all used elements in the public
interface. This manual should be automatically generated from the library code
using `Doxygen <http://doxygen.org>`_ comments. The current convention is that only
documented code will be displayed, unless the
`\internal` command is included in the Doxygen comment. Library code located
in a `detail` sub-directory is considered to be no part of the public
interface, and no documentation is generated for this code.

Doxygen documentation should specified as follows:

* For each directory and file in the public interface,
  `structural commands <http://www.stack.nl/~dimitri/doxygen/docblocks.html#structuralcommands>`_
  `\\dir <http://www.stack.nl/~dimitri/doxygen/commands.html#cmddir>`_ and
  `\\file <http://www.stack.nl/~dimitri/doxygen/commands.html#cmdfile>`_
  should be used.
* For each file member in the public interface, a
  `special documentation block <http://www.stack.nl/~dimitri/doxygen/docblocks.html#specialblock>`_
  should be put directly above the file member. File members include, but are
  not limited to, namespaces, classes, structs, unions, enums, functions,
  variables, defines and typedefs.

In the above specified documentation, the following
`Doxygen special commands <http://www.stack.nl/~dimitri/doxygen/commands.html>`_
should be used:

* `\\brief <http://www.stack.nl/~dimitri/doxygen/commands.html#cmdbrief>`_:
  a one-line description for each directory, file, and file member in the public
  interface. 
* `\\param <http://www.stack.nl/~dimitri/doxygen/commands.html#cmdparam>`_:
  a parameter description for each  parameter of a public function.
* `\\return <http://www.stack.nl/~dimitri/doxygen/commands.html#cmdreturn>`_:
  a return value description for each non-void public function.

General
-------
The following documentation guidelines apply to both tools and libraries.

Acknowledgements
^^^^^^^^^^^^^^^^
All tools and libraries should acknowledge authors of used or "inspirational"
code. This does include but is certainly not limited to fulfilling license
requirements. Acknowledgements are required in the following cases:

* The use of code - altered or not - of third parties.
* The use of algorithms, data structures, architectures etc. designed by third parties.
* The use of third-party code, tools, algorithms etc. as an inspirational basis.

However, things regarded as common knowledge are excluded from the latter two
requirements.

Besides these requirements, authors of the mCRL2 toolset are highly encouraged
to add references to related work (similar to adding related work in a paper).

Some examples to illustrate:

* Copying a snippet of code ad verbatim from outside the mCRL2 toolset requires
  acknowledgements.
* Modifying such a piece of code does not change this.
* Implementing `quicksort <http://en.wikipedia.org/wiki/Quicksort>` does not
  require acknowledgements as it can be considered common knowledge. This does
  ''not'' include using a third-party implementation of quicksort;
  acknowledgements to the author of that implementation are required.
* Implementation of an algorithm found in an article should refer to that article.
* When implementing a tool with a similar third-party tool in mind, one should
  refer to that third-party tool as source of inspiration.

We distinguish three types of acknowledgements:

#. full: This means that all relevant information should be given. That is,
        authors, articles, original source location and a description of the
        precise use of the code etc.
#. summary: A summary of the information of ''full''. This should include the
           authors, article name and functionality description.
#. short summary: Even shorter variant of the ''summary''. This should only
                 include authors or article name and the functionality. 

Some examples to illustrate:

* full: "The following function is based on the algorithm in A.U. Thor,
        "Algorithms for Dummies", Publish Inc., 1975. I have changed the data
        structure a bit such that lookup is now O(log(n)) instead of O(n)."
* summary: "Shribbing functionality based on A.U. Thor, "Algorithms for Dummies"."
* short summary: "Algorithms for Dummies (shribbing)"

Where to acknowledge and to what extent:

* Within a source file full acknowledgements should be added at relevant
  places. The only exception is a source file that as a whole is taken ad
  verbatim and already includes the relevant information (such as a license).
* Library documentation of functions should contain or refer to summary
  acknowledgements (but not to those within the source code itself). It is
  possible to group acknowledgements and put them all in one place as long as it
  is clear what the acknowledgements refer to.
* A library's user manual should contain summary acknowledgements.
* A tool's manual page should contain summary acknowledgements.
* When a tool's core functionality is part of a library and this part has
  acknowledgements, the tool itself should also acknowledge. This may be in a
  short form with a reference to the full(er) acknowledgement in the
  library. (For instance, when library X has function F with certain
  acknowledgements and one of the main tasks of a tool is to read a file apply
  F and return the result, then the tool should include the acknowledgements
  for F.)
  
Release guidelines
==================
* An mCRL2 toolset release takes place every 6 months.
* The precise next release date is determined well in advance by the mCRL2 developers.
* During a period of 15 days prior to the release date, only bug-fixing changes
  to the development source tree are allowed. Feature additions or large,
  structural changes are not allowed. Also, every bug fix should be checked by
  at least one person other than its author, prior to its submission into the
  source tree.
* The version number of a release consists of the year and month of
  the release date. For example: 2008.01.
* The release itself consists of at least (but not limited to) one binary
  Windows package and one source code package. These are made available for
  download on the mCRL2 website.
