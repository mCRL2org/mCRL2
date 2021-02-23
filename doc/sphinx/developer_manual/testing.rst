================================
Toolset tests
================================

The toolset contains several types of regression tests. These tests are
automatically executed on a continuous integration service to maintain the
quality of the toolset, for information see :doc:`teamcity_testing`. First of
all, each library contains a `test` directory consisting of several :file:`.cpp`
files in which unit and integration tests are defined using the unit testing
framework of the Boost library. Existing tests give an idea on how the tests are
defined.

There are also tests which execute the compiled tools themselves and check for
errors in the results. Here, we have random tests that generate random (P)BES or
mCRL2 specifications. See the section below for a detailed description of their
setup and how to declare new tests. There are also regression tests with
concrete specifications as input for issues that have been reported in the past
or that have been observed during development. Regression tests are located in
the :file:`tests/regression/` directory and use a similar setup as random
testing.

All tests are set up such that they can be executed locally by using ctest. For
example, by executing :option:`ctest -j8` in the build directory we can run all
test on eight cores. This can take more than an hour to complete. Furthermore,
using the :option:`-R` option we can only execute tests that contain
the given string in their name.

Random Testing
----------------------------

The random tests are implemented by the :file:`tests/random/random_testing.py`
script. There are three types of random tests, modeled by the following classes:

 - ProcessTest - tests with a randomly generated process specification as input
 - PbesTest - tests with a randomly generated PBES as input
 - BesTest - tests with a randomly generated BES as input

In the constructor of each of these classes the parameters of the random generation are set.

To add a random test, the following steps must be taken:

(1) Define a new class that inherits from one of those three. There are many examples of this in the script.
(2) Optionally change the parameters of the random generation in the constructor.
(3) Create a test specification in the directory tests/specification. This is a graph specified in YAML format, with three sections.

The section 'tools' contains nodes that represent tool invocations. For example
the following specifies an invocation of the tool lps2pbes with inputs l2 (an
LPS) and l5 (a modal formula), output l8 (a PBES) and with additional command
line flag --structured::

  t5:
    input: [l2, l5]
    output: [l8]
    args: [--structured]
    name: lps2pbes

The section 'nodes' contains nodes of the types below. These nodes are the inputs and outputs of tools:

  - mcrl2 - a process specification in text format
  - lps - a linearized process specification in .lps format
  - lts - an LTS in .lts format
  - aut - an LTS in .aut format
  - pbesspec - a PBES in text format
  - pbes - a PBES in .pbes format
  - mcf - a modal formula in text format
  - text - a text file

For example::

  nodes:
    l1:
      type: pbesspec
    l2:
      type: pbes
    l3:
      type: pbes

The section 'result' contains a python expression that is used to evaluate the result, for example::

  result: |
    result = t4.value['solution'] == t6.value['solution'] == t8.value['solution'] == (not t2.value['has-deadlock'])

Note that after a tool node has been executed, the results of the execution are put in the mapping value. The command line output of the execution is stored in the attributes stdout and stderr, so if needed this can also be used to determine the result of a test. There are many examples available in the directory tests/specifications. Moreover there is a python script draw.py that generates a graphical representation of the test. This is useful to check if the test has been specified correctly.

(4) Add an entry in the mapping available_tests. For example::

     'lpsconstelm' : lambda name, settings: LpsConstelmTest(name, settings)

The parameter settings is the dictionary that corresponds to the YAML file of the random test. This file has to be specified during construction of the class LpsConstelmTest.

