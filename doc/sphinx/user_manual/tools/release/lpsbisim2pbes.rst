.. index:: lpsbisim2pbes

.. _tool-lpsbisim2pbes:

lpsbisim2pbes
=============

Construct a PBES that expresses whether the two LPSs given as input are equal
modulo some behavioural equivalence relation specified with option `-b/--bisimulation`.
The two LPSs used to construct the PBES are equivalent if and only if the
solution to the PBES is true.

A precise definition of the construction of the PBES can be found in [CBPW07]_.

.. [CBPW07] T. Chen, B. Ploeger, J. van de Pol, and T. A. C. Willemse:
   Equivalence Checking for Infinite Systems using Parameterized Boolean
   Equation Systems, in CONCUR 2007, vol. 4703 of LNCS, pp. 120–135.

.. mcrl2_manual:: lpsbisim2pbes