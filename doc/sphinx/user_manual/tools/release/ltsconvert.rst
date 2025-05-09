.. index:: ltsconvert

.. _tool-ltsconvert:

ltsconvert
==========

Convert labelled transition systems (LTSs) from and to different formats.
Optionally, a minimisation method can be selected that is applied on the input.

In order to convert a non-mCRL2 LTS to a mCRL2 LTS one needs to supply the
original LPS with --lps. This is because actions need to be stored in the
internal mCRL2 format in mCRL2 and in non-mCRL2 LTSs are represented by strings
(lacking essential information such as data types).

When applied to an .lts file, and if the ltsconvert tool applies reductions, it
groups the state labels of all states that are equivalent into one set of state
labels for a number of reductions. The reduced state in the new state space is labelled with this set. The state labels can be
inspected using the tool :ref:`tool-ltsinfo` with the flag ``-l``, or ``--state-label``.
As these sets of state labels can be large, the flag ``-n``, or ``--no-state``
of :ref:`tool-ltsconvert` can be used to remove these state labels in the resulting
state space.

.. note::

   Tools that use the fsm format may depend on state information and parameter
   names. This requires that this information is available in the input LTS file
   or that the --lps option is used (see the options sections below).

.. mcrl2_manual:: ltsconvert
