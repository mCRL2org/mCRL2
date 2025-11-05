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

The tool can also be applied to parameterised Boolean equation systems that contain counter example information. The result can subsequently be solved with :ref:`tool-pbessolve`, using the following approach. Consider the PBES below where the counter example information is encoded by the :math:`Z`-variables: 

.. math::

   \begin{array}{l}
   \nu Y(n{:}\mathbb{B}) = (Y(\neg n) \land Zpos_{a}(n, n, \neg n)) \lor Zneg_{a}(n, n, \neg n)\\
   \nu Zpos_{a}(n, v, n'{:}\mathbb{B}) = \mathit{true}\\
   \mu Zneg_{a}(n, v, n'{:}\mathbb{B}) = \mathit{false}\\
   ~\\
   \mathbf{init}\ Y(\mathit{true})
   \end{array}

The :math:`Z`-variables and their equations must be preserved for counter example generation. Therefore, we remove the redundant parameter :math:`n` in two steps::

   pbesrewr --pbes-rewriter=remove-cex-variables original.pbes core.pbes
   pbesparelm --ignore-cex core.pbes parelmcore.pbes

In the first step we obtain the core of the PBES as defined in [SKLW25]_. The result of the second step is the PBES shown below. 

.. math::

   \begin{array}{l}
   \nu Y = Y\\
   \nu Zpos_{a}(n, v, n'{:}\mathbb{B}) = \mathit{true}\\
   \mu Zneg_{a}(n, v, n'{:}\mathbb{B}) = \mathit{false}\\
   ~\\
   \mathbf{init}\ Y
   \end{array}

We can now provide the two equation systems to :ref:`tool-pbessolve` and save some computation effort while preserving the solution and the counter example of the original PBES. As usual, we must also provide the LPS from which the original PBES was constructed. The command is as follows::

   pbessolve parelmcore.pbes --original-pbes=original.pbes -f original.lps

.. mcrl2_manual:: pbesparelm
