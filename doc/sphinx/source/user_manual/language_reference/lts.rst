.. _language-lts:

Labelled Transition Systems
===========================

.. _language-mcrl2-lts:

mCRL2 LTS format
----------------

An mCRL2 LTS file consist of two sections (although the second section is
optional). The first section, starting at offset 0, is an
`SVC file <http://db.cwi.nl/rapporten/abstract.php?abstractnr=1060>`_ with type
``mCRL2`` or ``mCRL2+info``. The appendix ``+info`` indicates
whether states are stored as just numbers or as actual mCRL2 states. The actions
of transitions are always stored as a pair of an mCRL2 multiaction and an mCRL2
expression of sort Time. The latter may also be ``Nil`` in absence of
timing. The precise form of states and transitions is as follows.

.. productionlist::
   State            : STATE(`DataExprList`)
   TransitionAction : pair(`MultAct`,`DataExprOrNil`)
   DataExprOrNil    : `DataExpr` | Nil

Optionally, this SVC file is followed by a second section containing information
such as a data specification or state parameter names. This section consist of
an ATerm in Binary format followed by a 64-bit offset (little endian)
and a marker :literal:`\    1STL2LRCm` (note the three initial
spaces). The offset indicates where this section starts in the file. The
ATerm is of the following form.

.. productionlist::
   mCRL2LTS1      : mCRL2LTS1(`DataSpecOrNil`,`ParamSpecOrNil`,`ActSpecOrNil`)
   DataSpecOrNil  : `DataSpec` | Nil
   ParamSpecOrNil : ParamSpec(`DataVarIdList`) | Nil
   ActSpecOrNil   ::= `ActSpec` | Nil

.. _language-fsm-lts:

FSM file format
---------------

Files in the FSM file format are accepted as input by several LTS visualisation
tools in the mCRL2 toolset.

An FSM file is a human-readable, plain-text file that specifies an LTS.
Its contents are of the following form:

.. productionlist::
   FSM : `PARAMETERS` '\n' '---' '\n' `STATES` '\n' '---' '\n' `TRANSITIONS`

containing three sections:

* The first section specifies the *state parameters* and their domains;
* The second section specifies the *states* of the LTS;
* The third section specifies the *transitions* of the LTS.

These sections are separated by lines that contain three dashes: ``---``.

The format of each of these sections is described separately below.

Parameters section
^^^^^^^^^^^^^^^^^^

The parameters section defines the state parameters (or state variables) and
their domains of possible values. In every state of the LTS, each of the
parameters has one specific value from its domain.

At least one parameter should be specified. Every parameter is specified on a
separate line of the following form:

.. productionlist::
   PARAMETER: `PARAMETER_NAME` '(' `DOMAIN_CARDINALITY` ')' `DOMAIN_NAME` ('"' `DOMAIN_VALUE` '"')*

containing the following items:

