.. index:: lpsuntime

.. _tool-lpsuntime:

lpsuntime
=========

The lpsuntime tool removes time from a linear process specification (LPS).

Consider an LPS with the following process section:

.. math::

   \begin{array}{l}
   P(d:D) =\\
   \quad \sum_{e_0:E_0} c_0(d,e_0) \to a_0(f_0(d,e_0))^ct_0(d,e_0) \cdot P(g_0(d,e_0)) + {}\\
   \quad \sum_{e_1:E_1} c_1(d,e_1) \to a_1(f_1(d,e_1))^ct_1(d,e_1) \cdot P(g_1(d,e_1)) + {}\\
   \quad \ldots
   \end{array}

The tool transforms this to the following equivalent untimed version: 

.. math::

   \begin{array}{l}
   P(d:D,t:\mathbb{R}) =\\
   \quad \sum_{e_0:E_0} c_0(d,e_0) \land t_0(d,e_0) > t \to a_0(f_0(d,e_0)) \cdot P(g_0(d,e_0),t_0(d,e_0)) + {}\\
   \quad \sum_{e_1:E_1} c_1(d,e_1) \land t_1(d,e_1) > t \to a_1(f_1(d,e_1)) \cdot P(g_1(d,e_1),t_1(d,e_1)) + {}\\
   \quad \ldots
   \end{array}





