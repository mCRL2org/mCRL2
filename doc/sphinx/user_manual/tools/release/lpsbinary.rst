.. index:: lpsbinary

.. highlight:: mcrl2

.. _tool-lpsbinary:

lpsbinary
=========

The tool ``lpsbinary`` takes a linear process and replace parameters with
a finite data domain by booleans. The advantage of this is that some tools
are much better in dealing with Booleans than with arbitrary data types. This
for instance the case when binary decision diagrams are used (as used by jittyp).

As an example consider the following linear process::

   sort D = struct d1 | d2 | d3;

   act  a: Nat # D # Bool;

   proc P(n_P: Nat, d_P: D, b_P: Bool) =
          sum e_P: D.
            a(n_P, d_P, b_P) .
            P(n_P = n_P + 1, d_P = e_P, b_P = !b_P)
        + delta;

   init P(0, d1, true);


When applying ``lpsbinary`` the sort ``D`` is replaced by two booleans. The sort Bool
as well as sorts of which it cannot be determined that they are finite are not changed.
The result is given by::

   sort D = struct d1 | d2 | d3;

   act  a: Nat # D # Bool;

   proc P(n_P: Nat, d_P1,d_P2,b_P: Bool) =
          sum e_P: D.
            a(n_P, if(d_P2, if(d_P1, d1, d1), if(d_P1, d2, d3)), b_P) .
            P(n_P = n_P + 1, d_P1 = e_P == d2, d_P2 = e_P == d1, b_P = !b_P)
        + delta;

   init P(0, d1 == d2, d1 == d1, true);

.. mcrl2_manual:: lpsbinary
