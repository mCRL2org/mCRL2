.. _language-bes:

Boolean Equation Systems
========================

Boolean equation systems (BESs) are a subset of PBESs in which the left hand
side does not bind data variables, and the predicate formulae that are allowed
on the right hand sides of the equations do not contain data variables,
have no quantifiers, and only refer to predicate variables without arguments.

BES expression
--------------

Expressions in a BES can take the following form.

.. dparser:: BesVar BesExpr

Mathematically, we also write the following.

.. math::

  f ::= true \mid false \mid X \mid f \land f \mid f \lor f

BES equation
------------

Boolean equations are defined according to the following syntax.

.. dparser:: FixedPointOperator BesEqnDecl BesEqnSpec

Mathematically, we also write :math:`(\mu X = f)` or :math:`(\nu X = f)`

BES specification
-----------------

A Boolean equation system is a sequence of Boolean equations, 

.. dparser:: BesInit BesSpec

The difference between PBESs and BESs is at the same conceptual level as the
difference between LPSs and LTSs: PBESs deal with data as first-class objects
whereas BESs do not recognise the concept of data. An on-the-fly translation of
PBESs into BESs is described in [DPW08]_, which shows an
exact correspondence between PBESs and BESs; in a sense, the translation can be
considered as an alternative semantics for PBESs containing countable data
types. BESs have been used for verifying properties of LTSs and are used
extensively in the tool suite CADP. For an excellent and very enlightning study
of their use for model checking, see [Mad97]_.

References
----------
.. [DPW08] A. van Dam, B. Ploeger and Tim A.C. Willemse. Instantiation for
           Parameterised Boolean Equation Systems, Eindhoven University of
           Technology, Department of Computer Science, CSR 08-11, 2008, 24 pp.
           `(PDF) <http://www.win.tue.nl/~timw/new/articles/CSR08-11.pdf>`_
           
.. [Mad97] Mader, A.H. (1997) Verification of Modal Properties Using Boolean
           Equation Systems. Edition versal 8, Bertz Verlag, Berlin.
           ISBN 3-929470-58-6.
           `(PDF) <http://eprints.eemcs.utwente.nl/1078/02/diss.pdf>`_
