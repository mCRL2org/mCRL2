.. index:: pbessolve

.. _tool-pbessolve:

pbessolve
=======

The tool ``pbessolve`` can solve a PBES. The solution is printed as ``true`` or ``false`` on
the command line output. If the property encoded in the PBES holds, in addition a witness can be constructed,
and if the property does not hold a counter example can be constructed. We refer to the witness or counter
example as the *evidence* of a property.

If the PBES was constructed via ``lps2pbes``, then the evidence is an LPS that is a submodel of the
LPS that was passed to ``lps2pbes``. If the PBES was constructed via ``lts2pbes``, then the evidence
is an LTS that is a submodel of the LTS that was passed to ``lts2pbes``. The evidence is written to a
file named ``<<pbesfile>>.evidence.lps`` or ``<<pbesfile>>.evidence.lts``. A different name can be
specified using the command line option :option:`--evidence-file`.

A counter example for the "infinitely often enabled then infinitely often taken" property of the ABP protocol
can be generated as follows::

   mcrl22lps abp.mcrl2 abp.lps
   lps2pbes -v -c -f infinitely_often_enabled_then_infinitely_often_taken.mcf abp.lps abp.pbes
   pbessolve -v --file=abp.lps abp.pbes
   lps2lts abp.pbes.evidence.lps abp.pbes.evidence.lts
   ltsgraph abp.pbes.evidence.lts

Note that ``lps2pbes`` is called with the option :option:`-c` to include counter example
information in the generated PBES. Also note that the specification ``abp.lps`` that was used
to create the PBES is passed as an argument to pbessolve with the option :option:`--file`. It
is needed to construct the counter example. The tool ``pbessolve`` stores the counter example in a
file with the extension ``<<pbesfile>>.evidence.lps``. A different name can be specified using the
command line option :option:`--evidence-file`.

Similarly it can be done starting from an LTS instead of an LPS::

   mcrl22lps abp.mcrl2 abp.lps
   lps2lts abp.lps abp.lts
   lts2pbes -v -c -f infinitely_often_enabled_then_infinitely_often_taken.mcf abp.lts abp.pbes
   pbessolve -v --file=abp.lts abp.pbes
   ltsgraph abp.pbes.evidence.lts

.. figure:: infinitely_often_enabled_then_infinitely_often_taken.png
   :align: center
   :width: 50%

   Counter example for the property "infinitely often enabled then infinitely often taken".

.. note::

   The interface of pbessolve is not stable yet. In particular the strategies that
   are available through the option :option:`-s` are subject to change.
