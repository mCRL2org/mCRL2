A Vending Machine
-----------------

**Contribution of this section**

#. specifying processes,
#. linearisation,
#. state space exploration,
#. visualisation of state spaces,
#. comparison/reduction using behavioural equivalences, and
#. verification of modal mu-calculus properties.

**New tools**:
:ref:`mcrl22lps`, :ref:`lps2lts`, :ref:`ltsgraph`, :ref:`ltscompare`,
:ref:`ltsconvert`, :ref:`lps2pbes`, :ref:`pbes2bool`.

Our first little step consists of number of variations on the good old
vending machine, a user ``User`` interacting with a machine ``Mach``. By
way of this example we will encounter the basic ingredients of
mCRL2. In the first variation of the vending machine, a very
primitive machine, and user, are specified. Some properties are
verified. In the second variation non-determinism is considered and,
additionally, some visualization and comparison tools from the toolset
are illustrated. The third variation comes closer to a rudimentary
prototype specification.

First variation
^^^^^^^^^^^^^^^
After inserting a coin of 10 cents, the user can push the button for
an apple. An apple will then be put in the drawer of the machine. See
Figure :ref:`fig-mach1`.

.. _fig-mach1:

.. figure:: img/mach1.*
   :align: center
   
   Vending machine 1

Vending machine 1 can be specified by the following mCRL2, also
included in the file :download:`vm01.mcrl2 <files/vm01.mcrl2>`.

.. literalinclude:: files/vm01.mcrl2
   :language: mcrl2

The specification is split in three sections:

#. ``act``, a declaration of actions of 6 actions,
#. ``proc``, the definition of 2 processes, and 
#. ``init``, the initialization of the system.

The process ``User`` is recursively defined as doing an ``ins10``
action, followed by an ``optA`` action, followed by the process ``User``
again. The process ``Mach`` is similar, looping on the action ``acc10``
followed by the action ``putA``. Note, only four actions are used in
the definition of the processes. In particular, the action ``coin`` and
``ready`` are not referred to.

The initialization of the system has a typical form. A number of
parallel processes, in the context of a communication function, with a
limited set of actions allowed. So, ``||`` is the parallel
operator, in this case putting the processes ``User`` and ``Mach`` in
parallel. The communication function is the first argument of the
``comm`` operator. Here, we have that synchronization of an
``ins10`` action and an ``acc10`` action yields the action ``coin``,
whereas synchronization of ``optA`` and ``putA`` yields ``ready``. The
actions of the system that are allowed, are mentioned in the first
argument of the allow operator ``allow``. Thus, for our first
system only ``coin`` and ``ready`` are allowed actions.

We compile the specification in the file :download:`vm01.mcrl2 <files/vm01.mcrl2>`
to a so-called linear process, saved in the file :file:`vm01.lps`. This
can be achieved by running::

  $ mcrl22lps vm01.mcrl2 vm01.lps

on the command line. The linear process in the internal representation
format of mCRL2, not meant for human inspection. However, from
:file:`vm01.lps` a labeled transition system, LTS for short, can be
obtained by running::

  $ lps2lts vm01.lps vm01.lts
   
which can be viewed by the :ref:`ltsgraph` facility, by typing::

   $ ltsgraph vm01.lts

at the prompt.  Some manual beautifying yields the picture in
Figure :ref:`fig-lts-vm01`.

.. _fig-lts-vm01:

.. figure:: img/vm01.*
   :align: center
   
   LTS of vending machine 1

Apparently, starting from state 0 the system shuttles between state
0 and 1 alternating the actions ``coin`` and ``ready``. Enforced by
the allow operator, unmatched ``ins10``, ``acc10``,
``optA`` and ``putA`` actions are excluded. The actions synchronize
pairwise, ``ins10`` with ``acc10``, ``optA`` with ``putA``, to produce
``coin`` and ``ready``, respectively.

As a first illustration of model checking in mCRL2, we consider some
simple properties to be checked against the
specification :download:`vm01.mcrl2 <files/vm01.mcrl2>`. Given the LTS of the
system, the properties obviously hold.

  a. *always, eventually a ready is possible* (true)
     ``[true*] <true*.ready> true``
  b. *a ready is always possible* (false)
     ``[true*] <ready> true``
  c. *after every ready only a coin follows* (true)
     ``[true*.ready.!coin] false``
  d. *any ready is followed by a coin and another ready* (true)
     ``[true*.ready.!coin] false && [true*.ready.true.!ready] false``

Dissecting property (a), ``[true*]`` represents all finite
sequences of actions starting from the initial
state. ``<true*.ready>`` expresses the existence of a sequence of
actions ending with the action ``ready``. The last occurence of ``true``
in property (a) is a logical formula to be evaluated in the current
state. Thus, if property (a) is satisfied by the system, then after
any finite sequence of actions, ``[true*]``, the system can
continue with some finite sequence of actions ending with ``ready``,
``<true*.ready>``, and reaches a state in which the formula
``true`` holds. Since ``true`` always holds, property (a) states that a
next ``ready`` is always possible.

Property (b) is less liberal than property (a). Here, ``<ready> true``
requires a ``ready`` action to be possible for the system,
after any finite sequence, ``[true*]``. This property does not
hold. A ``ready`` action is not immediately followed by a ``ready``
action again. Also, ``ready`` is not possible in the initial state.

