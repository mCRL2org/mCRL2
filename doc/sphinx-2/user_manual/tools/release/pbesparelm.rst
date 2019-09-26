.. index:: pbesparelm

.. _tool-pbesparelm:

pbesparelm
==========

The purpose of this tool is to find those parameters in parameterised
Boolean equation sytems that do not affect the solution, and remove those in a similar fashion as :ref:`tool-lpsparelm`.

Example:

.. math::

   \begin{array}{l}
   \nu V(n{:}\mathbb{N}, b{:}\mathbb{B}) = (b \Rightarrow V(n + 1, b)) \land W(n, b)\\
   \mu W(n{:}\mathbb{N}, b{:}\mathbb{B}) = b \lor (b \land W(n+1, b))\\
   ~\\
   \mathbf{init}\ V(0,\mathit{true})
   \end{array}

Instantiation of this PBES using tools such as :ref:`tool-pbes2bool` or :ref:`tool-pbessolve` does not terminate. However, in this PBES, parameter :math:`n` of both equations is redundant (it does not appear positively in any equation), and can be removed. The resulting PBES will be:

.. math::

   \begin{array}{l}
   \nu V(b{:}\mathbb{B}) = (b \Rightarrow V(b)) \land W(b)\\
   \mu W(b{:}\mathbb{B}) = b \lor (b \land W(b))\\
   ~\\
   \mathbf{init}\ V(0,\mathit{true})
   \end{array}

The algorithm underlying the tool is described in detail in [OWW09]_.


.. include:: man/pbesparelm.txt
