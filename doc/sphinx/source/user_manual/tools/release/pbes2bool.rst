.. index:: pbes2bool

.. _tool-pbes2bool:

pbes2bool
=========

Generation of a BES
-------------------

The tool uses the algorithm from :ref:`tool-pbes2bes` for generating a BES in
case the input file is a PBES.

.. note::

   Also see :ref:`tool-pbes2bes` for a description of the strategies that are
   referred to in this tool description.

Solving a BES
-------------

After the BES is generated, it can be solved. For this a backward approximation
technique is followed. First all instantiated BES variables of the highest
alternation depth are calculated. The variables are all simultaneously set to
``true`` or ``false`` depending on the fixpoint symbol at this depth. By a
normal fixpoint iteration, a stable solution is calculated. This process must be
symbolic, as variables at lower alternation depths can still occur in the
approximation. 

The results of this approximation are substituted in all equations of lower
alternation depth, just as in Gauss-elimination. In a sense this is the ordinary
Gauss elimination process, except that it treats all variables at the same
alternation depth at once. For instance when the alternation depth is 0, i.e.
there is no alternation, this approximation process converges in linear time.
For arbitrary nesting depths, this procedure is exponential.

Generation of counter examples
------------------------------

It is possible to let :ref:`tool-pbes2bool` generate a counter example. As it
stands, counterexamples are most useful when generated with strategy 2. A
counterexample is a tree with the initial instantiated pbes variable as its
root. The branches of each node are printed as lines with two extra spaces of
indentation. These branches are labelled with instantiated pbes variables of
which the value determined the value of our node. Typically a counter example
looks like::

   Solving the BES with 10 equations.
   The pbes is not valid
   Below the justification for this outcome is listed
   1: X(0)
     2: Subst:false X(2)
       4: Subst:false X(5)
     3: Subst:false X(9)

This says that when solving ``X(0)``, it could be determined that it 
had solution ``false``, because ``X(2)`` and ``X(9)`` where also ``false``.
The instantiated variable ``X(2)`` was ``false`` because ``X(5)`` was false.
Apparently, ``X(5)`` and ``X(9)`` where invalid by itself.

The phrase ``Subst:false`` indicates the reason why a substitution was made.
The following indications exist:

``Mu Cycle``

  the variable is set to false because it is part of a cycle in a class of
  variables that all have the same alternation depth, and are preceded by a mu
  (strategy 3).
  
``Nu Cycle``

  as in a Mu Cycle, except that the variable is set to true and the variables
  are preceded by nu (strategy 3).
  
``Subst:false``

  false is substituted using back or forward substitution (strategy 2).
   
``Subst:true``

  the value true is substituted using back or forward substitution (strategy 2).
  
``FSubst:false``

  the value false is substituted by forward substition (strategy 1).
  
``FSubst:true``

  the value true is substituted by backward substitution (strategy 1).
  
``Set:false``

  variable is set to false (currently not used).
  
``Set:true``

  variable is set to true (currently not used).
  
``Appr:false``

  false is substitued for the variable when solving it using approximation.
  
``Appr:true``

  true is substituted for the variable when solving it using approximation.
  
``Approxim``
  
  a value not equal to true or false is substituted when solving the BES using
  approximation.

Sometimes the counterexample is recursive, or has re-occuring parts. these parts
are only given once in the counterexample. A ``*`` at the end of a line in the
counterexample indicates that this instantiated variable did already occur
earlier in the counterexample and therefore, the reasons why it is true or false
are not printed again.

Known issues
------------

The counter example generated when the approximation algorithm of boolean
equations is being used is in general huge and not very helpful. This algorithm
is always employed with strategies 0 and 1. With strategies 2 and 3 it can be
that when generating the boolean equation system from a PBES, it is already
detected that the initial instantiated variable is either true or false and the
approximation algorithm is not necessary. Counter examples in this case are
compact (although we have no proof that the counter examples are always optimal)
and also very helpful.

