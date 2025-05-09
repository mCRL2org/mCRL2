.. index:: pbesrewr

.. _tool-pbesrewr:

pbesrewr
========

This tool is can be used to apply one of several rewriters on a parameterised
Boolean equation system. 

The simplifying rewriter simplifies pbes expressions by applying the rewrite
rules for the data types, and by simplifying boolean expressions. It pushes
quantifiers inside as much as possible, removes quantifiers over variables that
are not used, and applies straightforward boolean simplifications. 

The quantifier-all rewriter expands all quantifiers. The pbes::

   map  f: Nat -> Bool;
   pbes nu Y = forall x: Nat. val(x < 3 => f(x));
   init Y;

is rewriten to::

   map  f: Nat -> Bool;
   pbes nu Y = val(f(0)) && val(f(1)) && val(f(2));
   init Y;

In case the rewriter is requested to expand a quantifier over an unbounded domain it
will not terminate. The quantifier-finite rewriter only expands variables over data
types that are guaranteed to be finite. If applied to the example above where 
the quantifier ranges over natural numbers, it does not touch it there are an
infinite number of natural numbers. 

The quantifier one point rewriter tries to eliminate quantifiers if it can determine that they
range over only one element. This works independently of the domain. Contrary to the quantifier-all
and quantifier-finite rewriters, the bound variable can be equal to a symbolic value::

   map  f:Nat->Bool;
        N:Nat;
   pbes nu Y = forall x:Nat.(val(x==N => f(x)));
   init Y;

rewrites to::

   map  f: Nat -> Bool;
        N: Nat;
   pbes nu Y = val(f(N));
   init Y;

The quantifier-one-point rewriter works on both universal and existential quantifiers. It uses
equalities (==) and inequallities (!=) to determine to which the bound variable is equal. 

The quantifiers-inside rewriter tries to push quantifiers as deeply as possible inside a formula.
This is sometimes useful before applying the quantifier-one-point rewriter. It is also useful
before applying a quantifier-all or quantifier-finite as it can avoid many copies of the same
expression the the expanded pbesses. 

The pfnf rewriter transforms the pbes to Predicate Formula Normal Form. In this form all
quantifiers are moved to the front of each pbes equation. The right hand side is transformed
into a big conjunction over disjunctions of pbes variables. 

Specifications of the rewriters can be found in the
`developers documenation <https://mcrl2.org/web/developer_manual/developer.html>`_ in the 
document about `PBES rewriters <https://mcrl2.org/web/_downloads/fd0ada340742729e33a70a3666632073/pbes-rewriters.pdf>`_.

.. mcrl2_manual:: pbesrewr