The random tests can be run using commands like this::

  --- run one repetition of all available tests ---
  python3 random_testing.py -r1
  alphabet-reduce_0 Pass
  bessolve_0 Pass
  bisimulation-bisim_0 Pass
  bisimulation-bisim-gjkw_0 Pass
  bisimulation-bisim-gv_0 Indeterminate
  bisimulation-branching-bisim_0 Pass
  bisimulation-branching-bisim-gjkw_0 Pass
  bisimulation-branching-bisim-gv_0 Pass
  bisimulation-weak-bisim_0 Pass
  lps-quantifier-one-point_0 Pass
  lps2lts-algorithms_0 Pass
  lps2pbes_0 Pass
  lpsbinary_0 Indeterminate
  lpsconfcheck-commutative_0 Pass
  lpsconfcheck-commutative-disjoint_0 Pass
  lpsconfcheck-disjoint_0 Pass
  lpsconfcheck-triangular_0 Pass
  lpsconfcheck-trivial_0 Pass
  lpsconstelm_0 Pass
  lpsparelm_0 Pass
  lpsstategraph_0 Indeterminate
  lpssumelm_0 Pass
  lpssuminst_0 Pass
  lts2pbes_0 Pass
  ltscompare-bisim_0 Pass
  ltscompare-bisim-gjkw_0 Pass
  ltscompare-bisim-gv_0 Pass
  ltscompare-branching-bisim_0 Pass
  ltscompare-branching-bisim-gjkw_0 Pass
  ltscompare-branching-bisim-gv_0 Pass
  ltscompare-dpbranching-bisim_0 Pass
  ltscompare-dpbranching-bisim-gjkw_0 Pass
  ltscompare-dpbranching-bisim-gv_0 Pass
  ltscompare-dpweak-bisim_0 Pass
  ltscompare-ready-sim_0 Pass
  ltscompare-sim_0 Pass
  ltscompare-trace_0 Pass
  ltscompare-weak-bisim_0 Pass
  ltscompare-weak-trace_0 Pass
  pbes-srf_0 Indeterminate
  pbes-unify-parameters_0 Pass
  pbesconstelm_0 Pass
  pbesinst-alternative_lazy_0 Pass
  pbesinst-finite_0 Pass
  pbesinst-lazy_0 Pass
  pbesparelm_0 Pass
  pbespareqelm_0 Pass
  pbespgsolve_0 Pass
  pbespor2_0 Pass
  pbesrewr-data-rewriter_0 Pass
  pbesrewr-pfnf_0 Pass
  pbesrewr-quantifier-all_0 Pass
  pbesrewr-quantifier-finite_0 Pass
  pbesrewr-quantifier-inside_0 Pass
  pbesrewr-quantifier-one-point_0 Pass
  pbesrewr-simplify_0 Pass
  pbesrewr-simplify-data-rewriter_0 Pass
  pbesrewr-simplify-quantifiers-data-rewriter_0 Pass
  pbesrewr-simplify-quantifiers-rewriter_0 Pass
  pbesrewr-simplify-rewriter_0 Pass
  pbessolve_0 Pass
  pbessolve-counter-example-optimization-0_0 Pass
  pbessolve-counter-example-optimization-1_0 Pass
  pbessolve-counter-example-optimization-2_0 Pass
  pbessolve-counter-example-optimization-3_0 Pass
  pbessolve-counter-example-optimization-4_0 Pass
  pbessolve-counter-example-optimization-5_0 Pass
  pbessolve-counter-example-optimization-6_0 Pass
  pbessolve-counter-example-optimization-7_0 Pass
  pbessolve-depth-first_0 Pass
  pbesstategraph_0 Pass

  --- run 5 repetitions of all tests that match the pattern parelm ---
  python3 random_testing.py -pparelm -r5
  lpsparelm_0 Pass
  lpsparelm_1 Pass
  lpsparelm_2 Pass
  lpsparelm_3 Pass
  lpsparelm_4 Pass
  pbesparelm_0 Pass
  pbesparelm_1 Pass
  pbesparelm_2 Pass
  pbesparelm_3 Pass
  pbesparelm_4 Pass

  --- run 10 repetitions of bessolve and keep the results ---
  python3 random_testing.py -pbessolve -r10 -k
  bessolve_0 Pass
  bessolve_1 Pass
  bessolve_2 Pass
  bessolve_3 Pass
  bessolve_4 Pass
  bessolve_5 Pass
  bessolve_6 Pass
  bessolve_7 Pass
  bessolve_8 Pass
  bessolve_9 Pass

When a test results in Indeterminate, it means that there was either a timeout, or the memory limit was exceeded. 