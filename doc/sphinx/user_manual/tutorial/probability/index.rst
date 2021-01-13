Probabilistic processes
-----------------------

It is possible to model probabilistic processes in mCRL2, although this
is still an experimental extension. 

The tool :ref:`tool-mcrl22lps` linearises the process to a stochastic linear
process. Using :ref:`tool-lps2lts` a probabilistic state space can be generated
and :ref:`tool-ltspbisim` allows to reduce this state space. The tool :ref:`tool-ltsgraph`
supports visualisation of probabilistic processes by drawing probabilistic states
with a thicker circle. There are experimental
tools to evaluate probabilistic modal formulas, but these are not included in the
toolset. 

Probabilistic processes are specified using the ``dist d:D[f(d)].p`` operator.
This operator selects a variable ``d`` of sort ``D`` with probability ``f(d)``.
For example doing an action ``a`` with probability ``1/3`` and an action ``b``
with probability ``2/3`` can be denoted by ``dist d:Bool[if(d,1/3,2/3)](d -> a <> b)``.

A larger example is the following:

.. literalinclude:: files/probability1.mcrl2 
   :language: mcrl2

The generated state space for this example is

.. literalinclude:: files/probability1.aut

In this state space, ``0 1/7 1 3/7 2 2/7 3`` means that state ``0`` is reached
with probability ``1/7``, state ``1`` is reached with probability ``3/7``, 
state ``2`` is reached with probability ``2/7``, and state ``3`` is reached
with the remaining probability ``1/7``. 

The notation ``dist d:D[f(d)].p`` is chosen as it also allows the domain ``D`` and
the distribution to range over infinite domains, and as such supports both discrete
and continuous distributions. As it stands the tools do not support continuous 
distributions. A major reason is that the combination of continous probability
distribution in combination with nondeterminism over uncountable domains is not
properly understood as it stands. 

