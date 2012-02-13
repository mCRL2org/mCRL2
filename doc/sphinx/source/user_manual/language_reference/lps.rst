.. _language-lps:

Linear Process Specifications
=============================

.. highlight:: mcrl2

Within the mCRL2 toolset Linear Process Specifications (LPSs) play a pivotal
role. A linear process specification contains a single process definition
with a very simple basic structure. It does not contain parallelism,
communication or visibility operators. As such it is a good basic form to base
tools upon.

Before being able to do anything with it, any process specification is first
translated to linear form using :ref:`tool-mcrl22lps`. All subsequent
manipulations operate on this linear form. There are tools that translate one
linear process specification to another (e.g. :ref:`tool-lpsparelm`), other
tools that translate linear process specifications to labelled transition
systems (:ref:`tool-lps2lts`), tools that operate directly on linear process
specifications (such as the simulator :ref:`tool-lpsxsim`), and a tool to pretty
print linear process specifications (:ref:`tool-lpspp`). In essence any process
specification can be translated to linear form (although :ref:`tool-mcrl22lps`
puts certain restrictions on its input). Moreover, in practice the resulting
linear process specification is generally fairly small. Translating a parallel
process to linear form is hardly ever the bottleneck in the analysis of a
system.

The left-hand side of a linear process definition is one single process
reference (usually P) with a number of process parameters. The right hand side
of the equation consists of a sequence of summands which contain one sum
operator, possibly with zero or more variables, one condition, one action and
one invocation of the process P again. The parameters of the process reference
constitute the possible states of the system. The condition indicates whether
the action can be done in this particular state, and the invocation indicates
the state change. When it comes to process analysis, this summand form occurs in
many disguises. It for instance lives under the name condition-action-effect
rule.

Consider for instance, the specification of a buffer::

  proc Buffer = sum m:Nat.read(m).send(m).Buffer;
  init Buffer;

This specification is not linear, because there are two actions before the
recursive invocation of Buffer. The linear process specification of a buffer
must introduce a boolean b to indicate whether the action read was done::

  proc Buffer(b: Bool, n: Nat) = sum m: Nat. b -> read(m).Buffer(!b,m)
                               + !b -> send(n).Buffer(!b,n);
  init Buffer(true,0);

Note that for simple process specifications, the linear variants are easy to
read. For more complex process specifications this is generally not the case.

At some places in a linear process specifications, the values of some process
parameters are not really relevant. For this purpose global variables are
introduced. The meaning of global variables in a linear process specification is
that the behaviour of the process, starting in its start state is always the
same for any choice of values for the global variables. This allows tools such
as lpsconstelm, that finds and removes constant process parameters, to choose
optimal values for these global variables to remove as many constants as
possible. Actually, in the Buffer above, the value of ``n`` is irrelevant after the
``send(n)`` action. This will be observed during linearisation, and the result will
look like::

  glob dc: Nat;
  proc Buffer(b: Bool, n: Nat) = sum m: Nat. b -> read(m) . Buffer(!b, m)
                               +            !b -> send(n) . Buffer(!b, dc);
  init Buffer(true, 0);
  
For timed processes, a timed tag can be added to the actions during
linearisation. Moreover, timed deadlock summands ``delta@t`` can show up, indicating
that in the linear process time may proceed up till time ``t``.
