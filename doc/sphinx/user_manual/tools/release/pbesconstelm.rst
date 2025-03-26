.. index:: pbesconstelm

.. _tool-pbesconstelm:

pbesconstelm
============

The purpose of this tool is to eliminate constant parameters from parameterised
Boolean equation sytems, in a similar fashion as :ref:`tool-lpsconstelm`.

Example:

.. math::

   \begin{array}{l}
   \nu X(n, m{:}\mathbb{N}) = n \approx m + 2 \land X(n + 1, m) \land Y(0, m)\\
   \mu Y(n, m{:}\mathbb{N}) = n \approx 3*m \land Y(n, 2*m)\\
   ~\\
   \mathbf{init}\ X(4,7)
   \end{array}

In this PBES, the parameters :math:`m` of :math:`X` and :math:`n` of :math:`Y`
will be eliminated. Their constant values are 7 and 0, respectively. The
resulting PBES will be:

.. math::

   \begin{array}{l}
   \nu X(n{:}\mathbb{N}) = n \approx 7 + 2 \land X(n + 1) \land Y(7)\\
   \mu Y(m{:}\mathbb{N}) = 0 \approx 3*m \land Y(2*m)\\
   ~\\
   \mathbf{init}\ X(4)
   \end{array}

The option `-c/--compute-conditions` can be used to analyse conditions
with the contants that have been found. This can help the algorithm to find more
constants. However, this option typically leads to a large increase in the
runtime of the tool.

The algorithm underlying the tool is described in detail in [OWW09]_.

.. mcrl2_manual:: pbesconstelm
