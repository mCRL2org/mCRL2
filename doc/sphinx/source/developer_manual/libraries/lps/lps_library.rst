Linear process specifications
=============================

The LPS library contains classes and algorithms for linear processes. The code
in the LPS library is contained in the namespace =lps=. LPS is shorthand for
linear process specification.

Definitions
-----------
A *linear process* is defined as

.. math::  P(d:D)=\sum\limits_{i\in I}s_{i}(d)

with

.. math:: s_{i}(d)=\sum_{e:E_{i}}c_{i}(d,e)\rightarrow a_{i}(d,e)^@t_{i}(d,e)\cdot P(g_{i}(d,e))

and

:math:`a_{i}(d,e) = a_{i}^1(f_{i}^1(d,e)) \mid \ldots \mid a_{i}^n(f_{i}^n(d,e))` or :math:`\delta`

where

* :math:`d` is a vector of data variables, called the *process parameters*. The corresponding vector of sorts :math:`D` models the states of the process.
* :math:`s_{i}` is a linear process term, called a *summand*
* :math:`E_{i}` is a sort, and the elements :math:`e` of :math:`E_{i}` are called *summation variables*.
* :math:`c_{i}` is a boolean term, called the *condition*
* :math:`a_{i}` is a term called the *action*
* :math:`t_{i}` is a real valued term, called the *time*
* :math:`g_{i}` is an *assignment* function to data variables, with the following interpretation. The expression :math:`g_{i}(d,e)` is a state that can be reached from state :math:`d` by performing the action :math:`a_{i}` at time :math:`t_{i}`.

A linear process usually has an accompanying initial value :math:`d_0:D`.

.. note::

   The time :math:`t_{i}` and the condition :math:`c_{i}` are optional.
   The action :math:`a_{i}(d,e)` can have the value :math:`\delta`, corresponding to deadlock.

Specification
-------------
A process can be defined using a (textual) mCRL2 specification. A typical
process specification is the following:

.. code-block:: mcrl2

   sort
     D     = struct d1 | d2;
     Error = struct e;

   act
     r1,s4: D;
     s2,r2,c2: D # Bool;
     s3,r3,c3: D # Bool;
     s3,r3,c3: Error;
     s5,r5,c5: Bool;
     s6,r6,c6: Bool;
     s6,r6,c6: Error;
     i;

   proc
     S(b:Bool)     = sum d:D. r1(d).T(d,b);
     T(d:D,b:Bool) = s2(d,b).(r6(b).S(!b)+(r6(!b)+r6(e)).T(d,b));

     R(b:Bool)     = sum d:D. r3(d,b).s4(d).s5(b).R(!b)+
                     (sum d:D.r3(d,!b)+r3(e)).s5(!b).R(b);

     K             = sum d:D,b:Bool. r2(d,b).(i.s3(d,b)+i.s3(e)).K;

     L             = sum b:Bool. r5(b).(i.s6(b)+i.s6(e)).L;

   init
     allow({r1,s4,c2,c3,c5,c6,i},
       comm({r2|s2->c2, r3|s3->c3, r5|s5->c5, r6|s6->c6},
           S(true) || K || L || R(true)
       )
     );

This process can be stored in an instance of the class ``lps::specification`` after linearizing it
using

.. code-block:: c++

    std::string text = ... ;
    specification spec = linearise(text);

The class ``lps::specification`` represents a linearized process specification. It consists of

* a linear process
* a data specification, i.e. a specification of the sorts, constructors, mappings and equations
* an action specification, i.e. a sequence of actions that may occur during execution of the process
* an initial state
* global variables

Linear processes
----------------
An instance of ``lps::specification`` contains a linear process of type ``lps::linear_process``. This linear
process in turn contains a sequence of summands, and an initial state.
Note that the implementation distinguishes between `action summands` and `deadlock summands` (i.e. those
summands containing a :math:`\delta`).

.. code-block:: c++

    const linear_process& proc = spec.process();
    const atermpp::vector<action_summand>& as = proc.action_summands();
    const atermpp::vector<deadlock_summand>& ds = proc.deadlock_summands();
    const process_initializer& init = proc.init();

Both classes ``action_summand`` and ``deadlock_summand`` have an optional attribute time.
It is necessary to check if the time is available, before using it:

.. code-block:: c++

    action_summand s = proc.action_summands().front();
    if (s.has_time())
    {
      std::cout << "time = " << s.time() << std::endl;
    }

Action summands have an associated multi action, which consists of a sequence of actions.
An illustration of it's usage is

.. code-block:: c++

    action_summand s;
    for (const process::action& a: s.actions())
    {
      core::identifier_string name = a.label().name();
      data::data_expression_list arguments = a.arguments();
    }

Here ``name`` corresponds with :math:`a_{i}`, and ``arguments`` with :math:`f_i(d,e)` in
the earlier given formulas.

.. note::

   There is a convention that a linear process without any summands represents the process ``delta @ 0``.

Classes in the LPS library
==========================
Several classes in the LPS library are just thin wrappers around an ATerm pointer (see also the :ref:`atermpp_library`).
This means that instances of these classes are immutable, and instances with the same
value are shared in memory. The following table gives an overview of the ATerm based classes:

=============================== =
   ATerm based classes
=============================== =
 ``process::action_label``
 ``process::action_label_list``
 ``lps::process_initializer``
=============================== =

Correctness checks
------------------
For many classes there are restrictions to what expressions are considered
valid, the so called well typedness constraints. These constraints are implemented
in the class ``lps/detail/lps_well_typed_checker.h``. For example, the following
checks are done for linear processes:

  * process parameters have unique names
  * process parameters and summation variables have different names
  * the left hand sides of the assignments of summands are contained in the process parameters

Such constraints are only checked in debug mode in the `load` and `save` functions of
``lps::specification``. The descriptions of the well typedness constraints are found in the
reference documentation.
