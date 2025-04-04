.. index:: lpsconstelm

.. highlight:: mcrl2

.. _tool-lpsconstelm:

lpsconstelm
===========

This tool removes constant process parameters from the LPS. If it can be determined that
certain parameters of this LPS remain constant throughout any run of the
process, all occurrences of these process parameter are replaced by the initial
value and the process parameters are removed from the LPS. After substitution
expressions are simplified using a rewriter. Note that summands of which the
conditions are false are only removed with the ``--remove-trivial-summands`` flag.

If the initial value of a process parameter is a global variable and remains a
global variable throughout the run of the process, the process variable is
considered constant.

If the initial value of a process parameter is a global variable and is only
changed once to a certain value, the process parameter is constant and the
specific value is used for substitution.

A typical example of lpsconstelm is the following. Consider the linear process::

   act a:Nat;
   proc P(m,n:Nat)=
           a(m).P(m,n+1) +
           (m>0) -> a(m).P(m+1,n);
   init P(0,0);

It is determined that ``m`` can only have the value ``0``, and the second summand
can never take place. The result is::

   act  a: Nat;
   proc P(n: Nat) =
          a(0) .
            P(n+1);
   init P(0);

Note that ``lpsconstelm`` is very useful in simplifying linear processes. Its application
does not reduce the size of generated state spaces. But its application can enable other
tools, such as :ref:`tool-lpsparelm` to become more effective.

In some cases lpsconstelm can
reduce the number of summands quite dramatically. For instance when dealing with
similar communicating processes, such as in::

   proc P(id:Nat,...)= ....;
   init allow({...},comm({...},P(0,...)||P(1,...)||...||P(10,...)));

the variables ``id`` for each processes are replaced by the concrete ids.
When processes send messages to other processes indexed by numbers (e.g., ``send(message,sender_id,receiver_id)``)
then summands with communication that are not possible due to mismatching id's are removed from the lps
by ``lpsconstelm``. For a typical example such as Milner's scheduler, this reduces the number of summands
from quadratic to linear in the number of participating processes.


..  mcrl2_manual:: lpsconstelm
