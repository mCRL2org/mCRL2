.. index:: lpsconstelm

.. _tool-lpsconstelm:

lpsconstelm
===========

Remove constant process parameters from the LPS. If it can be determined that
certain parameters of this LPS remain constant throughout any run of the
process, all occurrences of these process parameter are replaced by the initial
value and the process parameters are removed from the LPS.

If the initial value of a process parameter is a global variable and remains a
global variable throughout the run of the process, the process variable is
considered constant.

If the initial value of a process parameter is a global variable and is only
changed once to a certain value, the process parameter is constant and the
specific value is used for substitution.
