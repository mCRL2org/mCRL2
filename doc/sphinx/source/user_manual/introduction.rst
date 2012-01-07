Introduction to mCRL2
=====================

mCRL2 stands for micro Common Representation Language 2. It is a specification
language that can be used to specify and analyse the behaviour of distributed
systems and protocols and is the successor to `µCRL <http://www.cwi.nl/~mcrl>`_.
Using its accompanying toolset systems can be analysed and verified
automatically. 

Philosophy
----------

mCRL2 is based on the Algebra of Communicating Processes (ACP) which is
extended to include data and time. Like in every process algebra, a fundamental
concept in mCRL2 is the process. Processes can perform actions and can
be composed to form new processes using algebraic operators. A system usually
consists of several processes (or components) in parallel.

A process can carry data as its parameters. The state of a process is a
specific combination of parameter values. This state may influence the possible
actions that the process can perform. In turn, the execution of an action may
result in a state change. Every process has a corresponding state space or
Labelled Transition System (LTS) which contains all states that the process can
reach, along with the possible transitions between those states.

Using the algebraic operators, very complex processes can be constructed
containing, for example, lots of parallelism. A central notion in mCRL2 is the
linear process. This is a process from which all parallelism has been
removed to produce a series of condition - action - effect rules.
Complex systems, consisting of hundreds or even thousands of processes, can be
translated to a single linear process. Even for systems with an infinite state
space, the linear process (being an abstract representation of that state
space) is finite and can often be obtained very easily. Therefore, most tools
in the mCRL2 toolset operate on linear processes rather than on state spaces.

Model checking is provided using Parameterised Boolean Equation Systems (PBES).
Given a linear process and a formula that expresses some desired behaviour of
the process, a PBES can be generated. The solution to this PBES indicates
whether the formula holds on the process or not. An attempt can be made to
remove data from a PBES in order to obtain a BES, which is often easier to
solve.

History
-------

Around 1980 many process algebras were designed to model behaviour. Most notably
were CCS (Calculus of Communicating Processes, Milner), ACP (Algebra of
Communicating Processes, Bergstra and Klop) and CSP (Communicating Sequential
Processes, Hoare). These process algebras were mainly used as an object of
study, mainly due to their lack of proper data types.

In order to use these languages for actual modelling of behaviour a number of
process algebraic specifiation languages have been designed, which invariably
were extended with equational datatypes. The most well known is LOTOS (Language
of Temporal Ordering Specifications , Brinksma), but others are PSF (Process
Specification Formalism, Mauw and Veltink) and µCRL (micro Common Representation
Language, Groote and Ponse).

Unfortunately, the use of abstract data types made these languages unpleasant
when it came to the specification of complex behaviour. Therefore, we designed
the language mCRL2 (the successor of µCRL) to contain exactly those data types 
that one would expect when writing specifications, namely Bool, Pos, Nat, Int,
Real, lists, sets, bags, functions and functional data types. These data types
are machine independent. For instance there is no upperbound on natural numbers,
sets are not necessarily finite, quantification can be used within boolean terms
and lambda abstraction is part of the language. Furthermore, the language
features time and multi-actions, which were not present in most of the process
specification languages of the previous generation.

Note that mCRL2 is extremely rich and it is easy to express non-computable
behavioural specifications in it. Typically, for those specifications, tool
supported analysis will not be very fruitful. The advanced use of mCRL2 requires
a good understanding of the language, the underlying notions and even of the
implementation of the analysis tools. For more straightforward use this is not
needed. An effective rule of thumb is that everything that could be done using
languages such as LOTOS, PSF and µCRL, can be done without a problem using
mCRL2.
