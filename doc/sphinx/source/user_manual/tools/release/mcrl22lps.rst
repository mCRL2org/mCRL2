.. index:: mcrl22lps

.. _tool-mcrl22lps:

mcrl22lps
=========

This translates an mCRL2 process specification to a linear process specification
(LPS).

The input process specification should adhere to its syntax description (see
also :ref:`language-mcrl2` for more information).

The process in the input must be in parallel pCRL format. This means that it
must consist of the parallel composition of a set of processes that are
described using actions, conditions, sum operators, timing, sequential and
alternative composition operators only. The parallel, communication and renaming
operators can only be used on the outer level. Typical input examples are found
in the directory examples of the mCRL2 repository.

For certain inputs, :ref:`tool-mcrl22lps` can take a large amount of time. In
some cases it will not terminate, for instance with a non terminating rewrite
rules. The following options can be used to speed up the linearisation, at the
expense of a less readable result.

The option :option:`--lin-method=stack` avoids non termination if the process
structure is not regular. The option :option:`--no-rewrite` causes rewriting to
be switched off, avoiding non termination due to non terminating rewrite rules.
The option :option:`--no-sumelm` avoids the use of sum elimination, which under
certain circumstances may take a lot of time. Finally, the option
:option:`--no-deltaelm` avoids removal of spurious delta summands, which uses a
quadratic algorithm in the number of summands, and therefore, may be very time
consuming.

The option :option:`--timed` assumes that the timing of the input is strict. By
default it is assumed that time can always progress in the sense that each
process ``p`` is interpreted as ``p+delta``, which is incorrect as it does not
preserve time. For instance the process ``a@1`` is interpreted as ``a@1+delta``,
meaning that the ``a`` does not have to happen at time ``1``. If time is
important, the option :option:`--timed` is required. However, this may lead to
substantially increased linearisation times and an increased size of the linear
process.

Linearisation methods
---------------------

The tool supports three different linearisation methods, configurable using
the option:`-l/--lin-method`:

``regular``

  The non parallel processes are translated to restricted Greibach Normal Form.
  Instead of using a stack, these processes are translated to an LPS with finite
  control variables. If some process has an infinite number of control states,
  the tool will attempt to generate all of them, causing it to run out of
  memory. In such a case the stack method can be used to produce a linear
  process. The regular method is almost the same as ``regular2``. The only
  difference is in the way new process variables are generated: ``regular``
  generates less parameters in the linear process than ``regular2``, but
  ``regular2`` generates an LPS in a few cases where the use of regular leads to
  non-termination. The difference between the two methods is explained best by
  an example.
  
  The tool sometimes has to replace a sequence of process variables
  by a single new variable. For instance, ``P1(f(x)) · P2(g(x))`` must be
  replaced by a new process ``P``. With the ``regular`` method, the new process
  has a single parameter ``x``, matching the single free variable ``x``. I.e.
  the definition of ``P`` is ``P(x) = P1(f(x)) · P2(g(x))``. With the
  ``regular2`` method a variable is introduced for every term. In this latter
  case ``P`` is defined by ``P(y,z) = P1(y) · P2(z)`` and the expression
  ``P1(f(x)) · P2(g(x))`` is replaced by ``P(f(x),g(x))``.

  We give an example in which linearisation with ``regular`` fails to terminate
  and the use of ``regular2`` succeeds. Consider the process definition
  ``P(n:Nat) = a·P(n+1) + b`` and let the initial process be ``P(0)·delta``. Now
  with regular infinitely many new processes are generated, each of the
  following form: ``P(0)·delta, P(1)·delta, etc. With ``regular2`` only one new
  process is generated, namely one for ``P(n)·delta``.

``regular2``

  See the explanation of regular. Note that with this option the number of
  parameters can be very large, and consequently, the number of global variables
  can be huge too. In this case, linearising using the option
  :option:`-f/--no-globvars` is advisable.
  
``stack``
  
  The LPS is generated using stack data types. The non parallel processes are
  transformed to restricted Greibach Normal Form and straightforwardly
  translated to linear processes using a stack. The resulting linear processes
  are then put in parallel. This works for any allowed input. Unfortunately, the
  linear process that is the result of this operation can basically only be used
  for state space generation. Symbolic operations on the stacks are generally
  not very effective, because the stack data type is too complex. For symbolic
  analysis, linearisation methods ``regular`` or ``regular2`` can be used.
  

