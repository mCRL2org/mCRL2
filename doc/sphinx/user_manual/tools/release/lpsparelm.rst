.. index:: lpsparelm

.. highlight:: mcrl2

.. _tool-lpsparelm:

lpsparelm
=========

The tool :ref:`tool-lpsparelm` tries to find those parameters in a linear process
that do not influence the behaviour and removes those. A parameter in a linear process
does not influence the behaviour if it does not occur in an action or in a condition,
either directly, or indirectly via other parameters. Removing parameters is very
useful as it generally substantially reduces the state space, sometimes even from
infinite to finite.

In the following example the parameter ``n`` does not influence the behaviour of the
process::

  act  a;
  proc P(n: Nat) =
         a.P(n+1);
  init P(0);

After applying :ref:`tool-lpsparelm` the linear process looks as follows::

  act  a;
  proc P=a.P;
  init P;

The state space has been reduced from an infinite one to a single state.

In the following example :ref:`tool-lpsparelm` cannot remove any parameter::

  act a:Nat;
  proc P(m,n,p:Nat)=(n<10) -> a(m).P(n+p,m+1,p+1);
  init P(0,0,0);

The parameter ``m`` occurs in an action, ``n`` occurs in a condition and ``p`` indirectly
influences the value of `n`.

It can be useful to remove parameters from actions (use :ref:`tool-lpsactionrename` for this purpose)
as it may allow :ref:`tool-lpsparelm` to remove more parameters. If in the previous example ``a(m)``
is renamed to ``a``, both parameters ``n`` and ``p`` can be removed.

Sometimes parameters of a process are of a complex datatype using constructors. E.g., a parameter can be of type
list, but only the first element in the list influences the behaviour. Then the list can be unfolded in its
first element and the rest using the tool :ref:`tool-lpsparunfold`. Using :ref:`tool-lpsparelm` the resulting
parameter modelling the rest of the list can be removed. A typical application of the combination of these
two tools are for board games, where the board is represented as a list of lists. Unfolding the board completely
and removing redundant parameters can speed up the generation of the state space. Another typical application
is the elimination of data from communication protocols. By unfolding the message frames in the protocol into
their individual components, by preventing data to be delivered, :ref:`tool-lpsparelm` can remove all the data
from the protocol, allowing to investigate its full behaviour, independently of the nature of the data.

.. mcrl2_manual:: lpsparelm
