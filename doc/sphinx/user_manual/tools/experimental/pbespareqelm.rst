.. index:: pbespareqelm

.. _tool-pbespareqelm:

pbespareqelm
============

This tool aims to detect invariants [OW10]_ of a very specific form in PBESs.
The invariants detected by the tool are identities between parameters of predicate variables. Pairs of parameters that are always equal are discovered, and one of these parameters is eliminated.

Consider, for instance, the following example (The Mutual Exclusion Problem (2) taken from [OW10]_).

.. math::

  \begin{array}{l}
  \nu X(n_r,n_w,t,r,w {:} \nat) = (r + w > 0 \implies r \cdot w =0)\\
  \qquad \wedge (t \ge 1 \implies X(n_r+1,n_w,t-1,r+1,w))\\
  \qquad \wedge
  (n_r > 0 \implies X(n_r-1,n_w,t+1,r-1,w))\\
  \qquad \wedge
  (t \ge N \implies X(n_r,n_w+1,t-N,r,w+1))\\
  \qquad \wedge
  (n_w > 0 \implies  X(n_r,n_w-1,t+N,r,w-1))
  ~\\
  \mathbf{init}\ X(0,0,N,0,0)
  \end{array}

Observe that with this initial configuration, $n_r = r$ and $n_w = w$ are invariants. The resulting PBES is as follows.

.. math::

  \begin{array}{l}
  \nu X(n_r,n_w,t {:} \nat) = (nr + nw > 0 \implies nr \cdot nw =0)\\
  \qquad \wedge (t \ge 1 \implies X(n_r+1,n_w,t-1))\\
  \qquad \wedge
  (n_r > 0 \implies X(n_r-1,n_w,t+1))\\
  \qquad \wedge
  (t \ge N \implies X(n_r,n_w+1,t-N))\\
  \qquad \wedge
  (n_w > 0 \implies  X(n_r,n_w-1,t+N))
  ~\\
  \mathbf{init}\ X(0,0,N)
  \end{array}

.. mcrl2_manual:: pbespareqelm