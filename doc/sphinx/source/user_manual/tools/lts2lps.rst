.. index:: lts2lps

.. _tool-lts2lps:

lts2lps
=======

Translate an LTS to an LPS. This tool is useful when an labelled transition
system in .aut, .svc, .fsm or .lts must be processed. Such processing tools are
generally only available on linear processes. By transforming the transition
system to a linear process, such processing can be performed. For processing one
can typically think on model checking a process, or applying lpsactionrename to
the process.

.. note::

   If the labelled transition system does not provide information on the data types
   and has no action declarations, these must be provided separately. Only the .lts
   format contains such information. There are several ways to provide this extra
   required info, see the :option:`-D`, :option:`-l` and :option:`-m` options.

