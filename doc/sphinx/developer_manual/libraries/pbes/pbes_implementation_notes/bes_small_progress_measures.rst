Small progress measures
=======================

Let :math:`\mathcal{E} = (\sigma_1 X_1 = \varphi_1)\,\cdots\,(\sigma_n X_n = \varphi_n)`
be a BES in standard recursive form. Let :math:`d` be the number of
:math:`\mu`-blocks appearing in :math:`\mathcal{E}`. For each variable
:math:`X_i` we define a corresponding attribute
:math:`\alpha_i \in \mathbb{N}^d`, which is called the *progress measure* of
:math:`X_i`. We define the algorithm :math:`\operatorname{SmallProgressMeasures}`
as follows:

.. math::

   \begin{array}{l}
   \operatorname{SmallProgressMeasures}(\mathcal{E}) \\
   V := \{X_i \mid 1 \leq i \leq n\} \\
   V_{\mathit{Even}} := \{X_i \in V \mid \varphi_i\ \text{is a disjunction}\} \\
   V_{\mathit{Odd}} := \{X_i \in V \mid \varphi_i\ \text{is not a disjunction}\} \\
   \mathbf{for}\ i := 1 \cdots n\ \mathbf{do}\quad \alpha_i := [0, \ldots, 0] \\
   \mathbf{while}\ \mathsf{liftable\_variables}(V) \neq \emptyset\ \mathbf{do} \\
   \quad \mathbf{choose}\ X_i \in \mathsf{liftable\_variables}(V) \\
   \quad \alpha_i := \left\{\begin{array}{ll}
     \mathsf{min}\{\gamma \mid X_j \in \mathsf{occ}(\varphi_i) \wedge \gamma = f(X_j, \mathsf{rank}(X_i))\}
       & \text{if } X_i \in V_{\mathit{Even}} \\
     \mathsf{max}\{\gamma \mid X_j \in \mathsf{occ}(\varphi_i) \wedge \gamma = f(X_j, \mathsf{rank}(X_i))\}
       & \text{if } X_i \in V_{\mathit{Odd}}
   \end{array}\right. \\
   \mathbf{return}\ (W_{\mathit{Even}}, W_{\mathit{Odd}})
   \end{array}

where :math:`\mathsf{min}/\mathsf{max}` is the minimum/maximum with respect to
the lexicographical order on :math:`\mathbb{N}^d`, and
:math:`\beta \in \mathbb{N}^d` is defined as

.. math::

   \beta_i = \left\{\begin{array}{ll}
   0 & \text{if } i \text{ is even} \\
   |\{X_j \in V \mid \mathsf{rank}(X_j) = i\}| & \text{if } i \text{ is odd}
   \end{array}\right.

and the function :math:`\mathsf{inc} : \mathbb{N}^d \times \mathbb{Z} \rightarrow \mathbb{N}^d`
is defined inductively as

.. math::

   \left\{\begin{array}{lll}
   \mathsf{inc}(\alpha, -1) &=& \top \\[4pt]
   \mathsf{inc}(\alpha, i) &=& \left\{\begin{array}{ll}
     \mathsf{inc}([\alpha_0,\ldots,\alpha_{i-1},0,\alpha_{i+1},\ldots,\alpha_d],\, i-1)
       & \text{if } \alpha_i = \beta_i \\
     {}[\alpha_0,\ldots,\alpha_{i-1},\alpha_i+1,\alpha_{i+1},\ldots,\alpha_d]
       & \text{otherwise}
   \end{array}\right.
   \end{array}\right.

and the function :math:`f : \mathbb{N}^d \times \mathbb{N} \rightarrow \mathbb{N}^d`
is defined as:

.. math::

   f(X_j, m) = \left\{\begin{array}{ll}
   [(\alpha_j)_1, \ldots, (\alpha_j)_m, 0, \ldots, 0]
     & \text{if } m \text{ is even} \\
   \mathsf{inc}([(\alpha_j)_1, \ldots, (\alpha_j)_m, 0, \ldots, 0],\, m)
     & \text{if } m \text{ is odd}
   \end{array}\right.

and

.. math::

   \mathsf{liftable\_variables}(V) =
   \{X_i \in V \mid \mathsf{min}\{\alpha \mid w \in \mathsf{occ}(\varphi_i) \wedge \alpha = f(w)\} < \alpha_i\}

and :math:`W_{\mathit{Even}}` and :math:`W_{\mathit{Odd}}` are defined as

.. math::

   \left\{\begin{array}{lll}
   W_{\mathit{Even}} &=& \{X_i \in V \mid \alpha_i < \top\} \\
   W_{\mathit{Odd}} &=& \{X_i \in V \mid \alpha_i = \top\}
   \end{array}\right.

and the function :math:`\mathsf{rank}` is defined inductively as follows:

.. math::

   \left\{\begin{array}{lll}
   \mathsf{rank}(X_1) &=& \left\{\begin{array}{ll}
     0 & \text{if } \sigma_1 = \nu \\
     1 & \text{if } \sigma_1 = \mu
   \end{array}\right. \\[8pt]
   \mathsf{rank}(X_{i+1}) &=& \left\{\begin{array}{ll}
     \mathsf{rank}(X_i) & \text{if } \sigma_{i+1} = \sigma_i \\
     \mathsf{rank}(X_i) + 1 & \text{if } \sigma_{i+1} \neq \sigma_i
   \end{array}\right.
   \end{array}\right.
