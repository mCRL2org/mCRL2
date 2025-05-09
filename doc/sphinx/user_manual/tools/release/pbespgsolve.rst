.. index:: pbespgsolve

.. _tool-pbespgsolve:

pbespgsolve
===========

This tool is intended to solve parameterised boolean equation systems (PBES)
using a parity game solver. This is achieved by instantiating the PBES to a
boolean equation system. The resulting BES is equivalent to some parity games
such that a boolean equation is true if and only if the corresponding vertex of
the parity game is won by player even. A parity game solver can be used to
obtain the winning players for all vertices of that parity game. This also
determines the solution for the initial equation of the (P)BES. That solution
is then printed to standard output. The tool also accepts a BES or a parity
game in PGSolver format directly.

.. mcrl2_manual:: pbespgsolve
