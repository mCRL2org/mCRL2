.. _language-mcrl2:

mCRL2 specification
===================

.. highlight:: mcrl2

Every mCRL2 specification defines a *process*. The language elements that allow
you to specify processes rely on the use of *data*. These aspects are presented
in separate parts of the documentation. For the sake of completeness, a separate
section is included that describes the lexical elements of the mCRL2 language.

.. toctree::
   :maxdepth: 2

   lex
   data
   process

Specification syntax
--------------------

The mCRL2 specification format requires only that an ``init`` statement be 
present. This statement says which process the specification represents. It 
might be the case that a specification file contains definitions for multiple
processes; the ``init`` statement specifies which of these definitions (if any)
is used.

.. dparser:: mCRL2SpecElt Init mCRL2Spec

.. admonition:: Example

   Assume that ``delta`` is a valid process (it is: it is the process that 
   cannot do any action). A fairly minimal valid mCRL2 specification would be 
   the following::

     init delta;

.. admonition:: Example

   A slightly more elaborate specification that does not use data can be made
   by using constructs from the :doc:`process <process>` language::

     act a, b;
     proc P = a . P + b;
     init P;

   This specification defines the process that can do any number of ``a`` 
   actions, followed by a single ``b``. After this, it terminates.

   Note that the three lines in this specification could have been put in any
   order, and it would still specify the same process.
