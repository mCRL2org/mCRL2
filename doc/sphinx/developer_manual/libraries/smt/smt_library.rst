SMT Interface library
=====================

This library can translate mCRL2 data expressions to SMTLIB2 format and send
them to the Z3 SMT solver. An example of its use can be found in the folder
libraries/smt/example.

The library still has a few limitations, among which:

- Overloaded constructors are not supported (this is a limitation in SMTLIB2).
- Higher-order expressions are not supported (also a limitation of SMTLIB2).
- The library does not deal perfectly with the :code:`Pos` and :code:`Nat`
  sorts. They are both translated to the native :code:`Int` type in SMTLIB2,
  combined with an assertion that limits their domain. However, the assertions
  are not complete. For example, for a function :code:`f: Pos -> Pos`, the
  following assertion is missing:

  .. code::

    (forall ((p Int)) (>= (f p) 1)).
