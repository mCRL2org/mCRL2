.. index:: chi2mcrl2

.. _tool-chi2mcrl2:

chi2mcrl2
=========

chi2mcrl2 translates a Chi specification to a matching mCRL2 specification. For
all the supported Chi operators individual translations to mCRL2 summands (with
their associated data parameters) are constructed.

Currently, only non-timed/non-hybrid Chi specifications can be translated. If a
specification is timed, e.g. the Chi statement ``delay``, the time progress is
translated to a ``skip``. If hybrid statements are used, e.g. the Chi Statement
``cont`` , the translation will fail to translate.

Supported Language Features
---------------------------

Like mCRL2, Chi also has a variety of tools for different translation and
simulation purposes. Due to this variety it is hard to keep track of all the
supported language features. We give an overview of the supported language
features by ``chi2mcrl2``, similar to the `Chi Tools page
<http://se.wtb.tue.nl/sewiki/chi/supported_language_features_of_each_tool_chain>`_
. Note that the presented link shows a "Chi to µCRL translation", which is the
predecessor of mCRL2.

In the tables below, a ``+`` means that the feature is supported and a ``-``
means that the language is not supported.

Variable Classes
^^^^^^^^^^^^^^^^

Each variable in a Chi program belongs to a variable class. Below are the
supported variable classes for chi2mcrl2.

  =================  =========
  Variable Class     chi2mcrl2
  =================  =========
  ``cont vardecl``   \-
  ``alg vardecl``    \-
  ``var vardecl``    \+
  ``alg parameter``  \-
  =================  =========
  
In this table ``vardecl`` means variable declaration, and ``parameter`` means
parameter declarations (in headers of models, processes)

Constant definitions
^^^^^^^^^^^^^^^^^^^^
Constant definitions are not supported by chi2mcrl2.

Imports
^^^^^^^
Imports are a general concept in Chi. Library files can be imported but also
other Chi specifications or even libraries written in other languages are
considered useful. However the import feature is not supported in
chi2mcrl2.

Enumerations
^^^^^^^^^^^^
Enumeration definitions are not supported by chi2mcrl2.

Functions
^^^^^^^^^
Enumeration definitions are not supported by chi2mcrl2.

Processes
^^^^^^^^^
The following language features are supported.

  =======================  =========
  Feature	                 chi2mcrl2
  =======================  =========
  time in guards	         \-
  folding	                 \-
  alternative composition	 \+
  parallel composition	   #1
  process instantiation	   #1
  recdef instantiation	   \-
  deadlock	               \+
  inconsistent	           \-
  delay predicate	         \-
  action predicate	       #2
  any delay	               \-
  signal emission	         \-
  delay operator	         #3
  jump enable	             \-
  local variable scope	   \-
  local channel scope	     \-
  local recdef scope	     \-
  recdef	                 \-
  urgent communication	   \-
  encapsulation	           \-
  guard	                   \+
  =======================  =========  

For the above table, the following convention applies:

* recdef means recursion definition
* #1: parallel composition is allowed only in a ``model`` definition
* #2: instead skips and multi-assignments are translated
* #3: ``delay`` is translated to a silent action

Data types
^^^^^^^^^^
In the table below are the supported data types for chi2mcrl2.

  =============  =========
  Data type	     chi2mcrl2
  =============  =========
  booleans	     \+
  naturals	     \+
  reals	         \-
  lists	         \+
  sets	         \+
  dictionaries	 \-
  vectors	       \-
  records	       \+
  distributions	 \-
  matrices	     \-
  =============  =========