Property (c) uses the complement construct. ``!coin`` are all
actions different from ``coin``. So, any sequence of actions with
``ready`` as its one but final action and ending with an action
different from ``coin``, leads to a state where ``false`` holds. Since
no such state exists, there are no path of the form
``true*.ready.!coin``. Thus, after any ``ready`` action, any
action that follows, if any, will be ``coin``. Property (d) is a
further variation involving conjunction ``&&``.

Model checking with mCRL2 is done by constructing a so-called
parameterised boolean equation system or PBES from a linear process
specification and a modal :math:`\mu`-calculus formula. For example, to
verify property (a) above, we call the :ref:`lps2pbes`
tool. Assuming property (a) to be in file :download:`vm01a.mcf
<files/vm01a.mcf>`, running::

   $ lps2pbes vm01.lps -f vm01a.mcf vm01a.pbes

creates from the system in linear format and the formula in the file
:download:`vm01.mcrl2 <files/vm01.mcrl2>` right after the ``-f`` switch, a PBES in the
file :file:`vm01a.pbes`. On calling the PBES solver on
:file:`vm01a.pbes`::

   $ pbes2bool vm01a.pbes
   
the mCRL2 tool answers::

   The pbes is valid

So, for vending machine 1 it holds that action ``ready`` is always
possible in the future. Instead of making separate steps explicity,
the verification can also be captured by a single, pipe-line command::

   $ mcrl22lps vm01.mcrl2 | lps2pbes -f vm01a.mcf | pbes2bool

Running the other properties yields the expected
results. Properties (c) and (d) do hold, property (b) does not hold.

Second variation
^^^^^^^^^^^^^^^^
Next, we add a chocolate bar to the assortment of the
vending machine. A chocolate bar costs 20 cents, an apple 10
cents. The machine will now accept coins of 10 and 20 cents.
The scenarios allowed are (i) insertion of 10 cent and purchasing an
apple, (ii) insertion of 10 cent twice or 20 cent once and purchasing a
chocolate bar.
Additionally, after insertion of money, the user can push the change
button, after which the inserted money is returned. See
Figure :ref:`fig-mach2`.

.. _fig-mach2:

.. figure:: img/mach2.*
   :align: center
   
   Vending machine 2

.. admonition:: Exercise

   Write an mCRL2 specification in file :file:`vm02.mcrl2` for the vending
   machine sketched above, involving the actions::

      ins10, ins20, acc10, acc20, coin10, coin20, 
      chg10, chg20, ret10, ret20, out10, out20,
      optA, optC, putA, putC, readyA, readyC, prod 

   A possible specification of the ``Mach`` process may read

   .. code-block:: mcrl2
   
      Mach =
        acc10.( putA.prod + acc10.( putC.prod + ret20 ) + ret10 ).Mach +
        acc20.( putA.prod.ret10 + putC.prod + ret20 ).Mach ;
        
  The machine is required to perform a ``prod`` action, for
  administration purposes.

A visualization of the specified system can be obtained by first
converting the linear process into a labeled transition system (in
so-called SVC-format) by::

  $  lps2lts vm02.lps vm02.svc
  
and next loading the SVC file :file:`vm02.svc` into the ltsgraph tool
by::

  $  ltsgraph vm02.svc

The LTS can be beautified (a bit) using the ``start`` button in the
optimization panel of the user interface. Manual manipulation by
dragging states is also possible. For small examples, increasing
the natural transition length may provide better results.

.. admonition:: Exercise

   Prove that your specification satisfies:
   
   #a. *no three 10ct coins can be inserted in a row*
   #b. *no chocolate after 10ct only*
   #c. *an apple only after 10ct, a chocolate after 20ct*

The file :file:`vm02-taus.mcrl2` contains the specification of a system
performing ``coin10`` and ``coin20`` actions as well as
so-called :math:`\tau`-steps.  Use the :ref:`ltscompare` tool to compare
your model under branching bisimilarity with the LTS of the system
:file:`vm02-taus`, after hiding the actions ``readyA``, ``readyC``,
``out10``, ``out20``, ``prod``. This can be done as follows::

  $ ltscompare -ebranching-bisim --tau=out10,out20,readyA,readyC,prod vm02.svc vm02-taus.svc

Minimize the LTS for :file:`vm02.mcrl2` using :ref:`ltsconvert` with
respect to branching bisimulation after hiding the readies and returns::

  $ ltsconvert -ebranching-bisim --tau=out10,out20,readyA,readyC,prod vm02.svc vm02min.svc
  
Also, compare the LTSs :file:`vm02min.svc` and `vm02-taus.svc` visually using
:ref:`ltsgraph`.

Third variation
^^^^^^^^^^^^^^^
A skeleton for a vending machine with parametrized actions is available
in the file :file:`vm03.mcrl2`.

.. admonition:: Exercise

   Modify this specification such that all coins of denomination
   50ct, 20ct, 10ct and 5ct can be inserted. The machine accumulates
   upto a total of 60 cents. If sufficient credit, an apple or
   chocolate bar is supplied after selection. Money is returned after
   pressing the change button. Prove that your specification satisfies the
   properties in file :file:`vm03.mcf`.
