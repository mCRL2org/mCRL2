.. index:: lpsrewr

.. _tool-lpsrewr:

lpsrewr
=======

Rewrite the following data expressions of and LPS:

* Conditions, action parameters, time expressions and next states of LPS summands.
* Process parameters of the initial state.

There are two rewriters. The default simplifying rewriter simplifies the above mentioned
expressions by applying the rewrite rules in the specification together with the rewrite
rules for the built-in data types.

The quantifier-one-point rewriter eliminates variables that are bound in quantifiers
if the values of these variables can be determined from the expression. Two examples::

   exists x:Nat.(x==3 && f(x))      ==>        f(3)
   forall x:Nat.(x!=3 || f(x))      ==>        f(3)

The elimination of quantifiers is generally very helpful. In solving pbesses or generating state
spaces it helps to avoid enumerating these quantifiers. Symbolic manipulation often becomes much 
easier if quantifiers are removed. 

A specification of the one point rule rewriter can be found in the
`developers documentation <https://mcrl2.org/web/developer_manual/developer.html>`_ 
in the `PBES rewriters <https://mcrl2.org/web/_downloads/fd0ada340742729e33a70a3666632073/pbes-rewriters.pdf>`_ document.

.. mcrl2_manual:: lpsrewr
