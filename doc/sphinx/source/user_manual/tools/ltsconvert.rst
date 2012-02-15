.. index:: ltsconvert

.. _tool-ltsconvert:

ltsconvert
==========

Convert labelled transition systems (LTSes) between different formats.
Optionally, a minimisation method can be selected that is applied on the input.

In order to convert a non-mCRL2 LTS to a mCRL2 LTS one needs to supply the
original LPS with --lps. This is because actions need to be stored in the
internal mCRL2 format in mCRL2 and in non-mCRL2 LTSs are represented by strings
(lacking essential information such as data types).

.. note::

   Tools that use the fsm format may depend on state information and parameter
   names. This requires that this information is available in the input LTS file
   or that the --lps option is used (see the options sections below).