* The *parameter name*: a string of alphanumerical characters;
* The *domain cardinality*: a natural number;
* The *domain name*: a string of alphanumerical characters;
* A list of *domain values*: a space-separated list of quoted values, where
  every value is a string of characters that does not contain quotes (``"``).

The number of domain values should be equal to the domain cardinality. If the
domain cardinality is 0, then the parameter will be ignored. It will not be
visible in the tool and the corresponding state values in the states section are
ignored.

States section
^^^^^^^^^^^^^^

The states section defines the number of states and the value of every parameter
in every state.

At least one state should be specified. The first state specified is taken to be
the *initial state* of the LTS. Every state is specified on a separate line
of the following form:

.. productionlist::
   STATE : (`PARAMETER_VALUE`)*
   
being a list of *parameter values*: a space-separated list of natural numbers.

The number of parameter values should be equal to the number of parameters
defined in the parameters section, including parameters with a domain
cardinality of 0.

The *i*th value in the list specifies the value of the *i*th parameter of the
parameters section in the following way: a value of *n* specifies that in this
state, that parameter has the *n*th value from its domain. These values are
0-based, meaning that a value of 0 corresponds to the first domain value of that
parameter.

Every value should be at least 0 and smaller than the domain cardinality of the
corresponding parameter. If that domain cardinality is 0, then the latter
restriction does not apply and the value will be ignored.

Transitions section
^^^^^^^^^^^^^^^^^^^

The transitions section defines the transitions between the states of the LTS.

Every transition is specified on a separate line of the following form:

.. productionlist::
   TRANSITION : SOURCE_STATE TARGET_STATE '"'LABEL'"'

containing the following items:

* The *source state*: a positive natural number;
* The *target state*: a positive natural number;
* The *label*: a quoted string of characters that does not contain quotes
  (``"``).

A value of *n* for either of the states indicates the *n*th state of the states
section. Each of these values should be at least 1 and at most the number of
states specified in the states section.

Example
^^^^^^^

.. image:: img/FSM_file_example.*
   :align: center

The following FSM file specifies the LTS depicted in the figure above. The state
parameter values are indicated next to every state. The state identifiers used
in the transitions section of the FSM file are shown inside every state::

   b(2) Bool "F" "T"
   n(2) Nat "1" "2"
   ---
   0 0
   0 1
   1 0
   1 1
   ---
   1 2 "increase"
   1 3 "on"
   2 4 "on"
   2 1 "decrease"
   3 1 "off"
   3 4 "increase"
   4 2 "off"
   4 3 "decrease"

.. _language-aut-lts:

Aldebaran format
----------------
 
The Aldebaran file format is a simple format for storing labelled transition
systems (LTS's) explicitly.

Syntax
^^^^^^

The syntax of an Aldebaran file consists of a number of lines, where the first
line is :token:`aut_header` and the remaining lines are :token:`aut_edge`. 

:token:`aut_header` is defined as follows:

.. productionlist::
   aut_header : 'des (' `first_state` ',' `nr_of_transitions` ',' `nr_of_states` ')'
   first_state : `number`
   nr_of_transitions : `number`
   nr_of_states: `number`

Here:

* :token:`first_state` is a number representing the first state, which should always be ``0``
* :token:`nr_of_transitions` is a number representing the number of transitions
* :token:`nr_of_states` is a number representing the number of states

An :token:`aut_edge` is defined as follows:

.. productionlist::
   aut_edge : '(' `start_state` ',' `label` ',' `end_state` ')'
   start_state: `number`
   label: '"' `string` '"'
   end_state: `number`

Here:

* :token:`start_state` is a number representing the start state of the edge;
* :token:`label` is a string enclosed in double quotes representing the label of the edge;
* :token:`end_state` is a number representing the end state of the edge.

Example
^^^^^^^^

The following example shows a simple labelled transition system of the dining
philosophers problem for two philosophers, visualised using
:ref:`tool-ltsgraph`:

.. image:: img/Dining2_ns_seq.*
   :width: 600px
   :align: center

This transition system is represented by the following Aldebaran file::

   des (0,12,10)
   (0,"lock(p2, f2)",1)
   (0,"lock(p1, f1)",2)
   (1,"lock(p1, f1)",3)
   (1,"lock(p2, f1)",4)
   (2,"lock(p2, f2)",3)
   (2,"lock(p1, f2)",5)
   (4,"eat(p2)",6)
   (5,"eat(p1)",7)
   (6,"free(p2, f2)",8)
   (7,"free(p1, f1)",9)
   (8,"free(p2, f1)",0)
   (9,"free(p1, f2)",0)

Acknowledgements
^^^^^^^^^^^^^^^^

The Aldebaran format is originally used in the `CADP toolset
<http://www.inrialpes.fr/vasy/cadp/>`_. To be fully compatible with the
`original syntax definition
<http://www.inrialpes.fr/vasy/cadp/man/aldebaran.html#sect6>`_, the labels of
the edges should consist of at most 5000 characters.

