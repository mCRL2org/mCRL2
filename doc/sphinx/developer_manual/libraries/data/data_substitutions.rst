Capture-avoiding substitutions
################################

This section describes how capture-avoiding substitutions are implemented in mCRL2,
using the data expression grammar from the
:ref:`data expressions <data-expressions>` section of the data library overview.

Substitutions
=============

A substitution :math:`\sigma` is a function that maps variables to expressions.
It is assumed that :math:`\sigma` has finite support, in other words there is a
finite number of variables :math:`v` for which :math:`\sigma(v) \neq v`. We
define the substitution update :math:`\sigma[v := v']` as follows:

.. math::

   \sigma[v := v'](w) = \begin{cases}
     v' & \text{if } w = v \\
     \sigma(w) & \text{otherwise}
   \end{cases}

The capture avoiding substitution
==================================

Let :math:`\sigma` be a substitution that maps variables to data expressions, and
let :math:`x` be an arbitrary data expression. Let :math:`FV(x)` be the free
variables in :math:`x`, and let :math:`FV(\sigma)` be the free variables in the
right-hand sides of :math:`\sigma`. More precisely,

.. math::

   FV(\sigma) = \bigcup_{v \in \mathit{domain}(\sigma)} FV(\sigma(v)) \setminus \{v\}.

We define a function :math:`C` that computes the capture-avoiding substitution
:math:`\sigma(x)` using :math:`C(x, \sigma, FV(x) \cup FV(\sigma))`. The function
:math:`C` is recursively defined as follows:

.. math::

   \begin{array}{lll}
   C(v, \sigma, V) & = & \sigma(v) \\
   C(f, \sigma, V) & = & f \\
   C(x(x_1), \sigma, V) & = & C(x, \sigma, V)(C(x_1, \sigma, V)) \\[1ex]
   C(x\ \mathbf{whr}\ v = x_1, \sigma, V) & = & \begin{cases}
     C(x, \sigma, V \cup \{v\})\ \mathbf{whr}\ v = C(x_1, \sigma, V \cup \{v\}) & \text{if } \sigma(v) = v \text{ and } v \notin V \\
     C(x, \sigma', V \cup \{v'\})\ \mathbf{whr}\ v' = C(x_1, \sigma', V \cup \{v'\}) & \text{otherwise}
   \end{cases} \\[4ex]
   C(\Lambda v.x, \sigma, V) & = & \begin{cases}
     \Lambda v.C(x, \sigma, V \cup \{v\}) & \text{if } \sigma(v) = v \text{ and } v \notin V \\
     \Lambda v'.C(x, \sigma', V \cup \{v'\}) & \text{otherwise}
   \end{cases}
   \end{array}

where :math:`\Lambda \in \{\forall, \exists, \lambda\}`, where :math:`v'` is an
arbitrary variable such that :math:`\sigma(v') = v'` and :math:`v' \notin V`,
and where :math:`\sigma' = \sigma[v := v']`.

.. caution::

   The extension of :math:`C` to assignments given below is not correct and does
   not match the actual implementation.

The function :math:`C` can be extended to assignments as follows:

.. math::

   \begin{array}{lll}
   C(v = x, \sigma, V) & = & \begin{cases}
     v = C(x, \sigma, V \cup \{v\}) & \text{if } \sigma(v) = v \text{ and } v \notin V \\
     v' = C(x, \sigma', V \cup \{v'\}) & \text{otherwise}
   \end{cases}
   \end{array}

**Example**

Let :math:`x = \forall b{:}\mathbb{B}.\ b \Rightarrow \forall c{:}\mathbb{B}.\ c \Rightarrow d`
and let :math:`\sigma = [d := b]`. Then
:math:`C(x, \sigma, FV(x) \cup FV(\sigma)) = \forall b'{:}\mathbb{B}.\ b' \Rightarrow \forall c{:}\mathbb{B}.\ c \Rightarrow b`.

Capture-avoiding substitutions with an identifier generator
============================================================

Let :math:`\sigma` be a substitution that maps variables to data expressions. In
this section a substitution is defined that is more efficient than the
capture-avoiding substitution of the previous section because it does not require
the calculation of a set :math:`V` of variables.

It does require that :math:`\sigma` can indicate efficiently whether a variable
occurs in :math:`\sigma(y)` (with :math:`\sigma(y) \neq y`) for some variable
:math:`y`. Furthermore, it requires an identifier generator that can generate
variable names that are guaranteed to be fresh in the sense that they do not
occur in any term.

This substitution has been implemented as
``replace_variables_capture_avoiding_with_an_identifier_generator``.
We use :math:`\mathit{FV}(x)`, :math:`\mathit{FV}(\sigma)` and
:math:`\sigma[v := v']` as defined in the previous section.

The substitution is defined as :math:`\hat{C}` that calculates :math:`\sigma(x)`
using :math:`\hat{C}(x, \sigma)` recursively as follows:

.. math::

   \begin{array}{lll}
   \hat{C}(v, \sigma) & = & \sigma(v) \\
   \hat{C}(f, \sigma) & = & f \\
   \hat{C}(x(x_1), \sigma) & = & \hat{C}(x, \sigma)(\hat{C}(x_1, \sigma)) \\[1ex]
   \hat{C}(x\ \mathbf{whr}\ v = x_1, \sigma) & = & \begin{cases}
     \hat{C}(x, \sigma[v := v])\ \mathbf{whr}\ v = \hat{C}(x_1, \sigma) & \text{if } v \notin \mathit{FV}(\sigma), \\
     \hat{C}(x, \sigma[v := v'])\ \mathbf{whr}\ v' = \hat{C}(x_1, \sigma) & \text{otherwise.}
   \end{cases} \\[4ex]
   \hat{C}(\Lambda v.x, \sigma, V) & = & \begin{cases}
     \Lambda v.\hat{C}(x, \sigma[v := v]) & \text{if } v \notin \mathit{FV}(\sigma), \\
     \Lambda v'.\hat{C}(x, \sigma', V \cup \{v'\}) & \text{otherwise,}
   \end{cases}
   \end{array}

where :math:`\Lambda \in \{\forall, \exists, \lambda\}`, where :math:`v'` is a
fresh variable such that :math:`\sigma(v') = v'` and
:math:`v' \notin \mathit{FV}(\sigma) \cup \mathit{FV}(x)`. The identifier
generator is used to generate the name for :math:`v'`.

In the examples below :math:`[]` is the substitution mapping each variable onto
itself and :math:`[w := v']` is the substitution mapping all variables onto
itself, except that :math:`w` is mapped to :math:`v'`.

**Example**

Let :math:`x = \forall b{:}\mathbb{B}.\ b \Rightarrow \forall c{:}\mathbb{B}.\ c \Rightarrow d`
and let :math:`\sigma = [d := b]`. Then
:math:`\hat{C}(x, \sigma) = \forall b'{:}\mathbb{B}.\ b' \Rightarrow \forall c{:}\mathbb{B}.\ c \Rightarrow b`
where :math:`b'` is a fresh variable.

**Example**

It is necessary that :math:`v'` above is chosen such that
:math:`v' \notin \mathit{FV}(\sigma) \cup \mathit{FV}(x)`. We provide two
examples to show what goes wrong if this condition is not satisfied.

1. If :math:`v' \notin \mathit{FV}(\sigma)` is not required, the following is
   possible: :math:`\hat{C}(\forall v.w,\ [w := v']) = \forall v'.\hat{C}(w,\ [w := v']) = \forall v'.v'`.
2. If :math:`v' \notin \mathit{FV}(x)` is not required, it is possible that:
   :math:`\hat{C}(\forall v.v',\ []) = \forall v'.\hat{C}(v',\ []) = \forall v'.v`.

**Example**

In a where clause the substitutions applied to the equations after the where can
remain unchanged. For example,

.. math::

   \hat{C}(f(u,v)\ \mathbf{whr}\ v=v,\ [u:=v]) =
   \hat{C}(f(u,v),\ [u:=v,\ v:=v'])\ \mathbf{whr}\ v' = \hat{C}(v,\ [u:=v]) =
   f(v,v')\ \mathbf{whr}\ v'=v.

In an expression :math:`f(u, v)\ \mathbf{whr}\ v = v` the variable :math:`v` at
the left-hand side of the :math:`=` is a local variable, whereas the :math:`v`
at the right-hand side is globally bound.
