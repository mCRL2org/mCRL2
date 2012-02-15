Lexical elements
================

.. highlight:: mcrl2

A number of reserved words is defined. These words include language keywords,
predefined operators and types, and predefined constants. The listing below 
shows all reserved words::

   act allow block comm cons delay div end eqn exists
   forall glob hide if in init lambda map mod mu nu pbes
   proc rename sort struct sum val var whr yaled  
   Bag Bool Int List Nat Pos Real Set
   delta false nil tau true

The following lexical elements (terminals in the grammar) are defined:

.. dparser:: Id Number whitespace

Comments
--------

The mCRL2 syntax allows single-line comments only, they are started with a
percent-character (``%``) and end at the end of a line (DOS, Linux and Mac style
line endings are supported)::

   proc X = a . X; % A simple recursive process

