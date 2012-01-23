The Rope Bridge
---------------


**Contribution of this section**

#. exercise with processes,
#. use of state space exploration for checking properties, and
#. use of advanced visualisation techniques.

**New tools**:
:ref:`ltsview`, :ref:`tracepp`

.. image:: img/bridge.*
   :align: center

In the middle of the night, four adventurers encounter a shabby rope bridge
spanning a deep ravine. For safety reasons, they decide that no more than two
persons should cross the bridge at the same time and that a flashlight needs to
be carried by one of them on every crossing. They have only one flashlight. The
four adventurers are not all equally skilled: crossing the bridge takes them 1,
2, 5 and 10 minutes, respectively. A pair of adventurers cross the bridge in an
amount of time equal to that of the slowest of the two adventurers.

One of the adventurers quickly proclaims that they cannot get all four of them
across in less than 19 minutes. However, one of her companions disagrees and
claims that it can be done in 17 minutes. We shall verify this claim and show
that there is no faster strategy using mCRL2.

The folder ``RopeBridge`` contains the files for these exercises:

* :download:`bridge.mcrl2 <files/bridge.mcrl2>` which contains an incomplete
  mCRL2 specification of the rope bridge problem;
* :download:`formula_A.mcf <files/formula_A.mcf>` and :download:`formula_B.mcf <files/formula_B.mcrl2>` to which
  :math:`\mu`-calculus formulae will be added.

.. admonition:: Exercise

  Open :download:`bridge.mcrl2 <files/bridge.mcrl2>` in a text editor and study
  its contents. Then add the process definition for an adventurer. For this,
  answer the following questions:
  
  * What data parameters will the process have?
  * What actions will the process be able to perform?
  
  You will have to add action declarations and a process definition
  at the designated places in the mCRL2 specification.

.. admonition:: Exercise

  Add the four adventurers to the initial process definition.
  Apart from adding parallel processes to the definition, you have to
  take care of the synchronisation between actions of these processes:
  
  * Declare actions for the following events:
    
    * Two adventurers and a flashlight move forward over the bridge.
    * One adventurer and a flashlight move back over the bridge.

  * For each of these actions to occur, certain actions of the separate
    processes have to be synchronised. Specify the synchronisation between the
    actions using the *communication operator*, ``comm``.
  * Ensure that only the synchronised actions can occur, using the
    *allow operator*, ``allow``. 

.. admonition:: Exercise

  Simulate the model using the mCRL2 toolset by executing the following commands:

  * Linearise the specification to obtain an LPS::

    $ mcrl22lps -D bridge.mcrl2 bridge.lps
    
    Here, the ``-D`` option is passed because the specification does not contain
    time operators.

  * If linearisation fails, try to fix the reported errors.
    Otherwise, start the GUI simulation tool::
    
    $ lpsxsim bridge.lps
    
    The bottom part of the window shows the state parameters along with their
    values in the current state. (The simulator starts in the initial state of
    the system.) The top part shows the actions that can be performed from the
    current state, along with their effects on the parameter values.
  * Simulate the system by executing a sequence of actions. You can execute an
    action by double-clicking it in the list. Notice how the state parameter
    values get updated in the bottom part.
    
  After playing around with the simulator for a while, did you notice any weird
  or incorrect behaviour? If so, try to improve your model of the rope bridge
  and simulate it again.


.. admonition:: Exercise

  Generate the state space of your model by executing the
  following command::
  
  $ lps2lts bridge.lps bridge.svc
  
  The state space can be viewed using the LTSGraph tool::
  
  $ ltsgraph bridge.svc
  
  An alternative, 3D view of the state space can be given by LTSView,
  for which the state space first has to be converted to the FSM file
  format::
  
  $ ltsconvert --lps=bridge.lps bridge.svc bridge.fsm
  $ ltsview bridge.fsm

.. admonition:: Exercise

  The total amount of time that the adventurers consumed so far, is not yet
  being measured within the model. For this purpose, add a new process to the
  specification, called ``Referee``, which:

  * counts the number of minutes passed and updates this counter every time the
    bridge is crossed by some adventurer(s);
  * reports this number when all adventurers have reached the *finish* side.
    (This implies that it also needs to be able to determine when this happens!)

  You will have to add action declarations, add a ``Referee`` process definition
  and extend the initial process definition, including the communication and
  allow operators.

We shall now verify the following properties using the toolset:

A. It is possible for all adventurers to reach the `finish' side in 17 minutes.
B. It is not possible for all adventurers to reach the `finish' side in less
   than 17 minutes.

.. admonition:: Exercise

Express each of these properties in the modal :math:`\mu`-calculus.
Add the formulas to the files :file:`formula_A.mcf` and :file:`formula_B.mcf`
using a text editor.

.. admonition:: Exercise

  Verify the formulas using the toolset by executing the following commands:

  * Generate a PBES from your LPS and one of the formulas::

    $ lps2pbes --formula=formula_X.mcf bridge.lps bridge_X.pbes
    
  * Solve the PBES::

    $ pbes2bool bridge_X.pbes

  Alternatively, this can be done with a single command::

    $ lps2pbes --formula=formula_X.mcf bridge.lps | pbes2bool

A disadvantage of using PBESs for model checking is that insightful
diagnostic information is hard to obtain.
We shall now verify both properties again using the LTS tools.


.. admonition:: Exercise

  Verify the properties by generating traces as follows. Assuming that the
  action that reports the time is called ``report``, execute::
  
  $ lps2lts --action=report -t20 bridge.lps
  
  This outputs a message every time a ``report`` action is encountered during
  state space generation. Also, a trace is written to file for the first 20
  occurrences of this action. Properties A and B can now be checked by observing
  the output messages. Moreover, the trace for property A can be printed by
  passing the corresponding trace file name as an argument to the :ref:`tracepp`
  command, e.g.::
  
  $ tracepp file.trc
  
  This gives an optimal strategy for crossing the bridge in 17 minutes as
  claimed by the computer scientist adventurer.
