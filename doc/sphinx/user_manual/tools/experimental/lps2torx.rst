.. index:: lps2torx

.. _tool-besconvert:

lps2torx
==========

lps2torx is intended to do model based testing using the JTorx environment.
It can be invoked (on a Windows environment) by::


   "lps2torx.exe" "your-mcrl2-model.lps"

You should note the following

#. Ensure that lps2torx.exe is in your system path.

#. Set the “Model” field to the location of the helper file (see Listing 1), e.g. “D:/jtorx-helper.tx.bat”.

#. Set the “Implementation” field to “real program, comm. labels via tcp, JTorX is server”. Set the text field below to the address of your SUT, e.g. “127.0.0.1:5555”.

#. Set “Timeout” appropriately.

#. Set “Interpretation” to “action names below”.

#. Set “Trace kind” to “Straces”.



Convert Boolean equation systems (BESs) from and to different formats.
Optionally, a minimisation method can be selected that is applied on the input.

The reductions that can be done are either strong bisimulation reduction or stuttering bisimulation reduction.

Strong bisimulation reduction of BESs is described in [KW11]_. Correctness of stuttering equivalence for BESs follows from the corresponding results on parity games in [CKW11]_
