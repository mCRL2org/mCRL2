.. index:: pbessolvesymbolic

.. _tool-pbessolvesymbolic:

pbessolvesymbolic
====================

This is a solving tool for parameterised Boolean equation systems (.pbes
extension) that is based on a parity game exploration technique that utilises
symbolic representations. The symbolic exploration works very similar to the one
implemented by :ref:`tool-lpsreach`, where it is described in great detail. The
main difference is that the underlying PBES is first transformed into a standard
recursive format (SRF), which is in some sense very similar to a linear process.

Next, we describe useful options that are exclusive to `pbessolvesymbolic`. One
option that can be useful to further refine the dependencies of transition
groups is `--split-conditions`, which introduces new transition groups based on
the structure of the SRF pbes. Generally, option `1` is safe, but options `2`
and `3` can yield an infinite PBES due to the conditions becoming weaker.

Similarly to :ref:`tool-pbessolve` the `pbessolvesymbolic` tool also contains
various partial solving strategies that attempt to optimistically solve the
intermediate parity games to present the solution (and terminate) early. These
can be enabled with the `--solve-strategy` option.

Limitations
-----------

Currently, `pbessolvesymbolic` can not provide counter examples when the property
does not hold, and solving PBESs with counter example information is extremely
slow.