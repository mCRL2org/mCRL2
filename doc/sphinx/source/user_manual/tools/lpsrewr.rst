.. index:: lpsrewr

.. _tool-lpsrewr:

lpsrewr
=======

Rewrite the following data expressions of and LPS:

* conditions, action parameters, time expressions and next states of LPS summands
* process parameters of the initial state
* conditions and right-hand sides of data equations

Rewriting LPS summands and the initial state is done to simplify these parts of
the LPS. Rewriting data equations is done to speed up state space generation. In
most cases, this results in a performance gain of at most 5%.
